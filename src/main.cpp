// ==============================================================================
// ExoSuite - Modern Windows Control Panel
// ==============================================================================
// Entry point for the ExoSuite application with Win32 UI.
// ==============================================================================

#include "Core/Platform.h"
#include "Core/CplLoader.h"
#include "Core/CplItem.h"

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <memory>

#pragma comment(lib, "comctl32.lib")

// ==============================================================================
// Constants
// ==============================================================================

constexpr wchar_t APP_CLASS_NAME[] = L"ExoSuiteMainWindow";
constexpr wchar_t APP_TITLE[] = L"ExoSuite - Control Panel";
constexpr int SIDEBAR_WIDTH = 220;
constexpr int TOOLBAR_HEIGHT = 40;
constexpr int STATUSBAR_HEIGHT = 24;

// Control IDs
constexpr int ID_SIDEBAR = 1001;
constexpr int ID_LISTVIEW = 1002;
constexpr int ID_TOOLBAR = 1003;
constexpr int ID_STATUSBAR = 1004;

// Menu IDs
constexpr int ID_FILE_EXIT = 40001;
constexpr int ID_VIEW_LARGEICONS = 40010;
constexpr int ID_VIEW_SMALLICONS = 40011;
constexpr int ID_VIEW_LIST = 40012;
constexpr int ID_VIEW_DETAILS = 40013;
constexpr int ID_VIEW_REFRESH = 40020;
constexpr int ID_TOOLS_RUN = 40030;
constexpr int ID_HELP_ABOUT = 40100;

// Sidebar item IDs
constexpr int SIDEBAR_ALL = 0;
constexpr int SIDEBAR_SYSTEM = 1;
constexpr int SIDEBAR_NETWORK = 2;
constexpr int SIDEBAR_SECURITY = 3;
constexpr int SIDEBAR_DISPLAY = 4;
constexpr int SIDEBAR_HARDWARE = 5;

// ==============================================================================
// Global State
// ==============================================================================

struct AppState {
    HWND hwndMain = nullptr;
    HWND hwndToolbar = nullptr;
    HWND hwndSidebar = nullptr;
    HWND hwndListView = nullptr;
    HWND hwndStatusBar = nullptr;
    HIMAGELIST hImageListLarge = nullptr;
    HIMAGELIST hImageListSmall = nullptr;
    std::unique_ptr<ExoSuite::Core::CplLoader> loader;
    int currentCategory = SIDEBAR_ALL;
};

static AppState g_app;

// ==============================================================================
// Forward Declarations
// ==============================================================================

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateMainMenu(HWND hwnd);
void CreateToolbar(HWND hwnd);
void CreateSidebar(HWND hwnd);
void CreateListView(HWND hwnd);
void CreateStatusBar(HWND hwnd);
void ResizeControls(HWND hwnd);
void LoadApplets();
void RefreshListView();
void UpdateStatusBar(const std::wstring& text);
void OnListViewDblClick();
void OnSidebarSelChange();
void OnAbout(HWND hwnd);

