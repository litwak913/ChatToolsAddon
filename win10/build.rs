extern crate winres;

use winres::WindowsResource;

fn main() {
    let mut res = WindowsResource::new();
    res.set_icon("icon1.ico");
    res.compile().unwrap();
}