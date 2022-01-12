#pragma once
#include <string>

namespace dlg_help_utils
{
    class stream_module_name
    {
    public:
        explicit stream_module_name(std::wstring_view const& name);

        [[nodiscard]] std::wstring_view const& name() const { return name_; }
        [[nodiscard]] std::wstring const& filename_with_extension() const { return filename_with_extension_; }
        [[nodiscard]] std::wstring const& filename_without_extension() const { return filename_without_extension_; }

    private:
        std::wstring_view name_;
        std::wstring filename_with_extension_;
        std::wstring filename_without_extension_;
    };
}
