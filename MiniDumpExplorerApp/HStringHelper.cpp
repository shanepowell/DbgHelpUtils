#include "pch.h"
#include "HStringHelper.h"

#include "../DbgHelpUtils/stream_hex_dump.h"

winrt::hstring to_hex_string(M128A const& value)
{
    std::wstringstream ss;
    ss << dlg_help_utils::stream_hex_dump::to_hex_full(value);
    return winrt::hstring{std::move(ss).str()};
}

winrt::hstring to_hex_string(uint64_t const& value)
{
    std::wstringstream ss;
    ss << dlg_help_utils::stream_hex_dump::to_hex_full(value);
    return winrt::hstring{std::move(ss).str()};
}
