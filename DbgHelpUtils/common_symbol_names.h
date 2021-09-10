﻿#pragma once
#include <string>

namespace dlg_help_utils::common_symbol_names
{
    extern std::wstring const peb_structure_symbol_name;
    extern std::wstring const peb_ldr_structure_symbol_name;
    extern std::wstring const rtl_user_process_parameters_structure_symbol_name;
    extern std::wstring const teb_structure_symbol_name;
    extern std::wstring const heap_structure_symbol_name;
    extern std::wstring const heap_segment_structure_symbol_name;
    extern std::wstring const heap_entry_structure_symbol_name;
    extern std::wstring const heap_free_entry_structure_symbol_name;
    extern std::wstring const heap_ucr_descriptor_structure_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_structure_symbol_name;
    extern std::wstring const list_entry_structure_symbol_name;
    extern std::wstring const lfh_heap_structure_symbol_name;
    extern std::wstring const lfh_block_zone_structure_symbol_name;
    extern std::wstring const heap_subsegment_structure_symbol_name;
    extern std::wstring const heap_user_data_header_structure_symbol_name;
    extern std::wstring const heap_user_data_offsets_structure_symbol_name;
    extern std::wstring const dph_heap_root_structure_symbol_name;
    extern std::wstring const rtl_balanced_links_structure_symbol_name;
    extern std::wstring const dph_heap_block_structure_symbol_name;
    extern std::wstring const rtl_stack_trace_entry_structure_symbol_name;
    extern std::wstring const dph_block_information_structure_symbol_name;
    extern std::wstring const segment_heap_structure_symbol_name;
    extern std::wstring const heap_seg_context_structure_symbol_name;
    extern std::wstring const heap_page_segment_structure_symbol_name;
    extern std::wstring const heap_page_range_descriptor_structure_symbol_name;
    extern std::wstring const rtl_rb_tree_structure_symbol_name;
    extern std::wstring const rtl_balanced_node_structure_symbol_name;
    extern std::wstring const heap_vs_context_structure_symbol_name;
    extern std::wstring const heap_vs_subsegment_structure_symbol_name;
    extern std::wstring const heap_vs_chunk_header_structure_symbol_name;
    extern std::wstring const heap_vs_chunk_free_header_structure_symbol_name;
    extern std::wstring const heap_large_alloc_structure_symbol_name;
    extern std::wstring const heap_lfh_context_structure_symbol_name;
    extern std::wstring const heap_lfh_bucket_structure_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_structure_symbol_name;
    extern std::wstring const heap_lfh_subsegment_structure_symbol_name;
    extern std::wstring const heap_lfh_subsegment_encoded_offsets_structure_symbol_name;
    extern std::wstring const heap_lfh_ondemand_pointer_structure_symbol_name;
    extern std::wstring const crt_mem_block_header_structure_symbol_name;
    extern std::wstring const stack_trace_database_structure_symbol_name;

    extern std::wstring const rtl_p_lfh_key_global_symbol_name;
    extern std::wstring const av_rfp_dph_page_heap_list_global_symbol_name;
    extern std::wstring const rtl_p_heap_key_global_symbol_name;
    extern std::wstring const rtl_p_hp_heap_globals_global_symbol_name;
    extern std::wstring const rtl_stack_trace_database;
    extern std::wstring const acrt_first_block;

