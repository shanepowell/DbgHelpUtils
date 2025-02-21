#include "dump_mini_dump_thread.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/stream_thread.h"
#include "DbgHelpUtils/stream_thread_context.h"
#include "DbgHelpUtils/stream_thread_ex.h"
#include "DbgHelpUtils/stream_thread_name.h"
#include "DbgHelpUtils/symbol_type_utils.h"
#include "DbgHelpUtils/thread_list_stream.h"
#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "DbgHelpUtils/thread_info_list_stream.h"
#include "DbgHelpUtils/thread_info_utils.h"
#include "DbgHelpUtils/thread_names_list_stream.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/time_units.h"
#include "DbgHelpUtils/vector_to_hash_set.h"

using namespace std;
using namespace std::string_literals;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

std::set<uint32_t> get_filtered_thread_ids(dump_file_options const& options);

std::set<uint32_t> get_filtered_thread_ids(dump_file_options const& options)
{
    return vector_to_hash_set<uint32_t>(options.filter_values(L"thread_id"s));
}

void dump_mini_dump_thread_context(std::wostream& log, stream_thread_context const& thread_context, dump_file_options const& options)
{
    log << L"  ThreadContext:\n";
    auto force_hex_dump = false;
    if (thread_context.x64_thread_context_available())
    {
        dump_mini_dump_x64_thread_context(log, thread_context.x64_thread_context());
    }
    else if (thread_context.wow64_thread_context_available())
    {
        dump_mini_dump_wow64_thread_context(log
            , thread_context.wow64_thread_context()
            , has_extended_registers_t{thread_context.wow64_thread_context_has_extended_registers()});
    }
    else if (thread_context.x86_thread_context_available())
    {
        dump_mini_dump_x86_thread_context(log
            , thread_context.x86_thread_context()
            , has_extended_registers_t{thread_context.x86_thread_context_has_extended_registers()});
    }
    else
    {
        force_hex_dump = true;
    }

    if (options.hex_dump_memory_data() || force_hex_dump)
    {
        hex_dump::hex_dump(log, thread_context.context(), options.hex_dump_memory_size(thread_context.size()), 4);
        log << L'\n';
    }
}

