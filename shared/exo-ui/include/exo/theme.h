#pragma once
// ── ExoUI Theme System ──────────────────────────────────────

#include <windows.h>
#include <dwmapi.h>
#include <cstdint>
#include "export.h"

namespace exo {

struct ColorPalette {
    COLORREF background;
    COLORREF surface;
    COLORREF surfaceHover;
    COLORREF surfaceActive;
    COLORREF toolbar;
    COLORREF text;
    COLORREF textSecondary;
    COLORREF border;
    COLORREF accent;
    COLORREF statusBar;
    COLORREF statusBarText;
};

EXOUI_API extern const ColorPalette DarkPalette;
EXOUI_API extern const ColorPalette LightPalette;

class EXOUI_API Theme {
public:
    static bool IsDarkMode();
    static const ColorPalette& Colors();
    static bool IsDark();
    static void Init();
    static void Toggle();
    static void SetDark(bool dark);
    static void ApplyToWindow(HWND hwnd);
    static uint32_t IconColor();
    static uint32_t SecondaryIconColor();

private:
    static bool s_dark;
};

} // namespace exo
