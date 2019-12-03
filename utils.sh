#!/bin/bash
log_step() {
  echo
  echo -e "\e[1;32m$1...\e[0m"
  echo
}

# sudo ln -sf $PATH_PROJECT/dnsmasq.conf /etc/dnsmasq.d/mascarpone_dnsmasq.conf
PATH_PROJECT="$PWD"
PATH_HOME="$HOME"

PATH_PROJECT_DNS_MASQ_CONF="$PATH_PROJECT/dnsmasq.conf"
PATH_SYSTEM_DNS_MASQ_FOLDER="/etc/dnsmasq.d"
PATH_SYSTEM_DNS_MASQ_CONF="$PATH_SYSTEM_DNS_MASQ_FOLDER/mascarpone_dnsmasq.conf"
PATH_SYSTEM_SUDOERS_FOLDER="/etc/sudoers.d"
PATH_SYSTEM_SUDOERS_CONF="$PATH_SYSTEM_SUDOERS_FOLDER/mascarpone_dnsmasq_sudoers"
PATH_USER_MASCARPONE_SERVICE_CONF="$HOME/.config/systemd/user/mascarpone.service"

PACKAGE_INSTALL=""
PACKAGE_UNINSTALL=""
PACKAGE_UPDATE=""
PACKAGE_QUERY=""

INSTALL_LIST_DNS_MASQ="\
  dnsmasq
  "

SYSTEM_DEPS_RASPBIAN="\
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
  git
  "

check_is_64_bit() {
  local arch=$(uname -m)
  local TRUE=0
  local FALSE=1

  if [ $arch == "armv7l" ]; then
    return $FALSE
  elif [[ $arch == i*86 ]]; then
    # i386, i686, i486
    return $FALSE
  elif [ $arch == "arm64" ]; then
    return $TRUE
  elif [ $arch == "x86_64" ]; then
    return $TRUE
  elif [ $arch == "aarch64" ]; then
    return $TRUE
  fi

  return $FALSE
}

set_package_manager() {
  log_step "Detecting package manager"
  local PKGR=""

  if command -v apt >/dev/null 2>&1; then
    PKGR="sudo apt"
    PACKAGE_INSTALL="$PKGR install -y"
    PACKAGE_UPDATE="$PKGR update"
    PACKAGE_UNINSTALL="$PKGR remove -y"
    PACKAGE_QUERY="dpkg-query -l"

  elif command -v apt-get >/dev/null 2>&1; then
    # apt is newer. If not available, fall back to apt-get
    PKGR="sudo apt-get"
    PACKAGE_INSTALL="$PKGR install -y"
    PACKAGE_UPDATE="$PKGR update"
    PACKAGE_UNINSTALL="$PKGR remove -y"
    PACKAGE_QUERY="dpkg-query -l"

  elif command -v pacman >/dev/null 2>&1; then
    PKGR="sudo pacman"
    PACKAGE_INSTALL="$PKGR -S"
    PACKAGE_UPDATE="$PKGR -Syu"
    PACKAGE_UNINSTALL="$PKGR -Rs"
    PACKAGE_QUERY="pacman -Qi"

  elif command -v brew >/dev/null 2>&1; then
    PKGR="brew"
    PACKAGE_INSTALL="$PKGR install"
    PACKAGE_UPDATE="$PKGR update"
    PACKAGE_UNINSTALL="$PKGR remove"
    PACKAGE_QUERY="brew list"

  elif command -v yum >/dev/null 2>&1; then
    PKGR="sudo yum"
    PACKAGE_INSTALL="$PKGR install"
    PACKAGE_UPDATE="$PKGR update"
    PACKAGE_UNINSTALL="$PKGR remove"
    PACKAGE_QUERY="yum -C list installed"

  else
    echo "Unable to find one of these package managers: apt, apt-get, pacman, brew, yum"
    exit 127
  fi

  echo "detected \"$PKGR\" as the package manager"
}

set_package_manager

check_installed() {
  if [ -z $1 ]; then
    echo Expected ${FUNCNAME[0]} to be called with package name, but was called with nothing. >&2
    return 1
  elif [ -n "$($PACKAGE_QUERY $1 2>/dev/null)" ]; then
    echo "found"
    return 0
  else
    echo
    return 1
  fi
}

install_if_needed() {
  local install_list=""
  for package in $1
  do
    if [ -z $(check_installed $package) ]; then
      install_list="$install_list $package"
    fi
  done

  if [ "$install_list" ]; then
    $PACKAGE_UPDATE
    $PACKAGE_INSTALL $INSTALL_LIST
  fi
}
