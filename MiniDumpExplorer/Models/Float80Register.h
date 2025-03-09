#pragma once

#include "Float80Register.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Float80;

    struct Float80Register : Float80RegisterT<Float80Register>, GlobalOptionsNotifyPropertyChangedBase<Float80Register>
    {
        Float80Register();

        hstring Name() const { return name_; }

        MiniDumpExplorer::Float80 Value() const { return value_; }

        void Set(std::wstring const& name, MiniDumpExplorer::Float80 const& value);

    private:
        hstring name_{};
        MiniDumpExplorer::Float80 value_{ nullptr };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Float80Register : Float80RegisterT<Float80Register, implementation::Float80Register>
    {
    };
}
