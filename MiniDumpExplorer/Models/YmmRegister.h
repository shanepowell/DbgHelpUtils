#pragma once

#include "YmmRegister.g.h"

#include "DbgHelpUtils/xstate_reader.h"
#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;

    struct YmmRegister : YmmRegisterT<YmmRegister>, NotifyPropertyChangedBase<YmmRegister>
    {
        YmmRegister();

        uint32_t Index() const { return index_; }

        MiniDumpExplorer::M128A Xmm() const { return xmm_; }
        MiniDumpExplorer::M128A Ymm() const { return ymm_; }

        void Set(dlg_help_utils::xstate_reader::ymm_register const& ymmRegister);

    private:
        uint32_t index_{};
        MiniDumpExplorer::M128A xmm_{ nullptr };
        MiniDumpExplorer::M128A ymm_{ nullptr };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct YmmRegister : YmmRegisterT<YmmRegister, implementation::YmmRegister>
    {
    };
}
