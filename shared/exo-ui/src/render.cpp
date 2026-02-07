#include <exo/render.h>

namespace exo {

ComPtr<ID2D1Factory> RenderContext::s_d2dFactory;
ComPtr<IDWriteFactory> RenderContext::s_dwriteFactory;

bool RenderContext::Init() {
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        s_d2dFactory.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(s_dwriteFactory.GetAddressOf())
    );
    return SUCCEEDED(hr);
}

ID2D1Factory* RenderContext::D2D() { return s_d2dFactory.Get(); }
IDWriteFactory* RenderContext::DWrite() { return s_dwriteFactory.Get(); }

ComPtr<ID2D1HwndRenderTarget> RenderContext::CreateHwndTarget(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    ComPtr<ID2D1HwndRenderTarget> target;
    auto props = D2D1::RenderTargetProperties();
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    auto hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);

    s_d2dFactory->CreateHwndRenderTarget(props, hwndProps, &target);
    return target;
}

ComPtr<ID2D1Bitmap> RenderContext::CreateBitmapFromRGBA(
    ID2D1RenderTarget* rt, const uint8_t* rgba, int width, int height)
{
    if (!rt || !rgba || width <= 0 || height <= 0) return nullptr;

    ComPtr<ID2D1Bitmap> bitmap;
    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    bmpProps.dpiX = 96.0f;
    bmpProps.dpiY = 96.0f;

    rt->CreateBitmap(
        D2D1::SizeU(width, height),
        rgba, width * 4,
        bmpProps, &bitmap
    );
    return bitmap;
}

ComPtr<IDWriteTextFormat> RenderContext::CreateTextFormat(
    const wchar_t* fontFamily, float size, DWRITE_FONT_WEIGHT weight)
{
    ComPtr<IDWriteTextFormat> fmt;
    s_dwriteFactory->CreateTextFormat(
        fontFamily, nullptr, weight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        size, L"", &fmt
    );
    if (fmt) {
        fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return fmt;
}

} // namespace exo
