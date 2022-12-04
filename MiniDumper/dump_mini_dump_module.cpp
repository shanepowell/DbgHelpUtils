#include "dump_mini_dump_module.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/file_version_info.h"
#include "DbgHelpUtils/guid_utils.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/stream_module.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_unloaded_module.h"
#include "DbgHelpUtils/symbol_engine.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"
#include "DbgHelpUtils/vector_to_hash_set.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

namespace
{
    void dump_vs_fixed_file_info(std::wostream& log, VS_FIXEDFILEINFO const& file_info, int const prefix_length)
    {
        std::wstring prefix(prefix_length, L' ');
        log << std::format(L"{0}Signature: {1}\n", prefix, to_hex(file_info.dwSignature));
        log << std::format(L"{0}Struct Version: {1}\n", prefix, system_info_utils::version_info_to_string(file_info.dwStrucVersion));
        log << std::format(L"{0}File Flags: {1} - {2}\n", prefix, to_hex(file_info.dwFileFlags), system_info_utils::version_file_flags_to_string(file_info.dwFileFlags, file_info.dwFileFlagsMask));
        log << std::format(L"{0}File Flags Mask: {1}\n", prefix, to_hex(file_info.dwFileFlagsMask));
        log << std::format(L"{0}File Type: {1} - {2}\n", prefix, to_hex(file_info.dwFileType), system_info_utils::version_file_type_to_string(file_info.dwFileType, file_info.dwFileSubtype));
        log << std::format(L"{0}File Subtype: {1}\n", prefix, to_hex(file_info.dwFileSubtype));
        log << std::format(L"{0}File OS: {1} - {2}\n", prefix, to_hex(file_info.dwFileOS), system_info_utils::version_file_os_to_string(file_info.dwFileOS));
        log << std::format(L"{0}File Date Raw : {1}.{2}\n", prefix, file_info.dwFileDateMS, file_info.dwFileDateLS);
        if (file_info.dwFileDateMS != 0 && file_info.dwFileDateLS != 0)
        {
            FILETIME ft;
            ft.dwHighDateTime = file_info.dwFileDateMS;
            ft.dwLowDateTime = file_info.dwFileDateLS;
            auto const time_stamp = time_utils::filetime_to_time_t(ft);
            log << std::format(L"{0}File Date: [local: {1}] [UTC: {2}]\n", prefix, time_utils::to_local_time(time_stamp), time_utils::to_utc_time(time_stamp));
        }
        log << std::format(L"{0}File Version: {1}\n", prefix, system_info_utils::version_info_to_string(file_info.dwFileVersionMS, file_info.dwFileVersionLS));
        log << std::format(L"{0}Product Version: {1}\n", prefix, system_info_utils::version_info_to_string(file_info.dwProductVersionMS, file_info.dwProductVersionLS));
    }

    void dump_version_info(std::wostream& log, file_version_info const& version_info, int const prefix_length)
    {
        std::wstring prefix(prefix_length, L' ');
        log << std::format(L"{0}Original Filename: {1}\n", prefix, version_info.original_filename());
        log << std::format(L"{0}Company Name: {1}\n", prefix, version_info.company_name());
        log << std::format(L"{0}File Description: {1}\n", prefix, version_info.file_description());
        log << std::format(L"{0}Comments: {1}\n", prefix, version_info.comments());
        log << std::format(L"{0}Copyright: {1}\n", prefix, version_info.legal_copyright());
        log << std::format(L"{0}Trademarks: {1}\n", prefix, version_info.legal_trademarks());
        log << std::format(L"{0}Internal Name: {1}\n", prefix, version_info.internal_name());
        log << std::format(L"{0}Product Name: {1}\n", prefix, version_info.product_name());
        log << std::format(L"{0}Product Version: {1}\n", prefix, version_info.product_version());
        log << std::format(L"{0}File Version: {1}\n", prefix,  version_info.file_version());
        log << std::format(L"{0}Private Build: {1}\n", prefix, version_info.private_build());
        log << std::format(L"{0}Special Build: {1}\n", prefix, version_info.special_build());

        if(auto const file_info = version_info.file_info(); file_info.has_value())
        {
            log << L'\n';
            dump_vs_fixed_file_info(log, *file_info, prefix_length);
        }
    }
}


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
        dump_vs_fixed_file_info(log, stream_module->VersionInfo, 5);
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

void dump_mini_dump_loaded_modules(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    for (auto const& loaded_module : symbol_engine.loaded_modules())
    {
        auto const info = symbol_engine.get_module_information(loaded_module);
        auto const& module_image_path = symbol_engine.get_module_image_path(loaded_module);
        log << loaded_module << L":\n";
        log << std::format(L"    SymType: {}\n", system_info_utils::sym_type_to_string(info.SymType));
        log << std::format(L"    ModuleName: {}\n", info.ModuleName);
        log << std::format(L"    ImageName: {}\n", info.ImageName);
        log << std::format(L"    ModuleImagePath: {}\n", module_image_path);
        log << std::format(L"    LoadedImageName: {}\n", info.LoadedImageName);
        log << std::format(L"    LoadedPdbName: {}\n", info.LoadedPdbName);
        log << std::format(L"    CheckSum: {}\n", to_hex(info.CheckSum));
        log << std::format(L"    ImageSize: {}\n", to_hex(info.ImageSize));
        log << std::format(L"    MachineType: {}\n", to_hex(info.MachineType));
        log << std::format(L"    PdbSig70: {}\n", guid_utils::to_string(info.PdbSig70));
        log << std::format(L"    PdbAge: {}\n", info.PdbAge);
        log << std::format(L"    PdbUnmatched: {}\n", info.PdbUnmatched);
        log << std::format(L"    DbgUnmatched: {}\n", info.DbgUnmatched);

        if(file_version_info const version_info{module_image_path};
            version_info.has_version_info())
        {
            log << L'\n';
            dump_version_info(log, version_info, 4);
        }
        else if(auto memory = peb.walker().get_process_memory_stream(info.BaseOfImage, info.ImageSize);
                !memory.eof())
        {
            if(file_version_info const memory_version_info{std::move(memory)};
                memory_version_info.has_version_info())
            {
                log << L'\n';
                dump_version_info(log, memory_version_info, 4);
            }
        }

        log << L"\n";
    }
}
