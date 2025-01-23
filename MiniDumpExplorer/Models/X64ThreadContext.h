#pragma once

#include "X64ThreadContext.g.h"

#include "DbgHelpUtils/stream_thread_context.h"
#include "DbgHelpUtils/xstate_reader.h"
#include "GlobalOptionsNotifyPropertyChangedBase.h"
 
namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;
    struct XmmRegister;
    struct YmmRegister;

    struct X64ThreadContext : X64ThreadContextT<X64ThreadContext>, GlobalOptionsNotifyPropertyChangedBase <X64ThreadContext>
    {
        X64ThreadContext();

        uint32_t ContextFlags() const { return context_.ContextFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> ContextFlagsList() const { return contextFlagsList_; }

        bool HasControl() const { return (context_.ContextFlags & X64_CONTEXT_CONTROL) == X64_CONTEXT_CONTROL; }
        uint64_t Rip() const { return context_.Rip; }
        uint64_t Rsp() const { return context_.Rsp; }
        uint64_t Rbp() const { return context_.Rbp; }
        uint16_t Cs() const { return context_.SegCs; }
        uint16_t Ss() const { return context_.SegSs; }
        uint32_t EFlags() const { return context_.EFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> EFlagsList() const { return eFlagsList_; }

        bool HasInteger() const { return (context_.ContextFlags & X64_CONTEXT_INTEGER) == X64_CONTEXT_INTEGER; }
        uint64_t Rax() const { return context_.Rax; }
        uint32_t Eax() const { return static_cast<uint32_t>(context_.Rax); }
        uint16_t Ax() const { return static_cast<uint16_t>(context_.Rax); }
        uint8_t Al() const { return static_cast<uint8_t>(context_.Rax); }
        uint8_t Ah() const { return static_cast<uint8_t>(context_.Rax >> 8); }
        uint64_t Rbx() const { return context_.Rbx; }
        uint32_t Ebx() const { return static_cast<uint32_t>(context_.Rbx); }
        uint16_t Bx() const { return static_cast<uint16_t>(context_.Rbx); }
        uint8_t Bl() const { return static_cast<uint8_t>(context_.Rbx); }
        uint8_t Bh() const { return static_cast<uint8_t>(context_.Rbx >> 8); }
        uint64_t Rcx() const { return context_.Rcx; }
        uint32_t Ecx() const { return static_cast<uint32_t>(context_.Rcx); }
        uint16_t Cx() const { return static_cast<uint16_t>(context_.Rcx); }
        uint8_t Cl() const { return static_cast<uint8_t>(context_.Rcx); }
        uint8_t Ch() const { return static_cast<uint8_t>(context_.Rcx >> 8); }
        uint64_t Rdx() const { return context_.Rdx; }
        uint32_t Edx() const { return static_cast<uint32_t>(context_.Rdx); }
        uint16_t Dx() const { return static_cast<uint16_t>(context_.Rdx); }
        uint8_t Dl() const { return static_cast<uint8_t>(context_.Rdx); }
        uint8_t Dh() const { return static_cast<uint8_t>(context_.Rdx >> 8); }
        uint64_t Rdi() const { return context_.Rdi; }
        uint32_t Edi() const { return static_cast<uint32_t>(context_.Rdi); }
        uint16_t Di() const { return static_cast<uint16_t>(context_.Rdi); }
        uint8_t Dil() const { return static_cast<uint8_t>(context_.Rdi); }
        uint64_t Rsi() const { return context_.Rsi; }
        uint32_t Esi() const { return static_cast<uint32_t>(context_.Rsi); }
        uint16_t Si() const { return static_cast<uint16_t>(context_.Rsi); }
        uint8_t Sil() const { return static_cast<uint8_t>(context_.Rsi); }
        uint64_t R8() const { return context_.R8; }
        uint32_t R8d() const { return static_cast<uint32_t>(context_.R8); }
        uint16_t R8w() const { return static_cast<uint16_t>(context_.R8); }
        uint8_t R8b() const { return static_cast<uint8_t>(context_.R8); }
        uint64_t R9() const { return context_.R9; }
        uint32_t R9d() const { return static_cast<uint32_t>(context_.R9); }
        uint16_t R9w() const { return static_cast<uint16_t>(context_.R9); }
        uint8_t R9b() const { return static_cast<uint8_t>(context_.R9); }
        uint64_t R10() const { return context_.R10; }
        uint32_t R10d() const { return static_cast<uint32_t>(context_.R10); }
        uint16_t R10w() const { return static_cast<uint16_t>(context_.R10); }
        uint8_t R10b() const { return static_cast<uint8_t>(context_.R10); }
        uint64_t R11() const { return context_.R11; }
        uint32_t R11d() const { return static_cast<uint32_t>(context_.R11); }
        uint16_t R11w() const { return static_cast<uint16_t>(context_.R11); }
        uint8_t R11b() const { return static_cast<uint8_t>(context_.R11); }
        uint64_t R12() const { return context_.R12; }
        uint32_t R12d() const { return static_cast<uint32_t>(context_.R12); }
        uint16_t R12w() const { return static_cast<uint16_t>(context_.R12); }
        uint8_t R12b() const { return static_cast<uint8_t>(context_.R12); }
        uint64_t R13() const { return context_.R13; }
        uint32_t R13d() const { return static_cast<uint32_t>(context_.R13); }
        uint16_t R13w() const { return static_cast<uint16_t>(context_.R13); }
        uint8_t R13b() const { return static_cast<uint8_t>(context_.R13); }
        uint64_t R14() const { return context_.R14; }
        uint32_t R14d() const { return static_cast<uint32_t>(context_.R14); }
        uint16_t R14w() const { return static_cast<uint16_t>(context_.R14); }
        uint8_t R14b() const { return static_cast<uint8_t>(context_.R14); }
        uint64_t R15() const { return context_.R15; }
        uint32_t R15d() const { return static_cast<uint32_t>(context_.R15); }
        uint16_t R15w() const { return static_cast<uint16_t>(context_.R15); }
        uint8_t R15b() const { return static_cast<uint8_t>(context_.R15); }

        bool HasSegments() const { return (context_.ContextFlags & X64_CONTEXT_SEGMENTS) == X64_CONTEXT_SEGMENTS; }
        uint16_t Ds() const { return context_.SegDs; }
        uint16_t Es() const { return context_.SegEs; }
        uint16_t Fs() const { return context_.SegFs; }
        uint16_t Gs() const { return context_.SegGs; }

        bool HasDebugRegisters() const { return (context_.ContextFlags & X64_CONTEXT_DEBUG_REGISTERS) == X64_CONTEXT_DEBUG_REGISTERS; }
        uint64_t Dr0() const { return context_.Dr0; }
        uint64_t Dr1() const { return context_.Dr1; }
        uint64_t Dr2() const { return context_.Dr2; }
        uint64_t Dr3() const { return context_.Dr3; }
        uint64_t Dr6() const { return context_.Dr6; }
        uint64_t Dr7() const { return context_.Dr7; }

        uint64_t DebugControl() const { return context_.DebugControl; }
        uint64_t LastBranchToRip() const { return context_.LastBranchToRip; }
        uint64_t LastBranchFromRip() const { return context_.LastBranchFromRip; }
        uint64_t LastExceptionToRip() const { return context_.LastExceptionToRip; }
        uint64_t LastExceptionFromRip() const { return context_.LastExceptionFromRip; }

        bool HasFloatingPoint() const { return (context_.ContextFlags & X64_CONTEXT_FLOATING_POINT) == X64_CONTEXT_FLOATING_POINT; }
        uint16_t ControlWord() const { return context_.FltSave.ControlWord; }
        uint16_t StatusWord() const { return context_.FltSave.StatusWord; }
        uint8_t TagWord() const { return context_.FltSave.TagWord; }
        uint16_t ErrorOpcode() const { return context_.FltSave.ErrorOpcode; }
        uint32_t ErrorOffset() const { return context_.FltSave.ErrorOffset; }
        uint16_t ErrorSelector() const { return context_.FltSave.ErrorSelector; }
        uint32_t DataOffset() const { return context_.FltSave.DataOffset; }
        uint16_t DataSelector() const { return context_.FltSave.DataSelector; }
        uint32_t MxCsr() const { return context_.FltSave.MxCsr; }
        Windows::Foundation::Collections::IObservableVector<hstring> MxCsrList() const { return mxCsrList_; }
        uint32_t MxCsrMask() const { return context_.FltSave.MxCsr_Mask; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> FloatRegisters() const { return floatRegisters_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> XmmRegisters() const { return xmmRegisters_; }

        bool HasAvx() const { return xstate_reader_.is_supported(); }
        bool HasAvxInInitState() const { return xstate_reader_.is_in_init_state(); }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::YmmRegister> YmmRegisters() const { return ymmRegisters_; }

        void Set(dlg_help_utils::stream_thread_context::context_x64 const& context);

    private:
        static MiniDumpExplorer::XmmRegister CreateM128A(std::wstring const& name, _M128A const& value);

    private:
        dlg_help_utils::stream_thread_context::context_x64 context_{};
        Windows::Foundation::Collections::IObservableVector<hstring> contextFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> eFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> mxCsrList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> floatRegisters_{single_threaded_observable_vector<MiniDumpExplorer::XmmRegister>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::XmmRegister> xmmRegisters_{single_threaded_observable_vector<MiniDumpExplorer::XmmRegister>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::YmmRegister> ymmRegisters_{single_threaded_observable_vector<MiniDumpExplorer::YmmRegister>()};
        dlg_help_utils::xstate_reader xstate_reader_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct X64ThreadContext : X64ThreadContextT<X64ThreadContext, implementation::X64ThreadContext>
    {
    };
}
