#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;
extern crate rocket_contrib;
extern crate serde;
extern crate serde_derive;
extern crate serde_json;

use serde::{Deserialize, Serialize};
use rocket_contrib::json::Json;
use rocket::request::Form;
use std::collections::HashMap;

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
    updated: u32,
}

#[get("/")]
fn root_get() -> &'static str {
    "Hello, get!\n"
}

type Blacklist = HashMap<String, DnsEntry>;

fn make_dummy_response() -> Json<Blacklist> {
    let mut dummy_response: HashMap<String, DnsEntry> = HashMap::new();
    dummy_response.insert(
        String::from("evil.com"),
        DnsEntry {
            redirect: String::from("127.0.0.1"),
            updated: 1234567890
        },
    );
    dummy_response.insert(
        String::from("google.com"),
        DnsEntry {
            redirect: String::from("127.0.0.1"),
            updated: 1234567890
        },
    );
    Json(dummy_response)
}

#[post("/", data = "<upsert>")]
fn root_post(upsert: Form<Upsert>) -> Json<Blacklist> {
    println!("post {:?}", upsert);
    make_dummy_response()
}

#[put("/", data = "<update>")]
fn root_put(update: Form<Update>) -> Json<Blacklist> {
    println!("put {:?}", update);
    make_dummy_response()
}

#[delete("/", data = "<delete_request>")]
fn root_delete(delete_request: Form<DeleteReqeust>) -> Json<Blacklist> {
    println!("delete {:?}", delete_request);
    make_dummy_response()
}

fn main() {
    rocket::ignite().mount("/", routes![root_get, root_post, root_put, root_delete]).launch();
}
