#include "pch.h"
#include "Wow64ThreadContext.h"

#include "DbgHelpUtils/context_utils.h"
#include "M128A.h"
#include "XmmRegister.h"
#include "YmmRegister.h"

#include <format>

#if __has_include("Wow64ThreadContext.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Wow64ThreadContext.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    Wow64ThreadContext::Wow64ThreadContext()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"ContextFlags",
                L"Eip",
                L"Esp",
                L"Ebp",
                L"Cs",
                L"Ss",
                L"EFlags",
                L"Eax",
                L"Ax",
                L"Al",
                L"Ah",
                L"Ebx",
                L"Bx",
                L"Bl",
                L"Bh",
                L"Ecx",
                L"Cx",
                L"Cl",
                L"Ch",
                L"Edx",
                L"Dx",
                L"Dl",
                L"Dh",
                L"Edi",
                L"Di",
                L"Dil",
                L"Esi",
                L"Si",
                L"Sil",
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
                L"ControlWord",
                L"StatusWord",
                L"TagWord",
                L"ErrorOffset",
                L"ErrorSelector",
                L"DataOffset",
                L"DataSelector",
                L"Cr0NpxState"
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

    void Wow64ThreadContext::Set(WOW64_CONTEXT const& context)
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

        //for (size_t index = 0; index < std::size(context.Legacy); ++index)
        //{
        //    floatRegisters_.Append(CreateM128A(std::format(L"ST{}", index), context.Legacy[index]));
        //}

        //xmmRegisters_.Append(CreateM128A(L"XMM0"s, context.Xmm0));
        //xmmRegisters_.Append(CreateM128A(L"XMM1"s, context.Xmm1));
        //xmmRegisters_.Append(CreateM128A(L"XMM2"s, context.Xmm2));
        //xmmRegisters_.Append(CreateM128A(L"XMM3"s, context.Xmm3));
        //xmmRegisters_.Append(CreateM128A(L"XMM4"s, context.Xmm4));
        //xmmRegisters_.Append(CreateM128A(L"XMM5"s, context.Xmm5));
        //xmmRegisters_.Append(CreateM128A(L"XMM6"s, context.Xmm6));
        //xmmRegisters_.Append(CreateM128A(L"XMM7"s, context.Xmm7));
        //xmmRegisters_.Append(CreateM128A(L"XMM8"s, context.Xmm8));
        //xmmRegisters_.Append(CreateM128A(L"XMM9"s, context.Xmm9));
        //xmmRegisters_.Append(CreateM128A(L"XMM10"s, context.Xmm10));
        //xmmRegisters_.Append(CreateM128A(L"XMM11"s, context.Xmm11));
        //xmmRegisters_.Append(CreateM128A(L"XMM12"s, context.Xmm12));
        //xmmRegisters_.Append(CreateM128A(L"XMM13"s, context.Xmm13));
        //xmmRegisters_.Append(CreateM128A(L"XMM14"s, context.Xmm14));
        //xmmRegisters_.Append(CreateM128A(L"XMM15"s, context.Xmm15));

        xstate_reader_ = dlg_help_utils::xstate_reader{ &context };

        for (auto const& ymm : xstate_reader_.ymm_registers())
        {
            MiniDumpExplorer::YmmRegister ymmRegister{};
            ymmRegister.as<YmmRegister>()->Set(ymm);
            ymmRegisters_.Append(ymmRegister);
        }
    }

    MiniDumpExplorer::XmmRegister Wow64ThreadContext::CreateM128A(std::wstring const& name, _M128A const& value)
    {
        MiniDumpExplorer::M128A m128a{};
        m128a.as<M128A>()->Set(value);

        MiniDumpExplorer::XmmRegister xmmRegister{};
        xmmRegister.as<XmmRegister>()->Set(name, m128a);
        return xmmRegister;
    }
}
