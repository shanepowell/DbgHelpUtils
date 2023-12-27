#include "pch.h"
#include "StreamsPage.xaml.h"
#if __has_include("StreamsPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "StreamsPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

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
