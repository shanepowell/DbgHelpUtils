#pragma once
#include "DbgHelpUtils/value_type_formatter.h"


class value_type_formatter : public dlg_help_utils::symbol_type_utils::i_value_type_formatter
{
public:
    std::wstring format_value(char value) const override;
    std::wstring format_value(wchar_t value) const override;

    std::wstring format_value(float value) const override;
    std::wstring format_value(double value) const override;

    std::wstring format_value(std::string_view value) const override;
    std::wstring format_value(std::wstring_view value) const override;

    std::wstring format_value(int8_t value) const override;
    std::wstring format_value(int16_t value) const override;
    std::wstring format_value(int32_t value) const override;
    std::wstring format_value(int64_t value) const override;

    std::wstring format_value(uint8_t value) const override;
    std::wstring format_value(uint16_t value) const override;
    std::wstring format_value(uint32_t value) const override;
    std::wstring format_value(uint64_t value) const override;
    std::wstring format_value(std::basic_string_view<char8_t> value) const override;
    std::wstring format_value(std::basic_string_view<char16_t> value) const override;
    std::wstring format_value(std::basic_string_view<char32_t> value) const override;
};
