#include "dump_file_options.h"

#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/assert_value.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/overload.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_graph.h"
#include "DbgHelpUtils/process_heap_graph_heap_entry.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#include <algorithm>
#include <ranges>

#include "DbgHelpUtils/register_names.h"

using namespace std;
using namespace dlg_help_utils;

namespace
{
    [[nodiscard]] bool is_root_node(heap::process_heap_graph_entry_type const& node)
    {
        return get_graph_node(node).is_root_node();
    }

    [[nodiscard]] uint64_t node_index(heap::process_heap_graph_entry_type const& node)
    {
        return get_graph_node(node).index();
    }

    [[nodiscard]] uint64_t node_start_address(heap::process_heap_graph_entry_type const& node)
    {
        auto start_address_of_node = overload {
            [](heap::process_heap_graph_heap_entry const& graph_node) { return graph_node.heap_entry().user_address(); },
            [](heap::process_heap_graph_global_variable_entry const& graph_node) { return graph_node.variable().symbol_type().address().value_or(0); },
            [](heap::process_heap_graph_thread_stack_entry const& graph_node) { return graph_node.stack_stream().current_address(); },
            [](heap::process_heap_graph_thread_context_entry const& graph_node) { return static_cast<uint64_t>(graph_node.register_type()); },
        };

        return std::visit(start_address_of_node, node);
    }

    std::wstring get_base_heap_entry_details(optional<heap::process_heap_graph_heap_entry> const& base_heap_entry, std::streamsize const hex_length)
    {
        if(!base_heap_entry.has_value())
        {
            return {};
        }

        auto const& heap_entry = base_heap_entry.value().heap_entry();

        // ReSharper disable once StringLiteralTypo
        return std::format(L" BaseHeapEntryAddr({0})", stream_hex_dump::to_hex(heap_entry.user_address(), hex_length));
    }

    std::wstring get_node_specific_data(heap::process_heap_graph_global_variable_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        using namespace size_units::base_16;
        auto const& entry = node.variable();

        auto const* module = walker.module_list().find_module(entry.symbol_type().module_base());
        auto module_name = module != nullptr ? module->name() : L"<unknown>"sv;
        auto const& base_heap_entry = node.base_heap_entry();

        // ReSharper disable once StringLiteralTypo
        return std::format(L" [Name({0}!{1}) Index({2}) Addr({3}) Size({4}){5}]", module_name, entry.symbol_type().name().value_or(L"<unknown>"sv), entry.symbol_type().sym_index(), stream_hex_dump::to_hex(entry.symbol_type().address().value_or(0), hex_length), to_wstring(bytes{entry.symbol_type().length().value_or(0)}), get_base_heap_entry_details(base_heap_entry, hex_length));
    }

    std::wstring get_node_specific_data(heap::process_heap_graph_heap_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        std::wostringstream ss;

        using namespace size_units::base_16;
        auto const& entry = node.heap_entry();
        // ReSharper disable once StringLiteralTypo
        ss << std::format(L" [UsrAddr({0}) Size({1})", stream_hex_dump::to_hex(entry.user_address(), hex_length), to_wstring(entry.user_requested_size()));
        if(!entry.filename().empty())
        {
            ss << std::format(L" {0}:{1}", entry.filename(), locale_formatting::to_wstring(entry.line_number()));
        }
        if(entry.has_request_number())
        {
            ss << std::format(L" #{}", locale_formatting::to_wstring(entry.request_number()));
        }
        if(!entry.allocation_stack_trace().empty())
        {
            ss << L" (has stack)";
        }

        ss << L"]";

        return std::move(ss).str();
    }

    std::wstring get_node_specific_data(heap::process_heap_graph_thread_stack_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        using namespace size_units::base_16;
        return std::format(L" [ThreadId({0}) Stack({1}) Size({2}) - {3}]", stream_hex_dump::to_hex(node.thread_id()), stream_hex_dump::to_hex(node.stack_stream().current_address(), hex_length), to_wstring(bytes{node.stack_stream().length()}), node.thread_name());
    }

    std::wstring get_node_specific_data(heap::process_heap_graph_thread_context_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        using namespace size_units::base_16;
        return std::format(L" [ThreadId({0}) Register({1}) Data({2}) - {3}]", stream_hex_dump::to_hex(node.thread_id()), register_names::get_register_name(node.register_type()), stream_hex_dump::to_hex(node.register_data(), hex_length), node.thread_name());
    }

