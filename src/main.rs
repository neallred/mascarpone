#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;
extern crate rocket_contrib;
extern crate serde;
extern crate serde_derive;
extern crate serde_json;
extern crate username;

use serde::{Deserialize, Serialize};
use rocket_contrib::json::Json;
use rocket_contrib::templates::Template;
use rocket_contrib::serve::StaticFiles;
use rocket::request::Form;
use std::fs::File;
use std::io::Read;
use std::collections::HashMap;
use std::error::Error;
use std::process::Command;
use std::time::{SystemTime, UNIX_EPOCH};
extern crate directories;
use directories::{ProjectDirs, BaseDirs};

#[derive(Serialize, Deserialize, FromForm, Debug)]
struct DeleteReqeust {
    domain: String,
}

#[derive(Serialize, Deserialize, FromForm, Debug)]
struct Upsert {
    domain: String,
    redirect: String,
}

#[derive(Serialize, Deserialize, FromForm, Debug)]
struct Update {
    domain: String,
    #[form(field = "newDomain")]
    new_domain: String,
    redirect: String,
}

#[derive(Serialize, Deserialize, FromForm, Debug)]
struct DnsEntry {
    redirect: String,
    updated: u64,
}

#[get("/")]
fn root_get() -> Template {
    let blacklist = read_blacklist().expect("Unable to read current blacklist");
    let serialized = serde_json::to_string(&blacklist).expect("failed to serialize blacklist");
    Template::render("index", IndexContext { blacklist: serialized })
}

#[derive(Serialize)]
struct IndexContext {
    blacklist: String
}

type Blacklist = HashMap<String, DnsEntry>;
type BlacklistResponse = Result<Json<Blacklist>, Box<dyn Error>>;

fn get_project_dirs() -> Option<ProjectDirs> {
    ProjectDirs::from("red.allthings", "",  "Mascarpone")
}

static BOOTSTRAP_BLACKLIST: &'static str = include_str!("../preconfigured_blacklist.json");

fn get_blacklist_folder() -> Result<String, Box<dyn Error>> {
    let project_path = if let Some(proj_dirs) = get_project_dirs() {
        String::from(proj_dirs.config_dir().as_os_str().to_string_lossy())
    } else {
        println!("Failed to find actual directory, usings backup dir based on executable path.");
        String::from(std::env::current_exe()?.as_path().as_os_str().to_string_lossy())
    };

    Ok(format!("{}", project_path))
}

fn get_blacklist_path() -> Result<String, Box<dyn Error>> {
    let folder = get_blacklist_folder()?;

    Ok(format!("{}{}", folder, "/current_blacklist.json"))
}

fn read_blacklist() -> Result<Blacklist, Box<dyn Error>> {
    println!("YOUR IP IS: {:?}", get_ip());
    let file_path = get_blacklist_path()?;
    let file_result = File::open(&file_path);
    match file_result {
        Ok(mut file) => {
            let mut contents = String::new();
            file.read_to_string(&mut contents)?;

            let data: Blacklist = serde_json::from_str(&contents)?;
            Ok(data)
        }
        Err(x) => {
            println!("Could not find current blacklist. {:?} Bootstrapping the file...", x);

            let data: Blacklist = serde_json::from_str(&BOOTSTRAP_BLACKLIST)?;

            match get_project_dirs() {
                Some(proj_dirs) => {
                    let conf_dir = proj_dirs.config_dir();
                    std::fs::create_dir_all(conf_dir)?;
                }
                None => {
                    std::fs::create_dir_all(std::env::current_exe()?.as_path())?;
                }
            };

            std::fs::write(&file_path, &BOOTSTRAP_BLACKLIST)?;
            Ok(data)
        }
    }
}

fn write_blacklist(blacklist: &Blacklist) -> Result<(), Box<dyn Error>> {
    let serialized = serde_json::to_string(blacklist).expect("failed to serialize blacklist");

    std::fs::write(get_blacklist_path()?, serialized)?;

    Ok(())
}

fn restart_dnsmasq() -> Result<(), Box<dyn Error>> {
    let restart_output = Command::new("sudo")
        .arg("systemctl")
        .arg("restart")
        .arg("dnsmasq")
        .output()?;

    println!("restart dnsmasq output: {:?}", restart_output);

    Ok(())
}

fn get_now() -> Result<u64, Box<dyn Error>> {
    let start = SystemTime::now();
    let since_the_epoch = start
        .duration_since(UNIX_EPOCH)
        .expect("Time went backwards");
    Ok(since_the_epoch.as_secs())
}


#[post("/", data = "<upsert>")]
fn root_post(upsert: Form<Upsert>) -> BlacklistResponse {
    println!("post {:?}", upsert);
    let mut blacklist = read_blacklist().expect("Unable to read current blacklist");
    let now = get_now()?;

    let insert_result = blacklist.insert(
        upsert.domain.clone(),
        DnsEntry {
            redirect: upsert.redirect.clone(),
            updated: now,
        }
    );
    let message = match insert_result {
        Some(_) => { "Updated domain." }
        None => { "Inserted record." }
    };
    println!("{}", message);

    write_blacklist(&blacklist)
        .expect("Unable to write blacklist after upsert");
    restart_dnsmasq()?;

    Ok(Json(blacklist))
}

