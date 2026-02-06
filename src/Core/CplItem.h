#pragma once
// ==============================================================================
// ExoSuite Core - CPL Item
// ==============================================================================
// Represents a single Control Panel applet item with its metadata.
// ==============================================================================

#include <Windows.h>
#include <string>
#include <memory>

namespace ExoSuite::Core {

/// Category for organizing CPL items
enum class CplCategory {
    Uncategorized = 0,
    System,
    Network,
    Security,
    Display,
    Hardware,
    Programs,
    UserAccounts,
    DateTime,
    Accessibility,
    Sound,
    Power,
    Custom
};

/// Represents a single Control Panel applet
class CplItem {
public:
    CplItem() = default;
    ~CplItem() = default;

    // Non-copyable, movable
    CplItem(const CplItem&) = delete;
    CplItem& operator=(const CplItem&) = delete;
    CplItem(CplItem&&) = default;
    CplItem& operator=(CplItem&&) = default;

    // ==============================================================================
    // Properties
    // ==============================================================================

    /// Display name of the applet
    const std::wstring& Name() const { return m_name; }
    void SetName(const std::wstring& name) { m_name = name; }

    /// Description/info text
    const std::wstring& Description() const { return m_description; }
    void SetDescription(const std::wstring& desc) { m_description = desc; }

    /// Full path to the .cpl file
    const std::wstring& FilePath() const { return m_filePath; }
    void SetFilePath(const std::wstring& path) { m_filePath = path; }

    /// Index within the .cpl file (for multi-applet CPLs)
    int Index() const { return m_index; }
    void SetIndex(int index) { m_index = index; }

    /// Category for grouping
    CplCategory Category() const { return m_category; }
    void SetCategory(CplCategory cat) { m_category = cat; }

    /// Icon handle (owned by this object)
    HICON Icon() const { return m_icon; }
    void SetIcon(HICON icon);

    /// Whether this applet requires administrator privileges
    bool RequiresAdmin() const { return m_requiresAdmin; }
    void SetRequiresAdmin(bool value) { m_requiresAdmin = value; }

    /// Whether this is a built-in ExoSuite extension vs. system CPL
    bool IsExtension() const { return m_isExtension; }
    void SetIsExtension(bool isExt) { m_isExtension = isExt; }

    // ==============================================================================
    // Methods
    // ==============================================================================

    /// Execute this applet
    bool Execute(HWND hwndParent = nullptr) const;

private:
    std::wstring m_name;
    std::wstring m_description;
    std::wstring m_filePath;
    int m_index = 0;
    CplCategory m_category = CplCategory::Uncategorized;
    HICON m_icon = nullptr;
    bool m_requiresAdmin = false;
    bool m_isExtension = false;
};

} // namespace ExoSuite::Core
