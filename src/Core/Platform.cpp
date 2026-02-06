// ==============================================================================
// ExoSuite Platform Validation Implementation
// ==============================================================================

#include "Platform.h"
#include <VersionHelpers.h>

namespace ExoSuite {

PlatformInfo ValidatePlatform() {
    PlatformInfo info{};
    info.isValid = true;
    
    // Check 64-bit architecture
#ifdef _WIN64
    info.is64Bit = true;
#else
    info.is64Bit = false;
    info.isValid = false;
    info.errorMessage = L"ExoSuite requires a 64-bit version of Windows.";
#endif

    // Check Windows version using RtlGetVersion (more accurate than GetVersionEx)
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        using RtlGetVersionFunc = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
        auto RtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(
            GetProcAddress(ntdll, "RtlGetVersion"));
        
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW osvi{};
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            
            if (RtlGetVersion(&osvi) == 0) {
                info.buildNumber = osvi.dwBuildNumber;
                
                // Windows 10 is version 10.0, build 10240+
                info.isWindows10OrLater = (osvi.dwMajorVersion >= 10);
                
                if (!info.isWindows10OrLater) {
                    info.isValid = false;
                    info.errorMessage = L"ExoSuite requires Windows 10 or later.\n\n"
                        L"Detected: Windows " + std::to_wstring(osvi.dwMajorVersion) +
                        L"." + std::to_wstring(osvi.dwMinorVersion) +
                        L" (Build " + std::to_wstring(osvi.dwBuildNumber) + L")";
                }
            }
        }
    }
    
    // Fallback check using VersionHelpers
    if (info.buildNumber == 0) {
        info.isWindows10OrLater = IsWindows10OrGreater();
        if (!info.isWindows10OrLater) {
            info.isValid = false;
            info.errorMessage = L"ExoSuite requires Windows 10 or later.";
        }
    }
    
    return info;
}

bool EnsurePlatformRequirements() {
    PlatformInfo info = ValidatePlatform();
    
    if (!info.isValid) {
        MessageBoxW(
            nullptr,
            info.errorMessage.c_str(),
            L"ExoSuite - System Requirements",
            MB_OK | MB_ICONERROR
        );
        return false;
    }
    
    return true;
}

} // namespace ExoSuite
