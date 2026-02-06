# ExoSuite Development Roadmap

This document outlines the complete development roadmap from initial structure to production-ready release.

**Tech Stack**: C++23, **Slint** UI Framework (No .NET Framework Required)

---

## Why Slint?

**Slint** is a modern declarative UI toolkit that compiles `.slint` UI definitions directly into native C++ code.

| Feature | Benefit |
|---------|--------|
| **Declarative UI** | Intuitive `.slint` file format - easy to read and design |
| **Live Preview** | VS Code extension for real-time UI preview without recompiling |
| **Dark/Light Mode** | Built-in system theme detection and switching |
| **Tiny Footprint** | Extremely lightweight, ideal for utility tools |
| **C++ Integration** | Compiles to native C++ code with zero runtime overhead |
| **Modern Design** | Fluent-style widgets that look native on Windows |

---

## Phase 0: Project Foundation & Setup

### 0.1: Repository Setup
- [x] Create directory structure (src, build, resources, docs, standards, prompts)
- [x] Create CMake project with C++23 configuration
- [x] Initialize Git repository
- [x] Create .gitignore for C++ projects
- [x] Create README.md with project overview
- [x] Set up GitHub repository
- [x] Set up issue templates
- [x] Create initial commit and push to GitHub

### 0.2: Build System Configuration
- [ ] Create CMakeLists.txt with Slint FetchContent integration
- [x] **Target Platform Requirements**:
  - [x] Set target OS to Windows 10 and Windows 11 only (no Windows 7/8 support)
  - [x] Configure build for 64-bit (x64) only (no 32-bit support - future standard)
  - [x] Set platform to x64 in all projects
  - [x] Update CMake to enforce Windows 10+ requirement
  - [x] Add platform validation on startup (check OS version, architecture)
- [x] Create build scripts (build.bat, build.ps1)
- [x] Configure build configurations (Debug, Release)
- [x] Set up single executable configuration
- [x] Configure extension projects to output to `system/` folder in app root
- [x] Set up CMake targets for extension compilation
- [x] Create solution structure for main app + extensions
- [x] Test Release compilation (64-bit only)
- [x] Verify standalone executable creation (64-bit)
- [x] Verify extension compilation output to system folder

### 0.3: Project Structure & Scaffolding
- [x] Create Core namespace structure (CplInterop, CplLoader, CplItem)
- [x] Create UI namespace structure (MainWindow)
- [ ] Create Resources folder structure
- [ ] Add application icon (Graphics/ExoSuite.ico)
- [ ] **Dual-Mode Extension Architecture** (Monorepo):
  - [ ] Design extensions to run both standalone and integrated into ExoSuite
  - [x] All extensions live in `extensions/` directory (monorepo approach)
  - [ ] Extensions link against shared libraries in `shared/`
  - [ ] Standalone mode: compile as independent executable (.exe)
  - [ ] Integrated mode: compile as plugin DLL for ExoSuite host
  - [ ] Create extension manifest format (JSON) for metadata and capabilities
  - [ ] See `docs/dev/extension-architecture.md` for details
- [ ] **Shared Library Infrastructure** (in `shared/`):
  - [ ] Create `ExoSuite.Core` shared library (common types, interfaces)
  - [ ] Create `ExoSuite.UI` shared library (WinUI3 common controls, themes)
  - [ ] Create `ExoSuite.Interop` shared library (Win32 API helpers)
  - [ ] Version shared libraries with semantic versioning
- [ ] **Extension System**:
  - [ ] Extensions built alongside main app in monorepo
  - [ ] Extension DLLs output to `system/` folder
  - [ ] Create extension template in `extensions/_template/`
  - [ ] CMake configuration for extension builds
  - [ ] Documentation and examples
- [ ] Create application manifest
- [ ] Set up version numbering system
- [ ] Create constants/configuration class
- [ ] Create Category enumeration and management system
- [ ] Set up localization/internationalization infrastructure
- [ ] Create theme system architecture (light/dark/system mode support, accent colors from Windows)
- [ ] Design performance monitoring infrastructure
- [ ] **Design UI with Slint**:
  - [ ] Set up Slint project structure with `.slint` files
  - [ ] Create `ui/main.slint` main window layout
  - [ ] Define UI components in `.slint` (ListView, sidebar, toolbar)
  - [ ] Design plugin host system for loading integrated extensions
  - [ ] Create extension discovery and registration system
  - [ ] Implement extension lifecycle management (load/unload/reload)

