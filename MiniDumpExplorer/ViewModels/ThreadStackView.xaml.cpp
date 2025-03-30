#include "pch.h"
#include "ThreadStackView.xaml.h"

#include "Helpers/UIHelper.h"
#include "Models/ThreadStackEntry.h"

#if __has_include("ThreadStackView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadStackView.g.cpp"// NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadStackView::ThreadStackView() = default;

    void ThreadStackView::InitializeComponent()
    {
        ThreadStackViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void ThreadStackView::OnTreeViewItemExpanding([[maybe_unused]] Controls::TreeView const& sender, Controls::TreeViewExpandingEventArgs const& args)
    {
        if (auto const item = args.Item().as<ThreadStackEntry>();
            item && !item->AreChildrenLoaded())
        {
            item->LoadChildren();
        }
    }

    void ThreadStackView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            stackStartOfMemoryRange(),
            stackEndOfMemoryRange()
        );

        UIHelper::CreateStandardSizeNumberMenu(
            stackDataSize()
        );
    }
}
