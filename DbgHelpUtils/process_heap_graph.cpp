﻿#include "process_heap_graph.h"

#include <algorithm>
#include <ranges>

#include "global_variable.h"
#include "global_variables.h"
#include "process_heaps.h"
#include "process_heaps_options.h"
#include "process_heap_entry.h"
#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"
#include "system_module_list.h"
#include "thread_ex_list_stream.h"
#include "thread_list_stream.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    namespace
    {
        using namespace allocation_graph;

        template<typename PointerType>
        class process_heap_graph_generator
        {
        public:
            process_heap_graph_generator(std::map<uint64_t, size_t> const& heap_entries, std::vector<process_heap_graph_entry_type> & nodes)
                : heap_entries_{heap_entries}
                , nodes_{nodes}
            {
            }

            void generate_references();
            void operator()(process_heap_graph_heap_entry& entry);
            void operator()(process_heap_graph_global_variable_entry& entry);
            void operator()(process_heap_graph_thread_stack_entry& entry);
            void operator()(process_heap_graph_thread_context_entry& entry);

        private:
            template<typename NodeType>
            void process_entry_reference(NodeType& entry, PointerType data, size_t offset, size_t& jump_amount);

        private:
            std::map<uint64_t, size_t> const& heap_entries_;
            std::vector<process_heap_graph_entry_type>& nodes_;
        };

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::generate_references()
        {
            for(auto& entry : nodes_)
            {
                std::visit(*this, entry);
            }
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_heap_entry& entry)
        {
            auto user_data = entry.heap_entry().all_user_data();
            [[maybe_unused]] auto const found = user_data.find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_global_variable_entry& entry)
        {
            [[maybe_unused]] auto const found = entry.variable().stream().find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_thread_stack_entry& entry)
        {
            [[maybe_unused]] auto const found = entry.stack_stream().find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_thread_context_entry& entry)
        {
            size_t jump_amount{};
            process_entry_reference(entry, static_cast<PointerType>(entry.register_data()), 0, jump_amount);
        }

        template <typename PointerType>
        template <typename NodeType>
        void process_heap_graph_generator<PointerType>::process_entry_reference(NodeType& entry, PointerType data, size_t offset, size_t& jump_amount)
        {
            auto it = heap_entries_.lower_bound(data);
            if(it == heap_entries_.end())
            {
                return;
            }

            auto& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
            if(node.index() == entry.index() || !node.heap_entry().contains_address_range(data, size_units::base_16::bytes{1}))
            {
                return;
            }

            entry.add_to_reference(process_heap_entry_reference{offset, data, node});
            node.add_from_reference(process_heap_entry_reference{offset, data, entry});
            jump_amount += sizeof(PointerType);
        }

        template<typename PointerType>
        void generate_graph_nodes(std::map<uint64_t, size_t> const& heap_entries, std::vector<process_heap_graph_entry_type> & nodes)
        {
            process_heap_graph_generator<PointerType> generator{heap_entries, nodes};
            generator.generate_references();
        }
    }

    process_heap_graph::process_heap_graph(mini_dump const& mini_dump, process_heaps const& process, system_module_list const& system_module_list)
        : mini_dump_{&mini_dump}
        , process_{&process}
        , system_module_bases_{system_module_list.generate_system_module_bases(process.peb().module_list(), process.peb().unloaded_module_list())}
    {
    }


    void process_heap_graph::generate_global_variable_references(std::map<uint64_t, size_t> const& heap_entries)
    {
        for(process::global_variables const variables{process_->peb().walker()};
            auto const& global_variable : variables.all_variables())
        {
            nodes_.emplace_back(process_heap_graph_global_variable_entry{global_variable, find_allocation_node_allocation(heap_entries, global_variable.variable_memory_range())});
        }
    }

    void process_heap_graph::generate_thread_context_references()
    {
        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadListStream); stream.has_value())
        {
            if (thread_list_stream const thread_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread const thread{ *mini_dump_, thread_list.thread_list().Threads[0], process_->peb().names_list(), process_->peb().memory_list(), process_->peb().memory64_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(thread);
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadExListStream); stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread_ex const thread{ *mini_dump_, thread_ex_list.thread_list().Threads[0], process_->peb().names_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(thread);
                }
            }
        }
    }

    void process_heap_graph::generate_x64_thread_context_references(stream_thread_context::context_x64 const& thread_context, uint32_t const thread_id, std::wstring_view const& thread_name)
    {
        generate_thread_context_reference(CV_AMD64_RAX, thread_context.Rax, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RBX, thread_context.Rbx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RCX, thread_context.Rcx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RDX, thread_context.Rdx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RSI, thread_context.Rsi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RDI, thread_context.Rdi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R8, thread_context.R8, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R9, thread_context.R9, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R10, thread_context.R10, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R11, thread_context.R11, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R12, thread_context.R12, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R13, thread_context.R13, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R14, thread_context.R14, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_R15, thread_context.R15, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RBP, thread_context.Rbp, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_RSP, thread_context.Rsp, thread_id, thread_name);
    }

    void process_heap_graph::generate_wow64_thread_context_references(WOW64_CONTEXT const& thread_context, uint32_t const thread_id, std::wstring_view const& thread_name)
    {
        generate_thread_context_reference(CV_AMD64_EAX, thread_context.Eax, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EBX, thread_context.Ebx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ECX, thread_context.Ecx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EDX, thread_context.Edx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ESI, thread_context.Esi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EDI, thread_context.Edi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EBP, thread_context.Ebp, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ESP, thread_context.Esp, thread_id, thread_name);
    }

    void process_heap_graph::generate_x86_thread_context_references(stream_thread_context::context_x86 const& thread_context, uint32_t const thread_id, std::wstring_view const& thread_name)
    {
        generate_thread_context_reference(CV_AMD64_EAX, thread_context.Eax, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EBX, thread_context.Ebx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ECX, thread_context.Ecx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EDX, thread_context.Edx, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ESI, thread_context.Esi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EDI, thread_context.Edi, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_EBP, thread_context.Ebp, thread_id, thread_name);
        generate_thread_context_reference(CV_AMD64_ESP, thread_context.Esp, thread_id, thread_name);
    }

    template <typename T>
    void process_heap_graph::generate_specific_thread_context_references(T const& thread)
    {
        if (thread.thread_context().x64_thread_context_available())
        {
            generate_x64_thread_context_references(thread.thread_context().x64_thread_context(), thread->ThreadId, thread.thread_name());
        }
        else if (thread.thread_context().wow64_thread_context_available())
        {
            generate_wow64_thread_context_references(thread.thread_context().wow64_thread_context(), thread->ThreadId, thread.thread_name());
        }
        else if (thread.thread_context().x86_thread_context_available())
        {
            generate_x86_thread_context_references(thread.thread_context().x86_thread_context(), thread->ThreadId, thread.thread_name());
        }

        if (thread.stack() != nullptr)
        {
            generate_thread_stack_references( mini_dump_memory_stream{thread.stack(), thread->Stack.Memory.DataSize}, thread->ThreadId, thread.thread_name());
        }
    }

    std::map<uint64_t, size_t> process_heap_graph::generate_allocation_references()
    {
        std::map<uint64_t, size_t> heap_entries;
        for (auto const& entry : process_->entries())
        {
            nodes_.emplace_back(process_heap_graph_heap_entry{entry, process_->is_system_allocation(entry.user_memory_range()) ? process_heap_graph_heap_entry_type::system_allocation : process_heap_graph_heap_entry_type::allocation});
            heap_entries.insert(std::make_pair(entry.user_address() + entry.user_requested_size().count() - 1, nodes_.size() - 1));
        }

        return heap_entries;
    }

    std::optional<process_heap_graph_heap_entry> process_heap_graph::find_allocation_node_allocation(std::map<uint64_t, size_t> const& heap_entries, memory_range const& data_range) const
    {
        if(data_range.start_range == data_range.end_range)
        {
            return std::nullopt;
        }

        auto const it = heap_entries.find(data_range.start_range);
        if(it == heap_entries.end())
        {
            return std::nullopt;
        }

        auto const& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
        if(!node.heap_entry().contains_address_range(data_range.start_range, size(data_range)))
        {
            return std::nullopt;
        }

        return node;
    }

    void process_heap_graph::generate_thread_context_reference(CV_HREG_e const register_type, uint64_t const register_data, uint32_t const thread_id, std::wstring_view const& thread_name)
    {
        if(register_data == 0) return;
        nodes_.emplace_back(process_heap_graph_thread_context_entry{register_type, register_data, thread_id, std::wstring{thread_name}});
    }

    void process_heap_graph::generate_thread_stack_references(mini_dump_memory_stream stack_stream, uint32_t const thread_id, std::wstring_view const& thread_name)
    {
        nodes_.emplace_back(process_heap_graph_thread_stack_entry{std::move(stack_stream), thread_id, std::wstring{thread_name}});
    }

    void process_heap_graph::generate_node_references(std::map<uint64_t, size_t> const& heap_entries)
    {
        if(process_->peb().is_x86_target())
        {
            generate_graph_nodes<uint32_t>(heap_entries, nodes_);
        }
        else
        {
            generate_graph_nodes<uint64_t>(heap_entries, nodes_);
        }
    }

    void process_heap_graph::remove_all_non_allocation_with_empty_to_references()
    {
        auto const [first, last] = std::ranges::remove_if(nodes_, [](auto const& node)
        {
            auto const& graph_node = get_graph_node(node);
            return graph_node.is_non_allocation_root_node() && graph_node.to_references().empty();
        });
        nodes_.erase(first, last);
    }

    void process_heap_graph::mark_all_system_module_global_variables_and_parents(std::unordered_map<uint64_t, bool>& result_cache)
    {
        for(auto& node : nodes_)
        {
            if(is_node_or_children_system_module_global_variable(node, result_cache))
            {
                auto& graph_node = get_graph_node(node);
                graph_node.is_system() = true;
                if(!graph_node.is_root_node())
                {
                    graph_node.mark_as_system_allocation();
                }
            }
        }
    }

    void process_heap_graph::remove_all_system_nodes(std::unordered_map<uint64_t, bool> const& result_cache)
    {
        auto const [first, last] = std::ranges::remove_if(nodes_, [this](auto const& node)
        {
            auto const& graph_node = get_graph_node(node);
            return graph_node.is_system_allocation() || graph_node.is_system();
        });

        nodes_.erase(first, last);

        for (auto const removed_node_index : result_cache | std::views::filter([](auto const& kv) { return kv.second; }) | std::views::keys)
        {
            for(auto& node : nodes_)
            {
                auto& graph_node = get_graph_node(node);
                graph_node.remove_references(removed_node_index);
            }
        }
    }

    bool process_heap_graph::is_node_or_children_system_module_global_variable(process_heap_graph_entry_type const& node, std::unordered_map<uint64_t, bool>& result_cache) const
    {
        auto const& graph_node = get_graph_node(node);
        if(auto const cache_result = result_cache.find(graph_node.index()); cache_result != result_cache.end())
        {
            return cache_result->second;
        }

        if(std::holds_alternative<process_heap_graph_global_variable_entry>(node))
        {
            if(auto const& global_variable_entry = std::get<process_heap_graph_global_variable_entry>(node);
                system_module_bases_.contains(global_variable_entry.variable().symbol_type().module_base()))
            {
                result_cache.insert(std::make_pair(graph_node.index(), true));
                return true;
            }
        }

        result_cache.insert(std::make_pair(graph_node.index(), false));
        if(std::ranges::any_of(graph_node.to_references(), [this, &result_cache](auto const& child_node) { return is_node_or_children_system_module_global_variable(get_node_from_index(child_node.node_index()), result_cache); }))
        {
            result_cache[graph_node.index()] = true;
            return true;
        }
        return false;
    }

    process_heap_graph_entry_type const& process_heap_graph::get_node_from_index(uint64_t const node_index) const
    {
        auto const it = std::ranges::find_if(nodes_, [node_index](auto const& node) { return get_graph_node(node).index() == node_index; });
        if(it == nodes_.end())
        {
            throw exceptions::wide_runtime_error{std::format(L"Graph Allocation Nodes does not contain node index [{}]", node_index)};
        }

        return *it;
    }

    void process_heap_graph::generate_graph()
    {
        // generate all reference nodes
        auto const heap_entries = generate_allocation_references();
        generate_global_variable_references(heap_entries);
        generate_thread_context_references();

        // link all reference nodes
        generate_node_references(heap_entries);

        // remove any reference nodes that is not a allocation and has no to references
        remove_all_non_allocation_with_empty_to_references();
        std::unordered_map<uint64_t, bool> result_cache;
        mark_all_system_module_global_variables_and_parents(result_cache);
        if(!process_->options().no_filter_heap_entries())
        {
            remove_all_system_nodes(result_cache);
        }
    }
}
