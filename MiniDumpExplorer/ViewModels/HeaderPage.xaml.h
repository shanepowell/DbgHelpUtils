#pragma once

#include "HeaderPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct HeaderPage : HeaderPageT<HeaderPage>
    {
        HeaderPage();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HeaderPage : HeaderPageT<HeaderPage, implementation::HeaderPage>
    {
    };
}
