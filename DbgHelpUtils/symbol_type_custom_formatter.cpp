#include "symbol_type_custom_formatter.h"

namespace dlg_help_utils::symbol_type_utils
{
    bool symbol_type_custom_formatter::is_type_equal(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , std::wstring_view const& path
        , std::wstring_view const& name
        , std::vector<dbg_help::symbol_type_info> const& parents
        , dbg_help::symbol_type_info const& equal_to_type)
    {
        if (auto data_type = type.type();
            data_type.has_value())
        {
            if (auto tag = type.sym_tag();
                tag.has_value())
            {
                switch (tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                {
                case dbg_help::sym_tag_enum::Data:
                    return data_type.value() == equal_to_type;

                case dbg_help::sym_tag_enum::PointerType:
                    return is_custom_type(walker, data_type.value(), path, name, parents);

                default:
                    break;
                }
            }
        }

        return false;
    }
}
