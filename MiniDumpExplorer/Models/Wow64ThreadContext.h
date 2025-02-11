#pragma once

#include "Wow64ThreadContext.g.h"

#include "DbgHelpUtils/stream_thread_context.h"
#include "DbgHelpUtils/xstate_reader.h"
#include "GlobalOptionsNotifyPropertyChangedBase.h"
 
namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;
    struct XmmRegister;
    struct YmmRegister;

    struct Wow64ThreadContext : Wow64ThreadContextT<Wow64ThreadContext>, GlobalOptionsNotifyPropertyChangedBase <Wow64ThreadContext>
    {
        Wow64ThreadContext();

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
        uint32_t StatusWord() const { return context_.FloatSave.StatusWord; }
        uint32_t TagWord() const { return context_.FloatSave.TagWord; }
        uint32_t ErrorOffset() const { return context_.FloatSave.ErrorOffset; }
        uint32_t ErrorSelector() const { return context_.FloatSave.ErrorSelector; }
        uint32_t DataOffset() const { return context_.FloatSave.DataOffset; }
        uint32_t DataSelector() const { return context_.FloatSave.DataSelector; }
        uint32_t Cr0NpxState() const { return context_.FloatSave.Cr0NpxState; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> FloatRegisters() const { return floatRegisters_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> XmmRegisters() const { return xmmRegisters_; }

        bool HasAvx() const { return xstate_reader_.is_supported(); }
        bool HasAvxInInitState() const { return xstate_reader_.is_in_init_state(); }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::YmmRegister> YmmRegisters() const { return ymmRegisters_; }

        void Set(WOW64_CONTEXT const& context);

    private:
        static MiniDumpExplorer::XmmRegister CreateM128A(std::wstring const& name, _M128A const& value);

    private:
        WOW64_CONTEXT context_{};
        Windows::Foundation::Collections::IObservableVector<hstring> contextFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> eFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> floatRegisters_{single_threaded_observable_vector<MiniDumpExplorer::XmmRegister>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> xmmRegisters_{single_threaded_observable_vector<MiniDumpExplorer::XmmRegister>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::YmmRegister> ymmRegisters_{single_threaded_observable_vector<MiniDumpExplorer::YmmRegister>()};
        dlg_help_utils::xstate_reader xstate_reader_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Wow64ThreadContext : Wow64ThreadContextT<Wow64ThreadContext, implementation::Wow64ThreadContext>
    {
    };
}
