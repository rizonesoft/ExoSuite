// ==============================================================================
// ExoSuite - Modern Windows Control Panel
// ==============================================================================
// Entry point for the ExoSuite application.
// ==============================================================================

#include "Core/Platform.h"
#include <Windows.h>

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Validate platform requirements (Windows 10+ x64)
    if (!ExoSuite::EnsurePlatformRequirements()) {
        return 1;
    }

    // TODO: Initialize WinUI3 application
    // TODO: Create main window
    // TODO: Run message loop

    MessageBoxW(nullptr, 
        L"ExoSuite is under development.\n\nPlatform validation passed!",
        L"ExoSuite", 
        MB_OK | MB_ICONINFORMATION);

    return 0;
}
