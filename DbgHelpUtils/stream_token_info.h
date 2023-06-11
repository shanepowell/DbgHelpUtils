#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class stream_token_info
    {
    public:
        stream_token_info(MINIDUMP_TOKEN_INFO_HEADER const& header);

        MINIDUMP_TOKEN_INFO_HEADER const* operator->() const { return &header_; }
        [[nodiscard]] void const* data() const { return data_; }
        [[nodiscard]] size_t size() const { return header_.TokenSize - sizeof(MINIDUMP_TOKEN_INFO_HEADER); }

    private:
        MINIDUMP_TOKEN_INFO_HEADER const& header_;
        void const* data_{nullptr};
    };
}
