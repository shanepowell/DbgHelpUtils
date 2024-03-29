#include "pch.h"
#include "MiniDumpPageParameters.h"

#if __has_include("MiniDumpPageParameters.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpPageParameters.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpPageParameters::MiniDumpPageParameters(MiniDumpExplorer::MiniDumpPage const& miniDump, hstring const& navigationItemTag, uint32_t const streamIndex)
        : miniDump_(miniDump)
        , navigationItemTag_(navigationItemTag)
        , streamIndex_(streamIndex)
    {
    }
}
