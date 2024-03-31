#include "size_units.h"

using namespace std::string_literals;

namespace dlg_help_utils::size_units::resources
{
    namespace
    {
        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> populate_default_strings()
        {
            std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> rv;
            rv[size_unit_type::exabytes] = make_tuple(L"exabyte"s, L"exabytes"s, L"eb"s);
            rv[size_unit_type::petabytes] = make_tuple(L"petabyte"s, L"petabytes"s, L"pb"s);
            rv[size_unit_type::terabytes] = make_tuple(L"terabyte"s, L"terabytes"s, L"tb"s);
            rv[size_unit_type::gigabytes] = make_tuple(L"gigabyte"s, L"gigabytes"s, L"gb"s);
            rv[size_unit_type::megabytes] = make_tuple(L"megabyte"s, L"megabytes"s, L"mb"s);
            rv[size_unit_type::kilobytes] = make_tuple(L"kilobyte"s, L"kilobytes"s, L"kb"s);
            rv[size_unit_type::bytes] = make_tuple(L"byte"s, L"bytes"s, L"b"s);
            return rv;
        }

        std::unordered_map const g_default_string_data{populate_default_strings()};
    }

    std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const& get_type_strings()
    {
        return g_default_string_data;
    }
}