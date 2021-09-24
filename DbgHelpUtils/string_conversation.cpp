#include "string_conversation.h"

#include "windows_setup.h"

namespace dlg_help_utils::string_conversation
{
    std::wstring utf8_to_wstring(const char* str)
    {
        [[maybe_unused]] auto [result, status] = code_page_string_to_wstring(str, strlen(str), CP_UTF8);
        return result;
    }

    // convert UTF-8 string to wstring
    std::wstring utf8_to_wstring(std::string_view const& str)
    {
        [[maybe_unused]] auto [result, status] = code_page_string_to_wstring(str.data(), str.length(), CP_UTF8);
        return result;
    }

    // convert wstring to UTF-8 string
    std::string wstring_to_utf8(std::wstring_view const& str)
    {
        [[maybe_unused]] auto [result, status] = to_code_page_string(str.data(), str.length(), CP_UTF8);
        return result;
    }

    std::wstring acp_to_wstring(std::string_view const& str)
    {
        [[maybe_unused]] auto [result, status] = code_page_string_to_wstring(str.data(), str.length(), CP_ACP);
        return result;
    }

    std::tuple<std::wstring, uint32_t> code_page_string_to_wstring(char const* str, const size_t length,
                                                                   const uint32_t code_page)
    {
        if (length == 0)
        {
            return make_tuple(std::wstring(), ERROR_SUCCESS);
        }

        const auto ret = MultiByteToWideChar(code_page, 0, str, static_cast<int>(length), nullptr, 0);
        if (ret == 0)
        {
            //something went wrong - GetLastError();
            return make_tuple(std::wstring(), GetLastError());
        }

        std::wstring rv(static_cast<size_t>(ret), static_cast<wchar_t>(0x0));
        if (static_cast<size_t>(MultiByteToWideChar(code_page, 0, str, static_cast<int>(length), &*rv.begin(),
                                                    static_cast<int>(rv.length()))) != rv.length())
        {
            //something went wrong - GetLastError();
            return make_tuple(std::wstring(), GetLastError());
        }
        return make_tuple(rv, ERROR_SUCCESS);
    }

    std::tuple<std::string, uint32_t> to_code_page_string(wchar_t const* str, const size_t length,
                                                          const uint32_t code_page)
    {
        if (length == 0)
        {
            return make_tuple(std::string(), ERROR_SUCCESS);
        }

        const auto ret = WideCharToMultiByte(code_page, 0, str, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
        if (ret == 0)
        {
            return make_tuple(std::string(), GetLastError());
        }

        std::string rv(static_cast<size_t>(ret), static_cast<char>(0x0));
        if (static_cast<size_t>(WideCharToMultiByte(code_page, 0, str, static_cast<int>(length), &*rv.begin(),
                                                    static_cast<int>(rv.length()), nullptr, nullptr)) != rv.length())
        {
            return make_tuple(std::string(), GetLastError());
        }

        return make_tuple(rv, ERROR_SUCCESS);
    }
}
