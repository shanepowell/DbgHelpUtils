#pragma once

#include "DumpFileTimeStamp.g.h"
#include "DbgHelpUtils/time_utils.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpFileTimeStamp : DumpFileTimeStampT<DumpFileTimeStamp>
    {
        DumpFileTimeStamp();

        void Set(FILETIME const& ft, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info);
        void Set(SYSTEMTIME const& st, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info);
        void Set(time_t const& timeStamp, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info);

        [[nodiscard]] bool HasSystemTime() const { return systemTime_.has_value(); }
        [[nodiscard]] SYSTEMTIME const& SystemTime() const { return systemTime_.value(); }

        [[nodiscard]] bool HasFileTime() const { return fileTime_.has_value(); }
        [[nodiscard]] FILETIME const& FileTime() const { return fileTime_.value(); }

        [[nodiscard]] bool HasTimeStamp() const { return timeStamp_ != 0; }
        [[nodiscard]] time_t const& TimeStamp() const { return timeStamp_; }

        [[nodiscard]] dlg_help_utils::time_utils::locale_timezone_info const& DumpFileTimezoneInfo() const { return dump_file_timezone_info_; }

    private:
        dlg_help_utils::time_utils::locale_timezone_info dump_file_timezone_info_{};
        std::optional<FILETIME> fileTime_{};
        std::optional<SYSTEMTIME> systemTime_{};
        time_t timeStamp_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpFileTimeStamp : DumpFileTimeStampT<DumpFileTimeStamp, implementation::DumpFileTimeStamp>
    {
    };
}
