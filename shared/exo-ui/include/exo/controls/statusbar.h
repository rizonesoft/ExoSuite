#pragma once
// ── ExoUI Status Bar Control ────────────────────────────────

#include <windows.h>
#include <wrl/client.h>
#include <d2d1.h>
#include <string>
#include "../export.h"
#include "../dpi.h"
#include "../theme.h"
#include "../render.h"

using Microsoft::WRL::ComPtr;

namespace exo {

class EXOUI_API StatusBar {
public:
    static constexpr int BASE_HEIGHT    = 24;
    static constexpr int BASE_FONT_SIZE = 12;
    static constexpr int BASE_PADDING_X = 10;

    int ScaledHeight() const;
    void Create(HWND parent, HINSTANCE hInst, int id);
    HWND Handle() const;
    void SetText(const wchar_t* text);
    void Resize(int x, int y, int w, int h);
    void Repaint();
    void UpdateDpi(int dpi);

private:
    HWND m_hwnd     = nullptr;
    HWND m_parent   = nullptr;
    int  m_dpi      = 96;
    std::wstring m_text = L"Ready";
    ComPtr<ID2D1HwndRenderTarget> m_rt;

    void OnPaint();
    static LRESULT CALLBACK StatusProc(HWND, UINT, WPARAM, LPARAM);
};

} // namespace exo
