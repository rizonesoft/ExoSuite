fn main() {
    // Compile Slint UI
    slint_build::compile("ui/main.slint").unwrap();
    
    // Embed Windows application icon
    #[cfg(windows)]
    {
        let mut res = winresource::WindowsResource::new();
        res.set_icon("resources/ExoSuite.ico");
        res.compile().unwrap();
    }
}
