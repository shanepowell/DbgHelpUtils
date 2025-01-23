#pragma once

#include "YmmRegisterView.g.h"

#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct YmmRegister;

    struct YmmRegisterView : YmmRegisterViewT<YmmRegisterView>, NotifyPropertyChangedBase<YmmRegisterView>
    {
        YmmRegisterView();

        MiniDumpExplorer::YmmRegister Value() const { return ymmRegister_; }
        void Value(MiniDumpExplorer::YmmRegister const& value) { ymmRegister_ = value; }

    private:
        MiniDumpExplorer::YmmRegister ymmRegister_{ nullptr };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct YmmRegisterView : YmmRegisterViewT<YmmRegisterView, implementation::YmmRegisterView>
    {
    };
}
