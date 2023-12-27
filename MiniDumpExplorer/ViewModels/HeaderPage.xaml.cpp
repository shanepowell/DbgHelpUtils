#include "pch.h"
#include "HeaderPage.xaml.h"
#if __has_include("HeaderPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HeaderPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    HeaderPage::HeaderPage()
    {
        InitializeComponent();
    }
}
