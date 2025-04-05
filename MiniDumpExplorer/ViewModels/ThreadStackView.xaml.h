#pragma once

#include "ThreadStackView.g.h"

#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadStack;

    struct ThreadStackView : ThreadStackViewT<ThreadStackView>, NotifyPropertyChangedBase<ThreadStackView>
    {
        ThreadStackView();

        MiniDumpExplorer::ThreadStack Stack() const { return stack_; }
        void Stack(MiniDumpExplorer::ThreadStack const& value) { stack_ = value; }

        static void OnTreeViewItemExpanding(Microsoft::UI::Xaml::Controls::TreeView const& sender, Microsoft::UI::Xaml::Controls::TreeViewExpandingEventArgs const& args);

    private:
        MiniDumpExplorer::ThreadStack stack_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadStackView : ThreadStackViewT<ThreadStackView, implementation::ThreadStackView>
    {
    };
}