void dump_mini_dump_x64_thread_context(std::wostream& log, stream_thread_context::context_x64 const& context)
{
    log << std::format(L"    ContextFlags: {}\n", to_hex_full(context.ContextFlags));
    log << std::format(L"    RIP: {}\n", to_hex_full(context.Rip));
    log << std::format(L"    RSP: {}\n", to_hex_full(context.Rsp));
    log << std::format(L"    RAX: {}\n", to_hex_full(context.Rax));
    log << std::format(L"    RBX: {}\n", to_hex_full(context.Rbx));
    log << std::format(L"    RCX: {}\n", to_hex_full(context.Rcx));
    log << std::format(L"    RDX: {}\n", to_hex_full(context.Rdx));
    log << std::format(L"    RDI: {}\n", to_hex_full(context.Rdi));
    log << std::format(L"    RSI: {}\n", to_hex_full(context.Rsi));
    log << std::format(L"    RBP: {}\n", to_hex_full(context.Rbp));
    log << std::format(L"    R8: {}\n", to_hex_full(context.R8));
    log << std::format(L"    R9: {}\n", to_hex_full(context.R9));
    log << std::format(L"    R10: {}\n", to_hex_full(context.R10));
    log << std::format(L"    R11: {}\n", to_hex_full(context.R11));
    log << std::format(L"    R12: {}\n", to_hex_full(context.R12));
    log << std::format(L"    R13: {}\n", to_hex_full(context.R13));
    log << std::format(L"    R14: {}\n", to_hex_full(context.R14));
    log << std::format(L"    R15: {}\n", to_hex_full(context.R15));
    log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
    log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
    log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
    log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
    log << std::format(L"    EFlags: {}\n", to_hex_full(context.EFlags));
    log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
    log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
    log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
    log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
    log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
    log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));
    log << std::format(L"    MxCsr: {}\n", to_hex_full(context.MxCsr));
    log << L"    XMM:\n";
    log << std::format(L"      Header: {0}-{1}\n", to_hex_full(context.Header[0]), to_hex_full(context.Header[1]));
    log << std::format(L"      Legacy: {0}-{1}\n", to_hex_full(context.Legacy[0]), to_hex_full(context.Legacy[1]));
    for (size_t index = 0; index < std::size(context.Legacy); index += 2)
    {
        log << std::format(L"      Legacy[{0}/{1}]: {2}-{3}\n", locale_formatting::to_wstring(index), locale_formatting::to_wstring(index + 1), to_hex_full(context.Legacy[index]), to_hex_full(context.Legacy[index + 1]));
    }
    log << std::format(L"      Xmm0: {}\n", to_hex_full(context.Xmm0));
    log << std::format(L"      Xmm1: {}\n", to_hex_full(context.Xmm1));
    log << std::format(L"      Xmm2: {}\n", to_hex_full(context.Xmm2));
    log << std::format(L"      Xmm3: {}\n", to_hex_full(context.Xmm3));
    log << std::format(L"      Xmm4: {}\n", to_hex_full(context.Xmm4));
    log << std::format(L"      Xmm5: {}\n", to_hex_full(context.Xmm5));
    log << std::format(L"      Xmm6: {}\n", to_hex_full(context.Xmm6));
    log << std::format(L"      Xmm7: {}\n", to_hex_full(context.Xmm7));
    log << std::format(L"      Xmm8: {}\n", to_hex_full(context.Xmm8));
    log << std::format(L"      Xmm9: {}\n", to_hex_full(context.Xmm9));
    log << std::format(L"      Xmm10: {}\n", to_hex_full(context.Xmm10));
    log << std::format(L"      Xmm11: {}\n", to_hex_full(context.Xmm11));
    log << std::format(L"      Xmm12: {}\n", to_hex_full(context.Xmm12));
    log << std::format(L"      Xmm13: {}\n", to_hex_full(context.Xmm13));
    log << std::format(L"      Xmm14: {}\n", to_hex_full(context.Xmm14));
    log << std::format(L"      Xmm15: {}\n", to_hex_full(context.Xmm15));
    for (size_t index = 0; index < std::size(context.VectorRegister); index += 2)
    {
        log << std::format(L"    VectorRegister[{0}/{1}]: {2}-{3}\n"
            , locale_formatting::to_wstring(index)
            , locale_formatting::to_wstring(index + 1)
            , to_hex_full(context.VectorRegister[index])
            , to_hex_full(context.VectorRegister[index + 1]));
    }
    log << std::format(L"    VectorControl: {}\n", to_hex_full(context.VectorControl));
    log << std::format(L"    DebugControl: {}\n", to_hex_full(context.DebugControl));
    log << std::format(L"    LastBranchToRip: {}\n", to_hex_full(context.LastBranchToRip));
    log << std::format(L"    LastBranchFromRip: {}\n", to_hex_full(context.LastBranchFromRip));
    log << std::format(L"    LastExceptionToRip: {}\n", to_hex_full(context.LastExceptionToRip));
    log << std::format(L"    LastExceptionFromRip: {}\n", to_hex_full(context.LastExceptionFromRip));
}

void dump_mini_dump_x86_thread_context(std::wostream& log, stream_thread_context::context_x86 const& context, has_extended_registers_t const has_extended_registers)
{
    log << std::format(L"    ContextFlags: {}\n", to_hex_full(context.ContextFlags));
    log << std::format(L"    EIP: {}\n", to_hex_full(context.Eip));
    log << std::format(L"    ESP: {}\n", to_hex_full(context.Esp));
    log << std::format(L"    EAX: {}\n", to_hex_full(context.Eax));
    log << std::format(L"    EBX: {}\n", to_hex_full(context.Ebx));
    log << std::format(L"    ECX: {}\n", to_hex_full(context.Ecx));
    log << std::format(L"    EDX: {}\n", to_hex_full(context.Edx));
    log << std::format(L"    EDI: {}\n", to_hex_full(context.Edi));
    log << std::format(L"    ESI: {}\n", to_hex_full(context.Esi));
    log << std::format(L"    EBP: {}\n", to_hex_full(context.Ebp));
    log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
    log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
    log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
    log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
    log << std::format(L"    EFlags: {}\n", to_hex_full(context.EFlags));
    log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
    log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
    log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
    log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
    log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
    log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));

    log << L"    FloatSave:\n";
    log << std::format(L"      ControlWord: {}\n", to_hex(context.FloatSave.ControlWord));
    log << std::format(L"      StatusWord: {}\n", to_hex(context.FloatSave.StatusWord));
    log << std::format(L"      TagWord: {}\n", to_hex(context.FloatSave.TagWord));
    log << std::format(L"      ErrorOffset: {}\n", to_hex(context.FloatSave.ErrorOffset));
    log << std::format(L"      ErrorSelector: {}\n", to_hex(context.FloatSave.ErrorSelector));
    log << std::format(L"      DataOffset: {}\n", to_hex(context.FloatSave.DataOffset));
    log << std::format(L"      DataSelector: {}\n", to_hex(context.FloatSave.DataSelector));
    log << L"      RegisterArea:\n";
    hex_dump::hex_dump(log, context.FloatSave.RegisterArea, sizeof(context.FloatSave.RegisterArea), 8);
    log << L'\n';

    if (has_extended_registers)
    {
        log << L"    ExtendedRegisters:\n";
        hex_dump::hex_dump(log, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 6);
        log << L'\n';
    }
}

