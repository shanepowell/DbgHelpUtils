#pragma once
#include "../DbgHelpUtils/symbol_address_info.h"

namespace MiniDumpExplorerApp
{
    struct StackFrame : winrt::implements<StackFrame, winrt::MiniDumpExplorer::IStackFrame>
    {
        StackFrame(dlg_help_utils::dbg_help::symbol_address_info info, bool is_x86_address);

        [[nodiscard]] uint64_t StackAddress() const;
        [[nodiscard]] uint64_t Address() const;
        [[nodiscard]] winrt::hstring ModuleName() const;
        [[nodiscard]] winrt::hstring SymbolName() const;
        [[nodiscard]] winrt::hstring FileName() const;
        [[nodiscard]] uint32_t LineNumber() const;
        [[nodiscard]] uint64_t ModuleDisplacement() const;
        [[nodiscard]] uint64_t SymbolDisplacement() const;
        [[nodiscard]] uint32_t LineDisplacement() const;
        [[nodiscard]] bool InLine() const;
        [[nodiscard]] bool Found() const;
        [[nodiscard]] bool FrameContentFound() const;

        [[nodiscard]] winrt::hstring DisplayLine() const;

    private:
        dlg_help_utils::dbg_help::symbol_address_info info_;
        winrt::hstring module_name_;
        winrt::hstring symbol_name_;
        winrt::hstring file_name_;
        winrt::hstring display_line_;
    };
}
