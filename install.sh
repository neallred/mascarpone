# [ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi
. "$DIR/utils.sh"

ensure_system_deps() {
  log_step "Installing system deps if needed"
  install_if_needed $SYSTEM_DEPS_RASPBIAN
}

ensure_dnsmasq() {
  log_step "Installing dnsmasq if needed"

  install_if_needed $INSTALL_LIST_DNS_MASQ
}

install_poetry() {
  log_step "Installing poetry if needed"
  if ! command -v poetry 1>/dev/null 2>&1; then
    curl -sSL https://raw.githubusercontent.com/sdispater/poetry/master/get-poetry.py | python
  fi
}

ensure_pyenv() {
  log_step "Installing pyenv if needed"
  if ! command -v pyenv 1>/dev/null 2>&1; then
    curl -L https://github.com/pyenv/pyenv-installer/raw/master/bin/pyenv-installer | bash

    grep pyenv ~/.bashrc ~/.bashrc_machine ~/.profile || echo '\
export PATH="~/.pyenv/bin:$PATH"\
eval "$(pyenv init -)"\
eval "$(pyenv virtualenv-init -)"\
    ' > ~/.bashrc
  fi
}

enter_project() {
  log_step "Entering project directory \"$PATH_PROJECT\""
  cd $PATH_PROJECT
}

ensure_python() {
  # TODO. Installing python and python-pip is made separate from other deps
  # because the user likely does not mean to remove python and will break things
  # if they do

  log_step "Installing python and pip if needed"
  if command -v python >/dev/null 2>&1; then
    echo "python already installed at $(which python)"
  else
    $PACKAGE_INSTALL python
  fi

  if command -v pip >/dev/null 2>&1; then
    echo "pip already installed at $(which pip)"
  else
    $PACKAGE_INSTALL python-pip
  fi
}

ensure_javascript() {
  log_step "Installing nvm and node if needed"
  if command -v nvm >/dev/null 2>&1; then
    echo "nvm already installed at $(which nvm)"
  else
    curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.0/install.sh | bash
    source ~/.bashrc || true
    source ~/.bash_profile || true
    source ~/.profile || true
    nvm install 10
    nvm alias default 10
    npm install -g yarn
  fi
  if command -v yarn >/dev/null 2>&1; then
    echo "yarn already installed at $(which yarn)"
  else
    npm install -g yarn
  fi

  export NVM_DIR="$HOME/.nvm"
  [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
  [ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads nvm bash_completion
}

back_end_build() {
  log_step "Building back end"
  ensure_python
  ensure_pyenv

  pyenv install 3.7.4 --skip-existing
  pyenv virtualenv 3.7.4 mascarpone || true
  pyenv local mascarpone

  install_poetry
  if [ -e $HOME/.poetry/env ]; then
    source $HOME/.poetry/env
  fi

  poetry install
}

front_end_build() {
  log_step "Building front end"
  # The front end build relies on bs-platform, which requires a 64-bit
  # architecture. See the following issues for more details:
  # https://github.com/BuckleScript/bucklescript/issues/773
  # https://github.com/BuckleScript/bucklescript/issues/2423

  if check_is_64_bit; then
    ensure_javascript
    yarn
    yarn build
  else
    local JS_BUNDLE_PATH="<PROJECT ROOT>/mascarpone/static/Index.js"

    cat << SKIP_JS_BUILD_EXPLANATION

Javascript build skipped; system architecture is not 64-bit.
Assuming Javascript bundle exists at $JS_BUNDLE_PATH.
If it does not, the server will work but will not be accessible by web browser.

SKIP_JS_BUILD_EXPLANATION
  fi
}

build_project() {
  log_step "building project"
  git pull > /dev/null

  back_end_build

  front_end_build
}

make_sudoers_file() {
  SYSTEM_CTL_PATH=$(which systemctl)
  ME=$(whoami)

  SUDOERS_FILE="\
  Cmnd_Alias DNS_MASQ_CMD = $SYSTEM_CTL_PATH start dnsmasq, $SYSTEM_CTL_PATH stop dnsmasq, $SYSTEM_CTL_PATH restart dnsmasq, $SYSTEM_CTL_PATH status dnsmasq, $SYSTEM_CTL_PATH reload dnsmasq\n$ME ALL=(ALL) NOPASSWD: DNS_MASQ_CMD"

  sudo mkdir -p $PATH_SYSTEM_SUDOERS_FOLDER
  echo -e $SUDOERS_FILE | sudo tee $PATH_SYSTEM_SUDOERS_CONF >/dev/null
}

symlink_dnsmasq_conf() {
  sudo mkdir -p $PATH_SYSTEM_DNS_MASQ_FOLDER
  sudo ln -sf $PATH_PROJECT_DNS_MASQ_CONF $PATH_SYSTEM_DNS_MASQ_CONF
}

ensure_mascarpone_daemon() {
  python3 mascarpone/bootstrap_app_daemon.py
}

log_manual_setup_steps() {
  INSTALL_MACHINE_IP=$(hostname -I | cut -d' ' -f1)

  echo
  echo -e "\e[1;32mInstall complete.\e[0m"

  cat << MANUAL_SETUP_STEPS

To have devices use the device mascarpone was installed on as a nameserver,
you must configure your router to use the install machine as primary nameserver
and a third party service as secondary nameserver.

Typical steps for configuring the router are:

1. Login to the router with the router admin's username and password.
   If you have not changed these, they can often be found on a sticker label
   on the router.

2. Find the advanced configuration section, sometimes called "Advanced Setup"
   or "Advanced Configuration"

3. Find the nameserver settings. It might be found underneath options such as
   "WAN Settings", "DNS Settings", "Static DNS" or "IP addressing type"

4. Before making changes, write down how to find the settings
   as well as the settings' values prior to making changes!
   It is very important to be able to revert to a known working solution if
   setup does not work correctly or something later malfunctions.

5. As the primary nameserver, enter the local IP address of the machine that
   mascarpone is installed on. Based on this script's detection, it seems to be:

   $INSTALL_MACHINE_IP

   If an option to choose a port exists, use port 53. If no option is given,
   then the router assumes port 53 by default.

6. As the secondary nameserver, enter the IP address of your chosen third
   party DNS service.

7. Save or Apply settings. If prompted, restart the router.

Some common third party DNS servers are:

1.1.1.1        (Cloudflare primary)
208.67.222.222 (OpenDNS primary)
8.8.8.8        (Google primary)
205.171.3.65   (Century Link primary)

Make sure to confirm the third party nameserver's IP address before using.

If the install machine's IP address is $INSTALL_MACHINE_IP,
you should be able configure the mascarpone internet filterer
on devices connected to your home network from $INSTALL_MACHINE_IP:8080

MANUAL_SETUP_STEPS
}

# DO STUFF SECTION
main () {
  enter_project
  symlink_dnsmasq_conf
  ensure_system_deps
  ensure_dnsmasq
  build_project
  make_sudoers_file
  ensure_mascarpone_daemon
  log_manual_setup_steps
}

main
