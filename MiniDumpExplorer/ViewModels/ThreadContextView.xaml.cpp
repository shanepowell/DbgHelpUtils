#include "pch.h"
#include "ThreadContextView.xaml.h"


#if __has_include("ThreadContextView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadContextView.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadContextView::ThreadContextView() = default;
}