## Phase 1: Core Architecture

### 1.0: Platform & Architecture Requirements
- [ ] **Windows 10/11 Only**: Implement OS version checking
  - [ ] Check minimum Windows version (Windows 10 build 10240 or later)
  - [ ] Display friendly error message if running on unsupported OS
  - [ ] Block execution on Windows 7/8/8.1 (explicitly rejected with version-specific messages)
- [ ] **64-bit Only**: Enforce 64-bit architecture requirement
  - [ ] Check CPU architecture at runtime
  - [ ] Display friendly error message if running on 32-bit system
  - [ ] Configure all projects for x64 only
- [ ] Document platform requirements in README and installer

### 1.1: Error Handling System
- [ ] Create ErrorInfo class/struct
- [ ] Implement error logging system
- [ ] Create LogError function with debug output
- [ ] Create ShowError function for user-facing errors
- [ ] Add try-catch blocks throughout codebase
- [ ] Implement graceful error recovery

### 1.2: Configuration Management
- [ ] Create Settings class
- [ ] Implement settings file (JSON/YAML)
- [ ] Create LoadSettings function
- [ ] Create SaveSettings function
- [ ] Implement default settings
- [ ] Add settings for view mode, icon size, window position
- [ ] Implement portable mode detection (settings in root folder)
- [ ] Implement installed mode (settings in registry for faster access)
- [ ] Create SettingsProvider abstraction (file-based vs registry-based)
- [ ] Auto-detect mode on startup (check registry first, fallback to file)
- [ ] Implement settings export/import functionality
- [ ] Add settings backup and restore
- [ ] Create settings validation system
- [ ] Support settings migration between versions
- [ ] Implement settings synchronization (if cloud sync added later)

### 1.3: Versioning System
- [ ] Create VersionInfo class
- [ ] Implement semantic versioning (MAJOR.MINOR.PATCH)
- [ ] Add version display in About dialog
- [ ] Create version file generation
- [ ] Integrate version into build process

### 1.4: Exception Handling System
- [ ] Create global unhandled exception handler
- [ ] Implement ExceptionDialog with WinUI3 ContentDialog
- [ ] Add exception details viewer (expandable sections)
- [ ] Implement troubleshooting suggestions based on exception type
- [ ] Add copy-to-clipboard functionality for exception details
- [ ] Add option to save exception report to file
- [ ] Include stack trace, inner exceptions, and system info
- [ ] Handle structured exception handling (SEH)
- [ ] Create ExceptionReport class for structured exception data
- [ ] Implement crash reporting (optional, user-configurable)
- [ ] Add "Send Error Report" option with privacy notice
- [ ] Create exception analytics (aggregated, anonymous)

### 1.5: Performance Monitoring & Optimization
- [ ] Create PerformanceMonitor class
- [ ] Implement startup time tracking
- [ ] Monitor memory usage
- [ ] Track CPU usage during operations
- [ ] Measure applet loading times
- [ ] Create performance metrics dashboard (dev mode)
- [ ] Implement performance benchmarks
- [ ] Add slow operation warnings
- [ ] Optimize first-run experience
- [ ] Implement lazy loading strategies
- [ ] Create memory pool for frequently allocated objects

## Phase 2: CPL Interop Implementation

### 2.1: Complete Win32 API Definitions
- [ ] Define CPL message constants
- [ ] Define CPlApplet function pointer
- [ ] Define CPLINFO struct
- [ ] Define NEWCPLINFO struct
- [ ] Add LoadIcon/LoadImage definitions
- [ ] Add resource loading functions
- [ ] Test definitions with sample .cpl

