#include "dump_mini_dump_app_info.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/handle_data_stream.h"
#include "DbgHelpUtils/handle_operation_list_stream.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/system_info_stream.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/token_info_list_stream.h"

using namespace std;
using namespace std::string_view_literals;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

std::wstring_view get_type_name(std::wstring_view const& type_name);

std::wstring_view get_type_name(std::wstring_view const& type_name)
{
    if (type_name.empty())
    {
        return L"<unnamed>"sv;
    }

    return type_name;
}

void dump_mini_dump_token_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index, dump_file_options const& options)
{
    token_info_list_stream const token_list{mini_dump, index};

    if (!token_list.found())
    {
        log << L"TokenStream not found!\n";
        return;
    }

    if (!token_list.is_valid())
    {
        log << L"TokenStream version unknown!\n";
        return;
    }

    log <<std::format( L"TokenListEntries: {}\n", token_list.size());
    for (size_t i = 0; auto const& entry : token_list.list())
    {
        using namespace size_units::base_16;
        log << std::format(L" [{}]:\n", i);
        log << std::format(L"   TokenId: {}\n", to_hex(entry->TokenId));
        log << std::format(L"   TokenHandle: {}\n", to_hex(entry->TokenHandle));
        log << std::format(L"   TokenSize: {0} ({1})\n", locale_formatting::to_wstring(entry->TokenSize), to_wstring(bytes{entry->TokenSize}));
        log << std::format(L"   Data Size: {0} ({1})\n", locale_formatting::to_wstring(entry.size()), to_wstring(bytes{entry.size()}));

        if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(log, entry.data(), options.hex_dump_memory_size(entry.size()), 3);
            log << L'\n';
        }

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_function_table_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                               dump_file_options const& options)
{
    function_table_stream const function_table{mini_dump, index};

    if (!function_table.found())
    {
        log << L"FunctionTableStream not found!\n";
        return;
    }

    if (!function_table.is_valid())
    {
        log << L"FunctionTableStream version unknown!\n";
        return;
    }

    using namespace size_units::base_16;
    log << std::format(L"NumberOfDescriptors: {}\n", locale_formatting::to_wstring(function_table.size()));
    for (size_t i = 0; auto const& table : function_table.list())
    {
        log << std::format(L" Function[{}]:\n", i);
        log << std::format(L"   MinimumAddress: {}\n", to_hex_full(table->MinimumAddress));
        log << std::format(L"   MaximumAddress: {}\n", to_hex_full(table->MaximumAddress));
        log << std::format(L"   BaseAddress: {}\n", to_hex_full(table->BaseAddress));

        log << L"   NativeDescriptor:\n";
        hex_dump::hex_dump(log, table.native_descriptor(), table.native_descriptor_size(), 5);
        log << L'\n';

        log << std::format(L"   EntryCount: {}\n", locale_formatting::to_wstring(table->EntryCount));
        switch (function_table.entry_type())
        {
        case function_table_entry_type::unknown:
            if (options.hex_dump_memory_data())
            {
                for (auto const& entry : table.list())
                {
                    log << std::format(L"     Entry[{}]:\n", locale_formatting::to_wstring(entry.index()));
                    hex_dump::hex_dump(log, entry.data(), options.hex_dump_memory_size(entry.size()), 7);
                    log << L'\n';
                }
            }
            break;
        case function_table_entry_type::fpo_data:
            for (auto const& entry : table.list())
            {
                auto const& fpo_data = entry.fpo_data();
                log << std::format(L"     Entry[{}]:\n", locale_formatting::to_wstring(entry.index()));
                log << std::format(L"       ulOffStart: {}\n", to_hex(fpo_data.ulOffStart));
                log << std::format(L"       cbProcSize: {}\n", to_hex(fpo_data.cbProcSize));
                log << std::format(L"       cdwLocals: {}\n", to_hex(fpo_data.cdwLocals));
                log << std::format(L"       cdwParams: {}\n", to_hex(fpo_data.cdwParams));
                log << std::format(L"       cbProlog: {}\n", locale_formatting::to_wstring(fpo_data.cbProlog));
                // ReSharper disable once StringLiteralTypo
                log << std::format(L"       cbRegs: {}\n", locale_formatting::to_wstring(fpo_data.cbRegs));
                log << std::format(L"       fHasSEH: {}\n", locale_formatting::to_wstring(fpo_data.fHasSEH));
                log << std::format(L"       fUseBP: {}\n", locale_formatting::to_wstring(fpo_data.fUseBP));
                log << std::format(L"       cbFrame: {}\n", locale_formatting::to_wstring(fpo_data.cbFrame));
            }
            break;
        case function_table_entry_type::image_function_entry:
            for (auto const& entry : table.list())
            {
                auto const& image_function_entry = entry.image_function_entry();
                log << std::format(L"     Entry[{}]:\n", locale_formatting::to_wstring(entry.index()));
                log << std::format(L"       StartingAddress: {}\n", to_hex(image_function_entry.StartingAddress));
                log << std::format(L"       EndingAddress: {}\n", to_hex(image_function_entry.EndingAddress));
                log << std::format(L"       EndOfPrologue: {}\n", to_hex(image_function_entry.EndOfPrologue));
            }
            break;
        case function_table_entry_type::image_function_entry_64:
            for (auto const& entry : table.list())
            {
                auto const& image_function_entry_64 = entry.image_function_entry_64();
                log << std::format(L"     Entry[{}]:\n", locale_formatting::to_wstring(entry.index()));
                log << std::format(L"       StartingAddress: {}\n", to_hex(image_function_entry_64.StartingAddress));
                log << std::format(L"       EndingAddress: {}\n", to_hex(image_function_entry_64.EndingAddress));
                log << std::format(L"       EndOfPrologue/UnwindInfoAddress: {}\n", to_hex(image_function_entry_64.EndOfPrologue));
            }
            break;
        }
        log << L'\n';

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_handle_operation_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                                      dump_file_options const& options,
                                                      dbg_help::symbol_engine& symbol_engine)
{
    handle_operation_list_stream const handle_operation_list{mini_dump, index};

    if (!handle_operation_list.found())
    {
        log << L"HandleOperationListStream not found!\n";
        return;
    }

    if (!handle_operation_list.is_valid())
    {
        log << L"HandleOperationListStream version unknown!\n";
        return;
    }

    system_info_stream const system_info{mini_dump};

    using namespace size_units::base_16;
    log << std::format(L"NumberOfEntries: {}\n", locale_formatting::to_wstring(handle_operation_list.size()));
    for (size_t i = 0; auto const& entry : handle_operation_list.list())
    {
        log << std::format(L" Handle[{}]:\n", locale_formatting::to_wstring(i));
        log << std::format(L"   Handle: {}\n", to_hex_full(entry->Handle));
        log << std::format(L"   ProcessId: {}\n", to_hex(entry->ProcessId));
        log << std::format(L"   OperationType: {0} ({1})\n", system_info_utils::handle_trace_operation_to_string(entry->OperationType), locale_formatting::to_wstring(entry->OperationType));
        log << L"   BackTraceInformation:\n";
        log << std::format(L"     Index: {}\n", locale_formatting::to_wstring(entry->BackTraceInformation.Index));

        if (options.display_symbols())
        {
            hex_dump_stack_raw(log
                , mini_dump
                , symbol_engine
                , 0
                , entry->BackTraceInformation.ReturnAddresses
                , entry->BackTraceInformation.Depth
                , stream_stack_dump::is_x86_target_t{system_info.is_x86()}
                , 6);
        }
        else
        {
            for (size_t stack_index = 0; stack_index < entry->BackTraceInformation.Depth; ++stack_index)
            {
                log << std::format(L"      [{0}]: {1}\n", locale_formatting::to_wstring(stack_index), to_hex_full(entry->BackTraceInformation.ReturnAddresses[index]));
            }
        }
        log << L'\n';
        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_handle_data_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    handle_data_stream const handle_data{mini_dump, index};

    if (!handle_data.found())
    {
        log << L"HandleDataStream not found!\n";
        return;
    }

    if (!handle_data.is_valid())
    {
        log << L"HandleDataStream version unknown!\n";
        return;
    }

    log << std::format(L"NumberOfDescriptors: {}\n", locale_formatting::to_wstring(handle_data.size()));
    for (auto const& handle_type_total : handle_data.handle_type_totals())
    {
        log << std::format(L"  {0}: {1}\n", get_type_name(handle_type_total.first), locale_formatting::to_wstring(handle_type_total.second));
    }
    log << L'\n';

    auto const types = options.filter_values(L"type");

    for (size_t i{0}; auto const& entry : handle_data.list())
    {
        if (!types.empty() && ranges::find_if(types,
                                              [name = get_type_name(entry.type_name())](auto const& type_match)
                                              {
                                                  return filesystem_utils::wildcard_match(name, type_match);
                                              }) == types.end())
        {
            ++i;
            continue;
        }

        auto const& descriptor = entry.descriptor();

        log << std::format(L" Handle[{}]:\n", locale_formatting::to_wstring(i));
        log << std::format(L"   Handle: {}\n", to_hex_full(descriptor.Handle));
        log << std::format(L"   TypeName: {}\n", get_type_name(entry.type_name()));
        log << std::format(L"   ObjectName: {}\n", entry.object_name());
        log << std::format(L"   Attributes: {}\n", to_hex(descriptor.Attributes));
        log << std::format(L"   GrantedAccess: {}\n", to_hex(descriptor.GrantedAccess));
        log << std::format(L"   HandleCount: {}\n", locale_formatting::to_wstring(descriptor.HandleCount));
        log << std::format(L"   PointerCount: {}\n", locale_formatting::to_wstring(descriptor.PointerCount));

        if (entry.handle_descriptor_version() >= 2)
        {
            for (size_t object_i{0}; auto const& handle_object_info : entry.list())
            {
                log << std::format(L"   Object[{}]:\n", locale_formatting::to_wstring(object_i));
                log << std::format(L"     InfoType: {0} ({1})\n", system_info_utils::handle_object_information_type_to_string(handle_object_info->InfoType), locale_formatting::to_wstring(handle_object_info->InfoType));

                hex_dump::hex_dump(log, handle_object_info.data(), handle_object_info->SizeOfInfo, 5);

                ++object_i;
                log << L'\n';
            }
        }
        else
        {
            log << L'\n';
        }
        ++i;
    }
}

void dump_mini_dump_ipt_trace_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    // as yet I haven't found a way to create a dump file with IPT support
    // I would assume this is just a dump of the Intel Processor Trace output
    // If I get examples will look at using libipt to decode trace
    auto const& entry = mini_dump.directory()[index];
    auto const* stream_data = mini_dump.rva32(entry.Location);
    hex_dump::hex_dump(log, stream_data, entry.Location.DataSize, 1);
}