void dump_mini_dump_wow64_thread_context(std::wostream& log, WOW64_CONTEXT const& context, has_extended_registers_t const has_extended_registers)
{
    log << std::format(L"    ContextFlags: {}\n", to_hex_full(context.ContextFlags));
    log << std::format(L"    EIP: {}\n", to_hex_full(context.Eip));
    log << std::format(L"    ESP: {}\n", to_hex_full(context.Esp));
    log << std::format(L"    EAX: {}\n", to_hex_full(context.Eax));
    log << std::format(L"    EBX: {}\n", to_hex_full(context.Ebx));
    log << std::format(L"    ECX: {}\n", to_hex_full(context.Ecx));
    log << std::format(L"    EDX: {}\n", to_hex_full(context.Edx));
    log << std::format(L"    EDI: {}\n", to_hex_full(context.Edi));
    log << std::format(L"    ESI: {}\n", to_hex_full(context.Esi));
    log << std::format(L"    EBP: {}\n", to_hex_full(context.Ebp));
    log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
    log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
    log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
    log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
    log << std::format(L"    EFlags: {}\n", to_hex_full(context.EFlags));
    log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
    log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
    log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
    log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
    log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
    log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));

    log << L"    FloatSave:\n";
    log << std::format(L"      ControlWord: {}\n", to_hex(context.FloatSave.ControlWord));
    log << std::format(L"      StatusWord: {}\n", to_hex(context.FloatSave.StatusWord));
    log << std::format(L"      TagWord: {}\n", to_hex(context.FloatSave.TagWord));
    log << std::format(L"      ErrorOffset: {}\n", to_hex(context.FloatSave.ErrorOffset));
    log << std::format(L"      ErrorSelector: {}\n", to_hex(context.FloatSave.ErrorSelector));
    log << std::format(L"      DataOffset: {}\n", to_hex(context.FloatSave.DataOffset));
    log << std::format(L"      DataSelector: {}\n", to_hex(context.FloatSave.DataSelector));
    log << std::format(L"      Cr0NpxState: {}\n", to_hex(context.FloatSave.Cr0NpxState));
    log << L"      RegisterArea:\n";
    hex_dump::hex_dump(log, context.FloatSave.RegisterArea, sizeof(context.FloatSave.RegisterArea), 8);
    log << L'\n';

    if (has_extended_registers)
    {
        log << L"    ExtendedRegisters:\n";
        hex_dump::hex_dump(log, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 6);
        log << L'\n';
    }
}

