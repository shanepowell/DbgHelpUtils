#pragma once
#include <ostream>

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
    void hex_dump_stack(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                        uint64_t stack_start_address, void const* stack, size_t stack_size,
                        stream_thread_context const& thread_context, size_t indent);
    void hex_dump_stack_raw(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                            uint64_t stack_start_address, uint64_t const* stack, size_t stack_size, size_t indent);
    void hex_dump_address(std::wostream& os, mini_dump const& mini_dump, module_list_stream const& module_list,
                          unloaded_module_list_stream const& unloaded_module_list,
                          dbg_help::symbol_engine& symbol_engine, uint64_t address, size_t indent);
};
