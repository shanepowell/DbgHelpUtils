#include "dump_file_options.h"

#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/assert_value.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/exit_scope.h"
#include "DbgHelpUtils/heap_node_type.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_graph.h"
#include "DbgHelpUtils/process_heap_graph_heap_entry.h"
#include "DbgHelpUtils/register_names.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#include <algorithm>
#include <chrono>
#include <ranges>


using namespace std;
using namespace dlg_help_utils;

namespace
{
    std::wstring get_base_heap_entry_details(optional<heap::allocation_graph::process_heap_graph_heap_entry> const& base_heap_entry, std::streamsize const hex_length)
    {
        if(!base_heap_entry.has_value())
        {
            return {};
        }

        auto const& heap_entry = base_heap_entry.value().heap_entry();

        // ReSharper disable once StringLiteralTypo
        return std::format(L" BaseHeapEntryAddr({0})", stream_hex_dump::to_hex(heap_entry.user_address(), hex_length));
    }

    std::wstring get_node_specific_data(heap::allocation_graph::process_heap_graph_global_variable_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        using namespace size_units::base_16;
        auto const& entry = node.variable();

        auto const* module = walker.module_list().find_module(entry.symbol_type().module_base());
        auto module_name = module != nullptr ? module->name() : L"<unknown>"sv;
        auto export_name = entry.symbol_type().export_name().empty() ? L""s : std::format(L"({})", entry.symbol_type().export_name());
        auto const& base_heap_entry = node.base_heap_entry();

        // ReSharper disable once StringLiteralTypo
        return std::format(L"[Name({0}!{1}{2}) Index({3}) Addr({4}) Size({5}){6}]"
            , module_name
            , entry.symbol_type().name().value_or(L"<unknown>"sv)
            , export_name
            , entry.symbol_type().sym_index()
            , stream_hex_dump::to_hex(entry.symbol_type().address().value_or(0), hex_length)
            , to_wstring(bytes{entry.symbol_type().length().value_or(0)})
            , get_base_heap_entry_details(base_heap_entry, hex_length));
    }

