#include "pch.h"
#include "StreamsPage.xaml.h"
#if __has_include("StreamsPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "StreamsPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MiniDumpExplorer::implementation
{
    StreamsPage::StreamsPage()
    {
        InitializeComponent();
    }

    void StreamsPage::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}
