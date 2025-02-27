#pragma once
#include "dph_heap.h"
#include "process_environment_block.h"
#include "size_units.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class statistic_view_options;
    }

    enum class heap_node_type : uint8_t;

    class system_module_list;
    class crt_entry;
    class heap_subsegment;
    class heap_entry;
    class process_heap_entry;
    class process_heaps_options;
    class process_heaps_statistics;

    class process_heaps
    {
    public:
        process_heaps(mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine, process_heaps_options const& options, system_module_list const& system_module_list, statistic_views::statistic_view_options const& statistic_view_options);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }
        [[nodiscard]] process_heaps_options const& options() const { return *options_; }
        [[nodiscard]] system_module_list const& system_module() const { return *system_module_list_; }
        [[nodiscard]] statistic_views::statistic_view_options const& statistic_view_options() const { return *statistic_view_options_; }

        void set_base_diff_filter(process_heaps& base_diff_filter);
        void clear_base_diff_filter();

        [[nodiscard]] dlg_help_utils::generator<process_heap_entry> entries() const;
        [[nodiscard]] dlg_help_utils::generator<process_heap_entry> free_entries() const;

        [[nodiscard]] bool is_address_filtered(uint64_t address, size_units::base_16::bytes size) const;
        [[nodiscard]] bool is_system_allocation(memory_range const& range) const;

        [[nodiscard]] process_heaps_statistics statistics() const;

        [[nodiscard]] static bool is_lfh_subsegment_in_entry(heap_entry const& entry, heap_subsegment const& subsegment);

    private:
        void clear_cache() const;
        [[nodiscard]] std::vector<heap_subsegment> get_all_nt_heap_lfh_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap) const;
        void get_all_nt_heap_segment_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap, std::vector<heap_subsegment> const& lfh_data) const;
        void get_all_nt_heap_virtual_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap) const;
        void get_all_segment_backend_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const;
        void get_all_segment_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const;
        void get_all_segment_lfh_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const;
        void get_all_segment_large_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const;
        void get_all_dph_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, dph_heap const& heap) const;
        void get_all_dph_virtual_entities(std::map<uint64_t, process_heap_entry> & all_entries, std::map<uint64_t, crt_entry> const& crt_entries, dph_heap const& heap) const;
        void get_all_virtual_alloc_entities(std::map<uint64_t, process_heap_entry>& all_entries) const;

        [[nodiscard]] std::map<uint64_t, process_heap_entry> all_entries() const;
        [[nodiscard]] dlg_help_utils::generator<process_heap_entry> all_free_entries() const;

        [[nodiscard]] dlg_help_utils::generator<process_heap_entry> filter_entries() const;
        [[nodiscard]] dlg_help_utils::generator<process_heap_entry> filter_free_entries() const;

        [[nodiscard]] std::tuple<crt_entry const*, bool> match_crt_entry(uint64_t user_address, size_units::base_16::bytes size, std::map<uint64_t, crt_entry> const& crt_entries, heap_node_type node_type) const;

        static void add_heap_entry(std::map<uint64_t, process_heap_entry> & entries, process_heap_entry process_heap_entry);
        [[nodiscard]] static bool does_entry_contain_entry(process_heap_entry const& container_heap_entry, process_heap_entry const& heap_entry);
        [[nodiscard]] static bool is_filtered(std::vector<process_heap_entry> const& filters, process_heap_entry const& entry);
        [[nodiscard]] static bool is_address_filtered(std::vector<process_heap_entry> const& filters, uint64_t address, size_units::base_16::bytes size);
        [[nodiscard]] static bool contains_address(uint64_t start_address, uint64_t size, uint64_t address);

    private:
        process_heaps_options const* options_;
        cache_manager* cache_manager_;
        process::process_environment_block peb_;
        system_module_list const* system_module_list_;
        statistic_views::statistic_view_options const* statistic_view_options_;
        process_heaps* base_diff_filter_{nullptr};
        mutable std::vector<process_heap_entry> entry_filters_cache_;
        mutable std::vector<process_heap_entry> free_entry_filters_cache_;
        mutable std::vector<process_heap_entry> entry_cache_;
        mutable std::vector<process_heap_entry> free_entry_cache_;
        mutable std::set<uint64_t> system_area_addresses_;
    };
}
