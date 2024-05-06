#include "pch.h"
#include "AvrfBackTraceInformation.h"

#if __has_include("AvrfBackTraceInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "AvrfBackTraceInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    AvrfBackTraceInformation::AvrfBackTraceInformation() = default;

    void AvrfBackTraceInformation::Set(AVRF_BACKTRACE_INFORMATION const* backtrace_information)
    {
        backtrace_information_ = backtrace_information;
    }
}
