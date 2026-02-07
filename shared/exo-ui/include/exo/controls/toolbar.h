#pragma once
// ── ExoUI Toolbar Control ───────────────────────────────────

#include <windows.h>
#include <wrl/client.h>
#include <d2d1.h>
#include "../export.h"
#include "../dpi.h"
#include "../theme.h"
#include "../render.h"

using Microsoft::WRL::ComPtr;

namespace exo {

enum ToolbarCmd : WORD {
    IDC_TB_VIEW_LARGE   = 1001,
    IDC_TB_VIEW_SMALL   = 1002,
    IDC_TB_VIEW_LIST    = 1003,
    IDC_TB_VIEW_DETAILS = 1004,
    IDC_TB_REFRESH      = 1010,
    IDC_TB_SETTINGS     = 1020,
    IDC_TB_THEME        = 1030,
};

class EXOUI_API Toolbar {
public:
    static constexpr int BASE_HEIGHT    = 40;
    static constexpr int BASE_FONT_SIZE = 13;
    static constexpr int BASE_ICON_SIZE = 16;

    int ScaledHeight() const;
    void Create(HWND parent, HINSTANCE hInst, int id);
    HWND Handle() const;
    void Resize(int x, int y, int w, int h);
    void Repaint();
    void UpdateDpi(int dpi);

private:
    struct Button {
        WORD            id;
        const wchar_t*  label;
        const char*     iconName;
        int             baseWidth;
        bool            toggle;
        bool            rightAlign;
    };

    static constexpr Button kButtons[] = {
        { IDC_TB_VIEW_LARGE,   L"Large",   "view-large-icons",  52, true,  false },
        { IDC_TB_VIEW_SMALL,   L"Small",   "view-small-icons",  50, true,  false },
        { IDC_TB_VIEW_LIST,    L"List",    "view-list",          42, true,  false },
        { IDC_TB_VIEW_DETAILS, L"Details", "view-details",       58, true,  false },
        { IDC_TB_REFRESH,      L"",        "refresh",            36, false, false },
        { IDC_TB_SETTINGS,     L"",        "settings",           36, false, true  },
        { IDC_TB_THEME,        L"",        "theme-dark",         36, false, true  },
    };
    static constexpr int kButtonCount = _countof(kButtons);

    HWND  m_hwnd       = nullptr;
    HWND  m_parent     = nullptr;
    WORD  m_activeView = IDC_TB_VIEW_LARGE;
    int   m_hovered    = -1;
    int   m_dpi        = 96;
    ComPtr<ID2D1HwndRenderTarget> m_rt;

    int BtnWidth(int idx) const;
    int Pad() const;
    int Margin() const;
    int Height() const;
    void CreateRenderTarget();
    D2D1_RECT_F ButtonRect(int idx, float totalWidth) const;
    int HitTest(int mx, int my, float totalWidth);
    void OnPaint();

    static LRESULT CALLBACK ToolbarProc(HWND, UINT, WPARAM, LPARAM);
};

} // namespace exo
