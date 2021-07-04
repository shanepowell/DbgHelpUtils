#include "dump_mini_dump_exception.h"

#include <iostream>

#include "dump_file_options.h"
#include "dump_mini_dump_thread.h"
#include "DbgHelpUtils/exception_stream.h"
#include "DbgHelpUtils/exception_utils.h"
#include "DbgHelpUtils/find_thread_stack.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/thread_ex_list_stream.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

void dump_mini_dump_exception_stream_data(mini_dump const& mini_dump, size_t const index,
                                          dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    exception_stream const exception_stream{mini_dump, index};

    if (!exception_stream.found())
    {
        wcout << L"ExceptionStream not found!\n";
        return;
    }

    auto const& exception = exception_stream.exception();
    wcout << L"  ThreadId: " << exception.ThreadId << L'\n';
    wcout << L"  ExceptionCode: " << exception_utils::exception_code_to_string(exception.ExceptionRecord.ExceptionCode)
        << L" (" << to_hex(exception.ExceptionRecord.ExceptionCode) << L")\n";
    wcout << L"  ExceptionFlags: " <<
        exception_utils::exception_flags_to_string(exception.ExceptionRecord.ExceptionFlags) << L" (" << to_hex(
            exception.ExceptionRecord.ExceptionFlags) << L")\n";
    wcout << L"  ExceptionRecord: " << to_hex(exception.ExceptionRecord.ExceptionRecord) << L'\n';
    wcout << L"  ExceptionAddress: " << to_hex(exception.ExceptionRecord.ExceptionAddress) << L'\n';
    wcout << L"  NumberParameters: " << exception.ExceptionRecord.NumberParameters << L'\n';

    for (size_t parameter = 0; parameter < exception.ExceptionRecord.NumberParameters; ++parameter)
    {
        wcout << L"    Parameter[" << parameter << "]: " << to_hex(
            exception.ExceptionRecord.ExceptionInformation[parameter]) << L'\n';
    }

    dump_mini_dump_thread_context(exception_stream.thread_context(), options);

    if (options.load_symbols())
    {
        if (auto const stack_info = find_thread_stack(mini_dump, exception.ThreadId))
        {
            wcout << L"  Stack:\n";
            stream_stack_dump::hex_dump_stack(wcout, mini_dump, symbol_engine, stack_info->stack_start_address,
                                              stack_info->stack, stack_info->stack_size,
                                              exception_stream.thread_context(), 5);
        }
    }
}
