#include "pch.h"
#include "FunctionStreamEntry.h"

#if __has_include("FunctionStreamEntry.g.cpp")
#include "FunctionStreamEntry.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    FunctionStreamEntry::FunctionStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"MinimumAddress",
                L"MaximumAddress"
                L"BaseAddress"
            }, 
            {
            },
            {
            },
            {
            })
    {
    }

    void FunctionStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_function_descriptor const& stream_function)
    {
        index_ = index;
        stream_function_ = stream_function;
    }
}
