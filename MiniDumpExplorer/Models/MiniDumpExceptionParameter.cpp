#include "pch.h"
#include "MiniDumpExceptionParameter.h"

#if __has_include("MiniDumpExceptionParameter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpExceptionParameter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpExceptionParameter::MiniDumpExceptionParameter(uint32_t const index, uint64_t const parameter)
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Parameter"
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
        , index_(index)
        , parameter_(parameter)
    {
    }
}
