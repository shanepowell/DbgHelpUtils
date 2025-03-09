#include "pch.h"
#include "Float80Register.h"

#if __has_include("Float80Register.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Float80Register.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    Float80Register::Float80Register()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Value"
            },
            {},
            {},
            {},
            {},
            {}
            )
    {
    }

    void Float80Register::Set(std::wstring const& name, MiniDumpExplorer::Float80 const& value)
    {
        name_ = name;
        value_ = value;
    }
}
