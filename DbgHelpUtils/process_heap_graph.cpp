#include "process_heap_graph.h"

#include "global_variable.h"
#include "global_variables.h"
#include "process_heaps.h"
#include "process_heap_entry.h"
#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"

namespace dlg_help_utils::heap
{
    namespace
    {
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

    process_heap_graph::process_heap_graph(process_heaps const& process)
        : process_{&process}
    {
    }


    void process_heap_graph::generate_global_variable_references()
    {
        for(process::global_variables const variables{process_->peb().walker()};
            auto const& global_variable : variables.all_variables())
        {
            nodes_.emplace_back(process_heap_graph_global_variable_entry{global_variable});
        }
    }

    std::map<uint64_t, size_t> process_heap_graph::generate_allocation_references()
    {
        std::map<uint64_t, size_t> heap_entries;
        for (auto const& entry : process_->entries())
        {
            nodes_.emplace_back(entry);
            heap_entries.insert(std::make_pair(entry.user_address() + entry.user_requested_size().count() - 1, nodes_.size() - 1));
        }

        return heap_entries;
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

    void process_heap_graph::generate_graph()
    {
        // generate all reference nodes
        generate_global_variable_references();
        auto const heap_entries = generate_allocation_references();

        // link all reference nodes
        generate_node_references(heap_entries);

        // remove any reference nodes that is not a allocation and has no to references
        remove_all_non_allocation_with_empty_to_references();
    }
}
