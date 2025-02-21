﻿#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "time_utils.h"

namespace dlg_help_utils
{
    class mini_dump;

    class misc_info_stream
    {
    public:
        misc_info_stream() = default;
        explicit misc_info_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] size_t misc_info_version() const { return misc_info_version_; }
        [[nodiscard]] MINIDUMP_MISC_INFO const& misc_info() const { return *misc_info_; }
        [[nodiscard]] MINIDUMP_MISC_INFO_2 const& misc_info_2() const { return *misc_info_2_; }
        [[nodiscard]] MINIDUMP_MISC_INFO_3 const& misc_info_3() const { return *misc_info_3_; }
        [[nodiscard]] MINIDUMP_MISC_INFO_4 const& misc_info_4() const { return *misc_info_4_; }
        [[nodiscard]] MINIDUMP_MISC_INFO_5 const& misc_info_5() const { return *misc_info_5_; }


        [[nodiscard]] static time_utils::locale_timezone_info get_dump_file_timezone_info(mini_dump const& dump, size_t index = 0);

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        size_t misc_info_version_{};
        MINIDUMP_MISC_INFO const* misc_info_{nullptr};
        MINIDUMP_MISC_INFO_2 const* misc_info_2_{nullptr};
        MINIDUMP_MISC_INFO_3 const* misc_info_3_{nullptr};
        MINIDUMP_MISC_INFO_4 const* misc_info_4_{nullptr};
        MINIDUMP_MISC_INFO_5 const* misc_info_5_{nullptr};
    };
}
