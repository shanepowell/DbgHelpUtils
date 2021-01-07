#pragma once

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamX86ThreadContext : winrt::implements<DumpFileStreamX86ThreadContext, winrt::MiniDumpExplorer::IDumpFileStreamX86ThreadContext>
    {
        DumpFileStreamX86ThreadContext(WOW64_CONTEXT const& thread_context);

        [[nodiscard]] uint32_t ContextFlags() const;
        [[nodiscard]] uint32_t Dr0() const;
        [[nodiscard]] uint32_t Dr1() const;
        [[nodiscard]] uint32_t Dr2() const;
        [[nodiscard]] uint32_t Dr3() const;
        [[nodiscard]] uint32_t Dr6() const;
        [[nodiscard]] uint32_t Dr7() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IWow64FloatingSaveArea FloatSave() const;
        [[nodiscard]] uint32_t SegGs() const;
        [[nodiscard]] uint32_t SegFs() const;
        [[nodiscard]] uint32_t SegEs() const;
        [[nodiscard]] uint32_t SegDs() const;
        [[nodiscard]] uint32_t Edi() const;
        [[nodiscard]] uint32_t Esi() const;
        [[nodiscard]] uint32_t Ebx() const;
        [[nodiscard]] uint32_t Edx() const;
        [[nodiscard]] uint32_t Ecx() const;
        [[nodiscard]] uint32_t Eax() const;
        [[nodiscard]] uint32_t Ebp() const;
        [[nodiscard]] uint32_t Eip() const;
        [[nodiscard]] uint32_t SegCs() const;
        [[nodiscard]] uint32_t EFlags() const;
        [[nodiscard]] uint32_t Esp() const;
        [[nodiscard]] uint32_t SegSs() const;
        [[nodiscard]] winrt::com_array<uint8_t> ExtendedRegisters() const;

        [[nodiscard]] winrt::hstring ExtendedRegistersHexDump() const;

    private:
        WOW64_CONTEXT const& thread_context_;
        winrt::MiniDumpExplorer::IWow64FloatingSaveArea float_save_;
        winrt::hstring extended_registers_hex_dump_;
    };
}
