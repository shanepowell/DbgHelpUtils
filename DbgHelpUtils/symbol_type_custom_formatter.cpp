#include "symbol_type_custom_formatter.h"

#include "symbol_data_dumper.h"

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

    uint64_t symbol_type_custom_formatter::get_address(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , i_value_type_formatter const& formatter)
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
                    break;

                case dbg_help::sym_tag_enum::PointerType:
                    if(auto const length = type.length(); length.has_value())
                    {
                        switch(length.value())
                        {
                        case 4:
                            if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                            {
                                return static_cast<uint64_t>(value);
                            }
                            break;

                        case 8:
                            if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                            {
                                symbol_data_dumper::fix_wow64_pointer(value, walker, formatter);
                                return value;
                            }
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }

        return variable_address;
    }
}
