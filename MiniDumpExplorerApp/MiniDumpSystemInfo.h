#pragma once

namespace dlg_help_utils
{
    class system_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpSystemInfo : winrt::implements<MiniDumpSystemInfo, winrt::MiniDumpExplorer::IMiniDumpSystemInfo>
    {
        MiniDumpSystemInfo(dlg_help_utils::system_info_stream const& system_info);

        [[nodiscard]] uint16_t ProcessorArchitecture() const;
        [[nodiscard]] winrt::hstring ProcessorArchitectureString() const;
        [[nodiscard]] uint16_t ProcessorLevel() const;
        [[nodiscard]] uint16_t ProcessorRevision() const;

        [[nodiscard]] uint8_t NumberOfProcessors() const;
        [[nodiscard]] uint8_t ProductType() const;
        [[nodiscard]] winrt::hstring ProductTypeString() const;

        [[nodiscard]] uint32_t MajorVersion() const;
        [[nodiscard]] uint32_t MinorVersion() const;
        [[nodiscard]] uint32_t BuildNumber() const;
        [[nodiscard]] winrt::hstring OsVersionString() const;
        [[nodiscard]] winrt::hstring WindowsOsVersionString() const;
        [[nodiscard]] uint32_t PlatformId() const;
        [[nodiscard]] winrt::hstring PlatformString() const;

        [[nodiscard]] uint32_t CSDVersionRva() const;

        [[nodiscard]] uint16_t SuiteMask() const;
        [[nodiscard]] uint16_t Reserved2() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> Suites() const;
    
        [[nodiscard]] winrt::MiniDumpExplorer::ICpuInformation Cpu() const;

    private:
        dlg_help_utils::system_info_stream const& system_info_;
        winrt::hstring processor_architecture_;
        winrt::hstring product_type_;
        winrt::hstring os_version_;
        winrt::hstring windows_os_version_;
        winrt::hstring platform_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> suites_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::MiniDumpExplorer::ICpuInformation cpu_info_;
    };
}
