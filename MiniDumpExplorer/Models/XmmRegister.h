#pragma once

#include "XmmRegister.g.h"

#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;

    struct XmmRegister : XmmRegisterT<XmmRegister>, NotifyPropertyChangedBase<XmmRegister>
    {
        XmmRegister();

        hstring Name() const { return name_; }

        MiniDumpExplorer::M128A Xmm() const { return xmm_; }

        void Set(std::wstring const& name, MiniDumpExplorer::M128A const& xmm);

    private:
        hstring name_{};
        MiniDumpExplorer::M128A xmm_{ nullptr };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct XmmRegister : XmmRegisterT<XmmRegister, implementation::XmmRegister>
    {
    };
}
