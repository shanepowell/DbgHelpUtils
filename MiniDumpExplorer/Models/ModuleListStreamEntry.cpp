#include "pch.h"
#include "ModuleListStreamEntry.h"

#include "CvInfoPdb70.h"
#include "DumpFileTimeStamp.h"
#include "MiniDumpLocationDescriptor.h"
#include "VsFixedFileInfo.h"

#if __has_include("ModuleListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ModuleListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ModuleListStreamEntry::ModuleListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Base",
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

    void ModuleListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_module const& module, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        index_ = index;
        module_ = module;
        name_ = module.filename_with_extension();
        path_ = module.name();

        fileInfo_.as<VsFixedFileInfo>()->Set(module_->VersionInfo, dump_file_timezone_info);
        pdbInfo_.as<CvInfoPdb70>()->Set(module.pdb_info());
        cvRecord_.as<MiniDumpLocationDescriptor>()->Set(module->CvRecord);
        miscRecord_.as<MiniDumpLocationDescriptor>()->Set(module->MiscRecord);
    }
}
