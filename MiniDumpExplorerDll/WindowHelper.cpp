#include "pch.h"
#include "WindowHelper.h"

#include <winrt/Windows.UI.Core.h>

struct
    __declspec(uuid("45D64A29-A63E-4CB6-B498-5781D298CB4F"))
    __declspec(novtable)
    ICoreWindowInterop : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* window) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MessageHandled(unsigned char value) = 0;
};


HWND WindowHelper::GetMainWindowHandle()
{
    const auto w = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();

    winrt::com_ptr<ICoreWindowInterop> interop{};
    winrt::check_hresult(winrt::get_unknown(w)->QueryInterface(interop.put()));
    HWND window{};
    winrt::check_hresult(interop->get_WindowHandle(&window));
    return window;
}

struct
    __declspec(uuid("3E68D4BD-7135-4D10-8018-9FB6D9F33FA1"))
    __declspec(novtable)
    IInitializeWithWindow : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Initialize(HWND window);
};

void WindowHelper::SetupFileOpenPicker(winrt::Windows::Storage::Pickers::FileOpenPicker const& picker)
{
    winrt::com_ptr<IInitializeWithWindow> interop{};
    winrt::check_hresult(winrt::get_unknown(picker)->QueryInterface(interop.put()));
    winrt::check_hresult(interop->Initialize(GetMainWindowHandle()));
}
