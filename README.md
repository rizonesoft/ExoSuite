# ExoSuite

**A modern Control Panel replacement for Windows 10/11**

ExoSuite is a native Windows application that provides a unified interface for accessing system Control Panel applets (.cpl files) and integrated utility extensions. Built with modern C++23 and **Slint**, it delivers a fast, visually polished experience without requiring .NET Framework.

## Features

- 🎛️ **Control Panel Integration** — Browse and launch system .cpl applets
- 🧩 **Extension System** — Modular utilities that run standalone or integrated
- 🔍 **Quick Search** — Find settings instantly with fuzzy search
- 🎨 **Modern UI** — Slint declarative UI with light/dark themes
- ⚡ **Native Performance** — Pure C++23, no managed runtime overhead
- 🛡️ **UAC Aware** — Shield overlays for admin-required applets

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++23 |
| UI Framework | [Slint](https://slint.dev) |
| Build System | CMake (FetchContent) |

### Why Slint?

- **Declarative UI** — Intuitive `.slint` file format
- **Live Preview** — VS Code extension for real-time UI preview
- **Dark/Light Mode** — Built-in system theme detection
- **Tiny Footprint** — Lightweight, ideal for utility tools
- **Native C++** — Compiles to native code with zero runtime overhead

## System Requirements

- **OS**: Windows 10 (build 10240+) or Windows 11
- **Architecture**: 64-bit (x64) only

## Building

```powershell
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

## Project Structure

```
ExoSuite/
├── src/           # C++ source code
├── ui/            # Slint UI files (.slint)
├── resources/     # Icons, assets
├── docs/          # Documentation
└── build/         # Build output (generated)
```

## Extension Architecture

Extensions can operate in two modes:

1. **Standalone** — Independent `.exe` with its own installer
2. **Integrated** — Plugin `.dll` loaded by ExoSuite host

Each extension lives in its own repository and links against shared libraries (`ExoSuite.Core`, `ExoSuite.UI`) when integrated.

## License

Copyright © Rizonesoft (Pty) Ltd. All rights reserved.

## Links

- **Website**: [rizonesoft.com](https://rizonesoft.com)
- **Issues**: [GitHub Issues](https://github.com/rizonesoft/ExoSuite/issues)
- **Slint Docs**: [slint.dev/docs](https://slint.dev/docs)
