#include "pch.h"
#include "HeaderPage.xaml.h"
#if __has_include("HeaderPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HeaderPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MiniDumpExplorer::implementation
{
    HeaderPage::HeaderPage()
    {
        InitializeComponent();
    }
}
