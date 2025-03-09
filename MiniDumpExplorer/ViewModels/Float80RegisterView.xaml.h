#pragma once

#include "Float80RegisterView.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Float80Register;

    struct Float80RegisterView : Float80RegisterViewT<Float80RegisterView>, GlobalOptionsNotifyPropertyChangedBase<Float80RegisterView>
    {
        Float80RegisterView();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::Float80Register Value() const { return value_; }
        void Value(MiniDumpExplorer::Float80Register const& value);

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::Float80Register value_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Float80RegisterView : Float80RegisterViewT<Float80RegisterView, implementation::Float80RegisterView>
    {
    };
}
