#include "pch.h"
#include "X86ThreadContext.h"

#include "Float80.h"
#include "Float80Register.h"

#include "DbgHelpUtils/context_utils.h"
#include "DbgHelpUtils/hex_dump.h"

#if __has_include("X86ThreadContext.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X86ThreadContext.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    X86ThreadContext::X86ThreadContext()
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

    void X86ThreadContext::Set(dlg_help_utils::stream_thread_context::context_x86 const& context)
    {
        context_ = context;

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_x86_thread_context_flags(context_.ContextFlags))
        {
            contextFlagsList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_flags_register(context_.EFlags))
        {
            eFlagsList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_npx_status_word(static_cast<uint16_t>(context_.FloatSave.StatusWord)))
        {
            statusWordList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_npx_control_word(static_cast<uint16_t>(context_.FloatSave.ControlWord)))
        {
            controlWordList_.Append(flag);
        }

        for (auto const& flag : dlg_help_utils::context_utils::resources::get_npx_tag_word(static_cast<uint16_t>(context_.FloatSave.TagWord)))
        {
            tagWordList_.Append(flag);
        }

        auto const* float_registers = reinterpret_cast<dlg_help_utils::float80_t const*>(context_.FloatSave.RegisterArea);
        for (size_t index = 0; index < 8; ++index)
        {
            auto value = float_registers[index];
            floatRegisters_.Append(CreateFloat80(std::format(L"ST{}", index), value));
        }
    
        if (HasExtendedRegisters())
        {
            std::wstringstream ss;
            dlg_help_utils::hex_dump::hex_dump(ss, context.ExtendedRegisters, sizeof(context.ExtendedRegisters), 0);
            extendedRegistersHexDump_ = std::move(ss).str();
        }
    }

    MiniDumpExplorer::Float80Register X86ThreadContext::CreateFloat80(std::wstring const& name, dlg_help_utils::float80_t const& value)
    {
        MiniDumpExplorer::Float80 float80{};
        float80.as<Float80>()->Set(value);

        MiniDumpExplorer::Float80Register float80Register{};
        float80Register.as<Float80Register>()->Set(name, float80);
        return float80Register;
    }
}