// ==============================================================================
// Entry Point
// ==============================================================================

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPWSTR /*lpCmdLine*/,
    _In_ int nCmdShow)
{
    // Validate platform requirements (Windows 10+ x64)
    if (!ExoSuite::EnsurePlatformRequirements()) {
        return 1;
    }

    // Initialize common controls
    INITCOMMONCONTROLSEX icex{};
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Register window class
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = APP_CLASS_NAME;
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    
    if (!RegisterClassExW(&wcex)) {
        return 1;
    }

    // Create main window
    g_app.hwndMain = CreateWindowExW(
        0,
        APP_CLASS_NAME,
        APP_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1200, 800,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_app.hwndMain) {
        return 1;
    }

    ShowWindow(g_app.hwndMain, nCmdShow);
    UpdateWindow(g_app.hwndMain);

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

// ==============================================================================
// Window Procedure
// ==============================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateMainMenu(hwnd);
        CreateToolbar(hwnd);
        CreateSidebar(hwnd);
        CreateListView(hwnd);
        CreateStatusBar(hwnd);
        
        // Initialize loader and load applets
        g_app.loader = std::make_unique<ExoSuite::Core::CplLoader>();
        LoadApplets();
        return 0;

    case WM_SIZE:
        ResizeControls(hwnd);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_EXIT:
            DestroyWindow(hwnd);
            break;
        case ID_VIEW_LARGEICONS:
            SetWindowLongPtrW(g_app.hwndListView, GWL_STYLE,
                (GetWindowLongPtrW(g_app.hwndListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_ICON);
            break;
        case ID_VIEW_SMALLICONS:
            SetWindowLongPtrW(g_app.hwndListView, GWL_STYLE,
                (GetWindowLongPtrW(g_app.hwndListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_SMALLICON);
            break;
        case ID_VIEW_LIST:
            SetWindowLongPtrW(g_app.hwndListView, GWL_STYLE,
                (GetWindowLongPtrW(g_app.hwndListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_LIST);
            break;
        case ID_VIEW_DETAILS:
            SetWindowLongPtrW(g_app.hwndListView, GWL_STYLE,
                (GetWindowLongPtrW(g_app.hwndListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_REPORT);
            break;
        case ID_VIEW_REFRESH:
            LoadApplets();
            break;
        case ID_HELP_ABOUT:
            OnAbout(hwnd);
            break;
        }
        return 0;

    case WM_NOTIFY: {
        auto nmhdr = reinterpret_cast<LPNMHDR>(lParam);
        if (nmhdr->hwndFrom == g_app.hwndListView) {
            if (nmhdr->code == NM_DBLCLK) {
                OnListViewDblClick();
            }
        } else if (nmhdr->hwndFrom == g_app.hwndSidebar) {
            if (nmhdr->code == TVN_SELCHANGED) {
                OnSidebarSelChange();
            }
        }
        return 0;
    }

    case WM_DESTROY:
        if (g_app.hImageListLarge) ImageList_Destroy(g_app.hImageListLarge);
        if (g_app.hImageListSmall) ImageList_Destroy(g_app.hImageListSmall);
        g_app.loader.reset();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ==============================================================================
// UI Creation
// ==============================================================================

void CreateMainMenu(HWND hwnd) {
    HMENU hMenuBar = CreateMenu();
    
    // File menu
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"E&xit\tAlt+F4");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), L"&File");
    
    // View menu
    HMENU hViewMenu = CreatePopupMenu();
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_LARGEICONS, L"&Large Icons");
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_SMALLICONS, L"&Small Icons");
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_LIST, L"&List");
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_DETAILS, L"&Details");
    AppendMenuW(hViewMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_REFRESH, L"&Refresh\tF5");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hViewMenu), L"&View");
    
    // Tools menu
    HMENU hToolsMenu = CreatePopupMenu();
    AppendMenuW(hToolsMenu, MF_STRING, ID_TOOLS_RUN, L"&Run...\tCtrl+R");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hToolsMenu), L"&Tools");
    
    // Help menu
    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenuW(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"&About ExoSuite");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hHelpMenu), L"&Help");
    
    SetMenu(hwnd, hMenuBar);
}

