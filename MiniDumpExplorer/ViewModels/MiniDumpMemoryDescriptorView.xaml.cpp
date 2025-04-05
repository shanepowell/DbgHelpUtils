#include "pch.h"
#include "MiniDumpMemoryDescriptorView.xaml.h"

#include "Helpers/GlobalOptions.h"
#include "Helpers/UIHelper.h"

#if __has_include("MiniDumpMemoryDescriptorView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpMemoryDescriptorView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    void MiniDumpMemoryDescriptorView::InitializeComponent()
    {
        MiniDumpMemoryDescriptorViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void MiniDumpMemoryDescriptorView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            startOfMemoryRange(),
            endOfMemoryRange()
        );

        UIHelper::CreateStandardSizeNumberMenu(
            dataSize()
        );
    }
}
