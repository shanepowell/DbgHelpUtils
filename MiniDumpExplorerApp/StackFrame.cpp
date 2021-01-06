#include "pch.h"
#include "StackFrame.h"

#include "../DbgHelpUtils/stream_stack_dump.h"

namespace MiniDumpExplorerApp
{
    StackFrame::StackFrame(dlg_help_utils::dbg_help::symbol_address_info info, bool const is_x86_address)
        : info_{std::move(info)}
        , module_name_{info_.module_name}
        , symbol_name_{info_.symbol_name}
        , file_name_{info_.file_name}
        , display_line_{dlg_help_utils::stream_stack_dump::hex_dump_stack_frame(info, is_x86_address)}
    {
    }

    uint64_t StackFrame::StackAddress() const
    {
        return info_.stack;
    }

    uint64_t StackFrame::Address() const
    {
        return info_.address;
    }

    winrt::hstring StackFrame::ModuleName() const
    {
        return module_name_;
    }

    winrt::hstring StackFrame::SymbolName() const
    {
        return symbol_name_;
    }

    winrt::hstring StackFrame::FileName() const
    {
        return file_name_;
    }

    uint32_t StackFrame::LineNumber() const
    {
        return info_.line_number;
    }

    uint64_t StackFrame::ModuleDisplacement() const
    {
        return info_.module_displacement;
    }

    uint64_t StackFrame::SymbolDisplacement() const
    {
        return info_.symbol_displacement;
    }

    uint32_t StackFrame::LineDisplacement() const
    {
        return info_.line_displacement;
    }

    bool StackFrame::InLine() const
    {
        return info_.in_line;
    }

    bool StackFrame::Found() const
    {
        return info_.found;
    }

    bool StackFrame::FrameContentFound() const
    {
        return info_.frame_content_found;
    }

    winrt::hstring StackFrame::DisplayLine() const
    {
        return display_line_;
    }
}

