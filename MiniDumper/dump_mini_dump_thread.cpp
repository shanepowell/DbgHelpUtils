#include "dump_mini_dump_thread.h"


#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/mini_dump.h"
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

void dump_mini_dump_thread_context(stream_thread_context const& thread_context, dump_file_options const& options)
{
    wcout << L"  ThreadContext:\n";
    auto force_hex_dump = false;
    if (thread_context.x64_thread_context_available())
    {
        dump_mini_dump_x64_thread_context(thread_context.x64_thread_context());
    }
    else if (thread_context.wow64_thread_context_available())
    {
        dump_mini_dump_x86_thread_context(thread_context.wow64_thread_context(),
                                          thread_context.wow64_thread_context_has_extended_registers());
    }
    else if (thread_context.x86_thread_context_available())
    {
        dump_mini_dump_x86_thread_context(thread_context.x86_thread_context(),
                                          thread_context.x86_thread_context_has_extended_registers());
    }
    else
    {
        force_hex_dump = true;
    }

    if (options.hex_dump_memory_data() || force_hex_dump)
    {
        hex_dump::hex_dump(wcout, thread_context.context(), thread_context.size(), 4);
        wcout << L'\n';
    }
}

void dump_mini_dump_x64_thread_context(CONTEXT const& context)
{
    wcout << L"    ContextFlags: " << to_hex_full(context.ContextFlags) << L'\n';
    wcout << L"    RIP: " << to_hex_full(context.Rip) << L'\n';
    wcout << L"    RSP: " << to_hex_full(context.Rsp) << L'\n';
    wcout << L"    RAX: " << to_hex_full(context.Rax) << L'\n';
    wcout << L"    RBX: " << to_hex_full(context.Rbx) << L'\n';
    wcout << L"    RCX: " << to_hex_full(context.Rcx) << L'\n';
    wcout << L"    RDX: " << to_hex_full(context.Rdx) << L'\n';
    wcout << L"    RDI: " << to_hex_full(context.Rdi) << L'\n';
    wcout << L"    RSI: " << to_hex_full(context.Rsi) << L'\n';
    wcout << L"    RBP: " << to_hex_full(context.Rbp) << L'\n';
    wcout << L"    R8: " << to_hex_full(context.R8) << L'\n';
    wcout << L"    R9: " << to_hex_full(context.R9) << L'\n';
    wcout << L"    R10: " << to_hex_full(context.R10) << L'\n';
    wcout << L"    R11: " << to_hex_full(context.R11) << L'\n';
    wcout << L"    R12: " << to_hex_full(context.R12) << L'\n';
    wcout << L"    R13: " << to_hex_full(context.R13) << L'\n';
    wcout << L"    R14: " << to_hex_full(context.R14) << L'\n';
    wcout << L"    R15: " << to_hex_full(context.R15) << L'\n';
    wcout << L"    CS: " << to_hex_full(context.SegCs) << L'\n';
    wcout << L"    DS: " << to_hex_full(context.SegDs) << L'\n';
    wcout << L"    ES: " << to_hex_full(context.SegEs) << L'\n';
    wcout << L"    FS: " << to_hex_full(context.SegFs) << L'\n';
    wcout << L"    GS: " << to_hex_full(context.SegGs) << L'\n';
    wcout << L"    SS: " << to_hex_full(context.SegSs) << L'\n';
    wcout << L"    EFlags: " << to_hex_full(context.EFlags) << L'\n';
    wcout << L"    DR0: " << to_hex_full(context.Dr0) << L'\n';
    wcout << L"    DR1: " << to_hex_full(context.Dr1) << L'\n';
    wcout << L"    DR2: " << to_hex_full(context.Dr2) << L'\n';
    wcout << L"    DR3: " << to_hex_full(context.Dr3) << L'\n';
    wcout << L"    DR6: " << to_hex_full(context.Dr6) << L'\n';
    wcout << L"    DR7: " << to_hex_full(context.Dr7) << L'\n';
    wcout << L"    MxCsr: " << to_hex_full(context.MxCsr) << L'\n';
    wcout << L"    XMM:\n";
    wcout << L"      Header: " << to_hex_full(context.Header[0]) << L'-' << to_hex_full(context.Header[1]) << L'\n';
    wcout << L"      Legacy: " << to_hex_full(context.Legacy[0]) << L'-' << to_hex_full(context.Legacy[1]) << L'\n';
    for (size_t index = 0; index < std::size(context.Legacy); index += 2)
    {
        wcout << L"      Legacy[" << index << L'/' << index + 1 << L"]: " << to_hex_full(context.Legacy[index]) << L'-'
            << to_hex_full(context.Legacy[index + 1]) << L'\n';
    }
    wcout << L"      Xmm0: " << to_hex_full(context.Xmm0) << L'\n';
    wcout << L"      Xmm1: " << to_hex_full(context.Xmm1) << L'\n';
    wcout << L"      Xmm2: " << to_hex_full(context.Xmm2) << L'\n';
    wcout << L"      Xmm3: " << to_hex_full(context.Xmm3) << L'\n';
    wcout << L"      Xmm4: " << to_hex_full(context.Xmm4) << L'\n';
    wcout << L"      Xmm5: " << to_hex_full(context.Xmm5) << L'\n';
    wcout << L"      Xmm6: " << to_hex_full(context.Xmm6) << L'\n';
    wcout << L"      Xmm7: " << to_hex_full(context.Xmm7) << L'\n';
    wcout << L"      Xmm8: " << to_hex_full(context.Xmm8) << L'\n';
    wcout << L"      Xmm9: " << to_hex_full(context.Xmm9) << L'\n';
    wcout << L"      Xmm10: " << to_hex_full(context.Xmm10) << L'\n';
    wcout << L"      Xmm11: " << to_hex_full(context.Xmm11) << L'\n';
    wcout << L"      Xmm12: " << to_hex_full(context.Xmm12) << L'\n';
    wcout << L"      Xmm13: " << to_hex_full(context.Xmm13) << L'\n';
    wcout << L"      Xmm14: " << to_hex_full(context.Xmm14) << L'\n';
    wcout << L"      Xmm15: " << to_hex_full(context.Xmm15) << L'\n';
    for (size_t index = 0; index < std::size(context.VectorRegister); index += 2)
    {
        wcout << L"    VectorRegister[" << index << L'/' << index + 1 << L"]: " <<
            to_hex_full(context.VectorRegister[index]) << L'-' << to_hex_full(context.VectorRegister[index + 1]) <<
            L'\n';
    }
    wcout << L"    VectorControl: " << to_hex_full(context.VectorControl) << L'\n';
    wcout << L"    DebugControl: " << to_hex_full(context.DebugControl) << L'\n';
    wcout << L"    LastBranchToRip: " << to_hex_full(context.LastBranchToRip) << L'\n';
    wcout << L"    LastBranchFromRip: " << to_hex_full(context.LastBranchFromRip) << L'\n';
    wcout << L"    LastExceptionToRip: " << to_hex_full(context.LastExceptionToRip) << L'\n';
    wcout << L"    LastExceptionFromRip: " << to_hex_full(context.LastExceptionFromRip) << L'\n';
}

