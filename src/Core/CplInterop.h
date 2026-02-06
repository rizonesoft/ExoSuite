#pragma once
// ==============================================================================
// ExoSuite Core - CPL Interop Definitions
// ==============================================================================
// Win32 API definitions for Control Panel applet interoperability.
// ==============================================================================

#include <Windows.h>
#include <string>

namespace ExoSuite::Core {

// ==============================================================================
// CPL Message Constants
// ==============================================================================

constexpr UINT CPL_INIT = 1;
constexpr UINT CPL_GETCOUNT = 2;
constexpr UINT CPL_INQUIRE = 3;
constexpr UINT CPL_SELECT = 4;
constexpr UINT CPL_DBLCLK = 5;
constexpr UINT CPL_STOP = 6;
constexpr UINT CPL_EXIT = 7;
constexpr UINT CPL_NEWINQUIRE = 8;
constexpr UINT CPL_STARTWPARMS = 9;
constexpr UINT CPL_SETUP = 200;

// ==============================================================================
// CPL Structures
// ==============================================================================

/// Legacy CPL information structure (ANSI)
struct CPLINFO {
    int idIcon;         // Icon resource ID
    int idName;         // Name string resource ID
    int idInfo;         // Description string resource ID
    LONG_PTR lData;     // Application-defined data
};

/// Extended CPL information structure (Unicode)
struct NEWCPLINFO {
    DWORD dwSize;       // Size of this structure
    DWORD dwFlags;      // Flags (reserved)
    DWORD dwHelpContext;// Help context ID
    LONG_PTR lData;     // Application-defined data
    HICON hIcon;        // Icon handle
    WCHAR szName[32];   // Applet name
    WCHAR szInfo[64];   // Applet description
    WCHAR szHelpFile[128]; // Help file path
};

// ==============================================================================
// CPL Function Pointer
// ==============================================================================

/// CPlApplet function signature
using CPlAppletFunc = LONG(CALLBACK*)(HWND hwndCPl, UINT msg, LPARAM lParam1, LPARAM lParam2);

// ==============================================================================
// Helper Functions
// ==============================================================================

/// Get the system Control Panel directory (System32)
std::wstring GetSystemCplDirectory();

/// Get the local ExoSuite system directory for extensions
std::wstring GetLocalSystemDirectory();

} // namespace ExoSuite::Core
