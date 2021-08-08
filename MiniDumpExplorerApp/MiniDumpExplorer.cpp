#include "pch.h"
#include "MiniDumpExplorer.h"

#include "XamlBridge.h"
#include <ShellScalingApi.h>


#include "DbgHelpDispatcher.h"
#include "DumpFileFactory.h"
#include "SymbolEngineUi.h"
#include "../DbgHelpUtils/symbol_engine.h"

// Global Variables:
HINSTANCE hInst = nullptr; // current instance
auto szWindowClass = L"MiniDumpExplorer";

class ExplorerWindow : public DesktopWindowT<ExplorerWindow>
{
public:
    ExplorerWindow(HINSTANCE const hInstance, int const nCmdShow) noexcept
    {
        WNDCLASSEXW windows_class;
        windows_class.cbSize = sizeof(WNDCLASSEX);
        windows_class.style = CS_HREDRAW | CS_VREDRAW;
        windows_class.lpfnWndProc = WndProc;
        windows_class.cbClsExtra = DLGWINDOWEXTRA;
        windows_class.cbWndExtra = 0;
        windows_class.hInstance = hInstance;
        windows_class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINI_DUMP_EXPLORER));
        windows_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windows_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windows_class.lpszMenuName = MAKEINTRESOURCEW(IDI_MINI_DUMP_EXPLORER);
        windows_class.lpszClassName = szWindowClass;
        windows_class.hIconSm = LoadIcon(windows_class.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        WINRT_VERIFY(RegisterClassEx(&windows_class));
        WINRT_ASSERT(!GetHandle());

        [[maybe_unused]] const auto wnd = InitInstance(hInstance, nCmdShow);
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
    HWND InitInstance(HINSTANCE const hInstance, int const nCmdShow)
    {
        hInst = hInstance; // Store instance handle in our global variable
        auto const main_window = CreateWindow(
            szWindowClass,
            L"MiniDumpExplorer",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, InitialWidth, InitialHeight,
            nullptr, nullptr, hInstance, this);

        if (!main_window)
        {
            winrt::check_hresult(E_FAIL);
        }

        ShowWindow(main_window, nCmdShow);
        UpdateWindow(main_window);
        SetFocus(main_window);
        return main_window;
    }

    bool OnCreate(HWND, LPCREATESTRUCT)
    {
        factory_ = *winrt::make_self<MiniDumpExplorerApp::DumpFileFactory>(dispatcher_, symbol_engine_);
        main_user_control_ = winrt::MiniDumpExplorer::MainPage{factory_};
        symbol_engine_ui_.SetUi(main_user_control_);
        h_wnd_xaml_island_ = wil::unique_hwnd(CreateDesktopWindowsXamlSource(WS_TABSTOP, main_user_control_));
        return true;
    }

    void OnDestroy(HWND const window)
    {
        if(main_user_control_)
        {
            main_user_control_.UnloadAllTabs();
        }

        base_type::OnDestroy(window);
    }

    void OnResize(HWND, [[maybe_unused]] UINT state, int const cx, int const cy) const
    {
        SetWindowPos(h_wnd_xaml_island_.get(), nullptr, 0, 0, cx, cy, SWP_SHOWWINDOW);
    }

private:
    wil::unique_hwnd h_wnd_xaml_island_ = nullptr;
    winrt::MiniDumpExplorer::MainPage main_user_control_ = nullptr;
    DbgHelpDispatcher dispatcher_;
    SymbolEngineUi symbol_engine_ui_;
    dlg_help_utils::dbg_help::symbol_engine symbol_engine_{ symbol_engine_ui_ };
    winrt::MiniDumpExplorer::IDumpFileFactory factory_;
};

int APIENTRY wWinMain(_In_ HINSTANCE const hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       const nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    init_apartment(winrt::apartment_type::single_threaded);
    winrt::MiniDumpExplorer::App app;

    ExplorerWindow myWindow(hInstance, nCmdShow);
    auto const retValue = myWindow.MessageLoop(nullptr);

    app.Close();
    app = nullptr;

    return retValue;
}
