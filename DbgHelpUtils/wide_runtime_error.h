#pragma once
#include <format>
#include <stdexcept>
#include "string_conversation.h"

namespace dlg_help_utils::exceptions
{
    class wide_runtime_error : public std::runtime_error
    {
    public:
        wide_runtime_error(std::wstring message)
            : runtime_error(string_conversation::wstring_to_utf8(message))
              , message_{std::move(message)}
        {
        }

        wide_runtime_error(std::wstring const& message, std::exception const& e)
            : runtime_error(std::format("{0} : Inner Exception: {1}", string_conversation::wstring_to_utf8(message), e.what()))
              , message_{std::format(L"{0} : Inner Exception: {1}", message, string_conversation::utf8_to_wstring(e.what()))}
        {
        }

        wide_runtime_error(std::wstring const& message, wide_runtime_error const& e)
            : runtime_error(std::format("{0} : Inner Exception: {1}", string_conversation::wstring_to_utf8(message), e.what()))
              , message_{std::format(L"{0} : Inner Exception: {1}", message, e.message())}
        {
        }

        [[nodiscard]] std::wstring const& message() const { return message_; }

    private:
        std::wstring message_;
    };
}
