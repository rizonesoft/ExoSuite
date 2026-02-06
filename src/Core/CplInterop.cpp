// ==============================================================================
// ExoSuite Core - CPL Interop Implementation
// ==============================================================================

#include "CplInterop.h"
#include <ShlObj.h>

namespace ExoSuite::Core {

std::wstring GetSystemCplDirectory() {
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_SYSTEM, nullptr, 0, path))) {
        return std::wstring(path);
    }
    return L"C:\\Windows\\System32";
}

std::wstring GetLocalSystemDirectory() {
    WCHAR modulePath[MAX_PATH];
    GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
    
    std::wstring path(modulePath);
    auto lastSlash = path.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        path = path.substr(0, lastSlash);
    }
    
    return path + L"\\system";
}

} // namespace ExoSuite::Core
