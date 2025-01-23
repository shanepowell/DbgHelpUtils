#include "pch.h"
#include "MemoryInfoListStreamEntry.h"

#include "DbgHelpUtils/memory_info_utils.h"

#if __has_include("MemoryInfoListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryInfoListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MemoryInfoListStreamEntry::MemoryInfoListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"BaseAddress",
                L"AllocationBase",
                L"State",
                L"Protect",
                L"Type",
            },
            {
                L"RegionSize",
            },
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void MemoryInfoListStreamEntry::Set(uint32_t const index, MINIDUMP_MEMORY_INFO const* memory_info)
    {
        index_ = index;
        memory_info_ = memory_info;

        stateString_ = dlg_help_utils::memory_info_utils::memory_state_to_string(memory_info_->State);
        typeString_ = dlg_help_utils::memory_info_utils::memory_type_to_string(memory_info_->Type);

        fill_protect_option_list(memory_info_->AllocationProtect, allocationProtectListString_, allocationProtectList_);
        fill_protect_option_list(memory_info_->Protect, protectListString_, protectList_);
    }

    void MemoryInfoListStreamEntry::fill_protect_option_list(uint32_t const protect_options, hstring& options_string, Windows::Foundation::Collections::IObservableVector<hstring> const& options_list)
    {
        options_string = dlg_help_utils::memory_info_utils::memory_protection_options_to_string(protect_options);
        options_list.Clear();
        for (auto option : dlg_help_utils::memory_info_utils::memory_protection_options_to_strings(protect_options))
        {
            options_list.Append(option);
        }
    }
}