    std::wstring get_node_attributes(heap::process_heap_graph_node const& node, std::optional<uint64_t> const& parent_offset, std::optional<uint64_t> const& pointer)
    {
        std::wostringstream ss;
        if(parent_offset.has_value())
        {
            ss << std::format(L" +{0}", stream_hex_dump::to_hex(parent_offset.value(), 4));
        }

        if(pointer.has_value())
        {
            ss << std::format(L" ({0})", stream_hex_dump::to_hex(pointer.value(), 4));
        }

        if(node.is_non_allocation_root_node())
        {
            ss << L" [root]";
        }
        else if(node.is_root_node())
        {
            ss << L" [disconnected root]";
        }

        if(node.is_disconnected_node_graph())
        {
            ss << L" [disconnected]";
        }

        if(node.is_cyclic_node_graphs_only())
        {
            ss << L" [cyclic]";
        }

        return std::move(ss).str();
    }

    using nodes_index_map = std::unordered_map<uint64_t, heap::process_heap_graph_entry_type const*>;

    template<typename T>
    std::wstring_view get_type_name()
    {
        if constexpr (std::is_same_v<T, heap::process_heap_graph_heap_entry>)
        {
            return L"Allocation"sv;
        }
        else if constexpr (std::is_same_v<T, heap::process_heap_graph_global_variable_entry>)
        {
            return L"GlobalVaraible"sv;
        }
        else if constexpr (std::is_same_v<T, heap::process_heap_graph_thread_stack_entry>)
        {
            return L"ThreadStack"sv;
        }
        else if constexpr (std::is_same_v<T, heap::process_heap_graph_thread_context_entry>)
        {
            return L"ThreadContext"sv;
        }
        else
        {
            static_assert(assert_value<false, T>::value, "unknown type");
            return L"Unknown"sv;
        }
    }

    template <typename T>
    void print_node_line(std::wostream& log, T const& node, size_t indent, std::optional<uint64_t> parent_offset, std::optional<uint64_t> pointer, auto const cycle_end_detected, std::streamsize const hex_length, bool const to_reference, bool const already_logged_children, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        log << std::format(L"{0:{1}}{2}{3}{4}{5}\n", ' ', indent, parent_offset.has_value() ? (to_reference ? L"->"sv : L"<-"sv) : get_type_name<T>(), get_node_attributes(node, parent_offset, pointer), get_node_specific_data(node, hex_length, walker), cycle_end_detected ? L" - cycle end"sv : (already_logged_children ? L" - already logged children"sv : L""sv));
    }

    heap::process_heap_graph_entry_type const& get_node_from_index(nodes_index_map const& nodes_index, uint64_t const index)
    {
        auto const it = nodes_index.find(index);
        if(it == nodes_index.end())
        {
            throw exceptions::wide_runtime_error{std::format(L"Nodes Index does not contain node index [{}]", index)};
        }
        return *it->second;
    }

    struct node_display_state
    {
        heap::process_heap_graph_entry_type const* node;
        size_t print_offset;
        size_t print_max_size;
        size_t indent;
        std::set<uint64_t> parents;
    };

    void print_display_node(std::wostream& log, heap::process_heap_graph_entry_type const& node, size_t const indent, bool const to_reference, std::optional<uint64_t> parent_offset, std::optional<uint64_t> pointer, bool const cycle_end_detected, bool const already_logged_children, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        std::visit([&log, indent, to_reference, &parent_offset, &pointer, cycle_end_detected, already_logged_children, hex_length, &walker](auto const& _) mutable
        {
            print_node_line(log, _, indent, parent_offset, pointer, cycle_end_detected, hex_length, to_reference, already_logged_children, walker);
        }, node);
    }

    void continue_display_node(wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , streamsize hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , vector<node_display_state>& node_stack
        , node_display_state state);

    void display_node(std::wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , heap::process_heap_graph_entry_type const& node
        , size_t const indent
        , bool const to_reference
        , std::optional<uint64_t> const& parent_offset
        , std::optional<uint64_t> const& pointer
        , bool const cycle_end_detected
        , bool const already_logged_children
        , std::set<uint64_t> const& parents
        , std::streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , std::vector<node_display_state>& node_stack)
    {
        auto const& graph_node = get_graph_node(node);
        print_display_node(log, node, indent, to_reference, parent_offset, pointer, cycle_end_detected, already_logged_children, hex_length, walker);

        if(cycle_end_detected || already_logged_children)
        {
            return;
        }

        if(!to_reference && !graph_node.from_references().empty())
        {
            log << std::format(L"{0:{1}}From References: ({2})\n", ' ', indent, graph_node.from_references().size());

            for(auto const& reference : graph_node.from_references())
            {
                auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
                print_display_node(log, child_node, indent+2, false, reference.offset(), reference.pointer(), false, false, hex_length, walker);
            }
        }

        if(!graph_node.to_references().empty())
        {
            if(!to_reference)
            {
                log << std::format(L"{0:{1}}To References: ({2})\n", ' ', indent, graph_node.to_references().size());
            }

            auto const print_max_size = to_reference ? options.display_heap_graph_to_reference_limit() : graph_node.to_references().size();

            auto node_parents = parents;
            node_parents.insert(graph_node.index());

            node_display_state state
            {
                &node,
                0,
                print_max_size,
                indent+2,
                std::move(node_parents)
            };
            continue_display_node(log, options, nodes_index, printed_nodes, hex_length, walker, node_stack, std::move(state));
        }
    }

