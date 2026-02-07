#include <exo/theme.h>

namespace exo {

const ColorPalette DarkPalette {
    RGB(30,  30,  30),   // background
    RGB(40,  40,  40),   // surface
    RGB(55,  55,  55),   // surfaceHover
    RGB(0,   120, 212),  // surfaceActive
    RGB(45,  45,  45),   // toolbar
    RGB(230, 230, 230),  // text
    RGB(160, 160, 160),  // textSecondary
    RGB(60,  60,  60),   // border
    RGB(0,   120, 212),  // accent
    RGB(35,  35,  35),   // statusBar
    RGB(160, 160, 160),  // statusBarText
};

const ColorPalette LightPalette {
    RGB(243, 243, 243),  // background
    RGB(235, 235, 235),  // surface
    RGB(220, 220, 220),  // surfaceHover
    RGB(0,   120, 212),  // surfaceActive
    RGB(249, 249, 249),  // toolbar
    RGB(25,  25,  25),   // text
    RGB(100, 100, 100),  // textSecondary
    RGB(210, 210, 210),  // border
    RGB(0,   120, 212),  // accent
    RGB(230, 230, 230),  // statusBar
    RGB(100, 100, 100),  // statusBarText
};

bool Theme::s_dark = true;

bool Theme::IsDarkMode() {
    DWORD value = 1;
    DWORD size  = sizeof(value);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_DWORD, nullptr, &value, &size
    );
    return value == 0;
}

const ColorPalette& Theme::Colors() { return s_dark ? DarkPalette : LightPalette; }
bool Theme::IsDark() { return s_dark; }
void Theme::Init() { s_dark = IsDarkMode(); }
void Theme::Toggle() { s_dark = !s_dark; }
void Theme::SetDark(bool dark) { s_dark = dark; }

void Theme::ApplyToWindow(HWND hwnd) {
    BOOL dark = s_dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
}

uint32_t Theme::IconColor() {
    auto& c = Colors();
    return (GetRValue(c.text) << 16) | (GetGValue(c.text) << 8) | GetBValue(c.text);
}

uint32_t Theme::SecondaryIconColor() {
    auto& c = Colors();
    return (GetRValue(c.textSecondary) << 16) | (GetGValue(c.textSecondary) << 8) | GetBValue(c.textSecondary);
}

} // namespace exo
