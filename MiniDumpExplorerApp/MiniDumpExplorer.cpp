#include "Pch.h"
#include "MiniDumpExplorer.h"

#include "XamlBridge.h"
#include <ShellScalingApi.h>

#define MAX_LOAD_STRING 100

// Global Variables:
HINSTANCE hInst = nullptr; // current instance
auto szWindowClass = L"MiniDumpExplorer";

class ExplorerWindow : public DesktopWindowT<ExplorerWindow>
{
public:
    ExplorerWindow(HINSTANCE hInstance, int nCmdShow) noexcept
    {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = DLGWINDOWEXTRA;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINI_DUMP_EXPLORER));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_MINI_DUMP_EXPLORER);
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        WINRT_VERIFY(RegisterClassEx(&wcex));
        WINRT_ASSERT(!GetHandle());

        const auto wnd = InitInstance(hInstance, nCmdShow);
        WINRT_ASSERT(wnd);
    }

    LRESULT MessageHandler(UINT const message, WPARAM const wParam, LPARAM const lParam) noexcept
    {
        switch (message)
        {
            HANDLE_MSG(GetHandle(), WM_CREATE, OnCreate);
            HANDLE_MSG(GetHandle(), WM_DESTROY, OnDestroy);
            HANDLE_MSG(GetHandle(), WM_SIZE, OnResize);
        default:
            return base_type::MessageHandler(message, wParam, lParam);
        }
    }

private:

    wil::unique_hwnd m_hWndXamlIsland = nullptr;
    winrt::MiniDumpExplorer::MainPage m_mainUserControl = nullptr;

    HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
    {
        hInst = hInstance; // Store instance handle in our global variable
        HWND hMainWnd = CreateWindow(
            szWindowClass,
            L"MiniDumpExplorer",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, InitialWidth, InitialHeight,
            nullptr, nullptr, hInstance, this);

        if (!hMainWnd)
        {
            winrt::check_hresult(E_FAIL);
        }

        ShowWindow(hMainWnd, nCmdShow);
        UpdateWindow(hMainWnd);
        SetFocus(hMainWnd);
        return hMainWnd;
    }

    bool OnCreate(HWND, LPCREATESTRUCT)
    {
        m_mainUserControl = winrt::MiniDumpExplorer::MainPage();
        m_hWndXamlIsland = wil::unique_hwnd(CreateDesktopWindowsXamlSource(WS_TABSTOP, m_mainUserControl));
        return true;
    }

    void OnDestroy(HWND hwnd)
    {
        base_type::OnDestroy(hwnd);
    }

    void OnResize(HWND, [[maybe_unused]] UINT state, int cx, int cy)
    {
        SetWindowPos(m_hWndXamlIsland.get(), nullptr, 0, 0, cx, cy, SWP_SHOWWINDOW);
    }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);
    winrt::MiniDumpExplorer::App app;

    ExplorerWindow myWindow(hInstance, nCmdShow);
    int const retValue = myWindow.MessageLoop(nullptr);

    app.Close();
    app = nullptr;

    return retValue;
}
