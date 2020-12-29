#include "pch.h"
#include "App.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    App::App()
    {
        Initialize();

        AddRef();
        m_inner.as<IUnknown>()->Release();
    }

    App::~App()
    {
        Close();
    }
}

void* winrt_make_MiniDumpExplorer_App()
{
    return detach_abi(winrt::make<MiniDumpExplorer::factory_implementation::App>());
}
