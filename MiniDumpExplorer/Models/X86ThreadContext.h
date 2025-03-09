#pragma once

#include "X86ThreadContext.g.h"

#include "DbgHelpUtils/stream_thread_context.h"
#include "DbgHelpUtils/xstate_reader.h"
#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/float80.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Float80;
    struct Float80Register;

    struct X86ThreadContext : X86ThreadContextT<X86ThreadContext>, GlobalOptionsNotifyPropertyChangedBase<X86ThreadContext>
    {
        X86ThreadContext();

        uint32_t ContextFlags() const { return context_.ContextFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> ContextFlagsList() const { return contextFlagsList_; }

        bool HasControl() const { return (context_.ContextFlags & WOW64_CONTEXT_CONTROL) == WOW64_CONTEXT_CONTROL; }
        uint32_t Eip() const { return context_.Eip; }
        uint32_t Esp() const { return context_.Esp; }
        uint32_t Ebp() const { return context_.Ebp; }
        uint32_t Cs() const { return context_.SegCs; }
        uint32_t Ss() const { return context_.SegSs; }
        uint32_t EFlags() const { return context_.EFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> EFlagsList() const { return eFlagsList_; }

        bool HasInteger() const { return (context_.ContextFlags & WOW64_CONTEXT_INTEGER) == WOW64_CONTEXT_INTEGER; }
        uint32_t Eax() const { return static_cast<uint32_t>(context_.Eax); }
        uint16_t Ax() const { return static_cast<uint16_t>(context_.Eax); }
        uint8_t Al() const { return static_cast<uint8_t>(context_.Eax); }
        uint8_t Ah() const { return static_cast<uint8_t>(context_.Eax >> 8); }
        uint32_t Ebx() const { return static_cast<uint32_t>(context_.Ebx); }
        uint16_t Bx() const { return static_cast<uint16_t>(context_.Ebx); }
        uint8_t Bl() const { return static_cast<uint8_t>(context_.Ebx); }
        uint8_t Bh() const { return static_cast<uint8_t>(context_.Ebx >> 8); }
        uint32_t Ecx() const { return static_cast<uint32_t>(context_.Ecx); }
        uint16_t Cx() const { return static_cast<uint16_t>(context_.Ecx); }
        uint8_t Cl() const { return static_cast<uint8_t>(context_.Ecx); }
        uint8_t Ch() const { return static_cast<uint8_t>(context_.Ecx >> 8); }
        uint32_t Edx() const { return static_cast<uint32_t>(context_.Edx); }
        uint16_t Dx() const { return static_cast<uint16_t>(context_.Edx); }
        uint8_t Dl() const { return static_cast<uint8_t>(context_.Edx); }
        uint8_t Dh() const { return static_cast<uint8_t>(context_.Edx >> 8); }
        uint32_t Edi() const { return static_cast<uint32_t>(context_.Edi); }
        uint16_t Di() const { return static_cast<uint16_t>(context_.Edi); }
        uint8_t Dil() const { return static_cast<uint8_t>(context_.Edi); }
        uint32_t Esi() const { return static_cast<uint32_t>(context_.Esi); }
        uint16_t Si() const { return static_cast<uint16_t>(context_.Esi); }
        uint8_t Sil() const { return static_cast<uint8_t>(context_.Esi); }

        bool HasSegments() const { return (context_.ContextFlags & WOW64_CONTEXT_SEGMENTS) == WOW64_CONTEXT_SEGMENTS; }
        uint32_t Ds() const { return context_.SegDs; }
        uint32_t Es() const { return context_.SegEs; }
        uint32_t Fs() const { return context_.SegFs; }
        uint32_t Gs() const { return context_.SegGs; }

        bool HasDebugRegisters() const { return (context_.ContextFlags & WOW64_CONTEXT_DEBUG_REGISTERS) == WOW64_CONTEXT_DEBUG_REGISTERS; }
        uint32_t Dr0() const { return context_.Dr0; }
        uint32_t Dr1() const { return context_.Dr1; }
        uint32_t Dr2() const { return context_.Dr2; }
        uint32_t Dr3() const { return context_.Dr3; }
        uint32_t Dr6() const { return context_.Dr6; }
        uint32_t Dr7() const { return context_.Dr7; }

        bool HasFloatingPoint() const { return (context_.ContextFlags & WOW64_CONTEXT_FLOATING_POINT) == WOW64_CONTEXT_FLOATING_POINT; }
        uint32_t ControlWord() const { return context_.FloatSave.ControlWord; }
        Windows::Foundation::Collections::IObservableVector<hstring> ControlWordList() const { return controlWordList_; }
        uint32_t StatusWord() const { return context_.FloatSave.StatusWord; }
        Windows::Foundation::Collections::IObservableVector<hstring> StatusWordList() const { return statusWordList_; }
        uint32_t TagWord() const { return context_.FloatSave.TagWord; }
        Windows::Foundation::Collections::IObservableVector<hstring> TagWordList() const { return tagWordList_; }
        uint32_t ErrorOffset() const { return context_.FloatSave.ErrorOffset; }
        uint32_t ErrorSelector() const { return context_.FloatSave.ErrorSelector; }
        uint32_t DataOffset() const { return context_.FloatSave.DataOffset; }
        uint32_t DataSelector() const { return context_.FloatSave.DataSelector; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Float80Register> FloatRegisters() const { return floatRegisters_; }

        bool HasExtendedRegisters() const { return (context_.ContextFlags & X86_CONTEXT_EXTENDED_REGISTERS) == X86_CONTEXT_EXTENDED_REGISTERS; }
        hstring ExtendedRegistersHexDump() const { return extendedRegistersHexDump_; }

        void Set(dlg_help_utils::stream_thread_context::context_x86 const& context);

    private:
        static MiniDumpExplorer::Float80Register CreateFloat80(std::wstring const& name, dlg_help_utils::float80_t const& value);

    private:
        dlg_help_utils::stream_thread_context::context_x86 context_{};
        Windows::Foundation::Collections::IObservableVector<hstring> contextFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> eFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> controlWordList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> statusWordList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> tagWordList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Float80Register> floatRegisters_{single_threaded_observable_vector<MiniDumpExplorer::Float80Register>()};
        dlg_help_utils::xstate_reader xstate_reader_{};
        hstring extendedRegistersHexDump_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct X86ThreadContext : X86ThreadContextT<X86ThreadContext, implementation::X86ThreadContext>
    {
    };
}
