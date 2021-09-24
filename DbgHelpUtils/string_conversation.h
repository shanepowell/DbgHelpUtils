#pragma once
#include <string>
#include <tuple>

namespace dlg_help_utils::string_conversation
{
    // convert UTF-8 string to wstring
    std::wstring utf8_to_wstring(const char* str);
    std::wstring utf8_to_wstring(std::string_view const& str);

    // convert wstring to UTF-8 string
    std::string wstring_to_utf8(std::wstring_view const& str);

    // convert ACP string to wstring 
    std::wstring acp_to_wstring(std::string_view const& str);

    // convert code page string to wstring
    std::tuple<std::wstring, uint32_t> code_page_string_to_wstring(char const* str, size_t length, uint32_t code_page);
    std::tuple<std::string, uint32_t> to_code_page_string(wchar_t const* str, size_t length, uint32_t code_page);
}

inline std::wstring to_wstring(std::wstring_view const& str)
{
    return std::wstring{str.data(), str.length()};
}
