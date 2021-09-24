#include "dump_mini_dump_module.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/guid_utils.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
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

void dump_mini_dump_module_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    module_list_stream const module_list{mini_dump, index};

    if (!module_list.found())
    {
        log << L"ModuleListStream not found!\n";
        return;
    }

    using namespace size_units::base_16;

    auto const modules = options.filter_values(L"module");
    auto const module_bases = vector_to_hash_set<uint64_t>(options.filter_values(L"module_base"s));

    log << std::format(L"NumberOfModules: {}\n", locale_formatting::to_wstring(module_list.module_list().NumberOfModules));
    for (size_t i = 0; auto const& stream_module : module_list.list())
    {
        std::filesystem::path p{stream_module.name()};
        auto const any_match = modules.empty() && module_bases.empty();
        auto const modules_match = !modules.empty() && ranges::find_if(modules,
                                                                       [name = p.filename().wstring()](
                                                                       auto const& name_match)
                                                                       {
                                                                           return filesystem_utils::wildcard_match(
                                                                               name, name_match);
                                                                       }) != modules.end();
        if (auto const module_bases_match = !module_bases.empty() && ranges::find(module_bases, stream_module->BaseOfImage) != module_bases.end(); !any_match && !modules_match && !module_bases_match)
        {
            ++i;
            continue;
        }

        log << std::format(L" [{0}]: {1}\n", locale_formatting::to_wstring(i), stream_module.name());
        log << std::format(L"   Base: {}\n", to_hex_full(stream_module->BaseOfImage));
        log << std::format(L"   CheckSum: {}\n", to_hex(stream_module->CheckSum));
        log << std::format(L"   Size: {0} ({1})\n", locale_formatting::to_wstring(stream_module->SizeOfImage), to_wstring(bytes{stream_module->SizeOfImage}));
        log << std::format(L"   Timestamp [local: {0}] [UTC: {1}]\n", time_utils::to_local_time(stream_module->TimeDateStamp), time_utils::to_utc_time(stream_module->TimeDateStamp));
        log << L"   VersionInfo: \n";
        log << std::format(L"     Signature: {}\n", to_hex(stream_module->VersionInfo.dwSignature));
        log << std::format(L"     Struct Version: {}\n", system_info_utils::version_info_to_string(stream_module->VersionInfo.dwStrucVersion));
        log << std::format(L"     File Flags: {0} - {1}\n", to_hex(stream_module->VersionInfo.dwFileFlags), system_info_utils::version_file_flags_to_string(stream_module->VersionInfo.dwFileFlags, stream_module->VersionInfo.dwFileFlagsMask));
        log << std::format(L"     File Flags Mask: {}\n", to_hex(stream_module->VersionInfo.dwFileFlagsMask));
        log << std::format(L"     File Type: {0} - {1}\n", to_hex(stream_module->VersionInfo.dwFileType), system_info_utils::version_file_type_to_string(stream_module->VersionInfo.dwFileType, stream_module->VersionInfo.dwFileSubtype));
        log << std::format(L"     File Subtype: {}\n", to_hex(stream_module->VersionInfo.dwFileSubtype));
        log << std::format(L"     File OS: {0} - {1}\n", to_hex(stream_module->VersionInfo.dwFileOS), system_info_utils::version_file_os_to_string(stream_module->VersionInfo.dwFileOS));
        log << std::format(L"     File Date Raw : {0}.{1}\n", stream_module->VersionInfo.dwFileDateMS, stream_module->VersionInfo. dwFileDateLS);
        if (stream_module->VersionInfo.dwFileDateMS != 0 && stream_module->VersionInfo.dwFileDateLS != 0)
        {
            FILETIME ft;
            ft.dwHighDateTime = stream_module->VersionInfo.dwFileDateMS;
            ft.dwLowDateTime = stream_module->VersionInfo.dwFileDateLS;
            auto const time_stamp = time_utils::filetime_to_time_t(ft);
            log << std::format(L"     File Date: [local: {0}] [UTC: {1}]\n", time_utils::to_local_time(time_stamp), time_utils::to_utc_time(time_stamp));
        }
        log << std::format(L"     File Version: {}\n", system_info_utils::version_info_to_string(stream_module->VersionInfo.dwFileVersionMS, stream_module->VersionInfo.dwFileVersionLS));
        log << std::format(L"     Product Version: {}\n", system_info_utils::version_info_to_string(stream_module->VersionInfo.dwProductVersionMS, stream_module->VersionInfo.dwProductVersionLS));
        log << std::format(L"   CvRecord (size): {0} ({1})\n", locale_formatting::to_wstring(stream_module->CvRecord.DataSize), to_wstring(bytes{stream_module->CvRecord.DataSize}));

        if (stream_module.pdb_info().is_valid())
        {
            log << L"     PDB Info:\n";
            log << std::format(L"       Signature: {}\n", guid_utils::to_string(stream_module.pdb_info().get_signature()));
            log << std::format(L"       Age: {}\n", locale_formatting::to_wstring(stream_module.pdb_info().get_age()));
            log << std::format(L"       Path: {}\n", stream_module.pdb_info().get_pdb_file_name());
        }

        log << std::format(L"   MiscRecord (size): {0} ({1})\n", locale_formatting::to_wstring(stream_module->MiscRecord.DataSize), to_wstring(bytes{stream_module->MiscRecord.DataSize}));
        log << std::format(L"   Reserved0: {}\n", to_hex(stream_module->Reserved0));
        log << std::format(L"   Reserved1: {}\n", to_hex(stream_module->Reserved1));

        if (options.hex_dump_memory_data())
        {
            if (stream_module->CvRecord.DataSize && !stream_module.pdb_info().is_valid())
            {
                log << L"   CvRecord:\n";
                hex_dump::hex_dump(log, stream_module.cv_record(), options.hex_dump_memory_size(stream_module->CvRecord.DataSize), 5);
                log << L'\n';
            }

            if (stream_module->MiscRecord.DataSize)
            {
                log << L"   MiscRecord:\n";
                hex_dump::hex_dump(log, stream_module.misc_record(), options.hex_dump_memory_size(stream_module->MiscRecord.DataSize), 5);
                log << L'\n';
            }
        }

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_unloaded_module_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    unloaded_module_list_stream const module_list{mini_dump, index};

    if (!module_list.found())
    {
        log << L"UnloadedModuleListStream not found!\n";
        return;
    }

    if (!module_list.is_valid())
    {
        log << L"UnloadedModuleListStream version unknown!\n";
        return;
    }

    log << std::format(L"NumberOfUnloadedModules: {}\n", locale_formatting::to_wstring(module_list.size()));
    for (size_t i = 0; auto const& module : module_list.list())
    {
        using namespace size_units::base_16;
        log << std::format(L" [{0}]: {1}\n", locale_formatting::to_wstring(i), module.name());
        log << std::format(L"   Base: {}\n", to_hex_full(module->BaseOfImage));
        log << std::format(L"   CheckSum: {}\n", locale_formatting::to_wstring(module->CheckSum));
        log << std::format(L"   Size: {0} ({1})\n", locale_formatting::to_wstring(module->SizeOfImage), to_wstring(bytes{module->SizeOfImage}));
        log << std::format(L"   Timestamp [local: {0}] [UTC: {1}]\n", time_utils::to_local_time(module->TimeDateStamp), time_utils::to_utc_time(module->TimeDateStamp));

        ++i;
    }
    log << L'\n';
}
