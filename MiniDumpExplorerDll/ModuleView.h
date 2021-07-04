#pragma once

#include "ModuleView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ModuleView : ModuleViewT<ModuleView>
    {
        ModuleView();

        IMiniDumpModule Module() const;
        void Module(IMiniDumpModule value);

        hstring Test() const;
        void Test(hstring value);

    private:
        IMiniDumpModule module_;
        hstring test_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ModuleView : ModuleViewT<ModuleView, implementation::ModuleView>
    {
    };
}
