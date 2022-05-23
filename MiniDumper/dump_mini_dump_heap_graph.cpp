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

    struct display_node
    {
        std::optional<heap::process_heap_graph_entry_type> node;
        bool cycle_end_detected;
        bool to_reference;
        bool already_logged_children;
        size_t indent;
        std::optional<uint64_t> parent_offset;
        std::optional<uint64_t> pointer;
        std::set<uint64_t> parents;
        bool expanded{false};
        std::wstring message;
    };


    void print_display_node(std::wostream& log, display_node const& display_node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        if(display_node.node.has_value())
        {
            std::visit([&log, &display_node, hex_length, &walker](auto const& _) mutable { print_node_line(log, _, display_node.indent, display_node.parent_offset, display_node.pointer, display_node.cycle_end_detected, hex_length, display_node.to_reference, display_node.already_logged_children, walker); }, display_node.node.value());
        }
        else
        {
            log << std::format(L"{0:{1}}{2}\n", ' ', display_node.indent, display_node.message);
        }
    }

    void expand_all_child_nodes(dump_file_options const& options, nodes_index_map const& nodes_index, std::set<uint64_t>& printed_nodes, std::list<display_node>& display_nodes)
    {
        bool all_expanded;
        do
        {
            all_expanded = true;

            for (auto it = display_nodes.begin(); it != display_nodes.end(); )
            {
                if(!it->expanded && it->node.has_value())
                {
                    it->expanded = true;
                    all_expanded = false;

                    auto const& parent_node{it->node.value()};
                    auto const& graph_parent_node = get_graph_node(parent_node);
                    auto const parent_indent = it->indent;

                    if(bool const already_printed{printed_nodes.contains(graph_parent_node.index())};
                        it->cycle_end_detected || already_printed)
                    {
                        ++it;
                        continue;
                    }

                    printed_nodes.insert(graph_parent_node.index());
                    it->parents.insert(graph_parent_node.index());

                    auto const to_reference = it->to_reference;
                    auto const& copy_parents = it->parents;

                    ++it;

                    if(!to_reference && !graph_parent_node.from_references().empty())
                    {
                        it = display_nodes.insert(it, {std::nullopt, false, false, false, parent_indent, std::nullopt, std::nullopt, {}, true, std::format(L"From References: ({})", graph_parent_node.from_references().size())});
                        ++it;

                        for(auto const& reference : graph_parent_node.from_references())
                        {
                            auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
                            it = display_nodes.insert(it, {child_node, false, false, false, parent_indent+2, reference.offset(), reference.pointer(), {}, true, L""s});
                            ++it;
                        }
                    }

                    if(!graph_parent_node.to_references().empty())
                    {
                        if(!to_reference)
                        {
                            it = display_nodes.insert(it, {std::nullopt, false, false, false, parent_indent, std::nullopt, std::nullopt, {}, true, std::format(L"To References: ({})", graph_parent_node.to_references().size())});
                            ++it;
                        }

                        auto const print_max_size = to_reference ? options.display_heap_graph_to_reference_limit() : graph_parent_node.to_references().size();

                        for(auto const& reference : graph_parent_node.to_references() | std::views::take(print_max_size))
                        {
                            auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
                            auto const& child_node_type = get_graph_node(child_node);
                            it = display_nodes.insert(it, {child_node, copy_parents.contains(child_node_type.index()), true, printed_nodes.contains(child_node_type.index()) && !child_node_type.to_references().empty(), parent_indent+2, reference.offset(), reference.pointer(), copy_parents, false, L""s});
                            ++it;
                        }

                        if(graph_parent_node.to_references().size() > print_max_size)
                        {
                            it = display_nodes.insert(it, {std::nullopt, false, false, false, parent_indent+3, std::nullopt, std::nullopt, {}, true, std::format(L"(limited to first {0} of {1})", options.display_heap_graph_to_reference_limit(), graph_parent_node.to_references().size())});
                            ++it;
                        }
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
        while(all_expanded == false);
    }

    void generate_display_node_to_print(dump_file_options const& options, heap::process_heap_graph_entry_type const& node, nodes_index_map const& nodes_index, size_t indent, std::set<uint64_t>& printed_nodes, std::list<display_node>& display_nodes)
    {
        display_nodes.emplace_back(node, false, false, false, indent, std::nullopt, std::nullopt);
        expand_all_child_nodes(options, nodes_index, printed_nodes, display_nodes);
    }

    auto get_sort_key(heap::process_heap_graph_entry_type const& entry)
    {
        return std::make_tuple(entry.index(), !is_root_node(entry), node_start_address(entry));
    }
}

void dump_mini_dump_heap_graph(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps const heaps{mini_dump, cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options()};
    heap::process_heap_graph graph{heaps};
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
    std::list<display_node> display_nodes;
    size_t index = 0;
    std::wstring go_back_line;
    auto constexpr print_update_gap = std::chrono::seconds{5};
    auto last_start_node_generate = std::chrono::system_clock::now() - print_update_gap - print_update_gap;

    for(auto const& node : nodes)
    {
        if(auto const start_node_generate = std::chrono::system_clock::now();
            start_node_generate - last_start_node_generate >= print_update_gap)
        {
            auto const progress_line = std::format(L"Generating Display Node {0} of {1} - {2}%", index, nodes.size(), static_cast<int>(std::round(100.0 / static_cast<double>(nodes.size()) * static_cast<double>(index+1))));
            std::wcerr << progress_line;
            if(go_back_line.size() != progress_line.size())
            {
                go_back_line = std::wstring(progress_line.size(), L'\b');
            }
            std::wcerr << go_back_line;
            last_start_node_generate = start_node_generate;
        }
        ++index;
        generate_display_node_to_print(options, node, nodes_index, 2, printed_nodes, display_nodes);
    }

    std::wcerr << std::wstring(go_back_line.size(), L' ') << go_back_line;

    end = std::chrono::system_clock::now();
    std::wcerr << std::format(L"Generate Display for Memory Allocation Graph Took ({0:%T})\n", end - start);
    log << L"Memory Allocation Graph:\n";
    for(auto const& display_node : display_nodes)
    {
        print_display_node(log, display_node, hex_length, heaps.peb().walker());
    }
}
