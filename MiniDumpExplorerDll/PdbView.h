#pragma once

#include "PdbView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct PdbView : PdbViewT<PdbView>
    {
        PdbView();

        ICvInfoPdb70 Pdb() const;
        void Pdb(ICvInfoPdb70 value);

    private:
        ICvInfoPdb70 pdb_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct PdbView : PdbViewT<PdbView, implementation::PdbView>
    {
    };
}
