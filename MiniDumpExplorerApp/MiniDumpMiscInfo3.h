#pragma once

namespace dlg_help_utils
{
    class misc_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpMiscInfo3 : winrt::implements<MiniDumpMiscInfo3, winrt::MiniDumpExplorer::IMiniDumpMiscInfo3>
    {
        MiniDumpMiscInfo3(dlg_help_utils::misc_info_stream const& misc_info);

        [[nodiscard]] bool HasProcessIntegrityLevel() const;
        [[nodiscard]] uint32_t ProcessIntegrityLevel() const;
        [[nodiscard]] winrt::hstring ProcessIntegrityLevelString() const;
        [[nodiscard]] bool HasProcessExecuteFlags() const;
        [[nodiscard]] uint32_t ProcessExecuteFlags() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> ProcessExecuteFlagStrings() const;
        [[nodiscard]] bool HasProtectedProcess() const;
        [[nodiscard]] uint32_t ProtectedProcess() const;
        [[nodiscard]] bool HasTimeZone() const;
        [[nodiscard]] uint32_t TimeZoneId() const;
        [[nodiscard]] winrt::MiniDumpExplorer::ITimeZoneInformation TimeZone() const;

    private:
        dlg_help_utils::misc_info_stream const& misc_info_;
        winrt::hstring process_integrity_level_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> process_execute_flags_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::MiniDumpExplorer::ITimeZoneInformation time_zone_;
    };
}
