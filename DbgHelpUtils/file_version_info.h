#pragma once
#include <optional>
#include <string>
#include <vector>

#include "windows_setup.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace dlg_help_utils
{
    class mini_dump_memory_stream;

    class file_version_info
    {
    public:
        file_version_info(std::wstring const& filename);
        explicit file_version_info(mini_dump_memory_stream stream);

        constexpr static uint32_t default_value = std::numeric_limits<uint32_t>::max();

        [[nodiscard]] bool has_version_info() const { return !file_version_info_.empty(); }

        [[nodiscard]] std::wstring product_name(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring product_version(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring file_version(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring file_description(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring company_name(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring legal_copyright(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring legal_trademarks(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring private_build(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring special_build(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring original_filename(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring comments(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::wstring internal_name(uint32_t language = default_value, uint32_t codepage = default_value) const;
        [[nodiscard]] std::optional<VS_FIXEDFILEINFO> file_info() const;

    private:
        [[nodiscard]] std::wstring load_resource_string(std::wstring_view resource, uint32_t language, uint32_t codepage) const;
        [[nodiscard]] std::wstring get_string_resource_string_for(std::wstring_view resource, uint32_t language, uint32_t codepage) const;

    private:
        std::vector<uint8_t> file_version_info_;
    };
}
