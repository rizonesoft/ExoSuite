# ExoSuite - TODO

A modern Control Panel replacement built with **Slint** - a lightweight, declarative UI toolkit.

---

## Why Slint?

**Slint** is a modern C++ UI framework that compiles `.slint` UI definitions directly into native code.

| Feature | Benefit |
|---------|---------|
| **Declarative UI** | Intuitive `.slint` file format - easy to read and design |
| **Live Preview** | VS Code extension for real-time UI preview without recompiling |
| **Dark/Light Mode** | Built-in system theme detection and switching |
| **Tiny Footprint** | Extremely lightweight, ideal for utility tools |
| **C++ Integration** | Compiles to native C++ code with zero runtime overhead |
| **Modern Design** | Fluent-style widgets that look native on Windows |

---

## Phase 1: Project Setup

- [ ] Install Slint VS Code extension for live preview
- [ ] Create CMakeLists.txt with Slint integration
- [ ] Add Slint as FetchContent dependency (no manual downloads)
- [ ] Create basic project structure:
  - `src/main.cpp` - Entry point
  - `ui/main.slint` - Main window UI
- [ ] Build and verify minimal window launches

---

## Phase 2: UI Shell Design

- [ ] Design main window layout in `main.slint`:
  - NavigationView sidebar (categories)
  - Content area (list of applets)
  - Toolbar (view modes, refresh)
  - Status bar (item count)
- [ ] Implement dark/light theme switching
- [ ] Add system theme detection

---

## Phase 3: CPL Engine Integration

- [ ] Create Core namespace:
  - `CplInterop.h/cpp` - Win32 CPL API wrappers
  - `CplItem.h/cpp` - Single applet representation
  - `CplLoader.h/cpp` - Enumerate and load CPLs
- [ ] Connect CPL data to Slint ListView model
- [ ] Implement double-click to launch applet

---

## Phase 4: Extension System

- [ ] Define extension manifest format (JSON/TOML)
- [ ] Create extension loader for `extensions/` directory
- [ ] Display extensions alongside system CPLs
- [ ] Implement extension execution

---

## Phase 5: Polish & Release

- [ ] Add application icon
- [ ] Create app manifest (DPI awareness, admin elevation)
- [ ] Build Release configuration
- [ ] Test on Windows 10 and 11
- [ ] Create GitHub release

---

## Resources

- [Slint Documentation](https://slint.dev/docs)
- [Slint C++ Tutorial](https://slint.dev/docs/tutorial/cpp)
- [Slint GitHub](https://github.com/slint-ui/slint)
- [VS Code Extension](https://marketplace.visualstudio.com/items?itemName=Slint.slint)
