# [ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

# DEFINE STUFF SECTION
PACKAGE_MANAGER=""
PACKAGE_INSTALL=""
PROJECT_PATH="$PWD"
HOME_PATH="$HOME"
DNS_MASQ_CONF_PATH="$HOME"

PYENV_DEPS_RASPBIAN=" \
  make
  build-essential
  libssl-dev
  zlib1g-dev
  libbz2-dev
  libreadline-dev
  libsqlite3-dev
  wget
  curl
  llvm
  libncurses5-dev
  libncursesw5-dev
  openssl
  bzip2
  "

INSTALL_LIST_DNS_MASQ=" \
  dnsmasq
  "

set_package_manager() {
  if command -v apt >/dev/null 2>&1; then
    PACKAGE_MANAGER="apt"
    PACKAGE_INSTALL="sudo $PACKAGE_MANAGER install -y"
    # "sudo $PACKAGE_MANAGER update && sudo $PACKAGE_MANAGER upgrade -y"
    PACKAGE_UPGRADE="sudo $PACKAGE_MANAGER update && sudo $PACKAGE_MANAGER upgrade -y"

  elif command -v apt-get >/dev/null 2>&1; then
    # apt is newer. If not available, fall back to apt-get
    PACKAGE_MANAGER="apt-get"
    PACKAGE_INSTALL="sudo $PACKAGE_MANAGER install -y"
    PACKAGE_UPGRADE="sudo $PACKAGE_MANAGER update && sudo $PACKAGE_MANAGER upgrade -y"

  elif command -v pacman >/dev/null 2>&1; then
    PACKAGE_MANAGER="pacman"
    PACKAGE_INSTALL="sudo $PACKAGE_MANAGER -S"
    PACKAGE_UPGRADE="sudo $PACKAGE_MANAGER -Syu"

  elif command -v brew >/dev/null 2>&1; then
    PACKAGE_MANAGER="brew"
    PACKAGE_INSTALL="$PACKAGE_MANAGER install"
    PACKAGE_INSTALL="$PACKAGE_MANAGER update && $PACKAGE_MANAGER upgrade"

  elif command -v yum >/dev/null 2>&1; then
    PACKAGE_MANAGER="yum"
    PACKAGE_INSTALL="sudo $PACKAGE_MANAGER install"
    PACKAGE_UPGRADE="sudo $PACKAGE_MANAGER update"

  else
    echo "Unable to find one of these package managers: apt, apt-get, pacman, brew, yum"
    exit 127
    PACKAGE_MANAGER="unknown"
  fi
}


upgrade_packages() {
  sudo $PACKAGE_MANAGER update && sudo $PACKAGE_MANAGER upgrade -y
}

install_dnsmasq() {
  $PACKAGE_INSTALL $INSTALL_LIST_DNS_MASQ
}

start_dnsmasq() {
  dnsmasq --test
  sudo systemctl enable dnsmasq 
  sudo systemctl start dnsmasq 
}

install_poetry() {
  if ! command -v poetry 1>/dev/null 2>&1; then
    curl -sSL https://raw.githubusercontent.com/sdispater/poetry/master/get-poetry.py | python
  fi
}

ensure_pyenv() {
  if ! command -v pyenv 1>/dev/null 2>&1; then
    curl -L https://github.com/pyenv/pyenv-installer/raw/master/bin/pyenv-installer | bash

    grep pyenv ~/.bashrc ~/.bashrc_maching ~/.profile || echo '\
export PATH="~/.pyenv/bin:$PATH"\
eval "$(pyenv init -)"\
eval "$(pyenv virtualenv-init -)"\
    ' > ~/.bashrc
  fi
}

enter_project() {
  cd $PROJECT_PATH
}

ensure_python() {
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
    echo "nvm already installed at $(which nvm)"
  else
    npm install -g yarn
  fi

  export NVM_DIR="$HOME/.nvm"
  [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
  [ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads nvm bash_completion
}

bootstrap_project() {
  $PACKAGE_INSTALL $PYENV_DEPS_RASPBIAN
  ensure_python
  ensure_pyenv


  # alias pyenv="~/.pyenv/bin/pyenv"

  # pyenv install 3.7.4 --force
  pyenv install 3.7.4 --skip-existing
  pyenv virtualenv 3.7.4 mascarpone || true
  pyenv local mascarpone

  install_poetry
  source $HOME/.poetry/env
  poetry install

  ensure_javascript
  yarn
  yarn build
}

# start_web_server() {
#   python $PROJECT_PATH/mascarpone/main.py
# }
# start_web_server


make_sudoers_file() {
  SYSTEM_CTL_PATH=$(which systemctl)
  ME=$(whoami)

  SUDOERS_FILE="\
  Cmnd_Alias DNS_MASQ_CMD = $SYSTEM_CTL_PATH start dnsmasq, $SYSTEM_CTL_PATH stop dnsmasq, $SYSTEM_CTL_PATH restart dnsmasq, $SYSTEM_CTL_PATH status dnsmasq, $SYSTEM_CTL_PATH reload dnsmasq\n$ME ALL=(ALL) NOPASSWD: DNS_MASQ_CMD"

  sudo mkdir -p /etc/sudoers.d/
  echo -e $SUDOERS_FILE | sudo tee /etc/sudoers.d/mascarpone_dnsmasq_sudoers
}

symlink_dnsmasq_conf() {
  sudo mkdir -p /etc/dnsmasq.d/
  sudo ln -sf $PROJECT_PATH/dnsmasq.conf /etc/dnsmasq.d/mascarpone_dnsmasq.conf
}

ensure_mascarpone_daemon() {
  python3 mascarpone/bootstrap_app_daemon.py
}

# DO STUFF SECTION
enter_project
symlink_dnsmasq_conf
set_package_manager
upgrade_packages
install_dnsmasq
bootstrap_project
make_sudoers_file
ensure_mascarpone_daemon
