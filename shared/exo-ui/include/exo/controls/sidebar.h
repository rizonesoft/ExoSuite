#pragma once
// ── ExoUI Sidebar Control ───────────────────────────────────

#include <windows.h>
#include <wrl/client.h>
#include <d2d1.h>
#include "../export.h"
#include "../dpi.h"
#include "../theme.h"
#include "../render.h"
#include "../icons.h"

using Microsoft::WRL::ComPtr;

namespace exo {

struct SidebarItem {
    const wchar_t* label;
    const char*    iconName;
};

inline constexpr SidebarItem kCategories[] = {
    { L"All",        "view-list"  },
    { L"System",     "system"     },
    { L"Network",    "network"    },
    { L"Security",   "security"   },
    { L"Display",    "display"    },
    { L"Programs",   "programs"   },
};
inline constexpr int kCategoryCount = _countof(kCategories);

class EXOUI_API Sidebar {
public:
    static constexpr int BASE_WIDTH       = 200;
    static constexpr int BASE_ITEM_HEIGHT = 40;
    static constexpr int BASE_PADDING_X   = 16;
    static constexpr int BASE_FONT_SIZE   = 14;
    static constexpr int BASE_HEADER_FONT = 11;
    static constexpr int BASE_ICON_SIZE   = 18;

    int ScaledWidth() const;
    void Create(HWND parent, HINSTANCE hInst, int id);
    HWND Handle() const;
    int  Selected() const;
    void Resize(int x, int y, int w, int h);
    void Repaint();
    void UpdateDpi(int dpi);

private:
    HWND m_hwnd     = nullptr;
    HWND m_parent   = nullptr;
    int  m_selected = 0;
    int  m_hovered  = -1;
    int  m_dpi      = 96;
    ComPtr<ID2D1HwndRenderTarget> m_rt;
    ComPtr<ID2D1Bitmap> m_iconBitmaps[kCategoryCount];
    int      m_cachedIconSize  = 0;
    uint32_t m_cachedIconColor = 0;

    int ItemHeight() const;
    int PaddingX() const;
    int IconSize() const;
    void CreateRenderTarget();
    void RebuildIconCache();
    void OnPaint();
    int  HitTest(int y);

    static LRESULT CALLBACK SidebarProc(HWND, UINT, WPARAM, LPARAM);
};

} // namespace exo
