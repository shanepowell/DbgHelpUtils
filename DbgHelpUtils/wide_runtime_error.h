#pragma once
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

        [[nodiscard]] std::wstring const& message() const { return message_; }

    private:
        std::wstring message_;
    };
}
