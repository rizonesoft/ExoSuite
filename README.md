# ExoSuite

**A modern Control Panel replacement for Windows 10/11**

ExoSuite is a native Windows application that provides a unified interface for accessing system Control Panel applets (.cpl files) and integrated utility extensions. Built with **Rust** and **Slint**, it delivers a fast, visually polished experience.

## Features

- 🎛️ **Control Panel Integration** — Browse and launch system .cpl applets
- 🧩 **Extension System** — Modular utilities that run standalone or integrated
- 🔍 **Quick Search** — Find settings instantly with fuzzy search
- 🎨 **Modern UI** — Slint declarative UI with light/dark themes
- ⚡ **Native Performance** — Pure Rust, no runtime overhead
- 🛡️ **UAC Aware** — Shield overlays for admin-required applets

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | Rust |
| UI Framework | [Slint](https://slint.dev) |
| Win32 API | [windows](https://crates.io/crates/windows) crate |
| Build System | Cargo |

### Why Rust + Slint?

- **Simple Build** — `cargo build` (no CMake, no MSVC setup)
- **Declarative UI** — Intuitive `.slint` file format
- **Live Preview** — VS Code extension for real-time UI preview
- **Dark/Light Mode** — Built-in system theme detection
- **Safe Win32** — Windows crate with Rust's safety guarantees
- **Fast Iteration** — `cargo run` to test

## System Requirements

- **OS**: Windows 10 (build 10240+) or Windows 11
- **Architecture**: 64-bit (x64) only

## Building

```powershell
# Build
cargo build --release

# Run
cargo run --release
```

## Project Structure

```
ExoSuite/
├── src/           # Rust source code
│   └── main.rs    # Entry point
├── ui/            # Slint UI files (.slint)
│   └── main.slint # Main window
├── Cargo.toml     # Dependencies
└── build.rs       # Slint build script
```

## Extension Architecture

Extensions can operate in two modes:

1. **Standalone** — Independent executable
2. **Integrated** — Plugin loaded by ExoSuite host

## License

Copyright © Rizonesoft (Pty) Ltd. All rights reserved.

## Links

- **Website**: [rizonesoft.com](https://rizonesoft.com)
- **Issues**: [GitHub Issues](https://github.com/rizonesoft/ExoSuite/issues)
- **Slint Docs**: [slint.dev/docs](https://slint.dev/docs)
