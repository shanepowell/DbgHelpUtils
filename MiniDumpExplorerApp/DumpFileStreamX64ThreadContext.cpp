#include "pch.h"
#include "DumpFileStreamX64ThreadContext.h"

#include "HStringHelper.h"

namespace MiniDumpExplorerApp
{
    namespace
    {
    }

    DumpFileStreamX64ThreadContext::DumpFileStreamX64ThreadContext(CONTEXT const& thread_context)
        : thread_context_{thread_context}
        , xmm0_{to_hex_string(thread_context_.Xmm0)}
        , xmm1_{to_hex_string(thread_context_.Xmm1)}
        , xmm2_{to_hex_string(thread_context_.Xmm2)}
        , xmm3_{to_hex_string(thread_context_.Xmm3)}
        , xmm4_{to_hex_string(thread_context_.Xmm4)}
        , xmm5_{to_hex_string(thread_context_.Xmm5)}
        , xmm6_{to_hex_string(thread_context_.Xmm6)}
        , xmm7_{to_hex_string(thread_context_.Xmm7)}
        , xmm8_{to_hex_string(thread_context_.Xmm8)}
        , xmm9_{to_hex_string(thread_context_.Xmm9)}
        , xmm10_{to_hex_string(thread_context_.Xmm10)}
        , xmm11_{to_hex_string(thread_context_.Xmm11)}
        , xmm12_{to_hex_string(thread_context_.Xmm12)}
        , xmm13_{to_hex_string(thread_context_.Xmm13)}
        , xmm14_{to_hex_string(thread_context_.Xmm14)}
        , xmm15_{to_hex_string(thread_context_.Xmm15)}
    {
        for(auto const& value : thread_context_.Header)
        {
            header_.Append(to_hex_string(value));
        }

        for(auto const& value : thread_context_.Legacy)
        {
            legacy_.Append(to_hex_string(value));
        }

        for(auto const& value : thread_context_.VectorRegister)
        {
            vector_register_.Append(to_hex_string(value));
        }
    }

    uint64_t DumpFileStreamX64ThreadContext::P1Home() const
    {
        return thread_context_.P1Home;
    }

    uint64_t DumpFileStreamX64ThreadContext::P2Home() const
    {
        return thread_context_.P2Home;
    }

    uint64_t DumpFileStreamX64ThreadContext::P3Home() const
    {
        return thread_context_.P3Home;
    }

    uint64_t DumpFileStreamX64ThreadContext::P4Home() const
    {
        return thread_context_.P4Home;
    }

    uint64_t DumpFileStreamX64ThreadContext::P5Home() const
    {
        return thread_context_.P5Home;
    }

    uint64_t DumpFileStreamX64ThreadContext::P6Home() const
    {
        return thread_context_.P6Home;
    }

    uint32_t DumpFileStreamX64ThreadContext::ContextFlags() const
    {
        return thread_context_.ContextFlags;
    }

    uint32_t DumpFileStreamX64ThreadContext::MxCsr() const
    {
        return thread_context_.MxCsr;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegCs() const
    {
        return thread_context_.SegCs;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegDs() const
    {
        return thread_context_.SegDs;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegEs() const
    {
        return thread_context_.SegEs;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegFs() const
    {
        return thread_context_.SegFs;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegGs() const
    {
        return thread_context_.SegGs;
    }

    uint16_t DumpFileStreamX64ThreadContext::SegSs() const
    {
        return thread_context_.SegSs;
    }

    uint32_t DumpFileStreamX64ThreadContext::EFlags() const
    {
        return thread_context_.EFlags;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr0() const
    {
        return thread_context_.Dr0;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr1() const
    {
        return thread_context_.Dr1;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr2() const
    {
        return thread_context_.Dr2;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr3() const
    {
        return thread_context_.Dr3;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr6() const
    {
        return thread_context_.Dr6;
    }

    uint64_t DumpFileStreamX64ThreadContext::Dr7() const
    {
        return thread_context_.Dr7;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rax() const
    {
        return thread_context_.Rax;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rcx() const
    {
        return thread_context_.Rcx;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rdx() const
    {
        return thread_context_.Rdx;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rbx() const
    {
        return thread_context_.Rbx;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rsp() const
    {
        return thread_context_.Rsp;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rbp() const
    {
        return thread_context_.Rbp;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rsi() const
    {
        return thread_context_.Rsi;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rdi() const
    {
        return thread_context_.Rdi;
    }

    uint64_t DumpFileStreamX64ThreadContext::R8() const
    {
        return thread_context_.R8;
    }

    uint64_t DumpFileStreamX64ThreadContext::R9() const
    {
        return thread_context_.R9;
    }

    uint64_t DumpFileStreamX64ThreadContext::R10() const
    {
        return thread_context_.R10;
    }

    uint64_t DumpFileStreamX64ThreadContext::R11() const
    {
        return thread_context_.R11;
    }

    uint64_t DumpFileStreamX64ThreadContext::R12() const
    {
        return thread_context_.R12;
    }

    uint64_t DumpFileStreamX64ThreadContext::R13() const
    {
        return thread_context_.R13;
    }

    uint64_t DumpFileStreamX64ThreadContext::R14() const
    {
        return thread_context_.R14;
    }

    uint64_t DumpFileStreamX64ThreadContext::R15() const
    {
        return thread_context_.R15;
    }

    uint64_t DumpFileStreamX64ThreadContext::Rip() const
    {
        return thread_context_.Rip;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> DumpFileStreamX64ThreadContext::Header() const
    {
        return header_;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> DumpFileStreamX64ThreadContext::Legacy() const
    {
        return legacy_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm0() const
    {
        return xmm0_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm1() const
    {
        return xmm1_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm2() const
    {
        return xmm2_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm3() const
    {
        return xmm3_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm4() const
    {
        return xmm4_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm5() const
    {
        return xmm5_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm6() const
    {
        return xmm6_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm7() const
    {
        return xmm7_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm8() const
    {
        return xmm8_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm9() const
    {
        return xmm9_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm10() const
    {
        return xmm10_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm11() const
    {
        return xmm11_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm12() const
    {
        return xmm12_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm13() const
    {
        return xmm13_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm14() const
    {
        return xmm14_;
    }

    winrt::hstring DumpFileStreamX64ThreadContext::Xmm15() const
    {
        return xmm15_;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> DumpFileStreamX64ThreadContext::VectorRegister() const
    {
        return vector_register_;
    }

    uint64_t DumpFileStreamX64ThreadContext::VectorControl() const
    {
        return thread_context_.VectorControl;
    }

    uint64_t DumpFileStreamX64ThreadContext::DebugControl() const
    {
        return thread_context_.DebugControl;
    }

    uint64_t DumpFileStreamX64ThreadContext::LastBranchToRip() const
    {
        return thread_context_.LastBranchToRip;
    }

    uint64_t DumpFileStreamX64ThreadContext::LastBranchFromRip() const
    {
        return thread_context_.LastBranchFromRip;
    }

    uint64_t DumpFileStreamX64ThreadContext::LastExceptionToRip() const
    {
        return thread_context_.LastExceptionToRip;
    }

    uint64_t DumpFileStreamX64ThreadContext::LastExceptionFromRip() const
    {
        return thread_context_.LastExceptionFromRip;
    }
}