void dump_mini_dump_thread_names_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    thread_names_list_stream const thread_names_list{mini_dump, index};

    if (!thread_names_list.found())
    {
        log << L"ThreadNamesStream not found!\n";
        return;
    }

    log << std::format(L"NumberOfThreadNames: {}\n", locale_formatting::to_wstring(thread_names_list.thread_names_list().NumberOfThreadNames));
    for (size_t i = 0; auto const& entry : thread_names_list.list())
    {
        log << std::format(L" [{0}]: {1}\n", locale_formatting::to_wstring(i), entry.name());
        log << std::format(L"   ThreadId: {}\n", to_hex(entry->ThreadId));

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_thread_list_stream_data(std::wostream& log
    , mini_dump const& mini_dump
    , size_t const index
    , dump_file_options const& options
    , dbg_help::symbol_engine& symbol_engine)
{
    thread_list_stream const thread_list{mini_dump, index};

    if (!thread_list.found())
    {
        log << L"ThreadListStream not found!\n";
        return;
    }

    auto const thread_ids = get_filtered_thread_ids(options);

    log << std::format(L"NumberOfThreads: {}\n", locale_formatting::to_wstring(thread_list.thread_list().NumberOfThreads));
    for (size_t i = 0; auto const& thread : thread_list.list())
    {
        if (!thread_ids.empty() && !thread_ids.contains(thread->ThreadId))
        {
            ++i;
            continue;
        }

        log << std::format(L" [{0}] Thread Id: {1} ({2})", locale_formatting::to_wstring(i), locale_formatting::to_wstring(thread->ThreadId), to_hex(thread->ThreadId));
        if (!thread.thread_name().empty())
        {
            log << std::format(L" ({})", thread.thread_name());
        }
        log << L'\n';
        log << std::format(L"   Priority: {}\n", locale_formatting::to_wstring(thread->Priority));
        log << std::format(L"   PriorityClass: {} ({})\n", thread_info_utils::thread_priority_class_to_string(thread->PriorityClass), to_hex_full(thread->PriorityClass));
        log << std::format(L"   SuspendCount: {}\n", locale_formatting::to_wstring(thread->SuspendCount));
        log << std::format(L"   TEB: {}\n", to_hex_full(thread->Teb));

        load_and_dump_teb(log, mini_dump, symbol_engine, thread->Teb);

        dump_mini_dump_thread_context(log, thread.thread_context(), options);

        using namespace size_units::base_16;
        log << std::format(L"   Stack: {0} - {1} ({2}) ({3})\n"
            , to_hex_full(thread->Stack.StartOfMemoryRange)
            , to_hex_full(thread->Stack.StartOfMemoryRange + thread->Stack.Memory.DataSize)
            , to_hex(thread->Stack.Memory.DataSize)
            , to_wstring(bytes{thread->Stack.Memory.DataSize}));

        if (thread.stack() != nullptr)
        {
            if (options.display_symbols())
            {
                stream_stack_dump::dump_stack_to_stream(log, mini_dump, symbol_engine, thread->Stack.StartOfMemoryRange,
                                                  thread.stack(), thread->Stack.Memory.DataSize,
                                                  thread.thread_context(), 5, options.display_stack_options());
            }
            else if (options.hex_dump_memory_data())
            {
                hex_dump::hex_dump(log
                    , thread.stack()
                    , options.hex_dump_memory_size(thread->Stack.Memory.DataSize)
                    , 5
                    , write_header_t{true}
                    , 16
                    , thread->Stack.StartOfMemoryRange);
                log << L'\n';
            }
        }

        ++i;
    }
    log << L'\n';
}


void dump_mini_dump_thread_list_ex_stream_data(std::wostream& log
    , mini_dump const& mini_dump
    , size_t const index
    , dump_file_options const& options
    , dbg_help::symbol_engine& symbol_engine)
{
    thread_ex_list_stream const thread_ex_list{mini_dump, index};

    if (!thread_ex_list.found())
    {
        log << L"ThreadListExStream not found!\n";
        return;
    }

    auto const thread_ids = get_filtered_thread_ids(options);

    log << std::format(L"NumberOfThreads: {}\n", locale_formatting::to_wstring(thread_ex_list.thread_list().NumberOfThreads));
    for (size_t i = 0; auto const& thread : thread_ex_list.list())
    {
        if (!thread_ids.empty() && !thread_ids.contains(thread->ThreadId))
        {
            ++i;
            continue;
        }

        log << std::format(L" [{0}] Thread Id: {1} ({2})", locale_formatting::to_wstring(i), locale_formatting::to_wstring(thread->ThreadId), to_hex(thread->ThreadId));
        if (!thread.thread_name().empty())
        {
            log << std::format(L" ({})", thread.thread_name());
        }
        log << L'\n';
        log << std::format(L"   Priority: {}\n", locale_formatting::to_wstring(thread->Priority));
        log << std::format(L"   PriorityClass: {}\n", locale_formatting::to_wstring(thread->PriorityClass));
        log << std::format(L"   SuspendCount: {}\n", locale_formatting::to_wstring(thread->SuspendCount));
        log << std::format(L"   TEB: {}\n", to_hex_full(thread->Teb));

        load_and_dump_teb(log, mini_dump, symbol_engine, thread->Teb);

        using namespace size_units::base_16;
        log << std::format(L"   Stack: {0} - {1} ({2}) ({3})\n"
            , to_hex_full(thread->Stack.StartOfMemoryRange)
            , to_hex_full(thread->Stack.StartOfMemoryRange + thread->Stack.Memory.DataSize)
            , to_hex(thread->Stack.Memory.DataSize)
            , to_wstring(bytes{thread->Stack.Memory.DataSize}));
        if (thread.stack() != nullptr)
        {
            if (options.display_symbols())
            {
                stream_stack_dump::dump_stack_to_stream(log
                    , mini_dump
                    , symbol_engine
                    , thread->Stack.StartOfMemoryRange
                    , thread.stack(), thread->Stack.Memory.DataSize
                    , thread.thread_context()
                    , 5
                    , options.display_stack_options());
            }
            else if (options.hex_dump_memory_data())
            {
                hex_dump::hex_dump(log
                    , thread.stack()
                    , options.hex_dump_memory_size(thread->Stack.Memory.DataSize)
                    , 5
                    , write_header_t{true}
                    , 16
                    , thread->Stack.StartOfMemoryRange);
                log << L'\n';
            }
        }

        dump_mini_dump_thread_context(log, thread.thread_context(), options);

        log << std::format(L"   BackingStore: {0} - {1} ({2}) ({3})\n"
            , to_hex_full(thread->BackingStore.StartOfMemoryRange)
            , to_hex_full(thread->BackingStore.StartOfMemoryRange + thread->BackingStore.Memory.DataSize)
            , to_hex(thread->Stack.Memory.DataSize)
            , to_wstring(bytes{thread->BackingStore.Memory.DataSize}));

        if (options.hex_dump_memory_data())
        {
            if (thread.backing_store() != nullptr)
            {
                hex_dump::hex_dump(log
                    , thread.backing_store()
                    , options.hex_dump_memory_size(thread->BackingStore.Memory.DataSize)
                    , 5
                    , write_header_t{true}
                    , 16
                    , thread->BackingStore.StartOfMemoryRange);
                log << L'\n';
            }
        }

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_thread_info_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    thread_info_list_stream const thread_info_list{mini_dump, index};

    if (!thread_info_list.found())
    {
        log << L"ThreadInfoListStream not found!\n";
        return;
    }

    if (!thread_info_list.is_valid())
    {
        log << L"ThreadInfoListStream version unknown!\n";
        return;
    }

    auto const local_info = misc_info_stream::get_dump_file_timezone_info(mini_dump);

    log << std::format(L"NumberOfEntries: {}\n", locale_formatting::to_wstring(thread_info_list.size()));
    for (size_t i = 0; auto const& thread : thread_info_list.list())
    {
        using namespace time_units;
        log << std::format(L" [{0}] Thread Id: {1} ({2})", locale_formatting::to_wstring(i), locale_formatting::to_wstring(thread->ThreadId), to_hex(thread->ThreadId));
        if (!thread.thread_name().empty())
        {
            log << std::format(L" ({})", thread.thread_name());
        }
        log << L'\n';
        log << std::format(L"   StartAddress: {}\n", to_hex_full(thread->StartAddress));
        log << std::format(L"   Affinity: {}\n", to_hex_full(thread->Affinity));
        if (thread->CreateTime > 0)
        {
            auto const st = time_utils::filetime_to_system_time(time_utils::to_filetime(thread->CreateTime));
            log << std::format(L"   CreateTime [local: {0}] [UTC: {1}] [DumpLocale: {2}]\n"
                , from_dump_file_to_local_timestamp_string(st, local_info)
                , from_dump_file_to_utc_timestamp_string(st, local_info)
                , to_dump_file_timestamp_string(st, local_info)
            );
        }
        if (thread->ExitTime > 0)
        {
            auto const st = time_utils::filetime_to_system_time(time_utils::to_filetime(thread->ExitTime));
            log << std::format(L"   ExitTime [local: {0}] [UTC: {1}] [DumpLocale: {2}]\n"
                , from_dump_file_to_local_timestamp_string(st, local_info)
                , from_dump_file_to_utc_timestamp_string(st, local_info)
                , to_dump_file_timestamp_string(st, local_info)
            );
        }
        log << std::format(L"   UserTime: {0} ({1})\n", to_timespan_wstring(filetime_nanoseconds{thread->UserTime}), locale_formatting::to_wstring(thread->UserTime));
        log << std::format(L"   KernelTime: {0} ({1})\n", to_timespan_wstring(filetime_nanoseconds{thread->KernelTime}), locale_formatting::to_wstring(thread->KernelTime));
        log << std::format(L"   ExitStatus: {}\n", locale_formatting::to_wstring(thread->ExitStatus));
        log << std::format(L"   DumpFlags: {0} ({1})\n", thread_info_utils::dump_flags_to_string(thread->DumpFlags), to_hex(thread->DumpFlags));
        log << std::format(L"   DumpError: {}\n", to_hex(thread->DumpError));

        ++i;
    }
    log << L'\n';
}

void load_and_dump_teb(std::wostream& log, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine, ULONG64 const teb_address)
{
    symbol_type_utils::dump_variable_type_at(log, mini_dump, symbol_engine, common_symbol_names::teb_structure_symbol_name, teb_address);
}
