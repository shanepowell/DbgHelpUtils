#include "pch.h"
#include "Float80.h"

#if __has_include("Float80.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Float80.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    Float80::Float80()
    {
    }

    void Float80::Set(dlg_help_utils::float80_t const& value)
    {
        value_ = value;
    }
}
