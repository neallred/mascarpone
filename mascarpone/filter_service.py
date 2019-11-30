# File for interacting with the external filter service
import os
import socket
import pwd
import re
import subprocess
import json
# of format ['ls', '-l']
def execute_command(command_and_args_list):
    return subprocess.run(command_and_args_list, stdout=subprocess.PIPE)

def get_command_output(command_and_args_list):
    result = subprocess.run(command_and_args_list, stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8')

def get_username():
    return pwd.getpwuid(os.getuid())[0]

DEFAULT_IP_ADDRESS = "192.168.0.101"
def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except:
        IP = DEFAULT_IP_ADDRESS
    finally:
        s.close()
    return IP

def get_virtual_env():
    # TODO: Is this robust enough?
    return f"{PATH_TO_HOME}/.pyenv/versions/mascarpone/"
    # virtual_envs = get_command_output(['pyenv', 'virtualenvs'])
    # smaller_container =  re.search(r'mascarpone(.*?)\n', virtual_envs).group(1)
    # virtal_env =  re.search(r'/[a-zA-Z-_0-9/.]*', smaller_container)
    # return smaller_container[virtal_env.start():virtal_env.end()]


USER_NAME=get_username()
PATH_TO_HOME=f"/home/{USER_NAME}"
PATH_TO_PYENV=f"{PATH_TO_HOME}/.pyenv/bin/pyenv"
PATH_TO_VIRTUAL_ENV=get_virtual_env()
PATH_TO_PROJECT_PYTHON=f"{PATH_TO_VIRTUAL_ENV}/bin/python"
PATH_TO_DAEMON_SERVICE_FILE=f"{PATH_TO_HOME}/.config/systemd/user/mascarpone.service"
OWN_LOCAL_ADDRESS = get_ip()
LOOPBACK_ADDRESS = "127.0.0.1"
NAMESERVER_PORT = "53"  # Leave as 53
CACHE_SIZE = "1000"  # Max size of domain lookups to cache
PRIMARY_NAME_SERVER = "205.171.3.65" # Century Link
SECONDARY_NAME_SERVER = "1.1.1.1" # Cloudflare
FILTERER_CONFIG_ADDRESS = "./dnsmasq.conf"
DNS_MASQ_CONFIG_HEADER = f"""
# Listen on this specific port instead of the standard DNS port
# (53). Setting this to zero completely disables DNS function,
# leaving only DHCP and/or TFTP.
no-resolv
port={NAMESERVER_PORT}
server={PRIMARY_NAME_SERVER}
server={SECONDARY_NAME_SERVER}

# Set the cachesize here.
# How many domains are cached for domain lookup.
cache-size={CACHE_SIZE}

# Or which to listen on by address (remember to include {LOOPBACK_ADDRESS} if
# you use this.)
listen-address=::1,{LOOPBACK_ADDRESS},{OWN_LOCAL_ADDRESS}
"""

own_path = os.path.dirname(os.path.abspath(__file__))


def write_file(filename, data):
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    with open(filename, "w") as f:
        f.write(data)

def update_filter_config(new_blacklist):
    new_dnsmasq_config = make_dnsmasq_config(new_blacklist)

    write_file(FILTERER_CONFIG_ADDRESS, new_dnsmasq_config)

    # get_command_output(['sudo', 'systemctl', 'restart', 'dnsmasq'])


def format_entry(entry):
    (domain, redirect) = entry
    return f"address=/{domain}/{redirect}"


def make_dnsmasq_config_default():
    with open("./preconfigured_blacklist.json", "r") as f:
        preconfigured_blacklist = json.loads(f.read())
    return make_dnsmasq_config(preconfigured_blacklist)


def make_dnsmasq_config(entries):
    formatted_entries = "\n".join(
        [f"address=/{k}/{v['redirect']}" for k, v in entries.items()]
    )
    return f"""
{DNS_MASQ_CONFIG_HEADER}

{formatted_entries}
"""

# def read_dns_masq_conf():
#     with open("dnsmasq.conf") as f:
#         read_data = f.read()
#     return read_data

def write_daemon_service_file():
    write_file(PATH_TO_DAEMON_SERVICE_FILE, make_daemon_service_file(PATH_TO_VIRTUAL_ENV))

def initialize_app_daemon():
    get_command_output(['systemctl', '--user', 'enable', 'mascarpone'])
    get_command_output(['systemctl', '--user', 'restart', 'mascarpone'])
    get_command_output(['systemctl', '--user', 'status', 'mascarpone'])

def make_daemon_service_file(path_to_virtual_env):
    return f"""
[Unit]
# Human readable name of the unit
Description=Mascarpone DNSMasq UI wrapper
After=network-online.target dnsmasq
# After=network-online.target dnsmasq

[Service]
# Command to execute when the service is started
Type=simple
ExecStart={path_to_virtual_env}/bin/python {own_path}/server.py

Restart=always
RestartSec=5
Environment=PYTHONUNBUFFERED=1
# User={USER_NAME}

[Install]
WantedBy=default.target
"""
