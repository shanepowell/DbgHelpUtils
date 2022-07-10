#pragma once
#include <cvconst.h>
#include <map>
#include <set>
#include <variant>
#include <vector>

#include "memory_range.h"
#include "overload.h"
#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"
#include "process_heap_graph_symbol_entry.h"
#include "process_heap_graph_thread_context_entry.h"
#include "process_heap_graph_thread_stack_entry.h"
#include "stream_thread_context.h"
#include "symbol_type_utils.h"
#include "tagged_bool.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::heap
{
    class system_module_list;
    class process_heaps;

    namespace allocation_graph
    {
        using process_heap_graph_entry_type = std::variant<process_heap_graph_global_variable_entry, process_heap_graph_thread_stack_entry, process_heap_graph_thread_context_entry, process_heap_graph_heap_entry, process_heap_graph_symbol_entry>;

        [[nodiscard]] inline process_heap_graph_node const& get_graph_node(process_heap_graph_entry_type const& node)
        {
            return std::visit([](auto const& _) -> process_heap_graph_node const& { return _; }, node);
        }

        [[nodiscard]] inline process_heap_graph_node& get_graph_node(process_heap_graph_entry_type& node)
        {
            return std::visit([](auto& _) -> process_heap_graph_node& { return _; }, node);

        }

        [[nodiscard]] inline bool is_root_node(process_heap_graph_entry_type const& node)
        {
            return get_graph_node(node).is_root_node();
        }

        [[nodiscard]] inline uint64_t node_index(process_heap_graph_entry_type const& node)
        {
            return get_graph_node(node).index();
        }

        [[nodiscard]] inline uint64_t node_start_address(process_heap_graph_entry_type const& node)
        {
            auto start_address_of_node = overload {
                [](process_heap_graph_heap_entry const& graph_node) { return graph_node.heap_entry().user_address(); },
                [](process_heap_graph_global_variable_entry const& graph_node) { return graph_node.variable().symbol_type().address().value_or(0); },
                [](process_heap_graph_thread_stack_entry const& graph_node) { return graph_node.stack_stream().current_address(); },
                [](process_heap_graph_thread_context_entry const& graph_node) { return static_cast<uint64_t>(graph_node.register_type()); },
                [](process_heap_graph_symbol_entry const& graph_node) { return graph_node.address(); },
            };

            return std::visit(start_address_of_node, node);
        }
    }

    class process_heap_graph
    {
    public:
        process_heap_graph(mini_dump const& mini_dump, process_heaps const& process, system_module_list const& system_module_list);

        void generate_graph();

        [[nodiscard]] std::vector<allocation_graph::process_heap_graph_entry_type> const& nodes() const { return nodes_; }

    private:
        using mark_as_system_t = tagged_bool<struct mark_as_system_type>;

        [[nodiscard]] std::map<uint64_t, size_t> generate_allocation_references();
        [[nodiscard]] std::optional<allocation_graph::process_heap_graph_heap_entry> find_allocation_node_allocation(std::map<uint64_t, size_t> const& heap_entries, memory_range const& data_range) const;
        void generate_global_variable_references(std::map<uint64_t, size_t> const& heap_entries);
        void generate_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t pointer_size);
        template<typename T>
        void generate_specific_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t pointer_size, T const& thread);
        void generate_x64_thread_context_references(stream_thread_context::context_x64 const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_wow64_thread_context_references(WOW64_CONTEXT const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_x86_thread_context_references(stream_thread_context::context_x86 const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_thread_context_reference(CV_HREG_e register_type, uint64_t register_data, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_thread_stack_references(mini_dump_memory_stream stack_stream, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_node_references(std::map<uint64_t, size_t> const& heap_entries);
        void generate_pointer_symbol_references(std::map<uint64_t, size_t> const& heap_entries, std::map<uint64_t, symbol_type_utils::pointer_info> const& pointers, size_t pointer_size, mark_as_system_t mark_as_system);
        void generate_pointer_symbol_reference(std::map<uint64_t, size_t> const& heap_entries, symbol_type_utils::pointer_info const& pointer, size_t pointer_size, mark_as_system_t mark_as_system);
        [[nodiscard]] uint64_t add_symbol_reference(std::map<uint64_t, size_t> const& heap_entries, uint64_t address, dbg_help::symbol_type_info symbol_type, uint32_t thread_id, std::wstring_view const& thread_name, mark_as_system_t mark_as_system);
        allocation_graph::process_heap_graph_symbol_entry& add_node_symbol_reference(uint64_t address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> thread_id, std::wstring_view const& thread_name, mark_as_system_t mark_as_system);
        allocation_graph::process_heap_graph_symbol_entry& find_or_add_node_symbol_reference(uint64_t address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> thread_id, std::wstring_view const& thread_name, mark_as_system_t mark_as_system);
        void add_heap_node_symbol_reference(allocation_graph::process_heap_graph_node& node, uint64_t address, dbg_help::symbol_type_info symbol_type, uint32_t thread_id, std::wstring_view const& thread_name) const;
        allocation_graph::process_heap_entry_symbol_address_reference& add_heap_node_symbol_reference(allocation_graph::process_heap_graph_node& node, uint64_t address, dbg_help::symbol_type_info symbol_type) const;
        void find_or_add_node_symbol_variable_reference(allocation_graph::process_heap_graph_node& node, uint64_t base_address, uint64_t base_address_offset, size_t pointer_size, dbg_help::symbol_type_info const& base_symbol_type, dbg_help::symbol_type_info const& pointer_symbol_type, std::wstring const& pointer_name) const;
        allocation_graph::process_heap_entry_symbol_address_reference& get_node_symbol_variable_reference(allocation_graph::process_heap_graph_node& node, uint64_t address, dbg_help::symbol_type_info const& symbol_type) const;
        void remove_all_non_allocation_with_empty_to_references();
        void mark_all_system_module_global_variables_and_references(std::unordered_map<uint64_t, bool>& result_cache);
        void remove_all_system_nodes(std::unordered_map<uint64_t, bool> const& result_cache);
        bool is_node_or_children_system_module_global_variable(allocation_graph::process_heap_graph_entry_type const& node, std::unordered_map<uint64_t, bool>& result_cache) const;
        void mark_all_children_as_system(allocation_graph::process_heap_graph_entry_type const& node, std::unordered_map<uint64_t, bool>& result_cache) const;
        [[nodiscard]] allocation_graph::process_heap_graph_entry_type const& get_node_from_index(uint64_t node_index) const;
        [[nodiscard]] allocation_graph::process_heap_graph_entry_type& get_node_from_index(uint64_t node_index);
        size_t machine_pointer_size() const;

    private:
        mini_dump const* mini_dump_;
        process_heaps const* process_;
        std::vector<allocation_graph::process_heap_graph_entry_type> nodes_{};
        std::set<uint64_t> system_module_bases_;
    };
}
