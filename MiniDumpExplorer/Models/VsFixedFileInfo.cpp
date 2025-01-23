#include "pch.h"
#include "VsFixedFileInfo.h"

#include "DbgHelpUtils/system_info_utils.h"
#include "Models/DumpFileTimeStamp.h"

#if __has_include("VsFixedFileInfo.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "VsFixedFileInfo.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils;

namespace winrt::MiniDumpExplorer::implementation
{
    VsFixedFileInfo::VsFixedFileInfo()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Signature",
                L"InfoVersion",
                L"FileFlagsMask",
                L"FileFlags",
                L"FileOS",
                L"FileType",
                L"FileSubtype",
            },
            {
            }, 
            {
                L"FileDate"
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void VsFixedFileInfo::Set(VS_FIXEDFILEINFO const& file_info, time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        file_info_ = &file_info;
        info_version_ = system_info_utils::version_info_to_string(file_info_->dwStrucVersion);
        for (auto const& flag : system_info_utils::version_file_flags_to_strings(file_info_->dwFileFlags, file_info_->dwFileFlagsMask))
        {
            fileFlagsList_.Append(flag);
        }
        file_type_ = system_info_utils::version_file_type_to_string(file_info_->dwFileType, file_info_->dwFileSubtype);
        file_os_ = system_info_utils::version_file_os_to_string(file_info_->dwFileOS);
        if (file_info.dwFileDateMS != 0 && file_info_->dwFileDateLS != 0)
        {
            FILETIME ft;
            ft.dwHighDateTime = file_info_->dwFileDateMS;
            ft.dwLowDateTime = file_info_->dwFileDateLS;
            file_date_.as<DumpFileTimeStamp>()->Set(ft, dump_file_timezone_info);
        }
        file_version_ = system_info_utils::version_info_to_string(file_info_->dwFileVersionMS, file_info_->dwFileVersionLS);
        product_version_ = system_info_utils::version_info_to_string(file_info_->dwProductVersionMS, file_info_->dwProductVersionLS);
    }

}
