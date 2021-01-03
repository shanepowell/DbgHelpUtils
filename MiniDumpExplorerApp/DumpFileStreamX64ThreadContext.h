#pragma once

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamX64ThreadContext : winrt::implements<DumpFileStreamX64ThreadContext, winrt::MiniDumpExplorer::IDumpFileStreamX64ThreadContext>
    {
        DumpFileStreamX64ThreadContext(CONTEXT const& thread_context);

        [[nodiscard]] uint64_t P1Home() const;
        [[nodiscard]] uint64_t P2Home() const;
        [[nodiscard]] uint64_t P3Home() const;
        [[nodiscard]] uint64_t P4Home() const;
        [[nodiscard]] uint64_t P5Home() const;
        [[nodiscard]] uint64_t P6Home() const;
        [[nodiscard]] uint32_t ContextFlags() const;
        [[nodiscard]] uint32_t MxCsr() const;
        [[nodiscard]] uint16_t SegCs() const;
        [[nodiscard]] uint16_t SegDs() const;
        [[nodiscard]] uint16_t SegEs() const;
        [[nodiscard]] uint16_t SegFs() const;
        [[nodiscard]] uint16_t SegGs() const;
        [[nodiscard]] uint16_t SegSs() const;
        [[nodiscard]] uint32_t EFlags() const;
        [[nodiscard]] uint64_t Dr0() const;
        [[nodiscard]] uint64_t Dr1() const;
        [[nodiscard]] uint64_t Dr2() const;
        [[nodiscard]] uint64_t Dr3() const;
        [[nodiscard]] uint64_t Dr6() const;
        [[nodiscard]] uint64_t Dr7() const;
        [[nodiscard]] uint64_t Rax() const;
        [[nodiscard]] uint64_t Rcx() const;
        [[nodiscard]] uint64_t Rdx() const;
        [[nodiscard]] uint64_t Rbx() const;
        [[nodiscard]] uint64_t Rsp() const;
        [[nodiscard]] uint64_t Rbp() const;
        [[nodiscard]] uint64_t Rsi() const;
        [[nodiscard]] uint64_t Rdi() const;
        [[nodiscard]] uint64_t R8() const;
        [[nodiscard]] uint64_t R9() const;
        [[nodiscard]] uint64_t R10() const;
        [[nodiscard]] uint64_t R11() const;
        [[nodiscard]] uint64_t R12() const;
        [[nodiscard]] uint64_t R13() const;
        [[nodiscard]] uint64_t R14() const;
        [[nodiscard]] uint64_t R15() const;
        [[nodiscard]] uint64_t Rip() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> Header() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> Legacy() const;
        [[nodiscard]] winrt::hstring Xmm0() const;
        [[nodiscard]] winrt::hstring Xmm1() const;
        [[nodiscard]] winrt::hstring Xmm2() const;
        [[nodiscard]] winrt::hstring Xmm3() const;
        [[nodiscard]] winrt::hstring Xmm4() const;
        [[nodiscard]] winrt::hstring Xmm5() const;
        [[nodiscard]] winrt::hstring Xmm6() const;
        [[nodiscard]] winrt::hstring Xmm7() const;
        [[nodiscard]] winrt::hstring Xmm8() const;
        [[nodiscard]] winrt::hstring Xmm9() const;
        [[nodiscard]] winrt::hstring Xmm10() const;
        [[nodiscard]] winrt::hstring Xmm11() const;
        [[nodiscard]] winrt::hstring Xmm12() const;
        [[nodiscard]] winrt::hstring Xmm13() const;
        [[nodiscard]] winrt::hstring Xmm14() const;
        [[nodiscard]] winrt::hstring Xmm15() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> VectorRegister() const;
        [[nodiscard]] uint64_t VectorControl() const;
        [[nodiscard]] uint64_t DebugControl() const;
        [[nodiscard]] uint64_t LastBranchToRip() const;
        [[nodiscard]] uint64_t LastBranchFromRip() const;
        [[nodiscard]] uint64_t LastExceptionToRip() const;
        [[nodiscard]] uint64_t LastExceptionFromRip() const;

    private:
        CONTEXT const& thread_context_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> header_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> legacy_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> vector_register_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::hstring xmm0_;
        winrt::hstring xmm1_;
        winrt::hstring xmm2_;
        winrt::hstring xmm3_;
        winrt::hstring xmm4_;
        winrt::hstring xmm5_;
        winrt::hstring xmm6_;
        winrt::hstring xmm7_;
        winrt::hstring xmm8_;
        winrt::hstring xmm9_;
        winrt::hstring xmm10_;
        winrt::hstring xmm11_;
        winrt::hstring xmm12_;
        winrt::hstring xmm13_;
        winrt::hstring xmm14_;
        winrt::hstring xmm15_;
    };
}
