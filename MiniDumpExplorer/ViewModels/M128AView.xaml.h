#pragma once

#include "M128AView.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;

    struct M128AView : M128AViewT<M128AView>, GlobalOptionsNotifyPropertyChangedBase<M128AView>
    {
        M128AView();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::M128A Value() const { return value_; }
        void Value(MiniDumpExplorer::M128A const& value) { value_ = value; }

        static bool IsInt128View();
        static bool IsUInt128View();
        static bool IsInt64View();
        static bool IsUInt64View();
        static bool IsInt32View();
        static bool IsUInt32View();
        static bool IsInt16View();
        static bool IsUInt16View();
        static bool IsInt8View();
        static bool IsUInt8View();
        static bool IsFloat32View();
        static bool IsFloat64View();

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::M128A value_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct M128AView : M128AViewT<M128AView, implementation::M128AView>
    {
    };
}