void CreateToolbar(HWND hwnd) {
    g_app.hwndToolbar = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        0, 0, 0, TOOLBAR_HEIGHT,
        hwnd,
        reinterpret_cast<HMENU>(ID_TOOLBAR),
        GetModuleHandleW(nullptr),
        nullptr
    );
    
    SendMessageW(g_app.hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    
    // Add standard image list
    TBADDBITMAP tbab{};
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessageW(g_app.hwndToolbar, TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>(&tbab));
    
    // Add buttons
    TBBUTTON buttons[] = {
        { STD_FILENEW, ID_VIEW_REFRESH, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0 },
        { VIEW_LARGEICONS, ID_VIEW_LARGEICONS, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { VIEW_SMALLICONS, ID_VIEW_SMALLICONS, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { VIEW_LIST, ID_VIEW_LIST, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { VIEW_DETAILS, ID_VIEW_DETAILS, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
    };
    
    SendMessageW(g_app.hwndToolbar, TB_ADDBUTTONS, _countof(buttons), reinterpret_cast<LPARAM>(buttons));
    SendMessageW(g_app.hwndToolbar, TB_AUTOSIZE, 0, 0);
}

void CreateSidebar(HWND hwnd) {
    g_app.hwndSidebar = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_TREEVIEWW,
        nullptr,
        WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
        0, TOOLBAR_HEIGHT, SIDEBAR_WIDTH, 400,
        hwnd,
        reinterpret_cast<HMENU>(ID_SIDEBAR),
        GetModuleHandleW(nullptr),
        nullptr
    );
    
    // Add categories
    auto addItem = [](const wchar_t* text, int id) {
        TVINSERTSTRUCTW tvis{};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
        tvis.item.pszText = const_cast<LPWSTR>(text);
        tvis.item.lParam = id;
        return TreeView_InsertItem(g_app.hwndSidebar, &tvis);
    };
    
    HTREEITEM hAll = addItem(L"All Items", SIDEBAR_ALL);
    addItem(L"System", SIDEBAR_SYSTEM);
    addItem(L"Network", SIDEBAR_NETWORK);
    addItem(L"Security", SIDEBAR_SECURITY);
    addItem(L"Display", SIDEBAR_DISPLAY);
    addItem(L"Hardware", SIDEBAR_HARDWARE);
    
    TreeView_SelectItem(g_app.hwndSidebar, hAll);
}

void CreateListView(HWND hwnd) {
    g_app.hwndListView = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEWW,
        nullptr,
        WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        SIDEBAR_WIDTH, TOOLBAR_HEIGHT, 600, 400,
        hwnd,
        reinterpret_cast<HMENU>(ID_LISTVIEW),
        GetModuleHandleW(nullptr),
        nullptr
    );
    
    ListView_SetExtendedListViewStyle(g_app.hwndListView, 
        LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    
    // Create image lists
    g_app.hImageListLarge = ImageList_Create(48, 48, ILC_COLOR32 | ILC_MASK, 32, 32);
    g_app.hImageListSmall = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 32, 32);
    
    ListView_SetImageList(g_app.hwndListView, g_app.hImageListLarge, LVSIL_NORMAL);
    ListView_SetImageList(g_app.hwndListView, g_app.hImageListSmall, LVSIL_SMALL);
    
    // Add columns for details view
    LVCOLUMNW lvc{};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    
    lvc.pszText = const_cast<LPWSTR>(L"Name");
    lvc.cx = 250;
    lvc.iSubItem = 0;
    ListView_InsertColumn(g_app.hwndListView, 0, &lvc);
    
    lvc.pszText = const_cast<LPWSTR>(L"Description");
    lvc.cx = 400;
    lvc.iSubItem = 1;
    ListView_InsertColumn(g_app.hwndListView, 1, &lvc);
    
    lvc.pszText = const_cast<LPWSTR>(L"File");
    lvc.cx = 200;
    lvc.iSubItem = 2;
    ListView_InsertColumn(g_app.hwndListView, 2, &lvc);
}

void CreateStatusBar(HWND hwnd) {
    g_app.hwndStatusBar = CreateWindowExW(
        0,
        STATUSCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        hwnd,
        reinterpret_cast<HMENU>(ID_STATUSBAR),
        GetModuleHandleW(nullptr),
        nullptr
    );
    
    int parts[] = { 300, -1 };
    SendMessageW(g_app.hwndStatusBar, SB_SETPARTS, 2, reinterpret_cast<LPARAM>(parts));
    UpdateStatusBar(L"Ready");
}

void ResizeControls(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    
    // Toolbar
    SendMessageW(g_app.hwndToolbar, WM_SIZE, 0, 0);
    RECT tbRect;
    GetWindowRect(g_app.hwndToolbar, &tbRect);
    int toolbarHeight = tbRect.bottom - tbRect.top;
    
    // Status bar
    SendMessageW(g_app.hwndStatusBar, WM_SIZE, 0, 0);
    RECT sbRect;
    GetWindowRect(g_app.hwndStatusBar, &sbRect);
    int statusHeight = sbRect.bottom - sbRect.top;
    
    // Content area
    int contentTop = toolbarHeight;
    int contentHeight = height - toolbarHeight - statusHeight;
    
    // Sidebar
    SetWindowPos(g_app.hwndSidebar, nullptr, 
        0, contentTop, 
        SIDEBAR_WIDTH, contentHeight, 
        SWP_NOZORDER);
    
    // ListView
    SetWindowPos(g_app.hwndListView, nullptr, 
        SIDEBAR_WIDTH, contentTop, 
        width - SIDEBAR_WIDTH, contentHeight, 
        SWP_NOZORDER);
}

// ==============================================================================
// Applet Loading
// ==============================================================================

void LoadApplets() {
    UpdateStatusBar(L"Loading applets...");
    
    g_app.loader->LoadSystemCpls();
    g_app.loader->LoadExtensions();
    
    RefreshListView();
    
    std::wstring status = L"Loaded " + std::to_wstring(g_app.loader->Count()) + L" applets";
    UpdateStatusBar(status);
}

void RefreshListView() {
    ListView_DeleteAllItems(g_app.hwndListView);
    ImageList_RemoveAll(g_app.hImageListLarge);
    ImageList_RemoveAll(g_app.hImageListSmall);
    
    // Add default icon
    HICON hDefaultIcon = LoadIconW(nullptr, IDI_APPLICATION);
    ImageList_AddIcon(g_app.hImageListLarge, hDefaultIcon);
    ImageList_AddIcon(g_app.hImageListSmall, hDefaultIcon);
    
    int itemIndex = 0;
    for (const auto& item : g_app.loader->Items()) {
        // Add item icon to image lists
        int imageIndex = 0;
        HICON hIcon = item->Icon();
        if (hIcon) {
            imageIndex = ImageList_AddIcon(g_app.hImageListLarge, hIcon);
            ImageList_AddIcon(g_app.hImageListSmall, hIcon);
        }
        
        LVITEMW lvi{};
        lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        lvi.iItem = itemIndex;
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPWSTR>(item->Name().c_str());
        lvi.iImage = imageIndex;
        lvi.lParam = itemIndex;
        
        int idx = ListView_InsertItem(g_app.hwndListView, &lvi);
        
        // Add subitems
        ListView_SetItemText(g_app.hwndListView, idx, 1, 
            const_cast<LPWSTR>(item->Description().c_str()));
        ListView_SetItemText(g_app.hwndListView, idx, 2, 
            const_cast<LPWSTR>(item->FilePath().c_str()));
        
        itemIndex++;
    }
}

void UpdateStatusBar(const std::wstring& text) {
    SendMessageW(g_app.hwndStatusBar, SB_SETTEXTW, 0, 
        reinterpret_cast<LPARAM>(text.c_str()));
}

void OnListViewDblClick() {
    int sel = ListView_GetNextItem(g_app.hwndListView, -1, LVNI_SELECTED);
    if (sel >= 0) {
        LVITEMW lvi{};
        lvi.mask = LVIF_PARAM;
        lvi.iItem = sel;
        ListView_GetItem(g_app.hwndListView, &lvi);
        
        const auto& items = g_app.loader->Items();
        if (lvi.lParam >= 0 && static_cast<size_t>(lvi.lParam) < items.size()) {
            items[lvi.lParam]->Execute(g_app.hwndMain);
        }
    }
}

void OnSidebarSelChange() {
    HTREEITEM hSel = TreeView_GetSelection(g_app.hwndSidebar);
    if (hSel) {
        TVITEMW tvi{};
        tvi.mask = TVIF_PARAM;
        tvi.hItem = hSel;
        TreeView_GetItem(g_app.hwndSidebar, &tvi);
        g_app.currentCategory = static_cast<int>(tvi.lParam);
        
        // TODO: Filter items by category
        RefreshListView();
    }
}

void OnAbout(HWND hwnd) {
    MessageBoxW(hwnd, 
        L"ExoSuite - Modern Control Panel\n\n"
        L"Version 0.1.0\n\n"
        L"A modern replacement for the classic Windows Control Panel.\n\n"
        L"(c) 2026 Rizonesoft",
        L"About ExoSuite",
        MB_OK | MB_ICONINFORMATION);
}
