#include "dump_mini_dump_app_info.h"

#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/handle_data_stream.h"
#include "DbgHelpUtils/handle_operation_list_stream.h"
#include "DbgHelpUtils/hex_dump.h"
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
        return L"none"sv;
    }

    return type_name;
}

void dump_mini_dump_token_stream_data(mini_dump const& mini_dump, size_t const index, dump_file_options const& options)
{
    token_info_list_stream const token_list{mini_dump, index};

    if (!token_list.found())
    {
        wcout << L"TokenStream not found!\n";
        return;
    }

    if (!token_list.is_valid())
    {
        wcout << L"TokenStream version unknown!\n";
        return;
    }

    wcout << L"TokenListEntries: " << token_list.size() << L'\n';
    for (size_t i = 0; auto const& entry : token_list.list())
    {
        using namespace size_units::base_10;
        wcout << L" [" << i << L"]:\n";
        wcout << L"   TokenId: " << entry->TokenId << L'\n';
        wcout << L"   TokenHandle: " << to_hex(entry->TokenHandle) << L'\n';
        wcout << L"   TokenSize: " << entry->TokenSize << L" (" << bytes{entry->TokenSize} << L")\n";
        wcout << L"   data size: " << entry.size() << L" (" << bytes{entry.size()} << L")\n";

        if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(wcout, entry.data(), options.hex_dump_memory_size(entry.size()), 3);
            wcout << L'\n';
        }

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_function_table_stream_data(mini_dump const& mini_dump, size_t const index,
                                               dump_file_options const& options)
{
    function_table_stream const function_table{mini_dump, index};

    if (!function_table.found())
    {
        wcout << L"FunctionTableStream not found!\n";
        return;
    }

    if (!function_table.is_valid())
    {
        wcout << L"FunctionTableStream version unknown!\n";
        return;
    }

    using namespace size_units::base_10;
    wcout << L"NumberOfDescriptors: " << function_table.size() << L'\n';
    for (size_t i = 0; auto const& table : function_table.list())
    {
        wcout << L" Function[" << i << L"]:\n";
        wcout << L"   MinimumAddress: " << to_hex_full(table->MinimumAddress) << L'\n';
        wcout << L"   MaximumAddress: " << to_hex_full(table->MaximumAddress) << L'\n';
        wcout << L"   BaseAddress: " << to_hex_full(table->BaseAddress) << L'\n';

        wcout << L"   NativeDescriptor:\n";
        hex_dump::hex_dump(wcout, table.native_descriptor(), table.native_descriptor_size(), 5);
        wcout << L'\n';

        wcout << L"   EntryCount: " << table->EntryCount << L'\n';
        switch (function_table.entry_type())
        {
        case function_table_entry_type::unknown:
            if (options.hex_dump_memory_data())
            {
                for (auto const& entry : table.list())
                {
                    wcout << L"     Entry[" << entry.index() << L"]:\n";
                    hex_dump::hex_dump(wcout, entry.data(), options.hex_dump_memory_size(entry.size()), 7);
                    wcout << L'\n';
                }
            }
            break;
        case function_table_entry_type::fpo_data:
            for (auto const& entry : table.list())
            {
                auto const& fpo_data = entry.fpo_data();
                wcout << L"     Entry[" << entry.index() << L"]:\n";
                wcout << L"       ulOffStart: " << to_hex(fpo_data.ulOffStart) << L'\n';
                wcout << L"       cbProcSize: " << to_hex(fpo_data.cbProcSize) << L'\n';
                wcout << L"       cdwLocals: " << to_hex(fpo_data.cdwLocals) << L'\n';
                wcout << L"       cdwParams: " << to_hex(fpo_data.cdwParams) << L'\n';
                wcout << L"       cbProlog: " << fpo_data.cbProlog << L'\n';
                // ReSharper disable once StringLiteralTypo
                wcout << L"       cbRegs: " << fpo_data.cbRegs << L'\n';
                wcout << L"       fHasSEH: " << fpo_data.fHasSEH << L'\n';
                wcout << L"       fUseBP: " << fpo_data.fUseBP << L'\n';
                wcout << L"       cbFrame: " << fpo_data.cbFrame << L'\n';
            }
            break;
        case function_table_entry_type::image_function_entry:
            for (auto const& entry : table.list())
            {
                auto const& image_function_entry = entry.image_function_entry();
                wcout << L"     Entry[" << entry.index() << L"]:\n";
                wcout << L"       StartingAddress: " << to_hex(image_function_entry.StartingAddress) << L'\n';
                wcout << L"       EndingAddress: " << to_hex(image_function_entry.EndingAddress) << L'\n';
                wcout << L"       EndOfPrologue: " << to_hex(image_function_entry.EndOfPrologue) << L'\n';
            }
            break;
        case function_table_entry_type::image_function_entry_64:
            for (auto const& entry : table.list())
            {
                auto const& image_function_entry_64 = entry.image_function_entry_64();
                wcout << L"     Entry[" << entry.index() << L"]:\n";
                wcout << L"       StartingAddress: " << to_hex(image_function_entry_64.StartingAddress) << L'\n';
                wcout << L"       EndingAddress: " << to_hex(image_function_entry_64.EndingAddress) << L'\n';
                wcout << L"       EndOfPrologue/UnwindInfoAddress: " << to_hex(image_function_entry_64.EndOfPrologue) <<
                    L'\n';
            }
            break;
        }
        wcout << L'\n';

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_handle_operation_list_stream_data(mini_dump const& mini_dump, size_t const index,
                                                      dump_file_options const& options,
                                                      dbg_help::symbol_engine& symbol_engine)
{
    handle_operation_list_stream const handle_operation_list{mini_dump, index};

    if (!handle_operation_list.found())
    {
        wcout << L"HandleOperationListStream not found!\n";
        return;
    }

    if (!handle_operation_list.is_valid())
    {
        wcout << L"HandleOperationListStream version unknown!\n";
        return;
    }

    system_info_stream const system_info{mini_dump};

    using namespace size_units::base_10;
    wcout << L"NumberOfEntries: " << handle_operation_list.size() << L'\n';
    for (size_t i = 0; auto const& entry : handle_operation_list.list())
    {
        wcout << L" Handle[" << i << L"]:\n";
        wcout << L"   Handle: " << to_hex_full(entry->Handle) << L'\n';
        wcout << L"   ProcessId: " << entry->ProcessId << L'\n';
        wcout << L"   OperationType: " << system_info_utils::handle_trace_operation_to_string(entry->OperationType) <<
            L" (" << entry->OperationType << L")\n";
        wcout << L"   BackTraceInformation:\n";
        wcout << L"     Index: " << entry->BackTraceInformation.Index << L'\n';

        if (options.display_symbols())
        {
            stream_stack_dump::hex_dump_stack_raw(wcout
                , mini_dump
                , symbol_engine
                , 0
                , entry->BackTraceInformation.ReturnAddresses
                , entry->BackTraceInformation.Depth
                , system_info.is_x86()
                , 6);
        }
        else
        {
            for (size_t stack_index = 0; stack_index < entry->BackTraceInformation.Depth; ++stack_index)
            {
                wcout << L"      [" << stack_index << L"]:" << to_hex_full(
                    entry->BackTraceInformation.ReturnAddresses[index]) << L'\n';
            }
        }
        wcout << L'\n';
        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_handle_data_stream_data(mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    handle_data_stream const handle_data{mini_dump, index};

    if (!handle_data.found())
    {
        wcout << L"HandleDataStream not found!\n";
        return;
    }

    if (!handle_data.is_valid())
    {
        wcout << L"HandleDataStream version unknown!\n";
        return;
    }

    wcout << L"NumberOfDescriptors: " << handle_data.size() << L'\n';
    for (auto const& handle_type_total : handle_data.handle_type_totals())
    {
        wcout << L" " << get_type_name(handle_type_total.first) << L": " << handle_type_total.second << L'\n';
    }
    wcout << L'\n';

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

        wcout << L" Handle[" << i << L"]:\n";
        wcout << L"   Handle: " << to_hex_full(descriptor.Handle) << L'\n';
        wcout << L"   TypeName: " << get_type_name(entry.type_name()) << L'\n';
        wcout << L"   ObjectName: " << entry.object_name() << L'\n';
        wcout << L"   Attributes: " << to_hex(descriptor.Attributes) << L'\n';
        wcout << L"   GrantedAccess: " << to_hex(descriptor.GrantedAccess) << L'\n';
        wcout << L"   HandleCount: " << descriptor.HandleCount << L'\n';
        wcout << L"   PointerCount: " << descriptor.PointerCount << L'\n';

        if (entry.handle_descriptor_version() >= 2)
        {
            for (size_t object_i{0}; auto const& handle_object_info : entry.list())
            {
                wcout << L"   Object[" << object_i << L"]:\n";
                wcout << L"     InfoType: " <<
                    system_info_utils::handle_object_information_type_to_string(handle_object_info->InfoType) << L" ("
                    << handle_object_info->InfoType << L")\n";

                hex_dump::hex_dump(wcout, handle_object_info.data(), handle_object_info->SizeOfInfo, 5);

                ++object_i;
                wcout << L'\n';
            }
        }
        else
        {
            wcout << L'\n';
        }
        ++i;
    }
}

void dump_mini_dump_ipt_trace_stream_data(mini_dump const& mini_dump, size_t const index)
{
    // as yet I haven't found a way to create a dump file with IPT support
    // I would assume this is just a dump of the Intel Processor Trace output
    // If I get examples will look at using libipt to decode trace
    auto const& entry = mini_dump.directory()[index];
    auto const* stream_data = mini_dump.rva32(entry.Location);
    hex_dump::hex_dump(wcout, stream_data, entry.Location.DataSize, 1);
}