    extern std::wstring const peb_structure_ldr_field_symbol_name;
    extern std::wstring const peb_structure_process_parameters_field_symbol_name;
    extern std::wstring const peb_structure_nt_global_flag_field_symbol_name;
    extern std::wstring const peb_structure_process_heap_field_symbol_name;
    extern std::wstring const peb_structure_number_of_heaps_field_symbol_name;
    extern std::wstring const peb_structure_process_heaps_field_symbol_name;
    extern std::wstring const rtl_user_process_parameters_structure_environment_field_symbol_name;
    extern std::wstring const teb_structure_process_environment_block_field_symbol_name;
    extern std::wstring const heap_segment_signature_field_symbol_name;
    extern std::wstring const heap_flags_field_symbol_name;
    extern std::wstring const heap_segment_first_entry_field_symbol_name;
    extern std::wstring const heap_segment_last_entry_field_symbol_name;
    extern std::wstring const heap_total_memory_reserved_field_symbol_name;
    extern std::wstring const heap_total_memory_committed_field_symbol_name;
    extern std::wstring const heap_total_memory_large_ucr_field_symbol_name;
    extern std::wstring const heap_total_size_in_virtual_blocks_field_symbol_name;
    extern std::wstring const heap_total_segments_field_symbol_name;
    extern std::wstring const heap_total_ucrs_field_symbol_name;
    extern std::wstring const heap_encode_flag_mask_field_symbol_name;
    extern std::wstring const heap_encoding_field_symbol_name;
    extern std::wstring const heap_ucr_list_field_symbol_name;
    extern std::wstring const heap_virtual_allocated_blocks_field_symbol_name;
    extern std::wstring const heap_free_lists_field_symbol_name;
    extern std::wstring const heap_total_free_size_field_symbol_name;
    extern std::wstring const heap_segment_list_field_symbol_name;
    extern std::wstring const heap_segment_list_entry_field_symbol_name;
    extern std::wstring const heap_allocator_back_trace_index_field_symbol_name;
    extern std::wstring const heap_front_end_heap_type_field_symbol_name;
    extern std::wstring const heap_front_end_heap_field_symbol_name;
    extern std::wstring const list_entry_flink_field_symbol_name;
    extern std::wstring const heap_entry_small_tag_index_field_symbol_name;
    extern std::wstring const heap_entry_size_field_symbol_name;
    extern std::wstring const heap_entry_flags_field_symbol_name;
    extern std::wstring const heap_entry_segment_offset_field_symbol_name;
    extern std::wstring const heap_entry_unused_bytes_field_symbol_name;
    extern std::wstring const heap_entry_unused_bytes_length_field_symbol_name;
    extern std::wstring const heap_entry_previous_size_field_symbol_name;
    extern std::wstring const heap_segment_segment_flags_field_symbol_name;
    extern std::wstring const heap_segment_number_of_pages_field_symbol_name;
    extern std::wstring const heap_segment_base_address_field_symbol_name;
    extern std::wstring const heap_segment_number_of_un_committed_pages_field_symbol_name;
    extern std::wstring const heap_segment_number_of_un_committed_ranges_field_symbol_name;
    extern std::wstring const heap_segment_segment_allocator_back_trace_index_field_symbol_name;
    extern std::wstring const heap_segment_ucr_segment_list_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_list_entry_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_address_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_size_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_segment_entry_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_entry_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_reserve_size_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_commit_size_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_busy_block_field_symbol_name;
    extern std::wstring const heap_free_entry_heap_entry_field_symbol_name;
    extern std::wstring const heap_free_entry_free_list_field_symbol_name;
    extern std::wstring const lfh_heap_sub_segment_zones_field_symbol_name;
    extern std::wstring const lfh_block_zone_list_entry_field_symbol_name;
    extern std::wstring const lfh_block_zone_next_index_field_symbol_name;
    extern std::wstring const lfh_block_zone_free_pointer_field_symbol_name;
    extern std::wstring const heap_subsegment_block_size_field_symbol_name;
    extern std::wstring const heap_subsegment_block_count_field_symbol_name;
    extern std::wstring const heap_subsegment_user_blocks_field_symbol_name;
    extern std::wstring const heap_user_data_encoded_offsets_field_symbol_name;
    extern std::wstring const heap_user_data_first_allocation_offset_field_symbol_name;
    extern std::wstring const dph_heap_root_next_heap_field_symbol_name;
    extern std::wstring const dph_heap_root_flags_field_symbol_name;
    extern std::wstring const dph_heap_root_extra_flags_field_symbol_name;
    extern std::wstring const dph_heap_root_seed_field_symbol_name;
    extern std::wstring const dph_heap_root_normal_heap_field_symbol_name;
    extern std::wstring const dph_heap_root_busy_nodes_table_field_symbol_name;
    extern std::wstring const dph_heap_root_virtual_storage_list_head_field_symbol_name;
    extern std::wstring const dph_heap_root_virtual_storage_list_tail_field_symbol_name;
    extern std::wstring const dph_heap_root_virtual_storage_ranges_field_symbol_name;
    extern std::wstring const dph_heap_root_virtual_storage_bytes_field_symbol_name;
    extern std::wstring const dph_heap_root_busy_allocations_field_symbol_name;
    extern std::wstring const dph_heap_root_busy_allocations_bytes_committed_field_symbol_name;
    extern std::wstring const dph_heap_root_free_allocations_list_head_field_symbol_name;
    extern std::wstring const dph_heap_root_free_allocations_list_tail_field_symbol_name;
    extern std::wstring const dph_heap_root_free_allocations_field_symbol_name;
    extern std::wstring const dph_heap_root_free_allocations_bytes_committed_field_symbol_name;
    extern std::wstring const rtl_balanced_links_left_child_field_symbol_name;
    extern std::wstring const rtl_balanced_links_right_child_field_symbol_name;
    extern std::wstring const dph_heap_block_virtual_block_field_symbol_name;
    extern std::wstring const dph_heap_block_virtual_block_size_field_symbol_name;
    extern std::wstring const dph_heap_block_user_allocation_field_symbol_name;
    extern std::wstring const dph_heap_block_user_requested_size_field_symbol_name;
    extern std::wstring const dph_heap_block_stack_trace_field_symbol_name;
    extern std::wstring const dph_heap_block_next_alloc_field_symbol_name;
    extern std::wstring const rtl_stack_trace_entry_index_field_symbol_name;
    extern std::wstring const rtl_stack_trace_entry_depth_field_symbol_name;
    extern std::wstring const rtl_stack_trace_entry_back_trace_field_symbol_name;
    extern std::wstring const dph_block_information_start_stamp_field_symbol_name;
    extern std::wstring const segment_heap_signature_field_symbol_name;
    extern std::wstring const segment_heap_global_flags_field_symbol_name;
    extern std::wstring const segment_heap_mem_stats_total_reserved_pages_field_symbol_name;
    extern std::wstring const segment_heap_mem_stats_total_committed_pages_field_symbol_name;
    extern std::wstring const segment_heap_mem_stats_free_committed_pages_field_symbol_name;
    extern std::wstring const segment_heap_mem_stats_lfh_free_committed_pages_field_symbol_name;
    extern std::wstring const segment_heap_allocated_base_field_symbol_name;
    extern std::wstring const segment_heap_uncommitted_base_field_symbol_name;
    extern std::wstring const segment_heap_reserved_limit_field_symbol_name;
    extern std::wstring const segment_heap_large_alloc_metadata_field_symbol_name;
    extern std::wstring const segment_heap_large_reserved_pages_field_symbol_name;
    extern std::wstring const segment_heap_large_committed_pages_field_symbol_name;
    extern std::wstring const segment_heap_segment_contexts_field_symbol_name;
    extern std::wstring const segment_heap_vs_context_field_symbol_name;
    extern std::wstring const segment_heap_lfh_context_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_max_allocation_size_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_segment_count_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_segment_mask_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_unit_shift_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_pages_per_unit_shift_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_segment_list_head_field_symbol_name;
    extern std::wstring const segment_heap_seg_context_free_page_ranges_field_symbol_name;
    extern std::wstring const heap_page_segment_list_entry_field_symbol_name;
    extern std::wstring const heap_page_segment_signature_field_symbol_name;
    extern std::wstring const heap_page_segment_desc_array_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_tree_node_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_unused_bytes_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_extra_present_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_range_flags_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_range_flags_bits_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_committed_page_count_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_unit_offset_field_symbol_name;
    extern std::wstring const heap_page_range_descriptor_unit_size_field_symbol_name;
    extern std::wstring const rtl_rb_tree_root_field_symbol_name;
    extern std::wstring const rtl_balanced_node_left_field_symbol_name;
    extern std::wstring const rtl_balanced_node_right_field_symbol_name;
    extern std::wstring const heap_vs_context_free_chunk_tree_field_symbol_name;
    extern std::wstring const heap_vs_context_subsegment_list_field_symbol_name;
    extern std::wstring const heap_vs_context_total_committed_units_field_symbol_name;
    extern std::wstring const heap_vs_context_free_committed_units_field_symbol_name;
    extern std::wstring const heap_vs_subsegment_list_entry_field_symbol_name;
    extern std::wstring const heap_vs_subsegment_size_field_symbol_name;
    extern std::wstring const heap_vs_subsegment_signature_field_symbol_name;
    extern std::wstring const heap_vs_subsegment_full_commit_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_sizes_memory_cost_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_sizes_unsafe_prev_size_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_sizes_unsafe_size_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_sizes_allocated_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_encoded_segment_page_offset_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_unused_bytes_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_skip_during_walk_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_spare_field_symbol_name;
    extern std::wstring const heap_vs_chunk_header_allocated_chunk_bits_field_symbol_name;
    extern std::wstring const heap_vs_chunk_free_header_node_field_symbol_name;
    extern std::wstring const heap_large_alloc_tree_node_field_symbol_name;
    extern std::wstring const heap_large_alloc_virtual_address_field_symbol_name;
    extern std::wstring const heap_large_alloc_unused_bytes_field_symbol_name;
    extern std::wstring const heap_large_alloc_extra_present_field_symbol_name;
    extern std::wstring const heap_large_alloc_guard_page_count_field_symbol_name;
    extern std::wstring const heap_large_alloc_guard_page_alignment_field_symbol_name;
    extern std::wstring const heap_large_alloc_spare_field_symbol_name;
    extern std::wstring const heap_large_alloc_allocated_pages_field_symbol_name;
    extern std::wstring const heap_lfh_context_max_affinity_field_symbol_name;
    extern std::wstring const heap_lfh_context_config_max_block_size_field_symbol_name;
    extern std::wstring const heap_lfh_context_config_with_old_page_crossing_blocks_field_symbol_name;
    extern std::wstring const heap_lfh_context_config_disable_randomization_field_symbol_name;
    extern std::wstring const heap_lfh_context_buckets_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_bucket_index_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_slot_count_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_available_subsegment_count_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_total_block_count_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_total_subsegment_count_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_reciprocal_block_size_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_shift_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_proc_affinity_mapping_field_symbol_name;
    extern std::wstring const heap_lfh_bucket_affinity_slots_field_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_bucket_index_field_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_slot_index_field_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_available_subsegment_count_field_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_available_subsegment_list_field_symbol_name;
    extern std::wstring const heap_lfh_affinity_slot_full_subsegment_list_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_list_entry_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_free_count_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_block_count_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_location_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_witheld_block_count_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_block_offsets_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_commit_unit_shift_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_commit_unit_count_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_block_bitmap_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_encoded_offsets_block_size_field_symbol_name;
    extern std::wstring const heap_lfh_subsegment_encoded_offsets_first_block_offset_field_symbol_name;
    extern std::wstring const heap_lfh_ondemand_pointer_usage_data_field_symbol_name;
    extern std::wstring const crt_mem_block_header_block_header_next_field_symbol_name;
    extern std::wstring const crt_mem_block_header_file_name_field_symbol_name;
    extern std::wstring const crt_mem_block_header_line_number_field_symbol_name;
    extern std::wstring const crt_mem_block_header_block_use_field_symbol_name;
    extern std::wstring const crt_mem_block_header_data_size_field_symbol_name;
    extern std::wstring const crt_mem_block_header_request_number_field_symbol_name;
    extern std::wstring const stack_trace_database_entry_index_array_field_symbol_name;
    extern std::wstring const stack_trace_database_number_of_entries_allocated_field_symbol_name;
}
