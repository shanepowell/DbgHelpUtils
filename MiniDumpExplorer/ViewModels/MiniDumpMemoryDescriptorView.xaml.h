#pragma once

#include "MiniDumpMemoryDescriptorView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpMemoryDescriptor;

    struct MiniDumpMemoryDescriptorView : MiniDumpMemoryDescriptorViewT<MiniDumpMemoryDescriptorView>
    {
        MiniDumpMemoryDescriptorView() = default;

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::MiniDumpMemoryDescriptor Value() const { return value_; }
        void Value(MiniDumpExplorer::MiniDumpMemoryDescriptor const& value) { value_ = value; }

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::MiniDumpMemoryDescriptor value_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpMemoryDescriptorView : MiniDumpMemoryDescriptorViewT<MiniDumpMemoryDescriptorView, implementation::MiniDumpMemoryDescriptorView>
    {
    };
}
