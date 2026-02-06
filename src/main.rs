// ==============================================================================
// ExoSuite - Control Panel Application
// ==============================================================================

#![windows_subsystem = "windows"]

use slint::winit_030::WinitWindowAccessor;

slint::include_modules!();

fn main() {
    let app = MainWindow::new().unwrap();
    
    // Set window icon from embedded Windows resource
    // winresource embeds icon with ID 1 (ordinal)
    app.window().with_winit_window(|winit_window| {
        use slint::winit_030::winit::platform::windows::IconExtWindows;
        use slint::winit_030::winit::window::Icon;
        
        // Load icon from resource section - try ordinal ID 1
        match Icon::from_resource(1, None) {
            Ok(icon) => {
                winit_window.set_window_icon(Some(icon));
            }
            Err(e) => {
                // Log error for debugging (won't show in release, but useful for dev)
                eprintln!("Failed to load icon from resource: {:?}", e);
            }
        }
    });
    
    app.run().unwrap();
}
