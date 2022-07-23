#include "process_heap_graph.h"

#include <algorithm>
#include <ranges>

#include "global_variable.h"
#include "global_variables.h"
#include "process_heaps.h"
#include "process_heaps_options.h"
#include "process_heap_entry.h"
#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"
#include "symbol_type_utils.h"
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
            void operator()(process_heap_graph_symbol_entry& entry);

        private:
            template<typename NodeType>
            void process_entry_reference(NodeType& entry, PointerType data, uint64_t address, size_t offset, size_t& jump_amount);

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
            auto start_address = user_data.current_address();
            std::ignore = user_data.find_pattern<PointerType>([this, &entry, start_address](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, start_address + offset, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_global_variable_entry& entry)
        {
            std::ignore = entry.variable().stream().find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, entry.variable().variable_memory_range().start_range + offset, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_thread_stack_entry& entry)
        {
            std::ignore = entry.stack_stream().find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable
                {
                    process_entry_reference(entry, data, offset, offset, jump_amount);
                    return true;
                }
                , [](size_t) { return false; });
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_thread_context_entry& entry)
        {
            size_t jump_amount{};
            process_entry_reference(entry, static_cast<PointerType>(entry.register_data()), 0, 0, jump_amount);
        }

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_symbol_entry& entry)
        {
            size_t jump_amount{};
            process_entry_reference(entry, static_cast<PointerType>(entry.address()), entry.address(), 0, jump_amount);
        }

        template <typename PointerType>
        template <typename NodeType>
        void process_heap_graph_generator<PointerType>::process_entry_reference(NodeType& entry, PointerType const data, uint64_t const address, size_t const offset, size_t& jump_amount)
        {
            auto const it = heap_entries_.lower_bound(data);
            if(it == heap_entries_.end())
            {
                return;
            }

            auto& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
            if(node.index() == entry.index() || !node.heap_entry().contains_address_range(data, size_units::base_16::bytes{1}))
            {
                return;
            }

            if(auto const variable_symbol_reference = entry.find_symbol_variable_reference(address);
                !variable_symbol_reference.has_value())
            {
                // no variable symbol match
                entry.add_to_reference(process_heap_entry_reference{offset, data, node});
                node.add_from_reference(process_heap_entry_reference{offset, data, entry});
                jump_amount += sizeof(PointerType);
            }
            else if(variable_symbol_reference->variable_address == address)
            {
                // symbol variable match
                entry.add_to_reference(process_heap_entry_reference{offset, data, node, variable_symbol_reference.value()});
                node.add_from_reference(process_heap_entry_reference{offset, data, entry, variable_symbol_reference.value()});
                jump_amount += sizeof(PointerType);
            }
            else
            {
                // symbol variable match that overlaps, so don't consider the current match a true pointer...
                [[maybe_unused]] auto break_point = 0;
            }
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

    void process_heap_graph::generate_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t const pointer_size)
    {
        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadListStream); stream.has_value())
        {
            if (thread_list_stream const thread_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread const thread{ *mini_dump_, thread_list.thread_list().Threads[0], process_->peb().names_list(), process_->peb().memory_list(), process_->peb().memory64_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(heap_entries, pointer_size, thread);
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadExListStream); stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread_ex const thread{ *mini_dump_, thread_ex_list.thread_list().Threads[0], process_->peb().names_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(heap_entries, pointer_size, thread);
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
    void process_heap_graph::generate_specific_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t const pointer_size, T const& thread)
    {
        if(process_->options().no_filter_heap_entries())
        {
            mark_as_system_t constexpr mark_as_system{true};
            auto const base_address = add_symbol_reference(heap_entries, thread->Teb, process_->peb().teb_symbol(), thread->ThreadId, thread.thread_name(), mark_as_system);

            std::map<uint64_t, symbol_type_utils::pointer_info> pointers;
            gather_all_pointers_from_symbol(process_->peb().walker(), process_->peb().teb_symbol(), process_->peb().teb_symbol(), base_address, thread->Teb - base_address, pointers, {});
            generate_pointer_symbol_references(heap_entries, pointers, pointer_size, mark_as_system);
        }

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

    void process_heap_graph::generate_pointer_symbol_references(std::map<uint64_t, size_t> const& heap_entries, std::map<uint64_t, symbol_type_utils::pointer_info> const& pointers, size_t const pointer_size, mark_as_system_t const mark_as_system)
    {
        for (auto const& pointer : pointers | std::views::values)
        {
            generate_pointer_symbol_reference(heap_entries, pointer, pointer_size, mark_as_system);
        }
    }

    void process_heap_graph::generate_pointer_symbol_reference(std::map<uint64_t, size_t> const& heap_entries, symbol_type_utils::pointer_info const& pointer, size_t const pointer_size, mark_as_system_t const mark_as_system)
    {
        auto const it = heap_entries.lower_bound(pointer.base_address);
        if(it == heap_entries.end())
        {
            
            auto& symbol_node = find_or_add_node_symbol_reference(pointer.base_address, pointer.base_variable_type, {}, {}, mark_as_system);
            if(mark_as_system)
            {
                symbol_node.mark_as_system_allocation();
            }

            find_or_add_node_symbol_variable_reference(symbol_node, pointer.base_address, pointer.base_variable_address_offset, pointer_size, pointer.base_variable_type, pointer.pointer_type, pointer.name);
            return;
        }

        auto& heap_node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
        if(mark_as_system)
        {
            heap_node.mark_as_system_allocation();
        }
        find_or_add_node_symbol_variable_reference(heap_node, pointer.base_address, pointer.base_variable_address_offset, pointer_size, pointer.base_variable_type, pointer.pointer_type, pointer.name);
    }

    uint64_t process_heap_graph::add_symbol_reference(std::map<uint64_t, size_t> const& heap_entries, uint64_t const address, dbg_help::symbol_type_info symbol_type, uint32_t const thread_id, std::wstring_view const& thread_name, mark_as_system_t const mark_as_system)
    {
        auto const it = heap_entries.lower_bound(address);
        if(it == heap_entries.end())
        {
            add_node_symbol_reference(address, std::move(symbol_type), thread_id, thread_name, mark_as_system);
            return address;
        }

        auto& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
        if(mark_as_system)
        {
            node.mark_as_system_allocation();
        }
        add_heap_node_symbol_reference(node, address, std::move(symbol_type), thread_id, thread_name);
        return it->first;
    }

    process_heap_graph_symbol_entry& process_heap_graph::add_node_symbol_reference(uint64_t const address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> const thread_id, std::wstring_view const& thread_name, mark_as_system_t const mark_as_system)
    {
        process_heap_graph_symbol_entry node{address, std::move(symbol_type), thread_id, std::wstring{thread_name}};
        if(mark_as_system)
        {
            node.mark_as_system_allocation();
        }
        return std::get<process_heap_graph_symbol_entry>(nodes_.emplace_back(std::move(node)));
    }

    process_heap_graph_symbol_entry& process_heap_graph::find_or_add_node_symbol_reference(uint64_t const address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> const thread_id, std::wstring_view const& thread_name, mark_as_system_t const mark_as_system)
    {
        auto const it = std::ranges::find_if(nodes_, [address](auto const& node)
            {
                if(!std::holds_alternative<process_heap_graph_symbol_entry>(node))
                {
                    return false;
                }

                if(auto& symbol_entry = std::get<process_heap_graph_symbol_entry>(node);
                    symbol_entry.address() == address)
                {
                    return true;
                }
                return false;
            });

        if(it == nodes_.end())
        {
           return add_node_symbol_reference(address, std::move(symbol_type), thread_id, thread_name, mark_as_system);
        }

        return std::get<process_heap_graph_symbol_entry>(*it);
    }

    void process_heap_graph::add_heap_node_symbol_reference(process_heap_graph_node& node, uint64_t address, dbg_help::symbol_type_info symbol_type, uint32_t thread_id, std::wstring_view const& thread_name) const
    {
        node.add_symbol_address_reference({address, std::move(symbol_type), thread_id, std::wstring{thread_name}});
    }

    process_heap_entry_symbol_address_reference& process_heap_graph::add_heap_node_symbol_reference(process_heap_graph_node& node, uint64_t address, dbg_help::symbol_type_info symbol_type) const
    {
        return node.add_symbol_address_reference({address, std::move(symbol_type)});
    }

    void process_heap_graph::find_or_add_node_symbol_variable_reference(process_heap_graph_node& node, uint64_t const base_address, uint64_t const base_address_offset, size_t const pointer_size, dbg_help::symbol_type_info const& base_symbol_type, dbg_help::symbol_type_info const& pointer_symbol_type, std::wstring const& pointer_name) const
    {
        auto& node_symbol_reference = get_node_symbol_variable_reference(node, base_address, base_symbol_type);
        node_symbol_reference.add_variable_symbol_reference(base_address + base_address_offset, base_address_offset, pointer_size, pointer_symbol_type, pointer_name);
    }

    process_heap_entry_symbol_address_reference& process_heap_graph::get_node_symbol_variable_reference(process_heap_graph_node& node, uint64_t const address, dbg_help::symbol_type_info const& symbol_type) const
    {
        auto const it = std::ranges::find_if(node.symbol_references(), [address](auto const& symbol_reference)
            {
                return symbol_reference.address() == address;
            });

        if(it == node.symbol_references().end())
        {
            return add_heap_node_symbol_reference(node, address, symbol_type);
        }

        return *it;
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

    void process_heap_graph::mark_all_system_module_global_variables_and_references(std::unordered_map<uint64_t, bool>& result_cache)
    {
        // pass one generate results
        for(auto& node : nodes_)
        {
            is_node_or_children_system_module_global_variable(node, result_cache);
        }

        // pass two, mark discovered system allocations
        for(auto& node : nodes_)
        {
            if(auto& graph_node = get_graph_node(node);
                result_cache[graph_node.index()])
            {
                graph_node.is_system() = true;
                if(!graph_node.is_root_node() && !graph_node.is_system_allocation())
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
                if(process_->options().mark_system_heap_entries_children_as_system())
                {
                    mark_all_children_as_system(node, result_cache);
                }
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

    void process_heap_graph::mark_all_children_as_system(process_heap_graph_entry_type const& node, std::unordered_map<uint64_t, bool>& result_cache) const
    {
        for(auto& to_reference : get_graph_node(node).to_references())
        {
            auto const& child_node = get_node_from_index(to_reference.node_index());
            auto const& child_graph_node = get_graph_node(child_node);
            if(auto const& cache_result = result_cache.find(child_graph_node.index()); cache_result == result_cache.end() || !cache_result->second)
            {
                result_cache[child_graph_node.index()] = true;
                mark_all_children_as_system(child_node, result_cache);
            }
        }
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

    process_heap_graph_entry_type& process_heap_graph::get_node_from_index(uint64_t node_index)
    {
        auto const it = std::ranges::find_if(nodes_, [node_index](auto const& node) { return get_graph_node(node).index() == node_index; });
        if(it == nodes_.end())
        {
            throw exceptions::wide_runtime_error{std::format(L"Graph Allocation Nodes does not contain node index [{}]", node_index)};
        }

        return *it;
    }

    size_t process_heap_graph::machine_pointer_size() const
    {
        return static_cast<size_t>(process_->peb().machine_pointer_size());
    }

    void process_heap_graph::generate_graph()
    {
        // generate all reference nodes
        auto const heap_entries = generate_allocation_references();
        generate_global_variable_references(heap_entries);
        generate_thread_context_references(heap_entries, machine_pointer_size());

        // link all reference nodes
        generate_node_references(heap_entries);

        // remove any reference nodes that is not a allocation and has no to references
        remove_all_non_allocation_with_empty_to_references();

        // find and optionally remove "system" allocations
        std::unordered_map<uint64_t, bool> result_cache;
        mark_all_system_module_global_variables_and_references(result_cache);
        if(!process_->options().no_filter_heap_entries())
        {
            remove_all_system_nodes(result_cache);
        }
    }
}
