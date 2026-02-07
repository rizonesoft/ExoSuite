#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include "resource.h"

// ExoUI shared library
#include <exo/dpi.h>
#include <exo/theme.h>
#include <exo/render.h>
#include <exo/icons.h>
#include <exo/controls/toolbar.h>
#include <exo/controls/sidebar.h>
#include <exo/controls/statusbar.h>

// ── Control IDs ─────────────────────────────────────────────
enum CtrlId : int {
    IDC_TOOLBAR   = 100,
    IDC_SIDEBAR   = 101,
    IDC_LISTVIEW  = 102,
    IDC_STATUSBAR = 103,
};

// ── Application State ───────────────────────────────────────
struct AppState {
    exo::Toolbar   toolbar;
    exo::Sidebar   sidebar;
    exo::StatusBar statusbar;
    HWND           listView  = nullptr;
    HBRUSH         bgBrush   = nullptr;
    int            dpi       = 96;

    void DestroyBrushes() {
        if (bgBrush) { DeleteObject(bgBrush); bgBrush = nullptr; }
    }

    void UpdateBrushes() {
        DestroyBrushes();
        bgBrush = CreateSolidBrush(exo::Theme::Colors().background);
    }
};

// ── Layout ──────────────────────────────────────────────────
static void LayoutChildren(HWND hwnd, AppState& app) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    int clientH  = rc.bottom - rc.top;
    int clientW  = rc.right - rc.left;
    int toolbarH = app.toolbar.ScaledHeight();
    int statusH  = app.statusbar.ScaledHeight();
    int sidebarW = app.sidebar.ScaledWidth();

    app.toolbar.Resize(0, 0, clientW, toolbarH);
    app.statusbar.Resize(0, clientH - statusH, clientW, statusH);

    int contentH = clientH - toolbarH - statusH;
    app.sidebar.Resize(0, toolbarH, sidebarW, contentH);

    MoveWindow(app.listView, sidebarW, toolbarH,
        clientW - sidebarW, contentH, TRUE);
}

// ── Theme Application ───────────────────────────────────────
static void ApplyTheme(HWND hwnd, AppState& app) {
    exo::Theme::ApplyToWindow(hwnd);
    app.UpdateBrushes();

    auto& c = exo::Theme::Colors();
    ListView_SetBkColor(app.listView, c.background);
    ListView_SetTextBkColor(app.listView, c.background);
    ListView_SetTextColor(app.listView, c.text);

    if (exo::Theme::IsDark()) {
        SetWindowTheme(app.listView, L"", L"");
    } else {
        SetWindowTheme(app.listView, L"Explorer", nullptr);
    }

    app.toolbar.Repaint();
    app.sidebar.Repaint();
    app.statusbar.Repaint();
    InvalidateRect(hwnd, nullptr, TRUE);
}

// ── Window Procedure ────────────────────────────────────────
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    AppState* app = nullptr;

    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        app = static_cast<AppState*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    } else {
        app = reinterpret_cast<AppState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
    case WM_CREATE: {
        auto hInst = reinterpret_cast<CREATESTRUCTW*>(lp)->hInstance;
        app->dpi = exo::Dpi::Get(hwnd);

        app->toolbar.Create(hwnd, hInst, IDC_TOOLBAR);
        app->sidebar.Create(hwnd, hInst, IDC_SIDEBAR);
        app->statusbar.Create(hwnd, hInst, IDC_STATUSBAR);

        app->listView = CreateWindowExW(
            0, WC_LISTVIEWW, nullptr,
            WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL,
            0, 0, 400, 400,
            hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_LISTVIEW)),
            hInst, nullptr
        );
        ListView_SetExtendedListViewStyle(app->listView,
            LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

        ApplyTheme(hwnd, *app);
        return 0;
    }

    case WM_SIZE:
        if (app) LayoutChildren(hwnd, *app);
        return 0;

    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        switch (id) {
        case exo::IDC_TB_THEME:
            exo::Theme::Toggle();
            ApplyTheme(hwnd, *app);
            break;

        case exo::IDC_TB_VIEW_LARGE:
            SetWindowLongPtrW(app->listView, GWL_STYLE,
                (GetWindowLongPtrW(app->listView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_ICON);
            app->statusbar.SetText(L"Large Icons");
            break;

        case exo::IDC_TB_VIEW_SMALL:
            SetWindowLongPtrW(app->listView, GWL_STYLE,
                (GetWindowLongPtrW(app->listView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_SMALLICON);
            app->statusbar.SetText(L"Small Icons");
            break;

        case exo::IDC_TB_VIEW_LIST:
            SetWindowLongPtrW(app->listView, GWL_STYLE,
                (GetWindowLongPtrW(app->listView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_LIST);
            app->statusbar.SetText(L"List");
            break;

        case exo::IDC_TB_VIEW_DETAILS:
            SetWindowLongPtrW(app->listView, GWL_STYLE,
                (GetWindowLongPtrW(app->listView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_REPORT);
            app->statusbar.SetText(L"Details");
            break;

        case exo::IDC_TB_REFRESH:
            app->statusbar.SetText(L"Refreshing...");
            break;
        }
        return 0;
    }

    case WM_DPICHANGED: {
        int newDpi = HIWORD(wp);
        app->dpi = newDpi;
        app->toolbar.UpdateDpi(newDpi);
        app->sidebar.UpdateDpi(newDpi);
        app->statusbar.UpdateDpi(newDpi);

        auto* suggested = reinterpret_cast<RECT*>(lp);
        SetWindowPos(hwnd, nullptr,
            suggested->left, suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    case WM_SETTINGCHANGE:
        if (lp && wcscmp(reinterpret_cast<LPCWSTR>(lp), L"ImmersiveColorSet") == 0) {
            exo::Theme::SetDark(exo::Theme::IsDarkMode());
            ApplyTheme(hwnd, *app);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH br = CreateSolidBrush(exo::Theme::Colors().background);
        FillRect(hdc, &ps.rcPaint, br);
        DeleteObject(br);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        if (app) app->DestroyBrushes();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ── Entry Point ─────────────────────────────────────────────
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC  = ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    // Initialize ExoUI (D2D, DirectWrite, Lucide icons)
    if (!exo::RenderContext::Init()) {
        MessageBoxW(nullptr, L"Failed to initialize D2D/DirectWrite.", L"ExoSuite", MB_ICONERROR);
        return 1;
    }
    exo::LucideIcons::Load();
    exo::Theme::Init();

    constexpr auto CLASS_NAME = L"ExoSuiteMain";

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_EXOSUITE));
    wc.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_EXOSUITE));
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class.", L"ExoSuite", MB_ICONERROR);
        return 1;
    }

    AppState app{};

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"ExoSuite",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1100, 720,
        nullptr, nullptr, hInstance, &app
    );

    if (!hwnd) {
        MessageBoxW(nullptr, L"Failed to create main window.", L"ExoSuite", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