void dump_mini_dump_x86_thread_context(WOW64_CONTEXT const& context, bool const has_extended_registers)
{
    wcout << L"    ContextFlags: " << to_hex_full(context.ContextFlags) << L'\n';
    wcout << L"    EIP: " << to_hex_full(context.Eip) << L'\n';
    wcout << L"    ESP: " << to_hex_full(context.Esp) << L'\n';
    wcout << L"    EAX: " << to_hex_full(context.Eax) << L'\n';
    wcout << L"    EBX: " << to_hex_full(context.Ebx) << L'\n';
    wcout << L"    ECX: " << to_hex_full(context.Ecx) << L'\n';
    wcout << L"    EDX: " << to_hex_full(context.Edx) << L'\n';
    wcout << L"    EDI: " << to_hex_full(context.Edi) << L'\n';
    wcout << L"    ESI: " << to_hex_full(context.Esi) << L'\n';
    wcout << L"    EBP: " << to_hex_full(context.Ebp) << L'\n';
    wcout << L"    CS: " << to_hex_full(context.SegCs) << L'\n';
    wcout << L"    DS: " << to_hex_full(context.SegDs) << L'\n';
    wcout << L"    ES: " << to_hex_full(context.SegEs) << L'\n';
    wcout << L"    FS: " << to_hex_full(context.SegFs) << L'\n';
    wcout << L"    GS: " << to_hex_full(context.SegGs) << L'\n';
    wcout << L"    SS: " << to_hex_full(context.SegSs) << L'\n';
    wcout << L"    EFlags: " << to_hex_full(context.EFlags) << L'\n';
    wcout << L"    DR0: " << to_hex_full(context.Dr0) << L'\n';
    wcout << L"    DR1: " << to_hex_full(context.Dr1) << L'\n';
    wcout << L"    DR2: " << to_hex_full(context.Dr2) << L'\n';
    wcout << L"    DR3: " << to_hex_full(context.Dr3) << L'\n';
    wcout << L"    DR6: " << to_hex_full(context.Dr6) << L'\n';
    wcout << L"    DR7: " << to_hex_full(context.Dr7) << L'\n';

    wcout << L"    FloatSave:\n";
    wcout << L"      ControlWord: " << to_hex(context.FloatSave.ControlWord) << L'\n';
    wcout << L"      StatusWord: " << to_hex(context.FloatSave.StatusWord) << L'\n';
    wcout << L"      TagWord: " << to_hex(context.FloatSave.TagWord) << L'\n';
    wcout << L"      ErrorOffset: " << to_hex(context.FloatSave.ErrorOffset) << L'\n';
    wcout << L"      ErrorSelector: " << to_hex(context.FloatSave.ErrorSelector) << L'\n';
    wcout << L"      DataOffset: " << to_hex(context.FloatSave.DataOffset) << L'\n';
    wcout << L"      DataSelector: " << to_hex(context.FloatSave.DataSelector) << L'\n';
    wcout << L"      Cr0NpxState: " << to_hex(context.FloatSave.Cr0NpxState) << L'\n';
    wcout << L"      RegisterArea:\n";
    hex_dump::hex_dump(wcout, context.FloatSave.RegisterArea, sizeof(context.FloatSave.RegisterArea), 8);
    wcout << L'\n';

    if (has_extended_registers)
    {
        wcout << L"    ExtendedRegisters:\n";
        hex_dump::hex_dump(wcout, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 6);
        wcout << L'\n';
    }
}

