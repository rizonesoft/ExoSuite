#pragma once
// ── ExoUI Rendering Context ─────────────────────────────────

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <cstdint>
#include "export.h"

using Microsoft::WRL::ComPtr;

namespace exo {

// D2D color from COLORREF
inline D2D1_COLOR_F ToD2DColor(COLORREF cr, float alpha = 1.0f) {
    return D2D1::ColorF(
        GetRValue(cr) / 255.0f,
        GetGValue(cr) / 255.0f,
        GetBValue(cr) / 255.0f,
        alpha
    );
}

class EXOUI_API RenderContext {
public:
    static bool Init();
    static ID2D1Factory* D2D();
    static IDWriteFactory* DWrite();

    static ComPtr<ID2D1HwndRenderTarget> CreateHwndTarget(HWND hwnd);

    static ComPtr<ID2D1Bitmap> CreateBitmapFromRGBA(
        ID2D1RenderTarget* rt, const uint8_t* rgba, int width, int height);

    static ComPtr<IDWriteTextFormat> CreateTextFormat(
        const wchar_t* fontFamily, float size,
        DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_REGULAR);

private:
    static ComPtr<ID2D1Factory> s_d2dFactory;
    static ComPtr<IDWriteFactory> s_dwriteFactory;
};

} // namespace exo
