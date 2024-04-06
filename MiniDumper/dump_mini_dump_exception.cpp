#include "dump_mini_dump_exception.h"

#include "dump_file_options.h"
#include "dump_mini_dump_thread.h"
#include "DbgHelpUtils/exception_stream.h"
#include "DbgHelpUtils/exception_utils.h"
#include "DbgHelpUtils/find_thread_stack.h"
#include "DbgHelpUtils/join.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

void dump_mini_dump_exception_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                          dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    exception_stream const exception_stream{mini_dump, index};

    if (!exception_stream.found())
    {
        log << L"ExceptionStream not found!\n";
        return;
    }

    auto const& exception = exception_stream.exception();
    log << std::format(L"  ThreadId: {}\n", to_hex(exception.ThreadId));
    log << std::format(L"  ExceptionCode: {0} ({1})\n", exception_utils::exception_code_to_string(exception.ExceptionRecord.ExceptionCode), to_hex(exception.ExceptionRecord.ExceptionCode));
    log << std::format(L"  ExceptionFlags: {0} ({1})\n", wjoin(exception_utils::exception_flags_to_list(exception.ExceptionRecord.ExceptionFlags), L", "sv), to_hex(exception.ExceptionRecord.ExceptionFlags));
    log << std::format(L"  ExceptionRecord: {}\n", to_hex(exception.ExceptionRecord.ExceptionRecord));
    log << std::format(L"  ExceptionAddress: {}\n", to_hex(exception.ExceptionRecord.ExceptionAddress));
    log << std::format(L"  NumberParameters: {}\n", locale_formatting::to_wstring(exception.ExceptionRecord.NumberParameters));

    for (size_t parameter = 0; parameter < exception.ExceptionRecord.NumberParameters; ++parameter)
    {
        log << std::format(L"    Parameter[{0}]: {1}\n", locale_formatting::to_wstring(parameter), to_hex(exception.ExceptionRecord.ExceptionInformation[parameter]));
    }

    dump_mini_dump_thread_context(log, exception_stream.thread_context(), options);

    if (options.display_symbols())
    {
        if (auto const stack_info = find_thread_stack(mini_dump, exception.ThreadId))
        {
            log << L"  Stack:\n";
            stream_stack_dump::hex_dump_stack(log, mini_dump, symbol_engine, stack_info->stack_start_address,
                                              stack_info->stack, stack_info->stack_size,
                                              exception_stream.thread_context(), 5, options.display_stack_options());
        }
    }
}
