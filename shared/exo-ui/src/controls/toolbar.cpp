#include <exo/controls/toolbar.h>

namespace exo {

int Toolbar::ScaledHeight() const { return Dpi::Scale(BASE_HEIGHT, m_dpi); }
HWND Toolbar::Handle() const { return m_hwnd; }
int Toolbar::BtnWidth(int idx) const { return Dpi::Scale(kButtons[idx].baseWidth, m_dpi); }
int Toolbar::Pad() const { return Dpi::Scale(4, m_dpi); }
int Toolbar::Margin() const { return Dpi::Scale(8, m_dpi); }
int Toolbar::Height() const { return Dpi::Scale(BASE_HEIGHT, m_dpi); }

void Toolbar::Create(HWND parent, HINSTANCE hInst, int id) {
    m_parent = parent;

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = ToolbarProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"ExoToolbar";
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(
        0, L"ExoToolbar", nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0, 800, BASE_HEIGHT,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        hInst, this
    );

    m_dpi = Dpi::Get(m_hwnd);
    CreateRenderTarget();
}

void Toolbar::Resize(int x, int y, int w, int h) {
    MoveWindow(m_hwnd, x, y, w, h, TRUE);
    if (m_rt) m_rt->Resize(D2D1::SizeU(w, h));
}

void Toolbar::Repaint() { InvalidateRect(m_hwnd, nullptr, FALSE); }

void Toolbar::UpdateDpi(int dpi) {
    m_dpi = dpi;
    Repaint();
}

void Toolbar::CreateRenderTarget() {
    m_rt = RenderContext::CreateHwndTarget(m_hwnd);
}

D2D1_RECT_F Toolbar::ButtonRect(int idx, float totalWidth) const {
    float pad    = static_cast<float>(Pad());
    float margin = static_cast<float>(Margin());
    float h      = static_cast<float>(Height());
    float x      = margin;
    float rightX = totalWidth - margin;

    float rightBtns[kButtonCount];
    for (int i = kButtonCount - 1; i >= 0; i--) {
        if (kButtons[i].rightAlign) {
            rightX -= static_cast<float>(BtnWidth(i)) + pad;
            rightBtns[i] = rightX;
        } else {
            rightBtns[i] = -1;
        }
    }

    if (kButtons[idx].rightAlign) {
        float bx = rightBtns[idx];
        return D2D1::RectF(bx, pad, bx + static_cast<float>(BtnWidth(idx)), h - pad);
    }

    for (int i = 0; i < idx; i++) {
        if (!kButtons[i].rightAlign)
            x += static_cast<float>(BtnWidth(i)) + pad;
    }

    if (idx > 0 && !kButtons[idx].toggle && kButtons[idx - 1].toggle)
        x += margin;

    return D2D1::RectF(x, pad, x + static_cast<float>(BtnWidth(idx)), h - pad);
}

int Toolbar::HitTest(int mx, int my, float totalWidth) {
    for (int i = 0; i < kButtonCount; i++) {
        auto r = ButtonRect(i, totalWidth);
        if (mx >= r.left && mx <= r.right && my >= r.top && my <= r.bottom)
            return i;
    }
    return -1;
}

void Toolbar::OnPaint() {
    if (!m_rt) { CreateRenderTarget(); if (!m_rt) return; }

    auto& c = Theme::Colors();
    float fontSize = Dpi::ScaleF(static_cast<float>(BASE_FONT_SIZE), m_dpi);

    m_rt->BeginDraw();
    m_rt->Clear(ToD2DColor(c.toolbar));

    auto size = m_rt->GetSize();

    // Bottom border
    ComPtr<ID2D1SolidColorBrush> borderBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.border), &borderBrush);
    m_rt->DrawLine(
        D2D1::Point2F(0, size.height - 0.5f),
        D2D1::Point2F(size.width, size.height - 0.5f),
        borderBrush.Get(), 1.0f
    );

    auto textFmt = RenderContext::CreateTextFormat(L"Segoe UI", fontSize);
    if (textFmt) {
        textFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        textFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    ComPtr<ID2D1SolidColorBrush> textBrush, whiteBrush, hoverBrush, accentBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.text), &textBrush);
    m_rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &whiteBrush);
    m_rt->CreateSolidColorBrush(ToD2DColor(c.surfaceHover), &hoverBrush);
    m_rt->CreateSolidColorBrush(ToD2DColor(c.accent), &accentBrush);

    for (int i = 0; i < kButtonCount; i++) {
        auto btnRc = ButtonRect(i, size.width);
        bool active = (kButtons[i].toggle && kButtons[i].id == m_activeView);
        bool hovered = (i == m_hovered);

        if (active) {
            m_rt->FillRoundedRectangle(
                D2D1::RoundedRect(btnRc, 4.0f, 4.0f), accentBrush.Get());
        } else if (hovered) {
            m_rt->FillRoundedRectangle(
                D2D1::RoundedRect(btnRc, 4.0f, 4.0f), hoverBrush.Get());
            m_rt->DrawRoundedRectangle(
                D2D1::RoundedRect(btnRc, 4.0f, 4.0f), borderBrush.Get(), 1.0f);
        }

        auto* brush = active ? whiteBrush.Get() : textBrush.Get();
        const wchar_t* label = kButtons[i].label;
        if (label && label[0] && textFmt) {
            m_rt->DrawText(label, static_cast<UINT32>(wcslen(label)),
                textFmt.Get(), btnRc, brush);
        }
    }

    HRESULT hr = m_rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) m_rt.Reset();
}

LRESULT CALLBACK Toolbar::ToolbarProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Toolbar* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<Toolbar*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->m_hwnd = hwnd;
    } else {
        self = reinterpret_cast<Toolbar*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
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

    case WM_LBUTTONDOWN: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        int idx = self->HitTest(LOWORD(lp), HIWORD(lp), static_cast<float>(rc.right));
        if (idx >= 0) {
            const auto& btn = kButtons[idx];
            if (btn.toggle) self->m_activeView = btn.id;
            InvalidateRect(hwnd, nullptr, FALSE);
            SendMessageW(self->m_parent, WM_COMMAND,
                MAKEWPARAM(btn.id, 0), reinterpret_cast<LPARAM>(hwnd));
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        int idx = self->HitTest(LOWORD(lp), HIWORD(lp), static_cast<float>(rc.right));
        if (idx != self->m_hovered) {
            self->m_hovered = idx;
            InvalidateRect(hwnd, nullptr, FALSE);
            TRACKMOUSEEVENT tme{};
            tme.cbSize    = sizeof(tme);
            tme.dwFlags   = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        if (self->m_hovered >= 0) {
            self->m_hovered = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace exo
