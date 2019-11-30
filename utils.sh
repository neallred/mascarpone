#!/bin/bash
log_step() {
  echo "$1..."
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
PACKAGE_UPGRADE=""

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

set_package_manager() {
  local PKGR=""

  if command -v apt >/dev/null 2>&1; then
    PKGR="sudo apt"
    PACKAGE_INSTALL="$PKGR install -y"
    PACKAGE_UPGRADE="$PKGR update && $PKGR upgrade -y"
    PACKAGE_UNINSTALL="$PKGR remove -y"

  elif command -v apt-get >/dev/null 2>&1; then
    # apt is newer. If not available, fall back to apt-get
    PKGR="sudo apt-get"
    PACKAGE_INSTALL="$PKGR install -y"
    PACKAGE_UPGRADE="$PKGR update && $PKGR upgrade -y"
    PACKAGE_UNINSTALL="$PKGR remove -y"

  elif command -v pacman >/dev/null 2>&1; then
    PKGR="sudo pacman"
    PACKAGE_INSTALL="$PKGR -S"
    PACKAGE_UPGRADE="$PKGR -Syu"
    PACKAGE_UNINSTALL="$PKGR -Rs"

  elif command -v brew >/dev/null 2>&1; then
    PKGR="brew"
    PACKAGE_INSTALL="$PKGR install"
    PACKAGE_UPGRADE="$PKGR update && $PKGR upgrade"
    PACKAGE_UNINSTALL="$PKGR remove"

  elif command -v yum >/dev/null 2>&1; then
    PKGR="sudo yum"
    PACKAGE_INSTALL="$PKGR install"
    PACKAGE_UPGRADE="$PKGR update"
    PACKAGE_UNINSTALL="$PKGR remove"

  else
    echo "Unable to find one of these package managers: apt, apt-get, pacman, brew, yum"
    exit 127
  fi
}

set_package_manager
