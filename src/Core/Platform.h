#pragma once
// ==============================================================================
// ExoSuite Platform Validation
// ==============================================================================
// Runtime checks for Windows 10+ and 64-bit architecture requirements.
// Call ValidatePlatform() at application startup before any UI initialization.
// ==============================================================================

#include <Windows.h>
#include <string>

namespace ExoSuite {

/// Platform validation result
struct PlatformInfo {
    bool isValid;
    bool isWindows10OrLater;
    bool is64Bit;
    DWORD buildNumber;
    std::wstring errorMessage;
};

/// Validate that the platform meets ExoSuite requirements.
/// Returns PlatformInfo with validation results.
/// If isValid is false, errorMessage contains a user-friendly description.
PlatformInfo ValidatePlatform();

/// Show error dialog and exit if platform requirements not met.
/// Call this at application startup.
/// Returns true if platform is valid, false if application should exit.
bool EnsurePlatformRequirements();

} // namespace ExoSuite
