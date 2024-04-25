#include "pch.h"
#include "UnloadedModuleListStreamEntry.h"

#if __has_include("UnloadedModuleListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "UnloadedModuleListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    UnloadedModuleListStreamEntry::UnloadedModuleListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"BaseOfImage",
                L"CheckSum",
                L"Reserved0",
                L"Reserved1",
                L"BuildFileHash",
            }, 
            {
                L"SizeOfImage"
            }, 
            {
            },
            {
            })
    {
    }

    void UnloadedModuleListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_unloaded_module const& module)
    {
        index_ = index;
        module_ = module;
        name_ = module.filename_with_extension();
        path_ = module.name();
    }
}
