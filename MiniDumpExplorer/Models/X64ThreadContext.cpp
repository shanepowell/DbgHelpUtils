#include "pch.h"
#include "X64ThreadContext.h"

#include "DbgHelpUtils/context_utils.h"
#include "M128A.h"
#include "XmmRegister.h"
#include "YmmRegister.h"

#include <format>

#if __has_include("X64ThreadContext.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X64ThreadContext.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

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
                L"Dil",
                L"Rsi",
                L"Esi",
                L"Si",
                L"Sil",
                L"R8",
                L"R8d",
                L"R8w",
                L"R8b",
                L"R9",
                L"R9d",
                L"R9w",
                L"R9b",
                L"R10",
                L"R10d",
                L"R10w",
                L"R10b",
                L"R11",
                L"R11d",
                L"R11w",
                L"R11b",
                L"R12",
                L"R12d",
                L"R12w",
                L"R12b",
                L"R13",
                L"R13d",
                L"R13w",
                L"R13b",
                L"R14",
                L"R14d",
                L"R14w",
                L"R14b",
                L"R15",
                L"R15d",
                L"R15w",
                L"R15b",
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
            floatRegisters_.Append(CreateM128A(std::format(L"ST{}", index), context.Legacy[index]));
        }

        xmmRegisters_.Append(CreateM128A(L"XMM0"s, context.Xmm0));
        xmmRegisters_.Append(CreateM128A(L"XMM1"s, context.Xmm1));
        xmmRegisters_.Append(CreateM128A(L"XMM2"s, context.Xmm2));
        xmmRegisters_.Append(CreateM128A(L"XMM3"s, context.Xmm3));
        xmmRegisters_.Append(CreateM128A(L"XMM4"s, context.Xmm4));
        xmmRegisters_.Append(CreateM128A(L"XMM5"s, context.Xmm5));
        xmmRegisters_.Append(CreateM128A(L"XMM6"s, context.Xmm6));
        xmmRegisters_.Append(CreateM128A(L"XMM7"s, context.Xmm7));
        xmmRegisters_.Append(CreateM128A(L"XMM8"s, context.Xmm8));
        xmmRegisters_.Append(CreateM128A(L"XMM9"s, context.Xmm9));
        xmmRegisters_.Append(CreateM128A(L"XMM10"s, context.Xmm10));
        xmmRegisters_.Append(CreateM128A(L"XMM11"s, context.Xmm11));
        xmmRegisters_.Append(CreateM128A(L"XMM12"s, context.Xmm12));
        xmmRegisters_.Append(CreateM128A(L"XMM13"s, context.Xmm13));
        xmmRegisters_.Append(CreateM128A(L"XMM14"s, context.Xmm14));
        xmmRegisters_.Append(CreateM128A(L"XMM15"s, context.Xmm15));

        xstate_reader_ = dlg_help_utils::xstate_reader{ &context };

        for (auto const& ymm : xstate_reader_.ymm_registers())
        {
            MiniDumpExplorer::YmmRegister ymmRegister{};
            ymmRegister.as<YmmRegister>()->Set(ymm);
            ymmRegisters_.Append(ymmRegister);
        }
    }

    MiniDumpExplorer::XmmRegister X64ThreadContext::CreateM128A(std::wstring const& name, _M128A const& value)
    {
        MiniDumpExplorer::M128A m128a{};
        m128a.as<M128A>()->Set(value);

        MiniDumpExplorer::XmmRegister xmmRegister{};
        xmmRegister.as<XmmRegister>()->Set(name, m128a);
        return xmmRegister;
    }
}
