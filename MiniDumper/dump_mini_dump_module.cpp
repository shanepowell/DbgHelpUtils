#include "dump_mini_dump_module.h"

#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/guid_utils.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/stream_module.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_unloaded_module.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"
#include "DbgHelpUtils/vector_to_hash_set.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

void dump_mini_dump_module_list_stream_data(mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    module_list_stream const module_list{mini_dump, index};

    if (!module_list.found())
    {
        wcout << L"ModuleListStream not found!\n";
        return;
    }

    using namespace size_units::base_10;

    auto const modules = options.filter_values(L"module");
    auto const module_bases = vector_to_hash_set<uint64_t>(options.filter_values(L"module_base"s));

    wcout << L"NumberOfModules: " << module_list.module_list().NumberOfModules << L'\n';
    size_t i = 0;
    for (auto const& stream_module : module_list.list())
    {
        std::filesystem::path p{stream_module.name()};
        auto const any_match = modules.empty() && module_bases.empty();
        auto const modules_match = !modules.empty() && std::find_if(modules.begin(), modules.end(),
                                                                    [name = p.filename().wstring()](
                                                                    auto const& name_match)
                                                                    {
                                                                        return filesystem_utils::wildcard_match(
                                                                            name, name_match);
                                                                    }) != modules.end();
        auto const module_bases_match = !module_bases.empty() && std::find(
            module_bases.begin(), module_bases.end(), stream_module->BaseOfImage) != module_bases.end();
        if (!any_match && !modules_match && !module_bases_match)
        {
            ++i;
            continue;
        }

        wcout << L" [" << i << "]: " << stream_module.name() << L'\n';
        wcout << L"   Base: " << to_hex_full(stream_module->BaseOfImage) << L'\n';
        wcout << L"   CheckSum: " << stream_module->CheckSum << L'\n';
        wcout << L"   Size: " << stream_module->SizeOfImage << L" (" << bytes{stream_module->SizeOfImage} << L")\n";
        wcout << L"   Timestamp [local: " << time_utils::to_local_time(stream_module->TimeDateStamp) << L"] [UTC: " <<
            time_utils::to_utc_time(stream_module->TimeDateStamp) << L"]\n";
        wcout << L"   VersionInfo: \n";
        wcout << L"     Signature: " << to_hex(stream_module->VersionInfo.dwSignature) << L'\n';
        wcout << L"     Struct Version: " << stream_module->VersionInfo.dwStrucVersion << L'\n';
        wcout << L"     File Flags: " << to_hex(stream_module->VersionInfo.dwFileFlags) << L'\n';
        wcout << L"     File Flags Mask: " << to_hex(stream_module->VersionInfo.dwFileFlagsMask) << L'\n';
        wcout << L"     File Type: " << to_hex(stream_module->VersionInfo.dwFileType) << L'\n';
        wcout << L"     File Subtype: " << to_hex(stream_module->VersionInfo.dwFileSubtype) << L'\n';
        wcout << L"     File OS: " << to_hex(stream_module->VersionInfo.dwFileOS) << L'\n';

        wcout << L"     File Date Raw : " << stream_module->VersionInfo.dwFileDateMS << L'.' << stream_module->VersionInfo.
            dwFileDateLS << L'\n';
        if (stream_module->VersionInfo.dwFileDateMS != 0 && stream_module->VersionInfo.dwFileDateLS != 0)
        {
            FILETIME ft;
            ft.dwHighDateTime = stream_module->VersionInfo.dwFileDateMS;
            ft.dwLowDateTime = stream_module->VersionInfo.dwFileDateLS;
            auto const time_stamp = time_utils::filetime_to_time_t(ft);
            wcout << L"     File Date: [local: " << time_utils::to_local_time(time_stamp) << L"] [UTC: " <<
                time_utils::to_utc_time(time_stamp) << L"]\n";
        }
        wcout << L"     File Version: " << system_info_utils::version_info_to_string(
            stream_module->VersionInfo.dwFileVersionMS, stream_module->VersionInfo.dwFileVersionLS) << L'\n';
        wcout << L"     Product Version: " << system_info_utils::version_info_to_string(
            stream_module->VersionInfo.dwProductVersionMS, stream_module->VersionInfo.dwProductVersionLS) << L'\n';
        wcout << L"   CvRecord (size): " << stream_module->CvRecord.DataSize << L" (" << bytes{stream_module->CvRecord.DataSize} <<
            L")\n";

        if (stream_module.pdb_info().is_valid())
        {
            wcout << L"     PDB Info:\n";
            wcout << L"       Signature: " << guid_utils::to_string(stream_module.pdb_info().get_signature()) << L'\n';
            wcout << L"       Age: " << stream_module.pdb_info().get_age() << L'\n';
            wcout << L"       Path: " << stream_module.pdb_info().get_pdb_file_name() << L'\n';
        }

        wcout << L"   MiscRecord (size): " << stream_module->MiscRecord.DataSize << L" (" << bytes{stream_module->MiscRecord.DataSize}
            << L")\n";
        wcout << L"   Reserved0: " << stream_module->Reserved0 << L'\n';
        wcout << L"   Reserved1: " << stream_module->Reserved1 << L'\n';

        if (options.hex_dump_memory_data())
        {
            if (stream_module->CvRecord.DataSize && !stream_module.pdb_info().is_valid())
            {
                wcout << L"   CvRecord:\n";
                hex_dump::hex_dump(wcout, stream_module.cv_record(), stream_module->CvRecord.DataSize, 5);
                wcout << L'\n';
            }

            if (stream_module->MiscRecord.DataSize)
            {
                wcout << L"   MiscRecord:\n";
                hex_dump::hex_dump(wcout, stream_module.misc_record(), stream_module->MiscRecord.DataSize, 5);
                wcout << L'\n';
            }
        }

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_unloaded_module_list_stream_data(mini_dump const& mini_dump, size_t const index)
{
    unloaded_module_list_stream const module_list{mini_dump, index};

    if (!module_list.found())
    {
        wcout << L"UnloadedModuleListStream not found!\n";
        return;
    }

    if (!module_list.is_valid())
    {
        wcout << L"UnloadedModuleListStream version unknown!\n";
        return;
    }

    wcout << L"NumberOfUnloadedModules: " << module_list.size() << L'\n';
    size_t i = 0;
    for (auto const& module : module_list.list())
    {
        wcout << L" [" << i << "]: " << module.name() << L'\n';
        wcout << L"   Base: " << to_hex_full(module->BaseOfImage) << L'\n';
        wcout << L"   CheckSum: " << module->CheckSum << L'\n';
        using namespace size_units::base_10;
        wcout << L"   Size: " << module->SizeOfImage << L" (" << bytes{module->SizeOfImage} << L")\n";
        wcout << L"   Timestamp [local: " << time_utils::to_local_time(module->TimeDateStamp) << L"] [UTC: " <<
            time_utils::to_utc_time(module->TimeDateStamp) << L"]\n";

        ++i;
    }
    wcout << L'\n';
}
