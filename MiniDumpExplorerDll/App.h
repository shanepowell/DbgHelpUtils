#pragma once
#include "App.g.h"
#include "App.base.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct App : AppT2<App>
    {
        App();
        ~App();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    class App : public AppT<App, implementation::App>
    {
    };
}

