#include <exo/controls/statusbar.h>

namespace exo {

int StatusBar::ScaledHeight() const { return Dpi::Scale(BASE_HEIGHT, m_dpi); }
HWND StatusBar::Handle() const { return m_hwnd; }

void StatusBar::Create(HWND parent, HINSTANCE hInst, int id) {
    m_parent = parent;

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = StatusProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"ExoStatusBar";
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(
        0, L"ExoStatusBar", nullptr,
        WS_CHILD | WS_VISIBLE,
        0, 0, 800, BASE_HEIGHT,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        hInst, this
    );

    m_dpi = Dpi::Get(m_hwnd);
    m_rt = RenderContext::CreateHwndTarget(m_hwnd);
    SetText(L"Ready");
}

void StatusBar::SetText(const wchar_t* text) {
    m_text = text ? text : L"";
    InvalidateRect(m_hwnd, nullptr, FALSE);
}

void StatusBar::Resize(int x, int y, int w, int h) {
    MoveWindow(m_hwnd, x, y, w, h, TRUE);
    if (m_rt) m_rt->Resize(D2D1::SizeU(w, h));
}

void StatusBar::Repaint() { InvalidateRect(m_hwnd, nullptr, FALSE); }

void StatusBar::UpdateDpi(int dpi) {
    m_dpi = dpi;
    Repaint();
}

void StatusBar::OnPaint() {
    if (!m_rt) {
        m_rt = RenderContext::CreateHwndTarget(m_hwnd);
        if (!m_rt) return;
    }

    auto& c = Theme::Colors();
    float fontSize = Dpi::ScaleF(static_cast<float>(BASE_FONT_SIZE), m_dpi);
    float padX = Dpi::ScaleF(static_cast<float>(BASE_PADDING_X), m_dpi);

    m_rt->BeginDraw();
    m_rt->Clear(ToD2DColor(c.statusBar));

    auto size = m_rt->GetSize();

    // Top border
    ComPtr<ID2D1SolidColorBrush> borderBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.border), &borderBrush);
    m_rt->DrawLine(
        D2D1::Point2F(0, 0.5f),
        D2D1::Point2F(size.width, 0.5f),
        borderBrush.Get(), 1.0f
    );

    // Text
    auto textFmt = RenderContext::CreateTextFormat(L"Segoe UI", fontSize);
    ComPtr<ID2D1SolidColorBrush> textBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.statusBarText), &textBrush);

    D2D1_RECT_F textRc = D2D1::RectF(padX, 0, size.width - padX, size.height);
    m_rt->DrawText(m_text.c_str(), static_cast<UINT32>(m_text.length()),
        textFmt.Get(), textRc, textBrush.Get());

    HRESULT hr = m_rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) m_rt.Reset();
}

LRESULT CALLBACK StatusBar::StatusProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    StatusBar* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<StatusBar*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->m_hwnd = hwnd;
    } else {
        self = reinterpret_cast<StatusBar*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    if (!self) return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        self->OnPaint();
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
        if (self->m_rt) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            self->m_rt->Resize(D2D1::SizeU(rc.right, rc.bottom));
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace exo
