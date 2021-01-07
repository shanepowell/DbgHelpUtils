#pragma once

namespace dlg_help_utils
{
    class exception_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpException : winrt::implements<MiniDumpException, winrt::MiniDumpExplorer::IMiniDumpException>
    {
        MiniDumpException(dlg_help_utils::exception_stream const& exception);

        [[nodiscard]] uint32_t ExceptionCode() const;
        [[nodiscard]] winrt::hstring ExceptionCodeString() const;
        [[nodiscard]] uint32_t ExceptionFlags() const;
        [[nodiscard]] winrt::hstring ExceptionFlagsString() const;
        [[nodiscard]] uint64_t ExceptionRecord() const;
        [[nodiscard]] uint64_t ExceptionAddress() const;
        [[nodiscard]] uint32_t NumberParameters() const;
        [[nodiscard]] winrt::com_array<uint64_t> ExceptionInformation() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> Parameters() const;

    private:
        dlg_help_utils::exception_stream const& exception_;
        winrt::hstring exception_code_;
        winrt::hstring exception_flags_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> parameters_{winrt::single_threaded_observable_vector<winrt::hstring>()};
    };
}
