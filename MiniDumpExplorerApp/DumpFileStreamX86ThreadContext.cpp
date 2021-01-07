#include "pch.h"
#include "DumpFileStreamX86ThreadContext.h"

#include "Wow64FloatingSaveArea.h"
#include "../DbgHelpUtils/hex_dump.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamX86ThreadContext::DumpFileStreamX86ThreadContext(WOW64_CONTEXT const& thread_context)
        : thread_context_{thread_context}
        , float_save_{*winrt::make_self<Wow64FloatingSaveArea>(thread_context_.FloatSave)}
    {
        std::wstringstream ss;
        dlg_help_utils::hex_dump::hex_dump(ss, thread_context_.ExtendedRegisters, sizeof(thread_context_.ExtendedRegisters), 0);
        extended_registers_hex_dump_ = std::move(ss).str();
    }

    uint32_t DumpFileStreamX86ThreadContext::ContextFlags() const
    {
        return thread_context_.ContextFlags;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr0() const
    {
        return thread_context_.Dr0;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr1() const
    {
        return thread_context_.Dr1;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr2() const
    {
        return thread_context_.Dr2;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr3() const
    {
        return thread_context_.Dr3;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr6() const
    {
        return thread_context_.Dr6;
    }

    uint32_t DumpFileStreamX86ThreadContext::Dr7() const
    {
        return thread_context_.Dr7;
    }

    winrt::MiniDumpExplorer::IWow64FloatingSaveArea DumpFileStreamX86ThreadContext::FloatSave() const
    {
        return float_save_;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegGs() const
    {
        return thread_context_.SegGs;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegFs() const
    {
        return thread_context_.SegFs;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegEs() const
    {
        return thread_context_.SegEs;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegDs() const
    {
        return thread_context_.SegDs;
    }

    uint32_t DumpFileStreamX86ThreadContext::Edi() const
    {
        return thread_context_.Edi;
    }

    uint32_t DumpFileStreamX86ThreadContext::Esi() const
    {
        return thread_context_.Esi;
    }

    uint32_t DumpFileStreamX86ThreadContext::Ebx() const
    {
        return thread_context_.Ebx;
    }

    uint32_t DumpFileStreamX86ThreadContext::Edx() const
    {
        return thread_context_.Edx;
    }

    uint32_t DumpFileStreamX86ThreadContext::Ecx() const
    {
        return thread_context_.Ecx;
    }

    uint32_t DumpFileStreamX86ThreadContext::Eax() const
    {
        return thread_context_.Eax;
    }

    uint32_t DumpFileStreamX86ThreadContext::Ebp() const
    {
        return thread_context_.Ebp;
    }

    uint32_t DumpFileStreamX86ThreadContext::Eip() const
    {
        return thread_context_.Eip;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegCs() const
    {
        return thread_context_.SegCs;
    }

    uint32_t DumpFileStreamX86ThreadContext::EFlags() const
    {
        return thread_context_.EFlags;
    }

    uint32_t DumpFileStreamX86ThreadContext::Esp() const
    {
        return thread_context_.Esp;
    }

    uint32_t DumpFileStreamX86ThreadContext::SegSs() const
    {
        return thread_context_.SegSs;
    }

    winrt::com_array<uint8_t> DumpFileStreamX86ThreadContext::ExtendedRegisters() const
    {
        return winrt::com_array<uint8_t>(thread_context_.ExtendedRegisters, thread_context_.ExtendedRegisters + WOW64_MAXIMUM_SUPPORTED_EXTENSION);
    }

    winrt::hstring DumpFileStreamX86ThreadContext::ExtendedRegistersHexDump() const
    {
        std::wstringstream ss;
        dlg_help_utils::hex_dump::hex_dump(ss, thread_context_.ExtendedRegisters, sizeof(thread_context_.ExtendedRegisters), 0);
        return winrt::hstring{std::move(ss).str()};
    }
}
