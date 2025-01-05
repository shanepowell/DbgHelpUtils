#pragma once

#include "M128A.g.h"

#include "DbgHelpUtils/windows_setup.h"
#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A : M128AT<M128A>, NotifyPropertyChangedBase<M128A>
    {
        M128A();

        uint64_t Low() const { return m128a_.Low; }
        int64_t High() const { return m128a_.High; }

        void Set(_M128A const& m128a);

    private:
        _M128A m128a_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct M128A : M128AT<M128A, implementation::M128A>
    {
    };
}