### 2.2: CPL Marshaling Implementation
- [ ] Implement CPL_INQUIRE handling
- [ ] Implement CPL_NEWINQUIRE handling
- [ ] Handle both ANSI and Unicode paths
- [ ] Test with various .cpl files
- [ ] Add error handling for failures

### 2.3: Icon Resource Extraction
- [ ] Implement LoadIconFromResource function
- [ ] Extract icon from hModule using idIcon
- [ ] Handle multiple icon sizes (16, 24, 32, 48, 64, 128, 256)
- [ ] Create icon cache to prevent reloading
- [ ] Test icon extraction with sample .cpl files

### 2.4: String Resource Extraction
- [ ] Implement LoadStringFromResource function
- [ ] Extract name string using idName
- [ ] Extract description string using idInfo
- [ ] Handle both ANSI and Unicode strings
- [ ] Add fallback to file name if resource missing

### 2.5: Administrator Privilege Detection
- [ ] Implement function to detect if current process has admin privileges
- [ ] Implement function to detect if a CPL requires admin privileges
- [ ] Check CPL manifest or metadata for admin requirement
- [ ] Add `RequiresAdministrator` property to `CplItem` class
- [ ] Test privilege detection with various CPL files
- [ ] Handle privilege escalation gracefully (request elevation when needed)

## Phase 3: CPL Loader Implementation

### 3.1: Dynamic Library Loading
- [ ] Implement LoadCplFile method
- [ ] Use LoadLibraryEx for dynamic loading
- [ ] Find CPlApplet export with GetProcAddress
- [ ] Call CPL_INIT
- [ ] Call CPL_GETCOUNT
- [ ] Add proper error handling for loading failures
- [ ] Handle locked/unloadable .cpl files

### 3.2: Applet Enumeration
- [ ] Iterate through applets in each .cpl
- [ ] Implement CPL_INQUIRE for each applet
- [ ] Extract applet name and description
- [ ] Extract applet icon
- [ ] Create CplItem for each applet
- [ ] Test with multiple .cpl files

### 3.3: System Folder Management
- [ ] Create system folder if missing
- [ ] Scan for .cpl files
- [ ] Add file watching for new .cpl additions (ReadDirectoryChangesW)
- [ ] Handle .cpl file removal
- [ ] Add refresh functionality
- [ ] Create default .cpl collection documentation
- [ ] Implement category detection/assignment for CPL files
- [ ] Support category metadata in CPL files or external config
- [ ] **Detect admin requirements for each CPL**:
  - [ ] Query each CPL for admin requirement during enumeration
  - [ ] Store admin requirement flag in CplItem
  - [ ] Handle privilege detection failures gracefully

### 3.4: Category Management System
- [ ] Create Category enum/class (System, Network, Security, Display, Programs, etc.)
- [ ] Implement category assignment for CplItem
- [ ] Create category metadata storage (JSON/XML)
- [ ] Support custom categories
- [ ] Implement category icons for sidebar
- [ ] Add category filtering logic
- [ ] Persist user category preferences
- [ ] Support "Uncategorized" for applets without category

## Phase 4: User Interface Components (WinUI3)

### 4.1: Main Window Layout
- [ ] Create MainWindow with WinUI3 XAML
- [ ] Set window properties (title, size, icon)
- [ ] Add MenuBar (File, View, Help, Tools)
- [ ] Add CommandBar (view mode buttons)
- [ ] Add SplitView for sidebar and main content area
- [ ] Add Category Sidebar (TreeView or ListView)
- [ ] Implement category filtering (All, System, Network, Security, etc.)
- [ ] Add ListView/GridView control for applet display
- [ ] Implement sidebar collapse/expand functionality
- [ ] Implement window resize handling
- [ ] Add InfoBar for status information
- [ ] Design sidebar for scalability (100+ settings/utilities)
- [ ] Implement smooth animations with WinUI3 animations
- [ ] Add multi-monitor support (remember position per monitor)
- [ ] Support window minimizing to system tray
- [ ] Implement Mica/Acrylic material effects
- [ ] Add customizable window chrome with WinUI3 TitleBar

