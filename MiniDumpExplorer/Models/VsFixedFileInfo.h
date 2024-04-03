#pragma once

#include "VsFixedFileInfo.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct VsFixedFileInfo : VsFixedFileInfoT<VsFixedFileInfo>, GlobalOptionsNotifyPropertyChangedBase<VsFixedFileInfo>
    {
        VsFixedFileInfo();

        uint32_t Signature() const { return file_info_->dwSignature; }
        uint32_t InfoVersion() const { return file_info_->dwStrucVersion; }
        hstring InfoVersionString() const { return info_version_; }
        hstring FileVersion() const { return file_version_; }
        hstring ProductVersion() const { return product_version_; }
        uint32_t FileFlagsMask() const { return file_info_->dwFileFlagsMask; }
        uint32_t FileFlags() const { return file_info_->dwFileFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> FileFlagsList() const { return fileFlagsList_; }
        uint32_t FileOS() const { return file_info_->dwFileOS; }
        hstring FileOSString() const { return file_os_; }
        uint32_t FileType() const { return file_info_->dwFileType; }
        uint32_t FileSubtype() const  { return file_info_->dwFileSubtype; }
        hstring FileTypeString() const { return file_type_; }
        int64_t FileDate() const { return file_date_; }

        void Set(VS_FIXEDFILEINFO const& file_info);

    private:
        VS_FIXEDFILEINFO const* file_info_{nullptr};
        hstring info_version_;
        hstring file_version_;
        hstring product_version_;
        Windows::Foundation::Collections::IObservableVector<hstring> fileFlagsList_{single_threaded_observable_vector<hstring>()};
        hstring file_os_;
        hstring file_type_;
        int64_t file_date_{0};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct VsFixedFileInfo : VsFixedFileInfoT<VsFixedFileInfo, implementation::VsFixedFileInfo>
    {
    };
}
