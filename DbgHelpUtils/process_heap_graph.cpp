#include "process_heap_graph.h"

#include "process_heaps.h"
#include "process_heap_entry.h"
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

            void operator()(process_heap_graph_heap_entry& entry);

        private:
            template<typename NodeType>
            void process_entry_reference(NodeType& entry, PointerType data, size_t offset, size_t& jump_amount);

        private:
            std::map<uint64_t, size_t> const& heap_entries_;
            std::vector<process_heap_graph_entry_type>& nodes_;
        };

        template <typename PointerType>
        void process_heap_graph_generator<PointerType>::operator()(process_heap_graph_heap_entry& entry)
        {
            auto user_data = entry.heap_entry().all_user_data();
            [[maybe_unused]] auto const found = user_data.find_pattern<PointerType>([this, &entry](PointerType const data, size_t const offset, size_t& jump_amount) mutable { process_entry_reference(entry, data, offset, jump_amount); return true; }, [](size_t) { return false; });
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

            if(auto& node = std::get<process_heap_graph_heap_entry>(nodes_[it->second]);
                node.index() != entry.index() && node.heap_entry().contains_address_range(data, size_units::base_16::bytes{1}))
            {
                entry.add_to_reference(process_heap_entry_reference{offset, data, node});
                node.add_from_reference(process_heap_entry_reference{offset, data, entry});
                jump_amount += sizeof(PointerType);
            }
        }


        template<typename PointerType>
        void generate_graph_nodes(std::map<uint64_t, size_t> const& heap_entries, std::vector<process_heap_graph_entry_type> & nodes)
        {
            process_heap_graph_generator<PointerType> generator{heap_entries, nodes};
            for(auto& entry : nodes)
            {
                std::visit(generator, entry);
            }
        }
    }

    process_heap_graph::process_heap_graph(heap::process_heaps const& process)
        : process_{&process}
    {
    }

    void process_heap_graph::generate_graph()
    {
        std::map<uint64_t, size_t> heap_entries;
        for (auto const& entry : process_->entries())
        {
            nodes_.emplace_back(entry);
            heap_entries.insert(std::make_pair(entry.user_address() + entry.user_requested_size().count() - 1, nodes_.size() - 1));
        }

        if(process_->peb().is_x86_target())
        {
            generate_graph_nodes<uint32_t>(heap_entries, nodes_);
        }
        else
        {
            generate_graph_nodes<uint64_t>(heap_entries, nodes_);
        }
    }
}
