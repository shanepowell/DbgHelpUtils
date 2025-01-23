#include "pch.h"
#include "YmmRegisterView.xaml.h"

#if __has_include("YmmRegisterView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "YmmRegisterView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    YmmRegisterView::YmmRegisterView() = default;
}