### 4.2: ListView/GridView Setup
- [ ] Configure GridView for Large Icon view
- [ ] Set up ImageSource for icons
- [ ] Add columns for Details view (Name, Category, Description)
- [ ] Implement item population from CplLoader
- [ ] Implement category-based filtering
- [ ] Add double-click/tap event handler
- [ ] Add right-click context menu (MenuFlyout)
- [ ] Update view when category selection changes
- [ ] Implement virtualization for performance (1000+ items)
- [ ] Add smooth scrolling
- [ ] Implement drag-and-drop support (reorder favorites)
- [ ] Add selection highlighting with smooth transitions
- [ ] Support multi-select mode
- [ ] Implement keyboard navigation

### 4.3: View Mode Implementation
- [ ] Implement Large Icons view (GridView)
- [ ] Implement Small Icons view
- [ ] Implement List view
- [ ] Implement Details view (ListView with GridView columns)
- [ ] Add view mode switching logic
- [ ] Persist view mode in settings
- [ ] Update CommandBar toggle states

### 4.4: Icon Display
- [ ] Create icon loading with WinUI3 ImageSource
- [ ] Populate icons from CplItem
- [ ] Assign icons to view items
- [ ] Handle missing icons gracefully
- [ ] Support icon size selection (16, 24, 32, 48)
- [ ] **Admin Privilege Shield Icon Overlay**:
  - [ ] Load Windows shield icon overlay
  - [ ] Create function to composite shield overlay on CPL icons
  - [ ] Overlay shield icon in bottom-right corner
  - [ ] Apply overlay only to CPL items that require admin privileges
  - [ ] Support shield overlay at all icon sizes
  - [ ] Test shield overlay rendering in all view modes

### 4.5: Menu System
- [ ] Implement File menu (Refresh, Exit)
- [ ] Implement View menu (Large Icons, Small Icons, List, Details)
- [ ] Implement Tools menu (Settings)
- [ ] Implement Help menu (About)
- [ ] Implement Tools menu with Troubleshooting options:
  - [ ] System File Checker (SFC /scannow)
  - [ ] Check Disk (CHKDSK)
  - [ ] Disk Cleanup
  - [ ] System Restore
  - [ ] Event Viewer
  - [ ] Task Manager
  - [ ] System Information
  - [ ] Separator
  - [ ] Run as Administrator (restart elevated)
