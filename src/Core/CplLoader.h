#pragma once
// ==============================================================================
// ExoSuite Core - CPL Loader
// ==============================================================================
// Loads and enumerates Control Panel applets from .cpl files.
// ==============================================================================

#include "CplItem.h"
#include "CplInterop.h"
#include <vector>
#include <memory>
#include <functional>

namespace ExoSuite::Core {

/// Callback for progress updates during loading
using LoadProgressCallback = std::function<void(const std::wstring& currentFile, int loaded, int total)>;

/// Loads and manages Control Panel applets
class CplLoader {
public:
    CplLoader() = default;
    ~CplLoader() = default;

    // Non-copyable
    CplLoader(const CplLoader&) = delete;
    CplLoader& operator=(const CplLoader&) = delete;

    // ==============================================================================
    // Loading Methods
    // ==============================================================================

    /// Load all CPL files from the system directory
    bool LoadSystemCpls();

    /// Load all extensions from the local system directory
    bool LoadExtensions();

    /// Load a single CPL file
    bool LoadCplFile(const std::wstring& filePath);

    /// Refresh the list (reload all)
    bool Refresh();

    // ==============================================================================
    // Access Methods
    // ==============================================================================

    /// Get all loaded items
    const std::vector<std::unique_ptr<CplItem>>& Items() const { return m_items; }

    /// Get items by category
    std::vector<CplItem*> GetItemsByCategory(CplCategory category) const;

    /// Find item by name (case-insensitive)
    CplItem* FindByName(const std::wstring& name) const;

    /// Get item count
    size_t Count() const { return m_items.size(); }

    // ==============================================================================
    // Callbacks
    // ==============================================================================

    /// Set progress callback for loading operations
    void SetProgressCallback(LoadProgressCallback callback) { m_progressCallback = std::move(callback); }

private:
    /// Extract applet information from a loaded CPL module
    bool ExtractAppletInfo(HMODULE hModule, const std::wstring& filePath, CPlAppletFunc appletFunc);

    /// Load icon from CPL resource
    HICON LoadCplIcon(HMODULE hModule, int iconId, int size = 32);

    /// Load string from CPL resource
    std::wstring LoadCplString(HMODULE hModule, int stringId);

    std::vector<std::unique_ptr<CplItem>> m_items;
    LoadProgressCallback m_progressCallback;
};

} // namespace ExoSuite::Core