    void continue_display_node(wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , vector<node_display_state>& node_stack
        , node_display_state state)
    {
        auto const& graph_node = get_graph_node(*state.node);
        for(auto const& reference : graph_node.to_references() | std::views::take(state.print_max_size) | std::views::drop(state.print_offset))
        {
            auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
            auto const& child_node_type = get_graph_node(child_node);
            auto const child_cycle_end_detected = state.parents.contains(child_node_type.index());
            auto const child_already_logged_children = printed_nodes.contains(child_node_type.index());

            printed_nodes.insert(child_node_type.index());
            ++state.print_offset;

            auto const& stack_node = node_stack.emplace_back(std::move(state));
            auto const stack_size = node_stack.size();

            display_node(log
                , options
                , nodes_index
                , printed_nodes
                , child_node
                , stack_node.indent
                , true
                , reference.offset()
                , reference.pointer()
                , child_cycle_end_detected
                , child_already_logged_children
                , stack_node.parents
                , hex_length
                , walker
                , node_stack
            );

            if(node_stack.size() != stack_size)
            {
                return;
            }

            state = std::move(node_stack.back());
            node_stack.pop_back();
        }

        if(graph_node.to_references().size() > state.print_max_size)
        {
            log << std::format(L"{0:{1}}(limited to first {2} of {3})\n", ' ', state.indent+1, options.display_heap_graph_to_reference_limit(), graph_node.to_references().size());
        }
    }

    void generate_display_node_and_print(std::wostream& log, dump_file_options const& options, nodes_index_map const& nodes_index, std::set<uint64_t>& printed_nodes, heap::process_heap_graph_entry_type const& node, size_t const indent, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        std::vector<node_display_state> node_stack;

        display_node(log, options, nodes_index, printed_nodes, node, indent, false, {}, {}, false, false, {}, hex_length, walker, node_stack);

        while(!node_stack.empty())
        {
            auto state = std::move(node_stack.back());
            node_stack.pop_back();
            continue_display_node(log, options, nodes_index, printed_nodes, hex_length, walker, node_stack, std::move(state));
        }
    }

    auto get_sort_key(heap::process_heap_graph_entry_type const& entry)
    {
        return std::make_tuple(entry.index(), !is_root_node(entry), node_start_address(entry));
    }
}

void dump_mini_dump_heap_graph(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps const heaps{mini_dump, cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options()};
    heap::process_heap_graph graph{mini_dump, heaps};
    auto const hex_length = heaps.peb().machine_hex_printable_length();

    std::wcerr << std::format(L"Generate Memory Allocation Graph\n");
    auto start = std::chrono::system_clock::now();
    graph.generate_graph();

    auto end = std::chrono::system_clock::now();
    std::wcerr << std::format(L"Generate Memory Allocation Graph Took ({0:%T})\n", end - start);
    start = end;
    std::wcerr << L"Sort Memory Allocation Graph\n";
    auto nodes = graph.nodes();
    std::ranges::sort(nodes, [](auto const& a, auto const& b)
        {
            return get_sort_key(a) < get_sort_key(b);
        });

    end = std::chrono::system_clock::now();
    std::wcerr << std::format(L"Sort Memory Allocation Graph Took ({0:%T})\n", end - start);
    start = end;
    std::wcerr << L"Generate Display for Memory Allocation Graph\n";
    nodes_index_map nodes_index;
    for (auto const& node : nodes)
    {
        nodes_index.insert(std::make_pair(node_index(node), &node));
    }

    // ReSharper disable once CppTooWideScope
    std::set<uint64_t> printed_nodes;

    log << L"Memory Allocation Graph:\n";
    for(auto const& node : nodes)
    {
        generate_display_node_and_print(log, options, nodes_index, printed_nodes, node, 2, hex_length, heaps.peb().walker());
    }

    end = std::chrono::system_clock::now();
    std::wcerr << std::format(L"Generate Display for Memory Allocation Graph Took ({0:%T})\n", end - start);
}