- [ ] Implement Help menu with:
  - [ ] Check for Updates
  - [ ] Report an Issue (opens GitHub/issues URL)
  - [ ] Home Page (opens https://rizonesoft.com)
  - [ ] ExoSuite Page (opens project-specific URL)
  - [ ] Separator
  - [ ] About ExoSuite...
- [ ] Add keyboard shortcuts (KeyboardAccelerator)
- [ ] Implement menu state updates
- [ ] **Admin Privilege Handling**:
  - [ ] Detect current process admin status on startup
  - [ ] Display status indicator if running as admin vs non-admin
  - [ ] Handle UAC elevation requests gracefully
  - [ ] Provide option to restart as administrator when needed
  - [ ] Show informative tooltip for admin-required CPL items
  - [ ] Handle privilege escalation failures gracefully
  - [ ] Test both admin and non-admin scenarios
- [ ] Show ProgressRing for long-running tools

### 4.6: About Dialog
- [ ] Create AboutDialog with WinUI3 ContentDialog
- [ ] Add application icon/branding
- [ ] Display version information from VersionInfo
- [ ] Add ClassicPanel_alt.png image
- [ ] Include copyright information (Rizonetech (Pty) Ltd)
- [ ] Add developer credit (Derick Payne)
- [ ] Add website link (https://rizonesoft.com)
- [ ] Implement donate link/button
- [ ] Style with WinUI3 design language
- [ ] Make responsive to dialog sizing

### 4.7: Update Check System
- [ ] Create UpdateChecker class
- [ ] Implement XML-based version manifest download
- [ ] Parse version information from XML file
- [ ] Compare installed version with latest version
- [ ] Create update notification with ContentDialog
- [ ] Display update information (version, release notes, download link)
- [ ] Add "Check for Updates" menu item handler
- [ ] Implement background update check on startup (optional)
- [ ] Handle network errors gracefully
- [ ] Cache update check results
- [ ] Support checking from multiple sources (primary + mirror)

### 4.8: NavigationView Implementation
- [ ] Create NavigationView for ribbon-like experience
- [ ] Implement navigation items and groups
- [ ] Create command buttons
- [ ] Support SVG icon rendering (via Image/PathIcon)
- [ ] Add quick access toolbar area
- [ ] Style to match modern Windows appearance

### 4.9: SVG Icon Support
- [ ] Design SVG icon storage system
- [ ] Implement SVG path storage
- [ ] Create PathIcon/FontIcon-based rendering
- [ ] Implement icon caching for performance
- [ ] Support icon theming (light/dark mode)
- [ ] Add icon scaling support
- [ ] Integrate icon library with CommandBar

### 4.10: CommandBar Implementation
- [ ] Add view mode toggle buttons (AppBarToggleButton)
- [ ] Add refresh button
- [ ] Implement button state management
- [ ] Add tooltips (ToolTipService)
- [ ] Style CommandBar appropriately
- [ ] Use PathIcon/FontIcon from icon library
- [ ] Add theme toggle button (only visible when system theme disabled)
- [ ] Update icons on theme change

### 4.11: Context Menu
- [ ] Add "Open" option (MenuFlyout)
- [ ] Add view mode options
- [ ] Add separator
- [ ] Position menu correctly
- [ ] Handle keyboard navigation
- [ ] Add "Add to Favorites" option
- [ ] Add "Pin to Quick Access" option
- [ ] Add "Properties" option (show applet details)

### 4.12: Search & Filter System
- [ ] Create AutoSuggestBox in main window
- [ ] Implement real-time search filtering
- [ ] Support fuzzy search
- [ ] Add search suggestions/autocomplete
- [ ] Highlight search matches in results
- [ ] Save recent searches
- [ ] Add advanced search filters:
  - [ ] Filter by category
  - [ ] Filter by keyword tags
  - [ ] Filter by recently used
  - [ ] Filter by favorites only
- [ ] Implement search keyboard shortcut (Ctrl+F)
- [ ] Add clear search button
- [ ] Show search result count

### 4.13: Favorites & Quick Access
- [ ] Create Favorites system
- [ ] Add "Favorites" category to sidebar
- [ ] Implement add/remove from favorites
- [ ] Persist favorites in settings
- [ ] Create Quick Access widget
- [ ] Support pinned items (always visible)
- [ ] Implement drag-and-drop to favorites
- [ ] Add favorites badge/indicator
- [ ] Support favorites groups/categories
- [ ] Export/import favorites

### 4.14: Recent Items & History
- [ ] Track recently opened applets
- [ ] Display "Recently Used" category
- [ ] Show usage frequency
- [ ] Implement history persistence
- [ ] Add "Clear History" option
- [ ] Limit history size (configurable)
- [ ] Show last used timestamp

### 4.15: Command Palette / Quick Actions
- [ ] Implement command palette (Ctrl+K or Ctrl+Shift+P)
- [ ] Quick access to all applets via typing
- [ ] Support keyboard shortcuts for common actions
- [ ] Add command suggestions
- [ ] Implement fuzzy matching for commands
- [ ] Support command aliases
- [ ] Customizable keyboard shortcuts

### 4.16: Themes & Customization
- [ ] Leverage WinUI3 theme system
- [ ] Implement light theme (default)
- [ ] Implement dark theme
- [ ] Implement system mode (follows Windows theme preference)
- [ ] Use Windows accent colors (ThemeResources)
- [ ] Persist theme preference
- [ ] Auto-detect Windows theme preference
- [ ] Implement smooth theme transitions

### 4.17: Animations & Transitions
- [ ] Implement smooth fade-in/fade-out with WinUI3 animations
- [ ] Add slide transitions between views (ConnectedAnimations)
- [ ] Smooth icon loading animations
- [ ] Hover effects on interactive elements
- [ ] Smooth category switching animations
- [ ] Loading skeleton screens (Shimmer)
- [ ] ProgressRing/ProgressBar with animations
- [ ] Minimize animation performance impact
- [ ] Respect user preference for reduced motion (UISettings)

### 4.18: Accessibility Features
- [ ] Implement keyboard navigation throughout UI
- [ ] Add screen reader support (AutomationProperties)
- [ ] Support high contrast mode
- [ ] Implement font scaling
- [ ] Add colorblind-friendly color schemes
- [ ] Support Windows Narrator
- [ ] Keyboard shortcuts for all actions
- [ ] Focus indicators for keyboard navigation
- [ ] Respect Windows accessibility settings
- [ ] Add accessibility settings panel

## Phase 5: Core Extensions/Utilities Implementation

> **Extension Architecture**: Each extension below is a separate GitHub repository that can be installed into ExoSuite (using shared libraries) or run as a standalone application.

### 5.0: Extension Infrastructure Setup
- [ ] Create `rizonesoft/ExoSuite.Core` repository (shared library)
- [ ] Create `rizonesoft/ExoSuite.UI` repository (shared UI components)
- [ ] Create `rizonesoft/ExoSuite.ExtensionTemplate` repository
- [ ] Implement extension host in ExoSuite main application
- [ ] Create extension installer/manager UI
- [ ] Document extension development workflow

### 5.1: System Properties Utility (System Category)
- [ ] Create `rizonesoft/ExoSuite.SystemProperties` repository
- [ ] **Standalone Mode**:
  - [ ] Build as `SystemProperties.exe` standalone application
  - [ ] Include all dependencies (self-contained)
  - [ ] Create standalone installer (optional)
- [ ] **Integrated Mode**:
  - [ ] Build as `SystemProperties.dll` plugin
  - [ ] Link against `ExoSuite.Core` and `ExoSuite.UI` shared libraries
  - [ ] Register as ExoSuite extension via manifest
- [ ] Design System Properties dialog layout (Windows 7/10 style)
- [ ] Create main System Properties window with tabs:
  - [ ] Computer Name tab
  - [ ] Hardware tab
  - [ ] Advanced tab
  - [ ] System Protection tab
  - [ ] Remote tab
- [ ] Use Graphics/Control/System/System.ico as applet icon
- [ ] Assign to "System" category
- [ ] **Implement with WinUI3 and C++/WinRT**

### 5.2: Computer Name Tab
- [ ] Display current computer name
- [ ] Display current workgroup/domain
- [ ] Implement "Change" button for computer name
- [ ] Implement "Network ID" button
- [ ] Create computer name change dialog
- [ ] Validate computer name input
- [ ] Handle workgroup/domain changes
- [ ] Request administrator privileges when needed
- [ ] Show pending restart warning if required

### 5.3: Hardware Tab
- [ ] Display hardware information:
  - [ ] Device Manager link
  - [ ] Driver signing options
  - [ ] Hardware profiles
- [ ] Implement Device Manager launch
- [ ] Create hardware profiles management UI
- [ ] Display hardware resource information

### 5.4: Advanced Tab
- [ ] Performance section:
  - [ ] Performance Options button
  - [ ] Visual effects settings
  - [ ] Virtual memory settings
- [ ] User Profiles section:
  - [ ] Display user profiles list
  - [ ] Delete profile functionality
  - [ ] Copy profile functionality
- [ ] Startup and Recovery section:
  - [ ] System startup options
  - [ ] System failure options
  - [ ] Memory dump settings
- [ ] Environment Variables button
- [ ] Create Performance Options dialog
- [ ] Create Environment Variables dialog

### 5.5: System Protection Tab
- [ ] Display available drives for System Restore
- [ ] Enable/disable System Restore per drive
- [ ] Configure System Restore settings
- [ ] Create Restore Point button
- [ ] System Restore button (launch restore wizard)
- [ ] Display restore point usage information
- [ ] Configure restore point space allocation

### 5.6: Remote Tab
- [ ] Remote Assistance section:
  - [ ] Enable/disable Remote Assistance
  - [ ] Configure Remote Assistance settings
- [ ] Remote Desktop section:
  - [ ] Enable/disable Remote Desktop
  - [ ] Configure Remote Desktop options
  - [ ] Select users allowed for Remote Desktop
- [ ] Create remote settings dialogs

### 5.7: System Information Integration
- [ ] Display Windows edition and version
- [ ] Display processor information
- [ ] Display installed memory (RAM)
- [ ] Display system type (32-bit/64-bit)
- [ ] Display computer name and domain
- [ ] Display product ID and activation status
- [ ] Integrate Windows Management Instrumentation (WMI) via C++
- [ ] Create SystemInformation class for data retrieval

### 5.8: System Properties Win32 API
- [ ] Implement Windows API calls for system changes
- [ ] Handle SetComputerName/SetComputerNameEx for name changes
- [ ] Implement registry modifications for system settings
- [ ] Handle UAC elevation for privileged operations
- [ ] Add error handling for system operations
- [ ] Create SystemPropertiesInterop class for Windows API calls

## Phase 6-10: Extensions
(Same extension items as before, all marked as incomplete, implemented with C++/WinRT and WinUI3)

## Phase 11: Applet Execution (Core Functionality)

### 11.0: Administrator Privilege Management
- [ ] **Check Admin Requirements Before Execution**:
  - [ ] Verify if selected CPL requires admin privileges
  - [ ] Check if current process has admin privileges
  - [ ] Compare requirements vs current privileges
- [ ] **Graceful Privilege Handling**:
  - [ ] If admin required but not running as admin:
    - [ ] Display user-friendly dialog explaining need for elevation
    - [ ] Offer option to restart ExoSuite as administrator
    - [ ] Offer option to launch CPL directly with elevation (via rundll32)
    - [ ] Provide "Cancel" option
    - [ ] Remember user preference for similar items
  - [ ] If admin required and already running as admin:
    - [ ] Execute CPL normally
  - [ ] If no admin required:
    - [ ] Execute CPL normally regardless of privilege level
- [ ] **Error Handling**:
  - [ ] Handle UAC cancellation gracefully
  - [ ] Handle privilege escalation failures
  - [ ] Display clear error messages
  - [ ] Log privilege-related issues for debugging
- [ ] **User Experience**:
  - [ ] Show shield icon overlay on admin-required items
  - [ ] Add tooltip explaining admin requirement
  - [ ] Update context menu to indicate admin requirement
  - [ ] Provide keyboard shortcut to restart as admin

### 11.1: Double-Click Handling
- [ ] Implement item double-click/tap event
- [ ] Get selected CplItem
- [ ] Check admin requirements before execution
- [ ] Execute applet using rundll32.exe (for external .cpl)
- [ ] Execute internal applets directly
- [ ] Handle execution errors
- [ ] Show error message if execution fails

### 11.2: Execution Methods
- [ ] Implement rundll32.exe method (Control_RunDLL) for external .cpl
- [ ] Pass correct .cpl path and index
- [ ] Implement direct execution for internal applets
- [ ] Check admin requirements before execution
- [ ] Handle UAC elevation if needed
- [ ] Launch processes with appropriate privilege level (CreateProcess/ShellExecute)
- [ ] Monitor process execution
- [ ] Add alternative execution methods if needed

### 11.3: Error Handling for Execution
- [ ] Catch execution failures
- [ ] Display user-friendly error messages
- [ ] Log execution errors
- [ ] Handle missing .cpl files
- [ ] Handle permission denied errors

## Phase 12-18: Polish, Distribution & Release
(Same items as before, all marked as incomplete, adapted for C++ build system and WinUI3)

## Notes

- **Tech Stack**: C++23, Slint UI Framework - No .NET Framework Required
- Each phase should be completed before moving to the next
- After completing each task, build, test, commit, and push
- Keep documentation updated throughout development
- Follow coding standards defined in `standards/` folder
- Use prompt templates from `prompts/` folder for AI assistance

## Resources

- [Slint Documentation](https://slint.dev/docs)
- [Slint C++ Tutorial](https://slint.dev/docs/tutorial/cpp)
- [Slint GitHub](https://github.com/slint-ui/slint)
- [VS Code Extension](https://marketplace.visualstudio.com/items?itemName=Slint.slint)
