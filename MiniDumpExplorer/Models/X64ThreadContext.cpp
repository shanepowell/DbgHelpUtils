#include "pch.h"
#include "X64ThreadContext.h"

#include "DbgHelpUtils/context_utils.h"
#include "M128A.h"
#include "YmmRegister.h"

#if __has_include("X64ThreadContext.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X64ThreadContext.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    X64ThreadContext::X64ThreadContext()
        : GlobalOptionsNotifyPropertyChangedBase(
        {
            L"ContextFlags",
            L"Rip",
            L"Rsp",
            L"Rbp",
            L"Cs",
            L"Ss",
            L"EFlags",
            L"Rax",
            L"Eax",
            L"Ax",
            L"Al",
            L"Ah",
            L"Rbx",
            L"Ebx",
            L"Bx",
            L"Bl",
            L"Bh",
            L"Rcx",
            L"Ecx",
            L"Cx",
            L"Cl",
            L"Ch",
            L"Rdx",
            L"Edx",
            L"Dx",
            L"Dl",
            L"Dh",
            L"Rdi",
            L"Edi",
            L"Di",
            L"Rsi",
            L"Esi",
            L"Si",
            L"R8",
            L"R9",
            L"R10",
            L"R11",
            L"R12",
            L"R13",
            L"R14",
            L"R15",
            L"Ds",
            L"Es",
            L"Fs",
            L"Gs",
            L"Dr0",
            L"Dr1",
            L"Dr2",
            L"Dr3",
            L"Dr6",
            L"Dr7",
            L"DebugControl",
            L"LastBranchToRip",
            L"LastBranchFromRip",
            L"LastExceptionToRip",
            L"LastExceptionFromRip",
            L"ControlWord",
            L"StatusWord",
            L"TagWord",
            L"ErrorOpcode",
            L"ErrorOffset",
            L"ErrorSelector",
            L"DataOffset",
            L"DataSelector",
            L"MxCsr",
            L"MxCsrMask"
        },
        {
        }, 
        {
        },
        {
        })
    {
    }

    void X64ThreadContext::Set(dlg_help_utils::stream_thread_context::context_x64 const& context)
    {
        context_ = context;

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_x64_thread_context_flags(context_.ContextFlags))
        {
            contextFlagsList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_flags_register(context_.EFlags))
        {
            eFlagsList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_mx_csr_register(context_.FltSave.MxCsr))
        {
            mxCsrList_.Append(flag);
        }

        for (size_t index = 0; index < std::size(context.Legacy); ++index)
        {
            floatRegisters_.Append(CreateM128A(context.Legacy[index]));
        }

        xmmRegisters_.Append(CreateM128A(context.Xmm0));
        xmmRegisters_.Append(CreateM128A(context.Xmm1));
        xmmRegisters_.Append(CreateM128A(context.Xmm2));
        xmmRegisters_.Append(CreateM128A(context.Xmm3));
        xmmRegisters_.Append(CreateM128A(context.Xmm4));
        xmmRegisters_.Append(CreateM128A(context.Xmm5));
        xmmRegisters_.Append(CreateM128A(context.Xmm6));
        xmmRegisters_.Append(CreateM128A(context.Xmm7));
        xmmRegisters_.Append(CreateM128A(context.Xmm8));
        xmmRegisters_.Append(CreateM128A(context.Xmm9));
        xmmRegisters_.Append(CreateM128A(context.Xmm10));
        xmmRegisters_.Append(CreateM128A(context.Xmm11));
        xmmRegisters_.Append(CreateM128A(context.Xmm12));
        xmmRegisters_.Append(CreateM128A(context.Xmm13));
        xmmRegisters_.Append(CreateM128A(context.Xmm14));
        xmmRegisters_.Append(CreateM128A(context.Xmm15));

        xstate_reader_ = dlg_help_utils::xstate_reader{ &context_ };

        for (auto const& ymm : xstate_reader_.ymm_registers())
        {
            MiniDumpExplorer::YmmRegister ymmRegister{};
            ymmRegister.as<YmmRegister>()->Set(ymm);
            ymmRegisters_.Append(ymmRegister);
        }
    }

    MiniDumpExplorer::M128A X64ThreadContext::CreateM128A(_M128A const& value)
    {
        MiniDumpExplorer::M128A m128a{};
        m128a.as<M128A>()->Set(value);
        return m128a;
    }
}
