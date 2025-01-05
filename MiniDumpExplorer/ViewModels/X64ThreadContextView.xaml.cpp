#include "pch.h"
#include "X64ThreadContextView.xaml.h"

#include "Helpers/UIHelper.h"

#if __has_include("X64ThreadContextView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X64ThreadContextView.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    X64ThreadContextView::X64ThreadContextView() = default;

    void X64ThreadContextView::InitializeComponent()
    {
        X64ThreadContextViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void X64ThreadContextView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            contextFlags()
        );
    }
}
