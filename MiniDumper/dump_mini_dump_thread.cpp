#include "dump_mini_dump_thread.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/context_utils.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/m128a_utils.h"
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
#include "DbgHelpUtils/xstate_reader.h"

using namespace std;
using namespace std::string_literals;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

namespace
{
    std::set<uint32_t> get_filtered_thread_ids(dump_file_options const& options)
    {
        return vector_to_hash_set<uint32_t>(options.filter_values(L"thread_id"s));
    }

}

void dump_mini_dump_thread_context(std::wostream& log, stream_thread_context const& thread_context, dump_file_options const& options)
{
    log << L"\n  ThreadContext:\n";
    auto force_hex_dump = false;
    if (thread_context.x64_thread_context_available())
    {
        dump_mini_dump_x64_thread_context(log, thread_context.x64_thread_context());
    }
    else if (thread_context.wow64_thread_context_available())
    {
        dump_mini_dump_wow64_thread_context(log, thread_context.wow64_thread_context());
    }
    else if (thread_context.x86_thread_context_available())
    {
        dump_mini_dump_x86_thread_context(log, thread_context.x86_thread_context());
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
    log << std::format(L"    ContextFlags: ({}) {}\n", to_hex_full(context.ContextFlags), context_utils::resources::get_x64_thread_context_flags_to_string(context.ContextFlags));

    if((context.ContextFlags & X64_CONTEXT_CONTROL) == X64_CONTEXT_CONTROL)
    {
        log << std::format(L"    RIP: {}\n", to_hex_full(context.Rip));
        log << std::format(L"    RSP: {}\n", to_hex_full(context.Rsp));
    }
    if((context.ContextFlags & X64_CONTEXT_INTEGER) == X64_CONTEXT_INTEGER)
    {
        log << std::format(L"    RAX: {}\n", to_hex_full(context.Rax));
        log << std::format(L"    RBX: {}\n", to_hex_full(context.Rbx));
        log << std::format(L"    RCX: {}\n", to_hex_full(context.Rcx));
        log << std::format(L"    RDX: {}\n", to_hex_full(context.Rdx));
        log << std::format(L"    RDI: {}\n", to_hex_full(context.Rdi));
        log << std::format(L"    RSI: {}\n", to_hex_full(context.Rsi));
    }
    if((context.ContextFlags & X64_CONTEXT_CONTROL) == X64_CONTEXT_CONTROL)
    {
        log << std::format(L"    RBP: {}\n", to_hex_full(context.Rbp));
    }
    if((context.ContextFlags & X64_CONTEXT_INTEGER) == X64_CONTEXT_INTEGER)
    {
        log << std::format(L"    R8: {}\n", to_hex_full(context.R8));
        log << std::format(L"    R9: {}\n", to_hex_full(context.R9));
        log << std::format(L"    R10: {}\n", to_hex_full(context.R10));
        log << std::format(L"    R11: {}\n", to_hex_full(context.R11));
        log << std::format(L"    R12: {}\n", to_hex_full(context.R12));
        log << std::format(L"    R13: {}\n", to_hex_full(context.R13));
        log << std::format(L"    R14: {}\n", to_hex_full(context.R14));
        log << std::format(L"    R15: {}\n", to_hex_full(context.R15));
    }
    if((context.ContextFlags & X64_CONTEXT_CONTROL) == X64_CONTEXT_CONTROL)
    {
        log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    }
    if((context.ContextFlags & X64_CONTEXT_SEGMENTS) == X64_CONTEXT_SEGMENTS)
    {
        log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
        log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
        log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
        log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    }
    if((context.ContextFlags & X64_CONTEXT_CONTROL) == X64_CONTEXT_CONTROL)
    {
        log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
        log << std::format(L"    EFlags: ({}) {}\n", to_hex_full(context.EFlags), context_utils::resources::get_flags_register_to_string(context.EFlags));
    }
    if((context.ContextFlags & X64_CONTEXT_DEBUG_REGISTERS) == X64_CONTEXT_DEBUG_REGISTERS)
    {
        log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
        log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
        log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
        log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
        log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
        log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));
    }

    if((context.ContextFlags & X64_CONTEXT_FLOATING_POINT) == X64_CONTEXT_FLOATING_POINT)
    {
        log << L"    Floating Point:\n";
        log << std::format(L"      ControlWord: {}\n", to_hex_full(context.FltSave.ControlWord));
        log << std::format(L"      StatusWord: {}\n", to_hex_full(context.FltSave.StatusWord));
        log << std::format(L"      TagWord: {}\n", to_hex_full(context.FltSave.TagWord));
        log << std::format(L"      ErrorOpcode: {}\n", to_hex_full(context.FltSave.ErrorOpcode));
        log << std::format(L"      ErrorOffset (FPIP): {}\n", to_hex_full(context.FltSave.ErrorOffset));
        log << std::format(L"      ErrorSelector (FPCS): {}\n", to_hex_full(context.FltSave.ErrorSelector));
        log << std::format(L"      DataOffset (FPDO): {}\n", to_hex_full(context.FltSave.DataOffset));
        log << std::format(L"      DataSelector (FPDS): {}\n", to_hex_full(context.FltSave.DataSelector));
        log << std::format(L"      MxCsr: ({}) {}\n", to_hex_full(context.FltSave.MxCsr), context_utils::resources::get_mx_csr_register_to_string(context.FltSave.MxCsr));
        log << std::format(L"      MxCsr_Mask: {}\n", to_hex_full(context.FltSave.MxCsr_Mask));
        log << std::format(L"      FloatRegisters:\n");
        for (size_t index = 0; index < std::size(context.Legacy); ++index)
        {
            auto value = m128a_utils::to_float80(context.Legacy[index]);
            log << std::format(L"        st{0}: {1:>55} ({2})\n", locale_formatting::to_wstring(index), dlg_help_utils::to_wstring(value), to_hex(value));
        }

        log << L"    XMM:\n";
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
    }

    if (xstate_reader xstate_reader{ &context };
        xstate_reader.is_supported())
    {
        log << L"    AVX:\n";
        if (xstate_reader.is_in_init_state())
        {
            log << L"      AVX is in the INIT state (YMM_H registers are all zero).\n";
        }

        for (auto const& ymm : xstate_reader.ymm_registers())
        {
            
            log << std::format(L"      Ymm{}: {} - {}\n", ymm.index, to_hex_full(*ymm.xmm), to_hex_full(*ymm.ymm));
        }
    }

    if((context.ContextFlags & X64_CONTEXT_DEBUG_REGISTERS) == X64_CONTEXT_DEBUG_REGISTERS)
    {
        log << std::format(L"    DebugControl: {}\n", to_hex_full(context.DebugControl));
        log << std::format(L"    LastBranchToRip: {}\n", to_hex_full(context.LastBranchToRip));
        log << std::format(L"    LastBranchFromRip: {}\n", to_hex_full(context.LastBranchFromRip));
        log << std::format(L"    LastExceptionToRip: {}\n", to_hex_full(context.LastExceptionToRip));
        log << std::format(L"    LastExceptionFromRip: {}\n", to_hex_full(context.LastExceptionFromRip));
    }
}

