#pragma once
#include <ostream>
#include <vector>

#include "symbol_address_info.h"

namespace dlg_help_utils
{
    class stream_thread_context;
    class unloaded_module_list_stream;
    class module_list_stream;

    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;

    void hex_dump_stack(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                        uint64_t stack_start_address, void const* stack, size_t stack_size,
                        stream_thread_context const& thread_context, size_t indent);
    void hex_dump_stack(std::wostream& os, mini_dump_stack_walk const& walker, std::vector<uint64_t> const& stack, bool is_x86_target, size_t indent);
    void hex_dump_stack_raw(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                            uint64_t stack_start_address, uint64_t const* stack, size_t stack_size, bool is_x86_target, size_t indent);
    void hex_dump_address(std::wostream& os, mini_dump const& mini_dump, module_list_stream const& module_list,
                          unloaded_module_list_stream const& unloaded_module_list,
                          dbg_help::symbol_engine& symbol_engine, uint64_t address, size_t indent);
    std::wstring hex_dump_stack_frame(dbg_help::symbol_address_info const& info, bool is_x86_address);

};
