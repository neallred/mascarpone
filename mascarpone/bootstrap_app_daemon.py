import os.path
import json
from mascarpone.filter_service import write_daemon_service_file, initialize_app_daemon, update_filter_config

def bootstrap_blacklist():
    with open("./preconfigured_blacklist.json", "r") as f_bootstrap:
        with open("./current_blacklist.json", "w") as f:
            bootstrap_string = f_bootstrap.read()
            f.write(bootstrap_string)
            update_filter_config(json.loads(bootstrap_string))

if __name__ == "__main__":
    write_daemon_service_file()
    initialize_app_daemon()
    if not os.path.isfile("./current_blacklist.json"):
        bootstrap_blacklist()
