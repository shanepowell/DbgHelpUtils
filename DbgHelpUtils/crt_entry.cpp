#include "crt_entry.h"

#include "common_symbol_names.h"
#include "crt_heap.h"
#include "stream_utils.h"
#include "string_conversation.h"

namespace dlg_help_utils::heap
{
    std::wstring const& crt_entry::symbol_name = common_symbol_names::crt_mem_block_header_structure_symbol_name;

    crt_entry::crt_entry(crt_heap const& heap, uint64_t const entry_address)
    : heap_{heap}
    , entry_address_{entry_address}
    , crt_mem_block_header_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , crt_mem_block_header_length_{stream_utils::get_type_length(crt_mem_block_header_symbol_type_, symbol_name)}
    , file_name_{get_filename()}
    {
    }

    process::process_environment_block const& crt_entry::peb() const
    {
        return heap().peb();
    }

    stream_stack_dump::mini_dump_stack_walk const& crt_entry::walker() const
    {
        return heap().walker();
    }

    uint32_t crt_entry::line_number() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::crt_mem_block_header_line_number_field_symbol_name);
    }

    bool crt_entry::block_use() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::crt_mem_block_header_block_use_field_symbol_name) != 0;
    }

    size_units::base_10::bytes crt_entry::data_size() const
    {
        return size_units::base_10::bytes{stream_utils::get_machine_size_field_value(*this, common_symbol_names::crt_mem_block_header_data_size_field_symbol_name)};
    }

    uint32_t crt_entry::request_number() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::crt_mem_block_header_request_number_field_symbol_name);
    }

    uint64_t crt_entry::user_address() const
    {
        return entry_address() + crt_mem_block_header_length_;
    }

    std::wstring crt_entry::get_filename() const
    {
        auto const address = stream_utils::get_field_pointer_raw(*this, common_symbol_names::crt_mem_block_header_file_name_field_symbol_name);
        if(address == 0)
        {
            return {};
        }

        auto stream = walker().get_process_memory_stream(address, std::numeric_limits<uint32_t>::max());
        std::string file_name;
        while(!stream.eof())
        {
            char ch;
            if(stream.read(&ch, sizeof ch) != sizeof ch)
            {
                break;
            }

            if(ch == 0)
            {
                break;
            }

            file_name += ch;
        }

        return string_conversation::acp_to_wstring(file_name);
    }
}
