#pragma once

#include "DumpStreamThreadNamesView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadNamesView : DumpStreamThreadNamesViewT<DumpStreamThreadNamesView>
    {
        DumpStreamThreadNamesView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadNamesView : DumpStreamThreadNamesViewT<DumpStreamThreadNamesView, implementation::DumpStreamThreadNamesView>
    {
    };
}