void dump_mini_dump_thread_names_stream_data(mini_dump const& mini_dump, size_t const index)
{
    thread_names_list_stream const thread_names_list{mini_dump, index};

    if (!thread_names_list.found())
    {
        wcout << L"ThreadNamesStream not found!\n";
        return;
    }

    wcout << L"NumberOfThreadNames: " << thread_names_list.thread_names_list().NumberOfThreadNames << L'\n';
    for (size_t i = 0; auto const& entry : thread_names_list.list())
    {
        wcout << L" [" << i << "]: " << entry.name() << L'\n';
        wcout << L"   ThreadId: " << entry->ThreadId << L'\n';

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_thread_list_stream_data(mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    thread_list_stream const thread_list{mini_dump, index};

    if (!thread_list.found())
    {
        wcout << L"ThreadListStream not found!\n";
        return;
    }

    auto const thread_ids = get_filtered_thread_ids(options);

    wcout << L"NumberOfThreads: " << thread_list.thread_list().NumberOfThreads << L'\n';
    for (size_t i = 0; auto const& thread : thread_list.list())
    {
        if (!thread_ids.empty() && !thread_ids.contains(thread->ThreadId))
        {
            ++i;
            continue;
        }

        wcout << L" [" << i << "] Thread Id: " << thread->ThreadId << L" (" << to_hex(thread->ThreadId) << L')';
        if (!thread.thread_name().empty())
        {
            wcout << L" (" << thread.thread_name() << L")";
        }
        wcout << L'\n';
        wcout << L"   Priority: " << thread->Priority << L'\n';
        wcout << L"   PriorityClass: " << thread->PriorityClass << L'\n';
        wcout << L"   SuspendCount: " << thread->SuspendCount << L'\n';
        wcout << L"   TEB: " << to_hex_full(thread->Teb) << L'\n';

        load_and_dump_teb(mini_dump, symbol_engine, thread->Teb);

        dump_mini_dump_thread_context(thread.thread_context(), options);

        using namespace size_units::base_10;
        wcout << L"   Stack: " << to_hex_full(thread->Stack.StartOfMemoryRange) << L" - " <<
            to_hex_full(thread->Stack.StartOfMemoryRange + thread->Stack.Memory.DataSize) << L" (" <<
            to_hex(thread->Stack.Memory.DataSize) << L") (" << bytes{thread->Stack.Memory.DataSize} << L")\n";

        if (thread.stack() != nullptr)
        {
            if (options.load_symbols())
            {
                stream_stack_dump::hex_dump_stack(wcout, mini_dump, symbol_engine, thread->Stack.StartOfMemoryRange,
                                                  thread.stack(), thread->Stack.Memory.DataSize,
                                                  thread.thread_context(), 5);
            }
            else if (options.hex_dump_memory_data())
            {
                hex_dump::hex_dump(wcout, thread.stack(), thread->Stack.Memory.DataSize, 5, true, 16,
                                   thread->Stack.StartOfMemoryRange);
                wcout << L'\n';
            }
        }

        ++i;
    }
    wcout << L'\n';
}


void dump_mini_dump_thread_list_ex_stream_data(mini_dump const& mini_dump, size_t const index,
                                               dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    thread_ex_list_stream const thread_ex_list{mini_dump, index};

    if (!thread_ex_list.found())
    {
        wcout << L"ThreadListExStream not found!\n";
        return;
    }

    auto const thread_ids = get_filtered_thread_ids(options);

    wcout << L"NumberOfThreads: " << thread_ex_list.thread_list().NumberOfThreads << L'\n';
    for (size_t i = 0; auto const& thread : thread_ex_list.list())
    {
        if (!thread_ids.empty() && !thread_ids.contains(thread->ThreadId))
        {
            ++i;
            continue;
        }

        wcout << L" [" << i << "] Thread Id: " << thread->ThreadId << L" (" << to_hex(thread->ThreadId) << L')';
        if (!thread.thread_name().empty())
        {
            wcout << L" (" << thread.thread_name() << L")";
        }
        wcout << L'\n';
        wcout << L"   Priority: " << thread->Priority << L'\n';
        wcout << L"   PriorityClass: " << thread->PriorityClass << L'\n';
        wcout << L"   SuspendCount: " << thread->SuspendCount << L'\n';
        wcout << L"   TEB: " << to_hex_full(thread->Teb) << L'\n';

        load_and_dump_teb(mini_dump, symbol_engine, thread->Teb);

        using namespace size_units::base_10;
        wcout << L"   Stack: " << to_hex_full(thread->Stack.StartOfMemoryRange) << L" - " <<
            to_hex_full(thread->Stack.StartOfMemoryRange + thread->Stack.Memory.DataSize) << L" (" <<
            to_hex(thread->Stack.Memory.DataSize) << L") (" << bytes{thread->Stack.Memory.DataSize} << L")\n";
        if (thread.stack() != nullptr)
        {
            if (options.load_symbols())
            {
                stream_stack_dump::hex_dump_stack(wcout, mini_dump, symbol_engine, thread->Stack.StartOfMemoryRange,
                                                  thread.stack(), thread->Stack.Memory.DataSize,
                                                  thread.thread_context(), 5);
            }
            else if (options.hex_dump_memory_data())
            {
                hex_dump::hex_dump(wcout, thread.stack(), thread->Stack.Memory.DataSize, 5, true, 16,
                                   thread->Stack.StartOfMemoryRange);
                wcout << L'\n';
            }
        }

        dump_mini_dump_thread_context(thread.thread_context(), options);

        wcout << L"   BackingStore: " << to_hex_full(thread->BackingStore.StartOfMemoryRange) << L" - " << to_hex_full(
                thread->BackingStore.StartOfMemoryRange + thread->BackingStore.Memory.DataSize) << L" (" <<
            to_hex(thread->Stack.Memory.DataSize) << L") (" << bytes{thread->BackingStore.Memory.DataSize} << L")\n";

        if (options.hex_dump_memory_data())
        {
            if (thread.backing_store() != nullptr)
            {
                hex_dump::hex_dump(wcout, thread.backing_store(), thread->BackingStore.Memory.DataSize, 5, true, 16,
                                   thread->BackingStore.StartOfMemoryRange);
                wcout << L'\n';
            }
        }

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_thread_info_list_stream_data(mini_dump const& mini_dump, size_t const index)
{
    thread_info_list_stream const thread_info_list{mini_dump, index};

    if (!thread_info_list.found())
    {
        wcout << L"ThreadInfoListStream not found!\n";
        return;
    }

    if (!thread_info_list.is_valid())
    {
        wcout << L"ThreadInfoListStream version unknown!\n";
        return;
    }

    wcout << L"NumberOfEntries: " << thread_info_list.size() << L'\n';
    for (size_t i = 0; auto const& thread : thread_info_list.list())
    {
        using namespace time_units;
        wcout << L" [" << i << "]: ThreadId: " << thread->ThreadId << L" (" << to_hex(thread->ThreadId) << L')';
        if (!thread.thread_name().empty())
        {
            wcout << L" (" << thread.thread_name() << L")";
        }
        wcout << L'\n';
        wcout << L"   StartAddress: " << to_hex_full(thread->StartAddress) << L'\n';
        wcout << L"   Affinity: " << to_hex_full(thread->Affinity) << L'\n';
        if (thread->CreateTime > 0)
        {
            wcout << L"   CreateTime [local: " <<
                time_utils::to_local_time(time_utils::filetime_to_time_t(thread->CreateTime)) << L"] [UTC: " <<
                time_utils::to_utc_time(time_utils::filetime_to_time_t(thread->CreateTime)) << L"]\n";
        }
        if (thread->ExitTime > 0)
        {
            wcout << L"   ExitTime [local: " <<
                time_utils::to_local_time(time_utils::filetime_to_time_t(thread->ExitTime)) << L"] [UTC: " <<
                time_utils::to_utc_time(time_utils::filetime_to_time_t(thread->ExitTime)) << L"]\n";
        }
        wcout << L"   UserTime: " << thread->UserTime << L" (" << time_utils::duration_to_ms(thread->UserTime) <<
            L")\n";
        wcout << L"   KernelTime: " << thread->KernelTime << L" (" << time_utils::duration_to_ms(thread->KernelTime) <<
            L")\n";
        wcout << L"   ExitStatus: " << thread->ExitStatus << L'\n';
        wcout << L"   DumpFlags: " << thread_info_utils::dump_flags_to_string(thread->DumpFlags) << L" (" <<
            to_hex(thread->DumpFlags) << L")\n";
        wcout << L"   DumpError: " << to_hex(thread->DumpError) << L'\n';

        ++i;
    }
    wcout << L'\n';
}

void load_and_dump_teb(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine, ULONG64 const teb_address)
{
    symbol_type_utils::dump_variable_type_at(wcout, mini_dump, symbol_engine, common_symbol_names::teb_structure_symbol_name, teb_address);
}
