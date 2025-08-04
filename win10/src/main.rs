use std::error::Error;
use std::path::PathBuf;
use std::{env, fs};
use tauri_winrt_notification::{Duration, IconCrop, Sound, Toast};
use winsafe::co::{KEY, REG_OPTION};
use winsafe::prelude::advapi_Hkey;
use winsafe::{HKEY, RegistryValue};

const APP_ID: &str = "70CentsApple.ChatTools.Win10Notify";

fn extract_icon() -> std::io::Result<()> {
    if !fs::exists("./ct_icon.ico")? {
        fs::write("./ct_icon.ico", include_bytes!("../icon_big.ico"))?;
    }
    Ok(())
}

fn install_registry(ico: Option<&String>) -> Result<(), Box<dyn Error>> {
    // Title and Color
    let key_aumid = HKEY::CURRENT_USER
        .RegCreateKeyEx(
            format!("Software\\Classes\\AppUserModelId\\{}", APP_ID).as_str(),
            None,
            REG_OPTION::NON_VOLATILE,
            KEY::ALL_ACCESS,
            None,
        )?
        .0;
    key_aumid.RegSetKeyValue(
        None,
        Some("DisplayName"),
        RegistryValue::Sz("Chat Tools Toast".to_string()),
    )?;
    key_aumid.RegSetKeyValue(
        None,
        Some("IconBackgroundColor"),
        RegistryValue::Sz("FFDDDDDD".to_string()),
    )?;
    // Set icon
    if ico.is_none() {
        match extract_icon() {
            Ok(_) => {}
            Err(_) => return Ok(()),
        }
    }
    match fs::canonicalize(ico.unwrap_or(&"./ct_icon.ico".to_string())) {
        Ok(p) => {
            let mut path = p.display().to_string();
            path.replace_range(..4, "");
            key_aumid.RegSetKeyValue(None, Some("IconUri"), RegistryValue::Sz(path))?;
        }
        Err(_) => {}
    }

    Ok(())
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let title = args.get(1);
    let ico_path = args.get(3);
    if title.is_some() && title.unwrap() == "--active" {
        return;
    }
    if let Err(_) = install_registry(ico_path) {
        println!("SEND_FAILED");
        return;
    }
    let text = args.get(2);
    let mut toast = Toast::new(APP_ID)
        .title(title.unwrap_or(&String::from("Message from Minecraft")))
        .text1(text.unwrap_or(&String::from("Hello")))
        .sound(Some(Sound::Default))
        .duration(Duration::Short);
    match PathBuf::from(args.get(3).unwrap_or(&"./ct_icon.ico".to_string())).canonicalize() {
        Ok(p) => toast = toast.icon(&p, IconCrop::Square, "ChatTools"),
        Err(_) => {}
    }

    match toast.show() {
        Ok(_) => {
            println!("SEND_OK");
        }
        Err(_) => {
            println!("SEND_FAILED")
        }
    }
}
