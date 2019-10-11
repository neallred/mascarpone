from datetime import datetime
from functools import reduce
import json
import os

from waitress import serve
from flask import Flask, request, render_template, request

from mascarpone.filter_service import update_filter_config
from mascarpone.validators import validate_upsert, validate_delete, validate_update
own_path = os.path.dirname(os.path.abspath(__file__))

app = Flask(
    __name__, template_folder=f"{own_path}/templates"
)

def write_new_blacklist(new_blacklist):
    with open("./current_blacklist.json", "w+") as f:
        f.write(json.dumps(new_blacklist))

    update_filter_config(new_blacklist)

    app.logger.info("write new blacklists, still need to implement service restarting")
    # TODO: restart the dnsmasq service.

@app.route("/", methods=["GET", "POST", "PUT", "DELETE"])
def index():
    current_blacklist_string = get_current_blacklist_string()

    if request.method == "GET":
        return render_template("build/index.html", blacklist=current_blacklist_string)

    current_blacklist = json.loads(current_blacklist_string)
    app.logger.info(current_blacklist)
    body = request.json

    if request.method == "POST":
        errors = validate_upsert(body)

        if errors:
            return {"errors": errors}, 400

        now = int(datetime.now().timestamp())
        domain = body["domain"]
        redirect = body["redirect"]

        current_blacklist[domain] = {
            "redirect": redirect,
            "updated": now,
        }
        write_new_blacklist(current_blacklist)

        return current_blacklist

    elif request.method == "PUT":
        errors = validate_update(body)
        print(body)

        if errors:
            return {"errors": errors}, 400

        now = int(datetime.now().timestamp())
        domain = body["domain"]
        new_domain = body["newDomain"]
        redirect = body["redirect"]

        try:
            current_blacklist.pop(domain)
        except Exception as err:
            pass
            # While this shouldn't happen, we don't care if it does.

        current_blacklist[new_domain] = {
            "redirect": redirect,
            "updated": now,
        }
        write_new_blacklist(current_blacklist)

        return current_blacklist

    elif request.method == "DELETE":
        errors = validate_delete(body)

        if errors:
            return {"errors": errors}, 400

        domain = body["domain"]

        try:
            current_blacklist.pop(domain)
        except Exception as err:
            return {"message": "No items deleted"}

        try:
            app.logger.info("Writing new values to source of truth file and dnsmasq.conf")
            write_new_blacklist(current_blacklist)
        except Exception as err:
            app.logger.error("err updating items")
            return err.message, 500

        return current_blacklist


    return "Method not allowed", 405


@app.route("/reset", methods=["POST"])
def reset():
    return "resetting to defaults"


def get_current_blacklist_string():
    try:
        with open("./current_blacklist.json", "r") as f:
            return f.read()
    except FileNotFoundError as e:
        print("current blacklist not found. Bootstrapping:")
        print(e.message)
        set_blacklist_to_predefined()

        with open("./current_blacklist.json", "r") as f:
            return f.read()


def set_blacklist_to_predefined():
    with open("./preconfigured_blacklist.json", "r") as f:
        write_new_blacklist(json.loads(f.read()))


# def get_current_blacklist():
#     return json.loads(get_current_blacklist_string())

if __name__ == "__main__":
    # DEV MODE
    # app.run()
    # PROD MODE
    serve(app, host='0.0.0.0', port=8080)
