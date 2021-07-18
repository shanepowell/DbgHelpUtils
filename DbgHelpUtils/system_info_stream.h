#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <string_view>

namespace dlg_help_utils
{
    class mini_dump;

    class system_info_stream
    {
    public:
        explicit system_info_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_SYSTEM_INFO const& system_info() const { return *system_info_; }
        [[nodiscard]] std::wstring_view const& csd_version() const { return csd_version_; }
        [[nodiscard]] std::wstring const& vendor_id() const { return vendor_id_; }
        [[nodiscard]] bool is_intel() const { return is_intel_; }
        [[nodiscard]] bool is_amd() const { return is_amd_; }
        [[nodiscard]] bool is_x86() const;
        [[nodiscard]] bool is_x64() const;
        [[nodiscard]] unsigned short processor_model() const { return processor_model_; }
        [[nodiscard]] unsigned short processor_stepping() const { return processor_stepping_; }

    private:
        bool found_{false};
        size_t index_{};
        MINIDUMP_SYSTEM_INFO const* system_info_{nullptr};
        std::wstring_view csd_version_;
        std::wstring vendor_id_;
        bool is_intel_{};
        bool is_amd_{};
        unsigned short processor_model_{};
        unsigned short processor_stepping_{};
    };
}