void dump_mini_dump_x86_thread_context(std::wostream& log, stream_thread_context::context_x86 const& context)
{
    log << std::format(L"    ContextFlags: ({}) {}\n", to_hex_full(context.ContextFlags), context_utils::resources::get_x86_thread_context_flags_to_string(context.ContextFlags));
    if((context.ContextFlags & X86_CONTEXT_CONTROL) == X86_CONTEXT_CONTROL)
    {
        log << std::format(L"    EIP: {}\n", to_hex_full(context.Eip));
        log << std::format(L"    ESP: {}\n", to_hex_full(context.Esp));
    }
    if((context.ContextFlags & X86_CONTEXT_INTEGER) == X86_CONTEXT_INTEGER)
    {
        log << std::format(L"    EAX: {}\n", to_hex_full(context.Eax));
        log << std::format(L"    EBX: {}\n", to_hex_full(context.Ebx));
        log << std::format(L"    ECX: {}\n", to_hex_full(context.Ecx));
        log << std::format(L"    EDX: {}\n", to_hex_full(context.Edx));
        log << std::format(L"    EDI: {}\n", to_hex_full(context.Edi));
        log << std::format(L"    ESI: {}\n", to_hex_full(context.Esi));
    }
    if((context.ContextFlags & X86_CONTEXT_CONTROL) == X86_CONTEXT_CONTROL)
    {
        log << std::format(L"    EBP: {}\n", to_hex_full(context.Ebp));
        log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    }
    if((context.ContextFlags & X86_CONTEXT_SEGMENTS) == X86_CONTEXT_SEGMENTS)
    {
        log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
        log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
        log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
        log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    }
    if((context.ContextFlags & X86_CONTEXT_CONTROL) == X86_CONTEXT_CONTROL)
    {
        log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
        log << std::format(L"    EFlags: ({}) {}\n", to_hex_full(context.EFlags), context_utils::resources::get_flags_register_to_string(context.EFlags));
    }
    if((context.ContextFlags & X86_CONTEXT_DEBUG_REGISTERS) == X86_CONTEXT_DEBUG_REGISTERS)
    {
        log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
        log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
        log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
        log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
        log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
        log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));
    }

    if((context.ContextFlags & X86_CONTEXT_FLOATING_POINT) == X86_CONTEXT_FLOATING_POINT)
    {
        log << L"    FloatSave:\n";
        log << std::format(L"      ControlWord: {}\n", to_hex(context.FloatSave.ControlWord));
        log << std::format(L"      StatusWord: {}\n", to_hex(context.FloatSave.StatusWord));
        log << std::format(L"      TagWord: {}\n", to_hex(context.FloatSave.TagWord));
        log << std::format(L"      ErrorOffset (FPIP): {}\n", to_hex(context.FloatSave.ErrorOffset));
        log << std::format(L"      ErrorSelector (FPCS): {}\n", to_hex(context.FloatSave.ErrorSelector));
        log << std::format(L"      DataOffset (FPDO): {}\n", to_hex(context.FloatSave.DataOffset));
        log << std::format(L"      DataSelector (FPDS): {}\n", to_hex(context.FloatSave.DataSelector));
        log << std::format(L"      FloatRegisters:\n");

        auto const* float_registers = reinterpret_cast<float80_t const*>(context.FloatSave.RegisterArea);
        for (size_t index = 0; index < 8; ++index)
        {
            auto value = float_registers[index];
            log << std::format(L"        st{0}: {1:>55} ({2})\n", locale_formatting::to_wstring(index), dlg_help_utils::to_wstring(value), to_hex(value));
        }
        log << L'\n';
    }

    if((context.ContextFlags & X86_CONTEXT_EXTENDED_REGISTERS) == X86_CONTEXT_EXTENDED_REGISTERS)
    {
        log << L"    ExtendedRegisters:\n";
        hex_dump::hex_dump(log, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 6);
        log << L'\n';
    }
}

