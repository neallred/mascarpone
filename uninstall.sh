#!/bin/bash
# This script can not safely remove deps,
# because the install script does not communicate which dependencies were already installed
# So only remove things that can only have been added by the install script

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi
. "$DIR/utils.sh"

stop_mascarpone() {
  log_step "Stopping mascarpone"

  systemctl --user disable mascarpone
  systemctl --user stop mascarpone
  # systemctl --user status mascarpone
}

remove_mascarpone() {
  log_step "Removing mascarpone"

  echo "Removing $PATH_USER_MASCARPONE_SERVICE_CONF"
  sudo rm -f $PATH_USER_MASCARPONE_SERVICE_CONF
}

stop_dnsmasq() {
  log_step "Stopping dnsmasq"

  sudo systemctl disable dnsmasq
  sudo systemctl stop dnsmasq
}

remove_dnsmasq() {
  log_step "Removing dnsmasq"
  local DNS_MASQ_INSTALL_ONE_LINE=$(echo $INSTALL_LIST_DNS_MASQ)

  echo "Removing $PATH_SYSTEM_SUDOERS_CONF"
  sudo rm -f $PATH_SYSTEM_SUDOERS_CONF

  echo "Removing $PATH_SYSTEM_DNS_MASQ_CONF"
  sudo rm -f $PATH_SYSTEM_DNS_MASQ_CONF

  echo "Uninstalling $DNS_MASQ_INSTALL_ONE_LINE"
  $PACKAGE_UNINSTALL $DNS_MASQ_INSTALL_ONE_LINE
}

inform_full_delete_command() {
  local DEPS_ONE_LINE=$(echo $SYSTEM_DEPS_RASPBIAN)
  cat << FULL_DELETE_INSTRUCTIONS

Uninstall complete.

Uninstall did not remove these system dependencies:
$SYSTEM_DEPS_RASPBIAN
because mascarpone scripts do not track whether system dependencies
were installed prior to installing mascarpone.
If you know nothing else uses these dependencies, you may remove them with:

  $PACKAGE_UNINSTALL $DEPS_ONE_LINE

Uninstall did not remove the project directory.
If you do not plan on using this project again, you may remove it with:

  rm -rf $PWD

This will delete the project directory and to use and install again
you will need to clone the git repository.

FULL_DELETE_INSTRUCTIONS
}

main() {
  stop_mascarpone
  remove_mascarpone
  stop_dnsmasq
  remove_dnsmasq
  inform_full_delete_command
}

main
