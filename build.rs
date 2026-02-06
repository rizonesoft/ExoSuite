fn main() {
    // Compile Slint UI
    slint_build::compile("ui/main.slint").unwrap();
    
    // Embed Windows application icon
    #[cfg(windows)]
    {
        // Use absolute path from manifest dir
        let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
        let icon_path = std::path::Path::new(&manifest_dir).join("resources/ExoSuite.ico");
        
        println!("cargo::rerun-if-changed=resources/ExoSuite.ico");
        println!("cargo::warning=Embedding icon from: {}", icon_path.display());
        
        let mut res = winresource::WindowsResource::new();
        res.set_icon(icon_path.to_str().unwrap());
        if let Err(e) = res.compile() {
            println!("cargo::warning=Failed to embed icon: {}", e);
        }
    }
}
