#pragma once

#include "Float80.g.h"

#include "DbgHelpUtils/float80.h"
#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Float80 : Float80T<Float80>, NotifyPropertyChangedBase<Float80>
    {
        Float80();

        void Set(dlg_help_utils::float80_t const& value);

        dlg_help_utils::float80_t const& Get() const { return value_; }

    private:
        dlg_help_utils::float80_t value_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Float80 : Float80T<Float80, implementation::Float80>
    {
    };
}
