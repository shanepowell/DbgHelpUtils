#include "pch.h"
#include "XmmRegister.h"

#include "M128A.h"

#if __has_include("XmmRegister.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "XmmRegister.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    XmmRegister::XmmRegister() = default;

    void XmmRegister::Set(std::wstring const& name, MiniDumpExplorer::M128A const& xmm)
    {
        name_ = name;
        xmm_ = xmm;
    }
}
