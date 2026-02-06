// ==============================================================================
// ExoSuite Core - CPL Loader Implementation
// ==============================================================================

#include "CplLoader.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace ExoSuite::Core {

bool CplLoader::LoadSystemCpls() {
    std::wstring systemDir = GetSystemCplDirectory();
    
    try {
        std::vector<fs::path> cplFiles;
        
        // Enumerate .cpl files in System32
        for (const auto& entry : fs::directory_iterator(systemDir)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().wstring();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
                if (ext == L".cpl") {
                    cplFiles.push_back(entry.path());
                }
            }
        }
        
        int loaded = 0;
        int total = static_cast<int>(cplFiles.size());
        
        for (const auto& cplPath : cplFiles) {
            if (m_progressCallback) {
                m_progressCallback(cplPath.filename().wstring(), loaded, total);
            }
            
            LoadCplFile(cplPath.wstring());
            loaded++;
        }
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool CplLoader::LoadExtensions() {
    std::wstring extDir = GetLocalSystemDirectory();
    
    try {
        if (!fs::exists(extDir)) {
            return true; // No extensions directory yet
        }
        
        for (const auto& entry : fs::directory_iterator(extDir)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().wstring();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
                if (ext == L".dll" || ext == L".exe") {
                    // TODO: Load extension manifest and create CplItem
                    auto item = std::make_unique<CplItem>();
                    item->SetFilePath(entry.path().wstring());
                    item->SetName(entry.path().stem().wstring());
                    item->SetIsExtension(true);
                    m_items.push_back(std::move(item));
                }
            }
        }
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool CplLoader::LoadCplFile(const std::wstring& filePath) {
    // Load the CPL as a data file first (don't execute DllMain)
    HMODULE hModule = LoadLibraryExW(filePath.c_str(), nullptr, 
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    
    if (!hModule) {
        return false;
    }
    
    // Now load it properly to get the CPlApplet function
    HMODULE hModuleExec = LoadLibraryExW(filePath.c_str(), nullptr, 0);
    if (!hModuleExec) {
        FreeLibrary(hModule);
        return false;
    }
    
    auto appletFunc = reinterpret_cast<CPlAppletFunc>(GetProcAddress(hModuleExec, "CPlApplet"));
    if (!appletFunc) {
        FreeLibrary(hModuleExec);
        FreeLibrary(hModule);
        return false;
    }
    
    bool result = ExtractAppletInfo(hModule, filePath, appletFunc);
    
    FreeLibrary(hModuleExec);
    FreeLibrary(hModule);
    
    return result;
}

bool CplLoader::Refresh() {
    m_items.clear();
    
    bool sysResult = LoadSystemCpls();
    bool extResult = LoadExtensions();
    
    return sysResult || extResult;
}

std::vector<CplItem*> CplLoader::GetItemsByCategory(CplCategory category) const {
    std::vector<CplItem*> result;
    for (const auto& item : m_items) {
        if (item->Category() == category) {
            result.push_back(item.get());
        }
    }
    return result;
}

CplItem* CplLoader::FindByName(const std::wstring& name) const {
    for (const auto& item : m_items) {
        if (_wcsicmp(item->Name().c_str(), name.c_str()) == 0) {
            return item.get();
        }
    }
    return nullptr;
}

bool CplLoader::ExtractAppletInfo(HMODULE hModule, const std::wstring& filePath, CPlAppletFunc appletFunc) {
    // Initialize the CPL
    if (appletFunc(nullptr, CPL_INIT, 0, 0) == 0) {
        return false;
    }
    
    // Get applet count
    LONG count = appletFunc(nullptr, CPL_GETCOUNT, 0, 0);
    if (count <= 0) {
        appletFunc(nullptr, CPL_EXIT, 0, 0);
        return false;
    }
    
    // Query each applet
    for (LONG i = 0; i < count; i++) {
        auto item = std::make_unique<CplItem>();
        item->SetFilePath(filePath);
        item->SetIndex(static_cast<int>(i));
        
        // Try NEWINQUIRE first (Unicode)
        NEWCPLINFO newInfo = { sizeof(NEWCPLINFO) };
        appletFunc(nullptr, CPL_NEWINQUIRE, i, reinterpret_cast<LPARAM>(&newInfo));
        
        if (newInfo.szName[0] != L'\0') {
            item->SetName(newInfo.szName);
            item->SetDescription(newInfo.szInfo);
            if (newInfo.hIcon) {
                item->SetIcon(CopyIcon(newInfo.hIcon));
            }
        }
        else {
            // Fall back to INQUIRE (resource IDs)
            CPLINFO info = {};
            appletFunc(nullptr, CPL_INQUIRE, i, reinterpret_cast<LPARAM>(&info));
            
            item->SetName(LoadCplString(hModule, info.idName));
            item->SetDescription(LoadCplString(hModule, info.idInfo));
            item->SetIcon(LoadCplIcon(hModule, info.idIcon));
        }
        
        // Skip items with no name
        if (!item->Name().empty()) {
            m_items.push_back(std::move(item));
        }
    }
    
    // Cleanup
    appletFunc(nullptr, CPL_EXIT, 0, 0);
    
    return true;
}

HICON CplLoader::LoadCplIcon(HMODULE hModule, int iconId, int size) {
    return static_cast<HICON>(LoadImageW(
        hModule, 
        MAKEINTRESOURCEW(iconId),
        IMAGE_ICON, 
        size, size, 
        LR_DEFAULTCOLOR));
}

std::wstring CplLoader::LoadCplString(HMODULE hModule, int stringId) {
    WCHAR buffer[256] = {};
    int len = LoadStringW(hModule, stringId, buffer, 256);
    if (len > 0) {
        return std::wstring(buffer, len);
    }
    return L"";
}

} // namespace ExoSuite::Core
