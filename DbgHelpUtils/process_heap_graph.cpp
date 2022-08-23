#include "process_heap_graph.h"

#include <algorithm>
#include <array>
#include <ranges>

#include "common_symbol_names.h"
#include "crt_entry.h"
#include "crt_heap.h"
#include "dph_entry.h"
#include "get_last_address.h"
#include "global_symbol.h"
#include "global_symbols.h"
#include "heap_entry.h"
#include "heap_lfh_affinity_slot.h"
#include "heap_lfh_bucket.h"
#include "heap_lfh_context.h"
#include "heap_lfh_entry.h"
#include "heap_lfh_subsegment.h"
#include "heap_page_segment.h"
#include "heap_segment.h"
#include "heap_segment_context.h"
#include "heap_subsegment.h"
#include "heap_virtual_block.h"
#include "heap_vs_context.h"
#include "heap_vs_entry.h"
#include "heap_vs_subsegment.h"
#include "large_alloc_entry.h"
#include "lfh_heap.h"
#include "lfh_segment.h"
#include "page_range_descriptor.h"
#include "page_range_flags_utils.h"
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

        using symbol_type_info_set = std::unordered_set<dbg_help::symbol_type_info, dbg_help::symbol_type_info::HashFunction>;

        symbol_type_info_set build_filtered_system_type_indexes(stream_stack_dump::mini_dump_memory_walker const& walker, process_heaps_options const& options)
        {
            static std::array const g_ignored_system_symbols
            {
                &common_symbol_names::acrt_first_block,
            };

            symbol_type_info_set rv;

            for (auto const* symbol_name : g_ignored_system_symbols)
            {
                for(process::global_symbols const variables{walker, *symbol_name};
                    auto const& global_symbol : variables.all_symbols())
                {
                    if(global_symbol.symbol_type().sym_tag().value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PublicSymbol)
                    {
                        rv.insert(global_symbol.symbol_type());
                    }
                }
            }

            for (auto const& filtered_system_type : options.filtered_system_types())
            {
                for(process::global_symbols const variables{walker, filtered_system_type};
                    auto const& global_symbol : variables.all_symbols())
                {
                    if(global_symbol.symbol_type().sym_tag().value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PublicSymbol)
                    {
                        rv.insert(global_symbol.symbol_type());
                    }
                }
            }

            return rv;
        }

        template<typename PointerType>
        class process_heap_graph_generator
        {
        public:
            process_heap_graph_generator(std::map<uint64_t, size_t> const& heap_entries
                , std::unordered_set<uint64_t> const& system_module_bases
                , std::vector<process_heap_graph_entry_type> & nodes
                , symbol_type_info_set filtered_system_type_indexes
                , process_heaps_options const& options)
                : heap_entries_{heap_entries}
                , system_module_bases_{system_module_bases}
                , nodes_{nodes}
                , filtered_system_type_indexes_{std::move(filtered_system_type_indexes)}
                , options_{options}
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
            void process_entry_reference(NodeType& entry, PointerType data, uint64_t address, size_t offset, size_t& jump_amount, mark_as_system_t mark_as_system = mark_as_system_t{false});

            [[nodiscard]] mark_as_system_t is_system_global_variable(process_heap_graph_global_variable_entry const& entry) const;

        private:
            std::map<uint64_t, size_t> const& heap_entries_;
            std::unordered_set<uint64_t> const& system_module_bases_;
            std::vector<process_heap_graph_entry_type>& nodes_;
            symbol_type_info_set filtered_system_type_indexes_;
            process_heaps_options const& options_;
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
                    process_entry_reference(entry, data, entry.variable().variable_memory_range().start_range + offset, offset, jump_amount, is_system_global_variable(entry));
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
        void process_heap_graph_generator<PointerType>::process_entry_reference(NodeType& entry, PointerType const data, uint64_t const address, size_t const offset, size_t& jump_amount, mark_as_system_t const mark_as_system)
        {
            jump_amount = 1;
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

            if(mark_as_system && !node.is_system_allocation())
            {
                node.mark_as_system_allocation();
            }

            if(auto const variable_symbol_reference = entry.find_symbol_variable_reference(address);
                !variable_symbol_reference.has_value())
            {
                // no variable symbol match
                auto const node_offset = data - node.start_address();
                entry.add_to_reference(process_heap_entry_reference{offset, address, node_offset, data, node});
                node.add_from_reference(process_heap_entry_reference{offset, address, node_offset, data, entry});
                jump_amount += sizeof(PointerType);
            }
            else if(variable_symbol_reference->variable_address == address)
            {
                // symbol variable match
                auto const node_offset = data - node.start_address();
                entry.add_to_reference(process_heap_entry_reference{offset, address, node_offset, data, node, variable_symbol_reference.value()});
                node.add_from_reference(process_heap_entry_reference{offset, address, node_offset, data, entry, variable_symbol_reference.value()});
                jump_amount += sizeof(PointerType);
            }
            else
            {
                // symbol variable match that overlaps, so don't consider the current match a true pointer...
                [[maybe_unused]] auto break_point = 0;
            }
        }

        template <typename PointerType>
        mark_as_system_t process_heap_graph_generator<PointerType>::is_system_global_variable(process_heap_graph_global_variable_entry const& entry) const
        {
            if (options_.no_mark_global_variable_data_pointers_as_system())
            {
                return mark_as_system_t{ false };
            }
            auto const& type = entry.variable().symbol_type();
            return mark_as_system_t{ system_module_bases_.contains(type.module_base()) && !filtered_system_type_indexes_.contains(type) };
        }

        template<typename PointerType>
        void generate_graph_nodes(std::map<uint64_t, size_t> const& heap_entries
            , std::unordered_set<uint64_t> const& system_module_bases
            , std::vector<process_heap_graph_entry_type> & nodes
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , process_heaps_options const& options)
        {
            process_heap_graph_generator<PointerType> generator{heap_entries, system_module_bases, nodes, build_filtered_system_type_indexes(walker, options), options};
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
        for(process::global_symbols const variables{process_->peb().walker()};
            auto const& global_symbol : variables.all_symbols())
        {
            if(global_symbol.symbol_type().sym_tag().value_or(dbg_help::sym_tag_enum::Null) == dbg_help::sym_tag_enum::Data)
            {
                auto heap_entry = find_allocation_node_allocation(heap_entries, global_symbol.variable_memory_range());
                process_heap_graph_global_variable_entry node{global_symbol, heap_entry};
                auto const system_global_variable = system_module_bases_.contains(global_symbol.symbol_type().module_base());
                node.set_as_system(system_global_variable);
                nodes_.emplace_back(std::move(node));

                if(system_global_variable && heap_entry.has_value() && !heap_entry.value().is_system_allocation())
                {
                    heap_entry.value().mark_as_system_allocation();
                }
            }
        }
    }

    void process_heap_graph::generate_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t const pointer_size, std::unordered_set<uint64_t>& ignore_pointers)
    {
        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadListStream); stream.has_value())
        {
            if (thread_list_stream const thread_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread const thread{ *mini_dump_, thread_list.thread_list().Threads[0], process_->peb().names_list(), process_->peb().memory_list(), process_->peb().memory64_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(heap_entries, pointer_size, thread, ignore_pointers, mark_as_system_t{true});
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(*mini_dump_, ThreadExListStream); stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ *mini_dump_, std::get<size_t>(stream.value()) }; thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread_ex const thread{ *mini_dump_, thread_ex_list.thread_list().Threads[0], process_->peb().names_list() }; thread->Teb != 0)
                {
                    generate_specific_thread_context_references(heap_entries, pointer_size, thread, ignore_pointers, mark_as_system_t{true});
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
    void process_heap_graph::generate_specific_thread_context_references(std::map<uint64_t, size_t> const& heap_entries, size_t const pointer_size, T const& thread, std::unordered_set<uint64_t>& ignore_pointers, mark_as_system_t const mark_as_system)
    {
        auto const base_address = add_symbol_reference(heap_entries
            , thread->Teb
            , process_->peb().teb_symbol()
            , thread->ThreadId
            , thread.thread_name()
            , mark_as_system
            , mark_as_can_contain_user_pointers_t{false});

        std::map<uint64_t, symbol_type_utils::pointer_info> pointers;
        gather_all_pointers_from_symbol(process_->peb().walker()
            , process_->peb().teb_symbol()
            , process_->peb().teb_symbol()
            , base_address
            , thread->Teb - base_address
            , pointers
            , {}
            , ignore_pointers);
        generate_pointer_symbol_references(heap_entries, pointers, pointer_size, mark_as_system, ignore_pointers);

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
            nodes_.emplace_back(process_heap_graph_heap_entry
                {
                    entry
                    , process_->is_system_allocation(entry.user_memory_range()) ? process_heap_graph_heap_entry_type::system_allocation : process_heap_graph_heap_entry_type::allocation
                });
            heap_entries.insert(std::make_pair(get_last_address(entry), nodes_.size() - 1));
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

    void process_heap_graph::generate_heap_metadata_symbol_references(std::map<uint64_t, size_t> const& heap_entries, size_t const pointer_size, std::unordered_set<uint64_t>& ignore_pointers)
    {
        crt_heap const crt_heap{ process_->cache(), process_->peb() };
        std::map<uint64_t, crt_entry> crt_entries;

        if(crt_heap.is_using_crt_heap())
        {
            for (auto const& entry : crt_heap.entries())
            {
                crt_entries.insert(std::make_pair(entry.end_entry_address() - 1, entry));
                std::ignore = add_symbol_reference(heap_entries, entry.symbol_address(), entry.symbol_type(), std::nullopt, {}, mark_as_system_t{false}, mark_as_can_contain_user_pointers_t{false});
            }
        }

        for (uint32_t heap_index = 0; heap_index < process_->peb().number_of_heaps(); ++heap_index)
        {
            if (auto const nt_heap = process_->peb().nt_heap(heap_index); nt_heap.has_value())
            {
                generate_nt_heap_references(nt_heap.value(), heap_entries, pointer_size, crt_entries, ignore_pointers);
            }
            else if (auto const segment_heap = process_->peb().segment_heap(heap_index); segment_heap.has_value())
            {
                generate_segment_references(segment_heap.value(), heap_entries, pointer_size, crt_entries, ignore_pointers);
            }
        }

        for (auto const& heap : dph_heap::dph_heaps(process_->cache(), process_->peb()))
        {
            generate_dph_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        }
    }

    void process_heap_graph::generate_system_symbol_reference(uint64_t const address
        , dbg_help::symbol_type_info const& symbol_type
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::unordered_set<uint64_t>& ignore_pointers
        , mark_as_system_t const mark_as_system
        , mark_as_can_contain_user_pointers_t const mark_as_can_contain_user_pointers)
    {
        auto const base_address = add_symbol_reference(heap_entries, address, symbol_type, std::nullopt, {}, mark_as_system, mark_as_can_contain_user_pointers);

        std::map<uint64_t, symbol_type_utils::pointer_info> pointers;
        gather_all_pointers_from_symbol(process_->peb().walker(), symbol_type, symbol_type, base_address, address - base_address, pointers, {}, ignore_pointers);
        generate_pointer_symbol_references(heap_entries, pointers, pointer_size, mark_as_system, ignore_pointers);
    }

    void process_heap_graph::generate_nt_heap_references(nt_heap const& heap, std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        generate_system_symbol_reference(heap.symbol_address(), heap.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});

        generate_nt_heap_lfh_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        generate_nt_heap_segment_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        generate_nt_heap_virtual_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
    }

    void process_heap_graph::generate_nt_heap_lfh_metadata_symbol_references(nt_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        if (auto const lfh_heap = heap.lfh_heap(); lfh_heap.has_value())
        {
            auto const& lfh_heap_value = lfh_heap.value();
            generate_system_symbol_reference(lfh_heap_value.symbol_address(), lfh_heap_value.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

            for (auto const& segment : lfh_heap_value.lfh_segments())
            {
                generate_system_symbol_reference(segment.symbol_address(), segment.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});
                
                for (auto const& subsegment : segment.subsegments())
                {
                    generate_system_symbol_reference(subsegment.symbol_address(), subsegment.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});
                    mark_all_non_crt_entries_as_system(subsegment, heap_entries, crt_entries);
                }
            }
        }
    }

    void process_heap_graph::generate_nt_heap_segment_metadata_symbol_references(nt_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& segment : heap.segments())
        {
            generate_system_symbol_reference(segment.symbol_address(), segment.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});
            mark_all_non_crt_entries_as_system(segment, heap_entries, crt_entries);
        }
    }

    void process_heap_graph::generate_nt_heap_virtual_metadata_symbol_references(nt_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& virtual_block : heap.heap_virtual_blocks())
        {
            generate_system_symbol_reference(virtual_block.symbol_address(), virtual_block.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});
            mark_all_non_crt_entries_as_system(virtual_block, heap_entries, crt_entries);
        }
    }

    void process_heap_graph::generate_segment_references(segment_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        generate_system_symbol_reference(heap.symbol_address(), heap.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

        generate_segment_heap_backend_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        generate_segment_heap_entities_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        generate_segment_heap_lfh_metadata_symbol_references(heap, heap_entries, pointer_size, crt_entries, ignore_pointers);
        generate_segment_heap_large_metadata_symbol_references(heap, heap_entries, crt_entries);
    }

    void process_heap_graph::generate_segment_heap_backend_metadata_symbol_references(segment_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& segment_context : heap.segment_contexts())
        {
            generate_system_symbol_reference(heap.symbol_address(), heap.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

            for (auto const& page : segment_context.pages())
            {
                generate_system_symbol_reference(page.symbol_address(), page.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});

                if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
                {
                    continue;
                }

                for (auto const& entry : page.entries())
                {
                    if (entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT || !is_crt_entry(entry.user_address(), crt_entries))
                    {
                        mark_heap_node_as_system(entry.user_address(), heap_entries);
                    }
                }
            }
        }
    }

    void process_heap_graph::generate_segment_heap_entities_metadata_symbol_references(segment_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& subsegment : heap.vs_context().subsegments())
        {
            generate_system_symbol_reference(subsegment.symbol_address(), subsegment.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});

            if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
            {
                continue;
            }

            for (auto const& entry : subsegment.entries())
            {
                if (!entry.uncommitted_range() && entry.allocated() && !is_crt_entry(entry.user_address(), crt_entries))
                {
                    mark_heap_node_as_system(entry.user_address(), heap_entries);
                }
            }
        }
    }

    void process_heap_graph::generate_segment_heap_lfh_metadata_symbol_references(segment_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& bucket : heap.lfh_context().active_buckets())
        {
            if (bucket.is_enabled())
            {
                generate_system_symbol_reference(bucket.symbol_address(), bucket.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

                for (auto const& affinity_slot : bucket.affinity_slots())
                {
                    generate_system_symbol_reference(affinity_slot.symbol_address(), affinity_slot.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

                    for (auto const& subsegment : affinity_slot.subsegments())
                    {
                        generate_system_symbol_reference(subsegment.symbol_address(), subsegment.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{true});

                        if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
                        {
                            continue;
                        }

                        // anything not in the crt list is marked as system...
                        for (auto const& entry : subsegment.entries())
                        {
                            if(entry.allocated() && !is_crt_entry(entry.user_address(), crt_entries))
                            {
                                mark_heap_node_as_system(entry.user_address(), heap_entries);
                            }
                        }
                    }
                }
            }
        }
    }

    void process_heap_graph::generate_segment_heap_large_metadata_symbol_references(segment_heap const& heap, std::map<uint64_t, size_t> const& heap_entries, std::map<uint64_t, crt_entry> const& crt_entries)
    {
        // anything not in the crt list is marked as system...
        if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
        {
            return;
        }

        for (auto const& entry : heap.large_entries())
        {
            if(!is_crt_entry(entry.user_address(), crt_entries))
            {
                mark_heap_node_as_system(entry.user_address(), heap_entries);
            }
        }
    }

    void process_heap_graph::generate_dph_symbol_references(dph_heap const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , size_t const pointer_size
        , std::map<uint64_t, crt_entry> const& crt_entries
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        generate_system_symbol_reference(heap.symbol_address(), heap.symbol_type(), heap_entries, pointer_size, ignore_pointers, mark_as_system_t{true}, mark_as_can_contain_user_pointers_t{false});

        generate_dph_entities_metadata_symbol_references(heap, heap_entries, crt_entries);
        generate_dph_virtual_metadata_symbol_references(heap, heap_entries, crt_entries);
    }

    void process_heap_graph::generate_dph_entities_metadata_symbol_references(dph_heap const& heap, std::map<uint64_t, size_t> const& heap_entries, std::map<uint64_t, crt_entry> const& crt_entries)
    {
        if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
        {
            return;
        }

        // anything not in the crt list is marked as system...
        for (auto const& entry : heap.busy_entries())
        {
            if (entry.is_allocated() && !is_crt_entry(entry.user_address(), crt_entries))
            {
                mark_heap_node_as_system(entry.user_address(), heap_entries);
            }
        }
    }

    void process_heap_graph::generate_dph_virtual_metadata_symbol_references(dph_heap const& heap, std::map<uint64_t, size_t> const& heap_entries, std::map<uint64_t, crt_entry> const& crt_entries)
    {
        if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
        {
            return;
        }

        // anything not in the crt list is marked as system...
        for (auto const& entry : heap.virtual_ranges())
        {
            if (entry.is_allocated() && !is_crt_entry(entry.user_address(), crt_entries))
            {
                mark_heap_node_as_system(entry.user_address(), heap_entries);
            }
        }
    }

    template <typename T>
    void process_heap_graph::mark_all_non_crt_entries_as_system(T const& heap
        , std::map<uint64_t, size_t> const& heap_entries
        , std::map<uint64_t, crt_entry> const& crt_entries)
    {
        if(crt_entries.empty() || process_->options().no_mark_all_crt_entries_as_system())
        {
            return;
        }

        // anything not in the crt list is marked as system...
        for (auto const& entry : heap.entries())
        {
            if(entry.is_busy() && !is_crt_entry(entry.address(), crt_entries))
            {
                mark_heap_node_as_system(entry.address(), heap_entries);
            }
        }
    }

    void process_heap_graph::generate_node_references(std::map<uint64_t, size_t> const& heap_entries)
    {
        if(process_->peb().is_x86_target())
        {
            generate_graph_nodes<uint32_t>(heap_entries, system_module_bases_, nodes_, process_->peb().walker(), process_->options());
        }
        else
        {
            generate_graph_nodes<uint64_t>(heap_entries, system_module_bases_, nodes_, process_->peb().walker(), process_->options());
        }
    }

    void process_heap_graph::generate_pointer_symbol_references(std::map<uint64_t, size_t> const& heap_entries
        , std::map<uint64_t, symbol_type_utils::pointer_info> const& pointers
        , size_t const pointer_size
        , mark_as_system_t const mark_as_system
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        for (auto const& pointer : pointers | std::views::values)
        {
            generate_pointer_symbol_reference(heap_entries, pointer, pointer_size, mark_as_system, ignore_pointers);
        }
    }

    void process_heap_graph::generate_pointer_symbol_reference(std::map<uint64_t, size_t> const& heap_entries
        , symbol_type_utils::pointer_info const& pointer
        , size_t const pointer_size
        , mark_as_system_t const mark_as_system
        , std::unordered_set<uint64_t>& ignore_pointers)
    {
        if(ignore_pointers.contains(pointer.base_address + pointer.base_variable_address_offset))
        {
            return;
        }

        ignore_pointers.insert(pointer.base_address + pointer.base_variable_address_offset);

        if(auto const it = heap_entries.lower_bound(pointer.base_address);
            it != heap_entries.end())
        {
            if(auto& heap_node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
                heap_node.heap_entry().contains_address_range(pointer.base_address + pointer.base_variable_address_offset, size_units::base_16::bytes{1}))
            {
                if(mark_as_system)
                {
                    heap_node.mark_as_system_allocation();
                }
                find_or_add_node_symbol_variable_reference(heap_node
                    , is_root_symbol_t{pointer.base_address + pointer.base_variable_address_offset == heap_node.heap_entry().user_address()}
                    , is_metadata_symbol_t{false}
                    , pointer.base_address
                    , pointer.base_variable_address_offset
                    , pointer_size
                    , pointer.base_variable_type
                    , pointer.pointer_type
                    , pointer.name);
                return;
            }
            else if(heap_node.heap_entry().contains_metadata_address_range(pointer.base_address + pointer.base_variable_address_offset, size_units::base_16::bytes{1}))
            {
                find_or_add_node_symbol_variable_reference(heap_node
                    , is_root_symbol_t{false}
                    , is_metadata_symbol_t{true}
                    , pointer.base_address
                    , pointer.base_variable_address_offset
                    , pointer_size
                    , pointer.base_variable_type
                    , pointer.pointer_type
                    , pointer.name);
                return;
            }
        }

        auto& symbol_node = find_or_add_node_symbol_reference(pointer.base_address, pointer.base_variable_type, {}, {}, mark_as_system);
        if(mark_as_system)
        {
            symbol_node.set_as_system();
        }

        find_or_add_node_symbol_variable_reference(symbol_node
            , is_root_symbol_t{false}
            , is_metadata_symbol_t{false}
            , pointer.base_address
            , pointer.base_variable_address_offset
            , pointer_size
            , pointer.base_variable_type
            , pointer.pointer_type
            , pointer.name);
    }

    bool process_heap_graph::is_crt_entry(uint64_t const address, std::map<uint64_t, crt_entry> const& crt_entries)
    {
        auto const it = crt_entries.lower_bound(address);
        return it != crt_entries.end() && it->second.entry_address() <= address;
    }

    void process_heap_graph::mark_heap_node_as_system(uint64_t const address, std::map<uint64_t, size_t> const& heap_entries)
    {
        if(process_->options().no_mark_all_crt_entries_as_system())
        {
            return;
        }

        auto const it = heap_entries.lower_bound(address);
        if(it == heap_entries.end())
        {
            return;
        }

        auto& heap_node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
        if(!heap_node.heap_entry().contains_address_range(address, size_units::base_16::bytes{1}))
        {
            return;
        }

        heap_node.mark_as_system_allocation();
    }

    uint64_t process_heap_graph::add_symbol_reference(std::map<uint64_t, size_t> const& heap_entries
        , uint64_t const address
        , dbg_help::symbol_type_info symbol_type
        , std::optional<uint32_t> const& thread_id
        , std::wstring_view const& thread_name
        , mark_as_system_t const mark_as_system
        , mark_as_can_contain_user_pointers_t const mark_as_can_contain_user_pointers)
    {
        if(auto const it = heap_entries.lower_bound(address);
            it != heap_entries.end())
        {
            if(auto& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
                node.heap_entry().contains_address_range(address, size_units::base_16::bytes{1}))
            {
                if(mark_as_system)
                {
                    node.mark_as_system_allocation();
                }
                if(mark_as_can_contain_user_pointers)
                {
                    node.can_contain_user_pointers() = true;
                }

                add_heap_node_symbol_reference(node, is_root_symbol_t{address == node.heap_entry().user_address()}, is_metadata_symbol_t{false}, address, std::move(symbol_type), thread_id, thread_name);
                return it->first;
            }
            else if(node.heap_entry().contains_metadata_address_range(address, size_units::base_16::bytes{1}))
            {
                add_heap_node_symbol_reference(node, is_root_symbol_t{false}, is_metadata_symbol_t{true}, address, std::move(symbol_type), thread_id, thread_name);
                return address;
            }
        }

        auto entry = add_node_symbol_reference(address, std::move(symbol_type), thread_id, thread_name, mark_as_system);
        if(mark_as_can_contain_user_pointers)
        {
            entry.can_contain_user_pointers() = true;
        }

        return address;
    }

    process_heap_graph_symbol_entry& process_heap_graph::add_node_symbol_reference(uint64_t const address
        , dbg_help::symbol_type_info symbol_type
        , std::optional<uint32_t> const thread_id
        , std::wstring_view const& thread_name
        , mark_as_system_t const mark_as_system)
    {
        process_heap_graph_symbol_entry node{address, std::move(symbol_type), thread_id, std::wstring{thread_name}};
        if(mark_as_system)
        {
            node.set_as_system();
        }
        return std::get<process_heap_graph_symbol_entry>(nodes_.emplace_back(std::move(node)));
    }

    process_heap_graph_symbol_entry& process_heap_graph::find_or_add_node_symbol_reference(uint64_t const address
        , dbg_help::symbol_type_info symbol_type
        , std::optional<uint32_t> const& thread_id
        , std::wstring_view const& thread_name
        , mark_as_system_t const mark_as_system)
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

    void process_heap_graph::add_heap_node_symbol_reference(process_heap_graph_node& node
        , is_root_symbol_t const is_root_symbol
        , is_metadata_symbol_t const is_metadata_symbol
        , uint64_t const address
        , dbg_help::symbol_type_info symbol_type
        , std::optional<uint32_t> const& thread_id
        , std::wstring_view const& thread_name) const
    {
        if(thread_id.has_value())
        {
            node.add_symbol_address_reference({is_root_symbol, is_metadata_symbol, address, node.index(), std::move(symbol_type), thread_id.value(), std::wstring{thread_name}});
        }
        else
        {
            add_heap_node_symbol_reference(node, is_root_symbol, is_metadata_symbol, address, std::move(symbol_type));
        }
    }

    process_heap_entry_symbol_address_reference& process_heap_graph::add_heap_node_symbol_reference(process_heap_graph_node& node
        , is_root_symbol_t const is_root_symbol
        , is_metadata_symbol_t const is_metadata_symbol
        , uint64_t address
        , dbg_help::symbol_type_info symbol_type) const
    {
        return node.add_symbol_address_reference({is_root_symbol, is_metadata_symbol, address, node.index(), std::move(symbol_type)});
    }

    void process_heap_graph::find_or_add_node_symbol_variable_reference(process_heap_graph_node& node
        , is_root_symbol_t const is_root_symbol
        , is_metadata_symbol_t const is_metadata_symbol
        , uint64_t const base_address
        , uint64_t const base_address_offset
        , size_t const pointer_size
        , dbg_help::symbol_type_info const& base_symbol_type
        , dbg_help::symbol_type_info const& pointer_symbol_type
        , std::wstring const& pointer_name) const
    {
        auto& node_symbol_reference = get_node_symbol_variable_reference(node, is_root_symbol, is_metadata_symbol, base_address, base_symbol_type);
        node_symbol_reference.add_variable_symbol_reference(base_address + base_address_offset, base_address_offset, pointer_size, pointer_symbol_type, pointer_name);
    }

    process_heap_entry_symbol_address_reference& process_heap_graph::get_node_symbol_variable_reference(process_heap_graph_node& node
        , is_root_symbol_t const is_root_symbol
        , is_metadata_symbol_t const is_metadata_symbol
        , uint64_t const address
        , dbg_help::symbol_type_info const& symbol_type) const
    {
        auto const it = std::ranges::find_if(node.symbol_references(), [address](auto const& symbol_reference)
            {
                return symbol_reference.address() == address;
            });

        if(it == node.symbol_references().end())
        {
            return add_heap_node_symbol_reference(node, is_root_symbol, is_metadata_symbol, address, symbol_type);
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

    void process_heap_graph::find_and_mark_system_nodes(std::unordered_map<uint64_t, bool>& result_cache)
    {
        // pass one, gather all the system nodes
        auto node_to_process = true;
        default_mark_as_other_node_t default_mark_as_other_node{false};
        while(node_to_process)
        {
            node_to_process = false;
            for(auto& node : nodes_)
            {
                if(get_node_type(node, result_cache, default_mark_as_other_node) == node_type::requires_other_node_processing)
                {
                    node_to_process = true;
                }
            }

            default_mark_as_other_node = default_mark_as_other_node_t{true};
        }

        // pass two, mark discovered system allocations
        for(auto& node : nodes_)
        {
            if(auto& graph_node = get_graph_node(node);
                result_cache[graph_node.index()])
            {
                graph_node.set_as_system();
                if(!graph_node.is_root_node() && !graph_node.is_system_allocation())
                {
                    graph_node.mark_as_system_allocation();
                }
            }
        }
    }

    void process_heap_graph::remove_all_system_nodes(std::unordered_map<uint64_t, bool>& result_cache)
    {
        auto const [first, last] = std::ranges::remove_if(nodes_, [this, &result_cache](auto const& node)
        {
            if(auto const& graph_node = get_graph_node(node);
                graph_node.is_system_allocation() || graph_node.is_system())
            {
                auto it = result_cache.find(graph_node.index());
                if(it == result_cache.end())
                {
                    result_cache.insert(std::make_pair(graph_node.index(), true));
                }
                else if(!it->second)
                {
                    it->second = true;
                }

                return true;
            }

            return false;
        });

        nodes_.erase(first, last);

        for(auto& node : nodes_)
        {
            auto& graph_node = get_graph_node(node);
            for (auto const removed_node_index : result_cache | std::views::filter([](auto const& kv) { return kv.second; }) | std::views::keys)
            {
                graph_node.remove_references(removed_node_index);
            }

            graph_node.remove_metadata_symbol_references();
        }
    }

    process_heap_graph::node_type process_heap_graph::get_node_type(process_heap_graph_entry_type const& node
        , std::unordered_map<uint64_t, bool>& result_cache
        , default_mark_as_other_node_t const default_mark_as_other_node
        , can_contain_user_pointers_t const can_contain_user_pointers) const
    {
        auto const& graph_node = get_graph_node(node);
        if(auto const cache_result = result_cache.find(graph_node.index()); cache_result != result_cache.end())
        {
            if(cache_result->second)
            {
                return can_contain_user_pointers ? node_type::system_node : (graph_node.can_contain_user_pointers() ? node_type::system_node_user_container : node_type::system_node);
            }

            return node_type::other_node;
        }

        if(graph_node.is_system())
        {
            result_cache.insert(std::make_pair(graph_node.index(), true));
            return can_contain_user_pointers ? node_type::system_node : (graph_node.can_contain_user_pointers() ? node_type::system_node_user_container : node_type::system_node);
        }

        if(!std::holds_alternative<process_heap_graph_heap_entry>(node))
        {
            result_cache.insert(std::make_pair(graph_node.index(), false));
            return node_type::other_node;
        }

        /*
        if(!graph_node.from_references().empty() && !std::ranges::all_of(graph_node.from_references()
            , [this, &result_cache](auto const& child_node)
            {
                auto const type = get_node_type(get_node_from_index(child_node.node_index()), result_cache, can_contain_user_pointers_t{false});
                return type == node_type::other_node || type == node_type::system_node_user_container;
            }))
        {
            result_cache[graph_node.index()] = true;
            return node_type::system_node;
        }
        */

        if(graph_node.can_contain_user_pointers())
        {
            result_cache.insert(std::make_pair(graph_node.index(), false));
            return node_type::other_node;
        }

        if(!graph_node.to_references().empty())
        {
            auto some_require_node_process = false;
            for(auto const& to_reference : graph_node.to_references())
            {
                if(!result_cache.contains(to_reference.node_index()))
                {
                    if(!default_mark_as_other_node)
                    {
                        some_require_node_process = true;
                    }
                }
                else if(get_node_type(get_node_from_index(to_reference.node_index()), result_cache, default_mark_as_other_node_t{false}, can_contain_user_pointers_t{true}) == node_type::system_node)
                {
                    result_cache.insert(std::make_pair(graph_node.index(), true));
                    return node_type::system_node;
                }
            }

            if(some_require_node_process)
            {
                return node_type::requires_other_node_processing;
            }
        }

        result_cache.insert(std::make_pair(graph_node.index(), false));
        return node_type::other_node;
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
        std::unordered_set<uint64_t> ignore_pointers;
        generate_global_variable_references(heap_entries);
        generate_thread_context_references(heap_entries, machine_pointer_size(), ignore_pointers);
        generate_heap_metadata_symbol_references(heap_entries, machine_pointer_size(), ignore_pointers);

        // link all reference nodes
        generate_node_references(heap_entries);

        // remove any reference nodes that is not a allocation and has no to references
        remove_all_non_allocation_with_empty_to_references();

        // find and optionally remove "system" allocations
        std::unordered_map<uint64_t, bool> result_cache;
        find_and_mark_system_nodes(result_cache);
        if(!process_->options().no_filter_heap_entries())
        {
            remove_all_system_nodes(result_cache);
        }
    }
}