    std::wstring get_node_specific_data(heap::allocation_graph::process_heap_graph_heap_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        std::wostringstream ss;

        using namespace size_units::base_16;
        auto const& entry = node.heap_entry();

        // ReSharper disable once StringLiteralTypo
        ss << std::format(L"[UsrAddr({0}) Size({1})", stream_hex_dump::to_hex(entry.user_address(), hex_length), to_wstring(entry.user_requested_size()));
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

    std::wstring get_node_specific_data(heap::allocation_graph::process_heap_graph_thread_stack_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        using namespace size_units::base_16;

        std::wostringstream ss;
        ss << std::format(L"[ThreadId({})", stream_hex_dump::to_hex(node.thread_id()));
        if(node.stack_stream().current_address() > 0)
        {
            ss << std::format(L" Stack({})", stream_hex_dump::to_hex(node.stack_stream().current_address(), hex_length));
        }

        ss << std::format(L" StackSize({})", to_wstring(bytes{node.stack_stream().length()}));
        if(!node.thread_name().empty())
        {
            ss << std::format(L" ThreadName({})", node.thread_name());
        }

        ss << "]";
        return std::move(ss).str();
    }

    std::wstring get_node_specific_data(heap::allocation_graph::process_heap_graph_thread_context_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const&)
    {
        using namespace size_units::base_16;

        std::wostringstream ss;
        ss << std::format(L"[ThreadId({0}) Register({1}) Data({2})]"
            , stream_hex_dump::to_hex(node.thread_id())
            , register_names::get_register_name(node.register_type())
            , stream_hex_dump::to_hex(node.register_data(), hex_length));
        if(!node.thread_name().empty())
        {
            ss << std::format(L" ThreadName({})", node.thread_name());
        }

        ss << "]";
        return std::move(ss).str();
    }

    std::wstring get_node_specific_data(heap::allocation_graph::process_heap_graph_symbol_entry const& node, std::streamsize const hex_length, stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        using namespace size_units::base_16;

        std::wostringstream ss;
        auto const* module = walker.module_list().find_module(node.symbol_type().module_base());
        auto module_name = module != nullptr ? module->name() : L"<unknown>"sv;
        auto export_name = node.symbol_type().export_name().empty() ? L""s : std::format(L"({})", node.symbol_type().export_name());

        // ReSharper disable once StringLiteralTypo
        ss << std::format(L"[Name({0}!{1}{2}) Index({3}) Addr({4})"
            , module_name, node.symbol_type().name().value_or(L"<unknown>"sv)
            , export_name
            , node.symbol_type().sym_index()
            , stream_hex_dump::to_hex(node.symbol_type().address().value_or(0), hex_length)
            , to_wstring(bytes{node.symbol_type().length().value_or(0)}));
        if(node.thread_id().has_value())
        {
            ss << std::format(L" ThreadId({})", stream_hex_dump::to_hex(node.thread_id().value()));
            if(!node.thread_name().empty())
            {
                ss << std::format(L" ThreadName({})", node.thread_name());
            }
        }

        ss << "]";
        return std::move(ss).str();
    }

    std::wstring get_symbol_reference_description(stream_stack_dump::mini_dump_memory_walker const& walker, heap::allocation_graph::process_heap_entry_symbol_address_reference const& symbol_reference, dump_file_options const& options)
    {
        std::wostringstream ss;
        auto const* module = walker.module_list().find_module(symbol_reference.symbol_type().module_base());
        auto module_name = module != nullptr ? module->name() : L"<unknown>"sv;
        auto export_name = symbol_reference.symbol_type().export_name().empty() ? L""s : std::format(L"({})", symbol_reference.symbol_type().export_name());

        // ReSharper disable once StringLiteralTypo
        ss << std::format(L" [Name({0}!{1}{2}) Index({3}) Size({4})"
            , module_name
            , symbol_reference.symbol_type().name().value_or(L"<unknown>"sv)
            , export_name
            , symbol_reference.symbol_type().sym_index()
            , to_wstring(size_units::base_16::bytes{symbol_reference.symbol_type().length().value_or(0)}));
        if(symbol_reference.thread_id().has_value())
        {
            ss << std::format(L" ThreadId({})", stream_hex_dump::to_hex(symbol_reference.thread_id().value()));
        }

        if(!symbol_reference.thread_name().empty())
        {
            ss << std::format(L" ThreadName({})", symbol_reference.thread_name());
        }

        ss << "]";

        if(options.debug_type_data())
        {
            if(symbol_reference.is_root_symbol())
            {
                ss << " [root]";
            }

            if(symbol_reference.is_metadata_symbol())
            {
                ss << " [metadata]";
            }
        }

        return std::move(ss).str();
    }

    using is_self_t = tagged_bool<struct is_self_type>;

    std::wstring get_node_attributes(heap::allocation_graph::process_heap_graph_node const& node
        , is_self_t const is_self
        , std::optional<uint64_t> const& parent_offset
        , std::optional<uint64_t> const& pointer
        , std::optional<heap::allocation_graph::graph_node_variable_symbol_reference_data> const& variable_symbol_info
        , std::streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , dump_file_options const& options)
    {
        std::wostringstream ss;
        if(parent_offset.has_value())
        {
            ss << std::format(L" +{}", stream_hex_dump::to_hex(parent_offset.value(), 4));
        }

        if(pointer.has_value())
        {
            ss << std::format(L" ({})", stream_hex_dump::to_hex(pointer.value(), hex_length));
        }

        if(is_self)
        {
            ss << L" [self]";
        }

        if(variable_symbol_info.has_value())
        {
            ss << std::format(L" {}", variable_symbol_info.value().name);
        }

        if(node.is_system_allocation() || node.is_system())
        {
            if(node.can_contain_user_pointers())
            {
                ss << L" [system-with-user-pointers]";
            }
            else
            {
                ss << L" [system]";
            }
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

        if(node.symbol_references().size() == 1 && node.symbol_references().front().is_root_symbol())
        {
            ss << get_symbol_reference_description(walker, node.symbol_references().front(), options);
        }

        return std::move(ss).str();
    }

    using nodes_index_map = std::unordered_map<uint64_t, heap::allocation_graph::process_heap_graph_entry_type const*>;

    template<typename T>
    std::wstring_view get_type_name()
    {
        if constexpr (std::is_same_v<T, heap::allocation_graph::process_heap_graph_heap_entry>)
        {
            return L"Allocation"sv;
        }
        else if constexpr (std::is_same_v<T, heap::allocation_graph::process_heap_graph_global_variable_entry>)
        {
            return L"GlobalVaraible"sv;
        }
        else if constexpr (std::is_same_v<T, heap::allocation_graph::process_heap_graph_thread_stack_entry>)
        {
            return L"ThreadStack"sv;
        }
        else if constexpr (std::is_same_v<T, heap::allocation_graph::process_heap_graph_thread_context_entry>)
        {
            return L"ThreadContext"sv;
        }
        else if constexpr (std::is_same_v<T, heap::allocation_graph::process_heap_graph_symbol_entry>)
        {
            return L"ThreadSymbol"sv;
        }
        else
        {
            static_assert(assert_value<false, T>::value, "unknown type");
            return L"Unknown"sv;
        }
    }

    using to_reference_t = tagged_bool<struct to_reference_type>;
    using already_logged_children_t = tagged_bool<struct already_logged_children_type>;
    using cycle_end_detected_t = tagged_bool<struct cycle_end_detected_type>;

    template <typename T>
    void print_node_line(std::wostream& log
        , T const& node
        , size_t indent
        , std::optional<uint64_t> parent_offset
        , std::optional<uint64_t> pointer
        , std::optional<heap::allocation_graph::graph_node_variable_symbol_reference_data> const& variable_symbol_info
        , cycle_end_detected_t const cycle_end_detected
        , std::streamsize const hex_length
        , to_reference_t const to_reference
        , is_self_t const is_self
        , already_logged_children_t const already_logged_children
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , dump_file_options const& options)
    {
        log << std::format(L"{0:{1}}{2}{3}{4}{5}{6}\n", 
            ' ',
            indent,
            parent_offset.has_value() ? (to_reference ? L"->"sv : L"<-"sv) : get_type_name<T>(),
            get_node_attributes(node, is_self, parent_offset, pointer, variable_symbol_info, hex_length, walker, options),
            parent_offset.has_value() ? L" " : L" >>",
            get_node_specific_data(node, hex_length, walker),
            cycle_end_detected ? L" - cycle end"sv : (already_logged_children ? L" - already logged children"sv : L""sv));
    }

    heap::allocation_graph::process_heap_graph_entry_type const& get_node_from_index(nodes_index_map const& nodes_index, uint64_t const index)
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
        heap::allocation_graph::process_heap_graph_entry_type const* node;
        size_t print_offset;
        size_t print_max_size;
        size_t indent;
        std::set<uint64_t> parents;
    };

    void print_display_node(std::wostream& log
        , heap::allocation_graph::process_heap_graph_entry_type const& node
        , size_t const indent
        , to_reference_t const to_reference
        , is_self_t const is_self
        , std::optional<uint64_t> parent_offset
        , std::optional<uint64_t> pointer
        , std::optional<heap::allocation_graph::graph_node_variable_symbol_reference_data> const& variable_symbol_info
        , cycle_end_detected_t const cycle_end_detected
        , already_logged_children_t const already_logged_children
        , std::streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , dump_file_options const& options)
    {
        std::visit([&log, indent, to_reference, is_self, &parent_offset, &pointer, &variable_symbol_info, cycle_end_detected, already_logged_children, hex_length, &walker, &options](auto const& _) mutable
        {
            print_node_line(log, _, indent, parent_offset, pointer, variable_symbol_info, cycle_end_detected, hex_length, to_reference, is_self, already_logged_children, walker, options);
        }, node);
    }


    void continue_display_node(wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , streamsize hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , vector<node_display_state>& node_stack
        , node_display_state state
        , size_t call_depth);

    void display_node(std::wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , heap::allocation_graph::process_heap_graph_entry_type const& node
        , size_t const indent
        , to_reference_t const to_reference
        , is_self_t const is_self
        , std::optional<uint64_t> const& parent_offset
        , std::optional<uint64_t> const& pointer
        , std::optional<heap::allocation_graph::graph_node_variable_symbol_reference_data> const& variable_symbol_info
        , cycle_end_detected_t const cycle_end_detected
        , already_logged_children_t const already_logged_children
        , std::set<uint64_t> const& parents
        , std::streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , std::vector<node_display_state>& node_stack
        , size_t const call_depth)
    {
        auto const& graph_node = get_graph_node(node);
        print_display_node(log, node, indent, to_reference, is_self, parent_offset, pointer, variable_symbol_info, cycle_end_detected, already_logged_children, hex_length, walker, options);

        if(cycle_end_detected || already_logged_children)
        {
            return;
        }

        if(!to_reference && !graph_node.symbol_references().empty())
        {
            auto const start = graph_node.start_address();
            log << std::format(L"{0:{1}}Symbols: ({2})\n", ' ', indent, graph_node.symbol_references().size());

            auto references{graph_node.symbol_references()};
            auto projection = [](heap::allocation_graph::process_heap_entry_symbol_address_reference const& value) { return value.address(); };

            std::ranges::sort(references, {}, projection);
            for(auto const& reference : references)
            {
                log << std::format(L"{0:{1}}", ' ', indent+2);
                if(reference.address() != 0)
                {
                    if(reference.address() >= start)
                    {
                        auto const offset = reference.address() - start;
                        log << std::format(L" +{}", stream_hex_dump::to_hex(offset, 4));
                    }
                    else
                    {
                        auto const offset = start - reference.address();
                        log << std::format(L" -{}", stream_hex_dump::to_hex(offset, 4));
                    }

                    if(options.debug_type_data())
                    {
                        log << std::format(L" [{}]", stream_hex_dump::to_hex(reference.address(), hex_length));
                    }
                }
                log << get_symbol_reference_description(walker, reference, options) << L'\n';
            }
        }

        if(!to_reference && !graph_node.from_references().empty())
        {
            log << std::format(L"{0:{1}}From References: ({2})\n", ' ', indent, graph_node.from_references().size());

            auto references = graph_node.from_references();
            auto projection = [](heap::allocation_graph::process_heap_entry_reference const& value) { return value.to_offset(); };
            std::ranges::sort(references, {}, projection);
            for(auto const& reference : references)
            {
                auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
                print_display_node(log
                    , child_node
                    , indent+2
                    , to_reference_t{false}
                    , is_self_t{graph_node.index() == child_node.index()}
                    , reference.from_offset()
                    , reference.from_pointer()
                    , reference.variable_symbol_info()
                    , cycle_end_detected_t{false}
                    , already_logged_children_t{false}
                    , hex_length
                    , walker
                    , options);
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

            if(call_depth > options.graph_display_max_call_depth())
            {
                node_stack.emplace_back(std::move(state));
            }
            else
            {
                continue_display_node(log, options, nodes_index, printed_nodes, hex_length, walker, node_stack, std::move(state), call_depth);
            }
        }
    }

    void continue_display_node(wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , vector<node_display_state>& node_stack
        , node_display_state state
        , size_t const call_depth)
    {
        auto const& graph_node = get_graph_node(*state.node);
        auto references = graph_node.to_references();
        auto projection = [](heap::allocation_graph::process_heap_entry_reference const& value) { return value.from_offset(); };
        std::ranges::sort(references, {}, projection);
        for(auto const& reference : references | std::views::take(state.print_max_size) | std::views::drop(state.print_offset))
        {
            auto const& child_node = get_node_from_index(nodes_index, reference.node_index());
            auto const& child_graph_node = get_graph_node(child_node);
            auto const child_cycle_end_detected = cycle_end_detected_t{state.parents.contains(child_graph_node.index())};
            auto const child_already_logged_children = already_logged_children_t{printed_nodes.contains(child_graph_node.index()) && !child_graph_node.to_references().empty()};

            printed_nodes.insert(child_graph_node.index());
            ++state.print_offset;

            auto const& stack_node = node_stack.emplace_back(std::move(state));
            auto const stack_size = node_stack.size();

            if(call_depth + 1 >= options.graph_display_max_call_depth())
            {
                return;
            }

            display_node(log
                , options
                , nodes_index
                , printed_nodes
                , child_node
                , stack_node.indent
                , to_reference_t{true}
                , is_self_t{graph_node.index() == child_node.index()}
                , reference.to_offset()
                , reference.to_pointer()
                , reference.variable_symbol_info()
                , child_cycle_end_detected
                , child_already_logged_children
                , stack_node.parents
                , hex_length
                , walker
                , node_stack
                , call_depth + 1
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

    void generate_display_node_and_print(std::wostream& log
        , dump_file_options const& options
        , nodes_index_map const& nodes_index
        , std::set<uint64_t>& printed_nodes
        , heap::allocation_graph::process_heap_graph_entry_type const& node
        , size_t const indent
        , std::streamsize const hex_length
        , stream_stack_dump::mini_dump_memory_walker const& walker)
    {
        std::vector<node_display_state> node_stack;

        display_node(log
            , options
            , nodes_index
            , printed_nodes
            , node
            , indent
            , to_reference_t{false}
            , is_self_t{false}
            , std::nullopt
            , std::nullopt
            , std::nullopt
            , cycle_end_detected_t{false}
            , already_logged_children_t{false}
            , {}
            , hex_length
            , walker
            , node_stack
            , 1);

        while(!node_stack.empty())
        {
            auto state = std::move(node_stack.back());
            node_stack.pop_back();
            continue_display_node(log, options, nodes_index, printed_nodes, hex_length, walker, node_stack, std::move(state), 1);
        }
    }

    auto get_sort_key(heap::allocation_graph::process_heap_graph_entry_type const& entry)
    {
        return std::make_tuple(entry.index(), !is_root_node(entry), node_start_address(entry));
    }

    [[nodiscard]] std::wstring to_wstring(heap::heap_node_type const value)
    {
        using namespace std::string_literals;
        using heap::heap_node_type;

        switch(value)
        {
            case heap_node_type::nt_heap_lfh_entry: return L"NT LFH Heap"s;
            case heap_node_type::nt_heap_segment_entry: return L"NT Heap Segment Entry"s;
            case heap_node_type::nt_heap_virtual_entry: return L"NT Heap Virtual Entry"s;
            case heap_node_type::segment_backend_entry: return L"Segment Heap Backend Entry"s;
            case heap_node_type::segment_entry: return L"Segment Heap Entry"s;
            case heap_node_type::segment_lfh_entry: return L"Segment Heap LFH Entry"s;
            case heap_node_type::segment_large_entry: return L"Segment Heap Large Entry"s;
            case heap_node_type::dph_entry: return L"DPH Entry"s;
            case heap_node_type::dph_virtual_entry: return L"DPH Virtual Entry"s;
            case heap_node_type::memory_range: return L"Memory Range"s;

        }

        return L"unknown"s;
    }

    void on_process_heap_match_crt_entry(std::wostream& log
        , dump_file_options const& options
        , heap::crt_entry const& crt_entry
        , uint64_t const user_address
        , size_units::base_16::bytes const user_size
        , heap::block_range_match_result const match
        , heap::heap_node_type const node_type
        , std::streamsize const hex_length)
    {
        if(!options.verbose_output())
        {
            return;
        }

        using stream_hex_dump::to_hex;

        switch(match)
        {
        case heap::block_range_match_result::block_match:
            if(options.verbose_output())
            {
                log << std::format(L"INFO: block [{0}-{1}({2})] ({3}) matches CRT Entry [{4}:{5}-{6}({7})]\n"
                    , to_hex(user_address, hex_length)
                    , to_hex(user_address + user_size.count(), hex_length)
                    , to_hex(user_size)
                    , to_wstring(node_type)
                    , to_hex(crt_entry.entry_address(), hex_length)
                    , locale_formatting::to_wstring(crt_entry.request_number())
                    , to_hex(crt_entry.end_entry_address(), hex_length)
                    , to_hex(crt_entry.entry_size()));
            }
            break;

        case heap::block_range_match_result::block_contains:
            log << std::format(L"WARNING: block [{0}-{1}({2})] ({3}) contains CRT Entry [{4}:{5}-{6}({7})]\n"
                , to_hex(user_address, hex_length)
                , to_hex(user_address + user_size.count(), hex_length)
                , to_hex(user_size)
                , to_wstring(node_type)
                , to_hex(crt_entry.entry_address(), hex_length)
                , locale_formatting::to_wstring(crt_entry.request_number())
                , to_hex(crt_entry.end_entry_address(), hex_length)
                , to_hex(crt_entry.entry_size()));
            break;

        case heap::block_range_match_result::user_contains_block:
            log << std::format(L"ERROR: CRT Entry [{4}:{5}-{6}({7})] contains block [{0}-{1}({2})] ({3})\n"
                , to_hex(user_address, hex_length)
                , to_hex(user_address + user_size.count(), hex_length)
                , to_hex(user_size)
                , to_wstring(node_type)
                , to_hex(crt_entry.entry_address(), hex_length)
                , locale_formatting::to_wstring(crt_entry.request_number())
                , to_hex(crt_entry.end_entry_address(), hex_length)
                , to_hex(crt_entry.entry_size()));
            break;

        case heap::block_range_match_result::block_partially_contains:
            log << std::format(L"ERROR: block [{0}-{1}({2})] ({3}) partially contains CRT Entry [{4}:{5}-{6}({7})]\n"
                , to_hex(user_address, hex_length)
                , to_hex(user_address + user_size.count(), hex_length)
                , to_hex(user_size)
                , to_wstring(node_type)
                , to_hex(crt_entry.entry_address(), hex_length)
                , locale_formatting::to_wstring(crt_entry.request_number())
                , to_hex(crt_entry.end_entry_address(), hex_length)
                , to_hex(crt_entry.entry_size()));
            break;

        case heap::block_range_match_result::block_no_match:
            break;
        }
    }
}

void dump_mini_dump_heap_graph(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dump_file_options& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    options.process_heaps_options().set_match_result_callback([&log, &options, hex_length](heap::crt_entry const& crt_entry
        , uint64_t const user_address
        , size_units::base_16::bytes const user_size
        , heap::block_range_match_result const match
        , heap::heap_node_type const node_type)
    {
        on_process_heap_match_crt_entry(log, options, crt_entry, user_address, user_size, match, node_type, hex_length);
    });
    auto const cleanup_process_heap_options = make_scope_exit([&options] () mutable { options.process_heaps_options().set_match_result_callback({}); });

    heap::process_heaps const heaps{mini_dump, cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options()};
    heap::process_heap_graph graph{mini_dump, heaps, options.system_module_list()};

    if(options.verbose_output())
    {
        std::wclog << std::format(L"Generate Memory Allocation Graph\n");
    }
    auto start = std::chrono::system_clock::now();
    graph.generate_graph();

    auto end = std::chrono::system_clock::now();
    if(options.verbose_output())
    {
        std::wclog << std::format(L"Generate Memory Allocation Graph Took ({0:%T})\n", end - start);
    }
    start = end;
    if(options.verbose_output())
    {
        std::wclog << L"Sort Memory Allocation Graph\n";
    }
    auto nodes = graph.nodes();
    std::ranges::sort(nodes, [](auto const& a, auto const& b)
        {
            return get_sort_key(a) < get_sort_key(b);
        });

    end = std::chrono::system_clock::now();
    if(options.verbose_output())
    {
        std::wclog << std::format(L"Sort Memory Allocation Graph Took ({0:%T})\n", end - start);
    }
    start = end;
    if(options.verbose_output())
    {
        std::wclog << L"Generate Display for Memory Allocation Graph\n";
    }
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
    if(options.verbose_output())
    {
        std::wclog << std::format(L"Generate Display for Memory Allocation Graph Took ({0:%T})\n", end - start);
    }
}