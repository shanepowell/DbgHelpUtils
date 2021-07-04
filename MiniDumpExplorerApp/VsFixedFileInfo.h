#pragma once

namespace MiniDumpExplorerApp
{
    struct VsFixedFileInfo : winrt::implements<VsFixedFileInfo, winrt::MiniDumpExplorer::IVsFixedFileInfo>
    {
        VsFixedFileInfo(VS_FIXEDFILEINFO const& version_info);

        [[nodiscard]] uint32_t Signature() const;
        [[nodiscard]] winrt::hstring StrucVersion() const;
        [[nodiscard]] winrt::hstring FileVersion() const;
        [[nodiscard]] winrt::hstring ProductVersion() const;
        [[nodiscard]] uint32_t FileFlagsMask() const;
        [[nodiscard]] uint32_t FileFlags() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FileFlagTypes() const;
        [[nodiscard]] uint32_t FileOS() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FileOSTypes() const;
        [[nodiscard]] uint32_t FileType() const;
        [[nodiscard]] uint32_t FileSubtype() const;
        [[nodiscard]] winrt::hstring FileTypeString() const;
        [[nodiscard]] bool HasFileDate() const;
        [[nodiscard]] winrt::clock::time_point FileDate() const;

    private:
        VS_FIXEDFILEINFO const& version_info_;
        winrt::hstring struc_version_;
        winrt::hstring file_version_;
        winrt::hstring product_version_;
        winrt::hstring file_type_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> file_flag_types_{winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> file_os_types_{winrt::single_threaded_observable_vector<winrt::hstring>()};
    };
}
