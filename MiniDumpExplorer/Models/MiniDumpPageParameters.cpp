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
    MiniDumpPageParameters::MiniDumpPageParameters(MiniDumpExplorer::MiniDumpPage const& miniDump, hstring const& findItemTag, hstring const& navigationItemTag, uint32_t const streamIndex, uint32_t const streamSubType, uint32_t const streamSubIndex)
        : miniDump_(miniDump)
        , findItemTag_(findItemTag)
        , navigationItemTag_(navigationItemTag)
        , streamIndex_(streamIndex)
        , streamSubType_(streamSubType)
        , streamSubIndex_(streamSubIndex)
    {
    }
}
