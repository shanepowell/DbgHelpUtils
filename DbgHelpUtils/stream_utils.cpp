#include "stream_utils.h"

#include <boost/algorithm/string.hpp>

#include "mini_dump.h"
#include "symbol_type_info.h"
#include "sym_tag_enum.h"


namespace dlg_help_utils::stream_utils
{
    std::optional<std::tuple<size_t, MINIDUMP_DIRECTORY const&>> find_stream_for_type(mini_dump const& dump_file, MINIDUMP_STREAM_TYPE const type)
    {
        auto const* header = dump_file.header();
        if (header == nullptr)
        {
            return std::nullopt;
        }

        auto const* directory = dump_file.directory();
        if (directory == nullptr)
        {
            return std::nullopt;
        }

        for (size_t index = 0; index < header->NumberOfStreams; ++index)
        {
            if (auto const & entry = directory[index]; static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType) == type)
            {
                return std::make_tuple(index, entry);
            }
        }

        return std::nullopt;
    }

    std::optional<DWORD64> find_field_pointer_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring const& field_name, DWORD64 const memory_address)
    {
        if(auto const type_tag_data = type.sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::UDT)
        {
            return std::nullopt;
        }

        for (auto const& child : type.children())
        {
            auto const name_data = child.name();
            if(!name_data.has_value())
            {
                continue;
            }

            if(!boost::iequals(field_name, name_data.value()))
            {
                continue;
            }

            auto const data_type = child.type(); 
            if(!data_type.has_value())
            {
                return std::nullopt;
            }

            if(auto const data_type_tag = data_type.value().sym_tag(); data_type_tag.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PointerType)
            {
                return std::nullopt;
            }

            auto const offset_data = child.offset();
            if(!offset_data.has_value())
            {
                return std::nullopt;
            }

            auto const data_type_length = data_type.value().length();
            if(!data_type_length.has_value())
            {
                return std::nullopt;
            }

            switch(data_type_length.value())
            {
            case 4:
                return read_field_value<uint32_t, DWORD64>(walker, memory_address + offset_data.value());

            case 8:
                return read_field_value<uint64_t, DWORD64>(walker, memory_address + offset_data.value());

            default:
                return std::nullopt;
            }
        }

        return std::nullopt;
    }
}