void dump_mini_dump_wow64_thread_context(std::wostream& log, WOW64_CONTEXT const& context)
{
    log << std::format(L"    ContextFlags: ({}) {}\n", to_hex_full(context.ContextFlags), context_utils::resources::get_wow64_thread_context_flags_to_string(context.ContextFlags));
    if((context.ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL)
    {
        log << std::format(L"    EIP: {}\n", to_hex_full(context.Eip));
        log << std::format(L"    ESP: {}\n", to_hex_full(context.Esp));
    }
    if((context.ContextFlags & WOW64_CONTEXT_INTEGER) == WOW64_CONTEXT_INTEGER)
    {
        log << std::format(L"    EAX: {}\n", to_hex_full(context.Eax));
        log << std::format(L"    EBX: {}\n", to_hex_full(context.Ebx));
        log << std::format(L"    ECX: {}\n", to_hex_full(context.Ecx));
        log << std::format(L"    EDX: {}\n", to_hex_full(context.Edx));
        log << std::format(L"    EDI: {}\n", to_hex_full(context.Edi));
        log << std::format(L"    ESI: {}\n", to_hex_full(context.Esi));
    }
    if((context.ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL)
    {
        log << std::format(L"    EBP: {}\n", to_hex_full(context.Ebp));
        log << std::format(L"    CS: {}\n", to_hex_full(context.SegCs));
    }
    if((context.ContextFlags & WOW64_CONTEXT_SEGMENTS) == WOW64_CONTEXT_SEGMENTS)
    {
        log << std::format(L"    DS: {}\n", to_hex_full(context.SegDs));
        log << std::format(L"    ES: {}\n", to_hex_full(context.SegEs));
        log << std::format(L"    FS: {}\n", to_hex_full(context.SegFs));
        log << std::format(L"    GS: {}\n", to_hex_full(context.SegGs));
    }
    if((context.ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL)
    {
        log << std::format(L"    SS: {}\n", to_hex_full(context.SegSs));
        log << std::format(L"    EFlags: ({}) {}\n", to_hex_full(context.EFlags), context_utils::resources::get_flags_register_to_string(context.EFlags));
    }
    if((context.ContextFlags & WOW64_CONTEXT_DEBUG_REGISTERS) == WOW64_CONTEXT_DEBUG_REGISTERS)
    {
        log << std::format(L"    DR0: {}\n", to_hex_full(context.Dr0));
        log << std::format(L"    DR1: {}\n", to_hex_full(context.Dr1));
        log << std::format(L"    DR2: {}\n", to_hex_full(context.Dr2));
        log << std::format(L"    DR3: {}\n", to_hex_full(context.Dr3));
        log << std::format(L"    DR6: {}\n", to_hex_full(context.Dr6));
        log << std::format(L"    DR7: {}\n", to_hex_full(context.Dr7));
    }

    if((context.ContextFlags & WOW64_CONTEXT_FLOATING_POINT) == WOW64_CONTEXT_FLOATING_POINT)
    {
        log << L"    FloatSave:\n";
        log << std::format(L"      ControlWord: {}\n", to_hex(context.FloatSave.ControlWord));
        log << std::format(L"      StatusWord: {}\n", to_hex(context.FloatSave.StatusWord));
        log << std::format(L"      TagWord: {}\n", to_hex(context.FloatSave.TagWord));
        log << std::format(L"      ErrorOffset (FPIP): {}\n", to_hex(context.FloatSave.ErrorOffset));
        log << std::format(L"      ErrorSelector (FPCS): {}\n", to_hex(context.FloatSave.ErrorSelector));
        log << std::format(L"      DataOffset (FPDO): {}\n", to_hex(context.FloatSave.DataOffset));
        log << std::format(L"      DataSelector (FPDS): {}\n", to_hex(context.FloatSave.DataSelector));
        log << std::format(L"      Cr0NpxState: {}\n", to_hex(context.FloatSave.Cr0NpxState));
        log << std::format(L"      FloatRegisters:\n");

        auto const* float_registers = reinterpret_cast<float80_t const*>(context.FloatSave.RegisterArea);
        for (size_t index = 0; index < 8; ++index)
        {
            auto value = float_registers[index];
            log << std::format(L"        st{0}: {1:>55} ({2})\n", locale_formatting::to_wstring(index), dlg_help_utils::to_wstring(value), to_hex(value));
        }
        log << L'\n';
    }

    if((context.ContextFlags & WOW64_CONTEXT_EXTENDED_REGISTERS) == WOW64_CONTEXT_EXTENDED_REGISTERS)
    {
        log << L"    ExtendedRegisters:\n";
        hex_dump::hex_dump(log, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 6);
        log << L'\n';
    }

    if (xstate_reader xstate_reader{ &context };
        xstate_reader.is_supported())
    {
        log << L"    AVX:\n";
        if (xstate_reader.is_in_init_state())
        {
            log << L"      AVX is in the INIT state (YMM_H registers are all zero).\n";
        }

        for (auto const& ymm : xstate_reader.ymm_registers())
        {
            
            log << std::format(L"      Ymm{}: {} - {}\n", ymm.index, to_hex_full(*ymm.xmm), to_hex_full(*ymm.ymm));
        }
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
                dump_stack_to_stream(log, mini_dump, symbol_engine, thread->Stack.StartOfMemoryRange,
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
                dump_stack_to_stream(log
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
    log << "\n";
}