#[put("/", data = "<update>")]
fn root_put(update: Form<Update>) -> BlacklistResponse {
    println!("put {:?}", update);

    let mut blacklist = read_blacklist().expect("Unable to read current blacklist");
    let now = get_now()?;

    match blacklist.remove(&update.domain) {
        Some(redir) => {
            println!("Removed {:?} {:?}", &update.domain, &redir);
        }
        None => {
            println!("{:?} not found in blacklist, so not deleted", &update.domain);
        }
    }

    let insert_result = blacklist.insert(
        update.new_domain.clone(),
        DnsEntry {
            redirect: update.redirect.clone(),
            updated: now,
        }
    );
    let message = match insert_result {
        Some(_) => { "Updated domain." }
        None => { "Inserted record." }
    };
    println!("{}", message);

    write_blacklist(&blacklist)
        .expect("Unable to write blacklist after update");
    restart_dnsmasq()?;

    Ok(Json(blacklist))
}

#[delete("/", data = "<delete_request>")]
fn root_delete(delete_request: Form<DeleteReqeust>) -> BlacklistResponse {
    println!("delete {:?}", delete_request);

    let mut blacklist = read_blacklist().expect("Unable to read current blacklist");
    match blacklist.remove(&delete_request.domain) {
        Some(redir) => {
            println!("Removed {:?} {:?}", &delete_request.domain, &redir);
            write_blacklist(&blacklist)
                .expect(&format!("Unable to write blacklist after deletion {}", delete_request.domain));
            restart_dnsmasq()?;
        }
        None => {
            println!("{:?} not found in blacklist, so not deleted", &delete_request.domain);
        }
    };

    Ok(Json(blacklist))
}

fn main() {
    rocket::ignite()
        .mount("/", routes![root_get, root_post, root_put, root_delete])
        .mount("/", StaticFiles::from("build"))
        .attach(Template::fairing())
        .launch();
}


fn write_daemon_service_file() -> Result<(), Box<dyn Error>> {
    let path_service_file = if let Some(base_dirs)= BaseDirs::new() {
        format!("{}/systemd/user/mascarpone.service", base_dirs.config_dir().as_os_str().to_string_lossy())
    } else {
        String::from("~/.config/systemd/user/mascarpone.service")
    };

    let path_executable = if let Some(base_dirs)= BaseDirs::new() {
        let bin_dir = if let Some(x) = base_dirs.executable_dir() {
            String::from(x.as_os_str().to_string_lossy())
        } else {
            String::from("~/")
        };
        format!("{}/mascarpone", bin_dir)
    } else {
        String::from("~/.local/bin/mascarpone")
    };

    let username = username::get_user_name()
        .expect("Expected to get find user name but failed");

    let service_file = format_service_file(path_executable, username);
    std::fs::write(path_service_file, service_file)?;
    Ok(())
}

use std::net::UdpSocket;

// from https://github.com/egmkang/local_ipaddress
// MIT License
//
// Copyright (c) 2019 egmkang wang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
fn get_ip() -> Option<String> {
    let socket = match UdpSocket::bind("0.0.0.0:0") {
        Ok(s) => s,
        Err(_) => return None,
    };

    match socket.connect("8.8.8.8:80") {
        Ok(()) => (),
        Err(_) => return None,
    };

    match socket.local_addr() {
        Ok(addr) => return Some(addr.ip().to_string()),
        Err(_) => return None,
    };
}

fn get_dnsmasq_header() -> String {
    let nameserver_port = 53;
    let primary_server = "205.171.3.65";
    let secondary_server = "1.1.1.1";
    let cache_size = 1000;
    let loopback_address = "127.0.0.1";
    let own_local_address = match get_ip() {
        Some(x) => x,
        None => String::from("127.0.0.1"),
    };

    format!("
# Listen on this specific port instead of the standard DNS port
# (53). Setting this to zero completely disables DNS function,
# leaving only DHCP and/or TFTP.
no-resolv
port={}
server={}
server={}

# Set the cachesize here.
# How many domains are cached for domain lookup.
cache-size={}

# Or which to listen on by address (remember to include {} if
# you use this.)
listen-address=::1,{},{}
",
        nameserver_port,
        primary_server,
        secondary_server,
        cache_size,
        loopback_address,
        loopback_address,
        own_local_address,
    )
}

fn format_service_file(mascarpone_executable_path: String, user_name: String) -> String {
    format!("
[Unit]
# Human readable name of the unit
Description=Mascarpone DNSMasq UI wrapper
After=network-online.target dnsmasq
# After=network-online.target dnsmasq

[Service]
# Command to execute when the service is started
Type=simple
ExecStart={}/mascarpone

Restart=always
RestartSec=5
Environment=PYTHONUNBUFFERED=1
# User={}

[Install]
WantedBy=default.target
",
        mascarpone_executable_path,
        user_name,
    )
}
