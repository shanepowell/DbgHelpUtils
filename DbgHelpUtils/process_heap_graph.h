#pragma once
#include <cvconst.h>
#include <map>
#include <variant>
#include <vector>

#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"
#include "process_heap_graph_thread_context_entry.h"
#include "process_heap_graph_thread_stack_entry.h"
#include "stream_thread_context.h"
#include "vector_to_hash_set.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::heap
{
    class system_module_list;
    class process_heaps;
    using process_heap_graph_entry_type = std::variant<process_heap_graph_global_variable_entry, process_heap_graph_thread_stack_entry, process_heap_graph_thread_context_entry, process_heap_graph_heap_entry>;

    [[nodiscard]] inline process_heap_graph_node const& get_graph_node(process_heap_graph_entry_type const& node)
    {
        return std::visit([](auto const& _) -> process_heap_graph_node const& { return _; }, node);
    }

    [[nodiscard]] inline process_heap_graph_node& get_graph_node(process_heap_graph_entry_type& node)
    {
        return std::visit([](auto& _) -> process_heap_graph_node& { return _; }, node);
    }

    class process_heap_graph
    {
    public:
        process_heap_graph(mini_dump const& mini_dump, process_heaps const& process, system_module_list const& system_module_list);

        void generate_graph();

        [[nodiscard]] std::vector<process_heap_graph_entry_type> const& nodes() const { return nodes_; }

    private:
        [[nodiscard]] std::map<uint64_t, size_t> generate_allocation_references();
        [[nodiscard]] std::optional<process_heap_graph_heap_entry> find_allocation_node_allocation(std::map<uint64_t, size_t> const& heap_entries, range const& data_range) const;
        void generate_global_variable_references(std::map<uint64_t, size_t> const& heap_entries);
        void generate_thread_context_references();
        template<typename T>
        void generate_specific_thread_context_references(T const& thread);
        void generate_x64_thread_context_references(stream_thread_context::context_x64 const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_wow64_thread_context_references(WOW64_CONTEXT const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_x86_thread_context_references(stream_thread_context::context_x86 const& thread_context, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_thread_context_reference(CV_HREG_e register_type, uint64_t register_data, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_thread_stack_references(mini_dump_memory_stream stack_stream, uint32_t thread_id, std::wstring_view const& thread_name);
        void generate_node_references(std::map<uint64_t, size_t> const& heap_entries);
        void remove_all_non_allocation_with_empty_to_references();
        void remove_all_system_module_global_variables_and_parents();
        [[nodiscard]] bool is_node_or_children_system_module_global_variable(process_heap_graph_entry_type const& node, std::unordered_map<uint64_t, bool>& result_cache) const;
        [[nodiscard]] process_heap_graph_entry_type const& get_node_from_index(uint64_t node_index) const;

    private:
        mini_dump const* mini_dump_;
        process_heaps const* process_;
        std::vector<process_heap_graph_entry_type> nodes_{};
        std::set<uint64_t> system_module_bases_;
    };
}
