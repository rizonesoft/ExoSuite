// ==============================================================================
// ExoSuite - Control Panel Application
// ==============================================================================

#![windows_subsystem = "windows"]

slint::include_modules!();

fn main() {
    let app = MainWindow::new().unwrap();
    app.run().unwrap();
}
