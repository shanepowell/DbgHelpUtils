#include "pch.h"
#include "M128A.h"
#if __has_include("M128A.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "M128A.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    M128A::M128A() = default;

    void M128A::Set(_M128A const& m128a)
    {
        m128a_ = m128a;
    }
}
