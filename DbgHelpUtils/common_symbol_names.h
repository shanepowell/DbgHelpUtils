#pragma once
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

    // ReSharper disable once IdentifierTypo
    extern std::wstring const rtlp_lfh_key_global_symbol_name;

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
    extern std::wstring const heap_ucr_list_flink_field_symbol_name;
    extern std::wstring const heap_virtual_allocated_blocks_field_symbol_name;
    extern std::wstring const heap_virtual_allocated_blocks_flink_field_symbol_name;
    extern std::wstring const heap_free_lists_field_symbol_name;
    extern std::wstring const heap_free_lists_flink_field_symbol_name;
    extern std::wstring const heap_total_free_size_field_symbol_name;
    extern std::wstring const heap_segment_list_field_symbol_name;
    extern std::wstring const heap_segment_list_flink_field_symbol_name;
    extern std::wstring const heap_segment_list_entry_field_symbol_name;
    extern std::wstring const heap_segment_list_entry_flink_field_symbol_name;
    extern std::wstring const heap_allocator_back_trace_index_field_symbol_name;
    extern std::wstring const heap_front_end_heap_type_field_symbol_name;
    extern std::wstring const heap_front_end_heap_field_symbol_name;
    extern std::wstring const list_entry_flink_field_symbol_name;
    extern std::wstring const heap_entry_small_tag_index_field_symbol_name;
    extern std::wstring const heap_entry_size_field_symbol_name;
    extern std::wstring const heap_entry_flags_field_symbol_name;
    extern std::wstring const heap_entry_segment_offset_field_symbol_name;
    extern std::wstring const heap_entry_unused_bytes_field_symbol_name;
    extern std::wstring const heap_entry_previous_size_field_symbol_name;
    extern std::wstring const heap_segment_segment_flags_field_symbol_name;
    extern std::wstring const heap_segment_number_of_pages_field_symbol_name;
    extern std::wstring const heap_segment_base_address_field_symbol_name;
    extern std::wstring const heap_segment_number_of_un_committed_pages_field_symbol_name;
    extern std::wstring const heap_segment_number_of_un_committed_ranges_field_symbol_name;
    extern std::wstring const heap_segment_segment_allocator_back_trace_index_field_symbol_name;
    extern std::wstring const heap_segment_ucr_segment_list_field_symbol_name;
    extern std::wstring const heap_segment_ucr_segment_list_flink_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_address_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_size_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_segment_entry_field_symbol_name;
    extern std::wstring const heap_ucr_descriptor_segment_entry_flink_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_reserve_size_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_commit_size_field_symbol_name;
    extern std::wstring const heap_virtual_alloc_entry_busy_block_field_symbol_name;
    extern std::wstring const heap_free_entry_heap_entry_field_symbol_name;
    extern std::wstring const heap_free_entry_free_list_field_symbol_name;
    extern std::wstring const lfh_heap_sub_segment_zones_field_symbol_name;
    extern std::wstring const lfh_heap_sub_segment_zones_flink_field_symbol_name;
    extern std::wstring const lfh_block_zone_next_index_field_symbol_name;
    extern std::wstring const lfh_block_zone_free_pointer_field_symbol_name;
    extern std::wstring const heap_subsegment_block_size_field_symbol_name;
    extern std::wstring const heap_subsegment_block_count_field_symbol_name;
    extern std::wstring const heap_subsegment_user_blocks_field_symbol_name;
    extern std::wstring const heap_user_data_encoded_offsets_field_symbol_name;
    extern std::wstring const heap_user_data_first_allocation_offset_field_symbol_name;
}
