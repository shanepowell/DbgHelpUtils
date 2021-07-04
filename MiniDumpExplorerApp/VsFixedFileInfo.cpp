#include "pch.h"
#include "VsFixedFileInfo.h"

#include "../DbgHelpUtils/system_info_utils.h"
#include "../DbgHelpUtils/time_utils.h"

namespace MiniDumpExplorerApp
{
    VsFixedFileInfo::VsFixedFileInfo(VS_FIXEDFILEINFO const& version_info)
        : version_info_{version_info}
        , struc_version_{dlg_help_utils::system_info_utils::version_info_to_string(version_info_.dwStrucVersion)}
        , file_version_{dlg_help_utils::system_info_utils::version_info_to_string(version_info_.dwFileVersionMS, version_info_.dwFileVersionLS)}
        , product_version_{dlg_help_utils::system_info_utils::version_info_to_string(version_info_.dwProductVersionMS, version_info_.dwProductVersionLS)}
        , file_type_{dlg_help_utils::system_info_utils::version_file_type_to_string(version_info_.dwFileType, version_info_.dwFileSubtype)}
    {
        for(auto const& value : dlg_help_utils::system_info_utils::version_file_flags_to_strings(version_info_.dwFileFlags, version_info_.dwFileFlagsMask))
        {
            file_flag_types_.Append(winrt::hstring{value});
        }

        for(auto const& value : dlg_help_utils::system_info_utils::version_file_os_to_strings(version_info_.dwFileOS))
        {
            file_os_types_.Append(winrt::hstring{value});
        }
    }

    uint32_t VsFixedFileInfo::Signature() const
    {
        return version_info_.dwSignature;
    }

    winrt::hstring VsFixedFileInfo::StrucVersion() const
    {
        return struc_version_;
    }

    winrt::hstring VsFixedFileInfo::FileVersion() const
    {
        return file_version_;
    }

    winrt::hstring VsFixedFileInfo::ProductVersion() const
    {
        return product_version_;
    }

    uint32_t VsFixedFileInfo::FileFlagsMask() const
    {
        return version_info_.dwFileFlagsMask;
    }

    uint32_t VsFixedFileInfo::FileFlags() const
    {
        return version_info_.dwFileFlags;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> VsFixedFileInfo::FileFlagTypes() const
    {
        return file_flag_types_;
    }

    uint32_t VsFixedFileInfo::FileOS() const
    {
        return version_info_.dwFileOS;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> VsFixedFileInfo::FileOSTypes() const
    {
        return file_os_types_;
    }

    uint32_t VsFixedFileInfo::FileType() const
    {
        return version_info_.dwFileType;
    }

    uint32_t VsFixedFileInfo::FileSubtype() const
    {
        return version_info_.dwFileSubtype;
    }

    winrt::hstring VsFixedFileInfo::FileTypeString() const
    {
        return file_type_;
    }

    bool VsFixedFileInfo::HasFileDate() const
    {
        return version_info_.dwFileDateMS != 0 && version_info_.dwFileDateLS != 0;
    }

    winrt::clock::time_point VsFixedFileInfo::FileDate() const
    {
        if(!HasFileDate())
        {
            return {};
        }

        FILETIME ft;
        ft.dwHighDateTime = version_info_.dwFileDateMS;
        ft.dwLowDateTime = version_info_.dwFileDateLS;
        return winrt::clock::from_FILETIME(ft);
    }
}
