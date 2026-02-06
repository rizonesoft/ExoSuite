// ==============================================================================
// ExoSuite Core - CPL Item Implementation
// ==============================================================================

#include "CplItem.h"
#include <shellapi.h>

namespace ExoSuite::Core {

void CplItem::SetIcon(HICON icon) {
    // Destroy previous icon if we own it
    if (m_icon && m_icon != icon) {
        DestroyIcon(m_icon);
    }
    m_icon = icon;
}

bool CplItem::Execute(HWND hwndParent) const {
    if (m_filePath.empty()) {
        return false;
    }

    // Build command line for rundll32
    std::wstring cmdLine;
    
    if (m_isExtension) {
        // For extensions, execute directly
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.hwnd = hwndParent;
        sei.lpVerb = m_requiresAdmin ? L"runas" : L"open";
        sei.lpFile = m_filePath.c_str();
        sei.nShow = SW_SHOWNORMAL;
        
        return ShellExecuteExW(&sei) != FALSE;
    }
    else {
        // For system CPL files, use rundll32
        cmdLine = L"rundll32.exe shell32.dll,Control_RunDLL \"" + m_filePath + L"\"";
        
        if (m_index > 0) {
            cmdLine += L",,@" + std::to_wstring(m_index);
        }
        
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.hwnd = hwndParent;
        sei.lpVerb = m_requiresAdmin ? L"runas" : L"open";
        sei.lpFile = L"cmd.exe";
        sei.lpParameters = (L"/c " + cmdLine).c_str();
        sei.nShow = SW_HIDE;
        
        // Actually use ShellExecute directly on rundll32
        sei.lpFile = L"rundll32.exe";
        sei.lpParameters = (L"shell32.dll,Control_RunDLL \"" + m_filePath + L"\"").c_str();
        sei.nShow = SW_SHOWNORMAL;
        
        return ShellExecuteExW(&sei) != FALSE;
    }
}

} // namespace ExoSuite::Core
