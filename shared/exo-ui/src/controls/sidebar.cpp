#include <exo/controls/sidebar.h>

namespace exo {

int Sidebar::ScaledWidth() const { return Dpi::Scale(BASE_WIDTH, m_dpi); }
HWND Sidebar::Handle() const { return m_hwnd; }
int  Sidebar::Selected() const { return m_selected; }
int  Sidebar::ItemHeight() const { return Dpi::Scale(BASE_ITEM_HEIGHT, m_dpi); }
int  Sidebar::PaddingX() const { return Dpi::Scale(BASE_PADDING_X, m_dpi); }
int  Sidebar::IconSize() const { return Dpi::Scale(BASE_ICON_SIZE, m_dpi); }

void Sidebar::Create(HWND parent, HINSTANCE hInst, int id) {
    m_parent = parent;

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = SidebarProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"ExoSidebar";
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(
        0, L"ExoSidebar", nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0, BASE_WIDTH, 400,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        hInst, this
    );

    m_dpi = Dpi::Get(m_hwnd);
    CreateRenderTarget();
}

void Sidebar::Resize(int x, int y, int w, int h) {
    MoveWindow(m_hwnd, x, y, w, h, TRUE);
    if (m_rt) m_rt->Resize(D2D1::SizeU(w, h));
}

void Sidebar::Repaint() { InvalidateRect(m_hwnd, nullptr, FALSE); }

void Sidebar::UpdateDpi(int dpi) {
    m_dpi = dpi;
    m_cachedIconSize = 0;  // force rebuild
    Repaint();
}

void Sidebar::CreateRenderTarget() {
    m_rt = RenderContext::CreateHwndTarget(m_hwnd);
    m_cachedIconSize = 0;
}

void Sidebar::RebuildIconCache() {
    if (!m_rt) return;
    int sz = IconSize();
    uint32_t color = Theme::IconColor();
    if (sz == m_cachedIconSize && color == m_cachedIconColor) return;

    for (int i = 0; i < kCategoryCount; i++) {
        m_iconBitmaps[i].Reset();
        auto* rgba = LucideIcons::Render(kCategories[i].iconName, sz, color);
        if (rgba) {
            m_iconBitmaps[i] = RenderContext::CreateBitmapFromRGBA(
                m_rt.Get(), rgba, sz, sz);
            LucideIcons::Free(rgba);
        }
    }
    m_cachedIconSize = sz;
    m_cachedIconColor = color;
}

void Sidebar::OnPaint() {
    if (!m_rt) { CreateRenderTarget(); if (!m_rt) return; }
    RebuildIconCache();

    auto& c = Theme::Colors();
    int itemH   = ItemHeight();
    int padX    = PaddingX();
    int iconSz  = IconSize();
    float fontSize   = Dpi::ScaleF(static_cast<float>(BASE_FONT_SIZE), m_dpi);
    float headerSize = Dpi::ScaleF(static_cast<float>(BASE_HEADER_FONT), m_dpi);

    m_rt->BeginDraw();
    m_rt->Clear(ToD2DColor(c.surface));

    auto size = m_rt->GetSize();

    // Right border
    ComPtr<ID2D1SolidColorBrush> borderBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.border), &borderBrush);
    m_rt->DrawLine(
        D2D1::Point2F(size.width - 0.5f, 0),
        D2D1::Point2F(size.width - 0.5f, size.height),
        borderBrush.Get(), 1.0f
    );

    // Header
    auto headerFmt = RenderContext::CreateTextFormat(L"Segoe UI", headerSize, DWRITE_FONT_WEIGHT_SEMI_BOLD);
    ComPtr<ID2D1SolidColorBrush> secBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.textSecondary), &secBrush);
    float headerTop = Dpi::ScaleF(14.0f, m_dpi);
    float headerBot = Dpi::ScaleF(34.0f, m_dpi);
    m_rt->DrawText(L"CATEGORIES", 10, headerFmt.Get(),
        D2D1::RectF(static_cast<float>(padX), headerTop, size.width - static_cast<float>(padX), headerBot),
        secBrush.Get());

    // Items
    auto itemFmt = RenderContext::CreateTextFormat(L"Segoe UI", fontSize);
    ComPtr<ID2D1SolidColorBrush> textBrush, whiteBrush, hoverBrush, activeBrush;
    m_rt->CreateSolidColorBrush(ToD2DColor(c.text), &textBrush);
    m_rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &whiteBrush);
    m_rt->CreateSolidColorBrush(ToD2DColor(c.surfaceHover), &hoverBrush);
    m_rt->CreateSolidColorBrush(ToD2DColor(c.surfaceActive), &activeBrush);

    float yStart = Dpi::ScaleF(44.0f, m_dpi);
    float margin = Dpi::ScaleF(4.0f, m_dpi);
    float inflate = Dpi::ScaleF(2.0f, m_dpi);
    float gap = Dpi::ScaleF(8.0f, m_dpi);

    for (int i = 0; i < kCategoryCount; i++) {
        float top = yStart + i * static_cast<float>(itemH);
        float bot = top + static_cast<float>(itemH);
        D2D1_RECT_F itemRc = D2D1::RectF(margin, top + inflate, size.width - margin - 2, bot - inflate);

        ID2D1SolidColorBrush* labelBrush = textBrush.Get();

        if (i == m_selected) {
            m_rt->FillRoundedRectangle(
                D2D1::RoundedRect(itemRc, 4.0f, 4.0f), activeBrush.Get());
            labelBrush = whiteBrush.Get();
        } else if (i == m_hovered) {
            m_rt->FillRoundedRectangle(
                D2D1::RoundedRect(itemRc, 4.0f, 4.0f), hoverBrush.Get());
        }

        // Icon
        float iconX = itemRc.left + static_cast<float>(padX);
        float iconY = (top + bot - static_cast<float>(iconSz)) / 2.0f;

        if (m_iconBitmaps[i]) {
            D2D1_RECT_F iconRc = D2D1::RectF(
                iconX, iconY,
                iconX + static_cast<float>(iconSz),
                iconY + static_cast<float>(iconSz)
            );
            m_rt->DrawBitmap(m_iconBitmaps[i].Get(), iconRc,
                (i == m_selected) ? 1.0f : 0.75f);
        }

        // Label
        float labelX = iconX + static_cast<float>(iconSz) + gap;
        D2D1_RECT_F labelRc = D2D1::RectF(labelX, top, size.width, bot);
        m_rt->DrawText(kCategories[i].label,
            static_cast<UINT32>(wcslen(kCategories[i].label)),
            itemFmt.Get(), labelRc, labelBrush);
    }

    HRESULT hr = m_rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) m_rt.Reset();
}

int Sidebar::HitTest(int y) {
    int yStart = Dpi::Scale(44, m_dpi);
    int itemH  = ItemHeight();
    int idx = (y - yStart) / itemH;
    if (idx >= 0 && idx < kCategoryCount) return idx;
    return -1;
}

LRESULT CALLBACK Sidebar::SidebarProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Sidebar* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<Sidebar*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->m_hwnd = hwnd;
    } else {
        self = reinterpret_cast<Sidebar*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
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
        int idx = self->HitTest(HIWORD(lp));
        if (idx >= 0 && idx != self->m_selected) {
            self->m_selected = idx;
            InvalidateRect(hwnd, nullptr, FALSE);
            SendMessageW(self->m_parent, WM_COMMAND,
                MAKEWPARAM(GetDlgCtrlID(hwnd), idx),
                reinterpret_cast<LPARAM>(hwnd));
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        int idx = self->HitTest(HIWORD(lp));
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
