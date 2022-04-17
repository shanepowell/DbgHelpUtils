set(PROJECT_NAME DbgHelpUtils)

################################################################################
# Source groups
################################################################################
set(DbgHelp
    "basic_type.h"
    "call_convention.h"
    "data_kind.h"
    "i_stack_walk_callback.h"
    "i_symbol_load_callback.h"
    "local_variable.h"
    "sym_tag_enum.h"
    "symbol_address_info.h"
    "symbol_engine.cpp"
    "symbol_engine.h"
    "symbol_info_buffer.cpp"
    "symbol_info_buffer.h"
    "symbol_type_info.cpp"
    "symbol_type_info.h"
    "udt_kind_type.h"
)
source_group("DbgHelp" FILES ${DbgHelp})

set(Heap
    "ust_address_stack_trace.cpp"
    "ust_address_stack_trace.h"
    "heap_match_utils.cpp"
    "heap_match_utils.h"
)
source_group("Heap" FILES ${Heap})

set(Heap__Crt
    "crt_entry.cpp"
    "crt_entry.h"
    "crt_heap.cpp"
    "crt_heap.h"
)
source_group("Heap\\Crt" FILES ${Heap__Crt})

set(Heap__DebugHeap
    "dph_entry.cpp"
    "dph_entry.h"
    "dph_heap.cpp"
    "dph_heap.h"
)
source_group("Heap\\DebugHeap" FILES ${Heap__DebugHeap})

set(Heap__NtHeap
    "heap_entry.cpp"
    "heap_entry.h"
    "heap_segment.cpp"
    "heap_segment.h"
    "heap_subsegment.cpp"
    "heap_subsegment.h"
    "heap_ucr_descriptor.cpp"
    "heap_ucr_descriptor.h"
    "heap_virtual_block.cpp"
    "heap_virtual_block.h"
    "lfh_heap.cpp"
    "lfh_heap.h"
    "lfh_segment.cpp"
    "lfh_segment.h"
    "nt_heap.cpp"
    "nt_heap.h"
)
source_group("Heap\\NtHeap" FILES ${Heap__NtHeap})

set(Heap__ProcessHeaps
    "interface_process_heaps_statistic_view_impl.cpp"
    "interface_process_heaps_statistic_view_impl.h"
    "process_heap_entry.cpp"
    "process_heap_entry.h"
    "process_heaps.cpp"
    "process_heaps.h"
    "process_heaps_statistic_bucket_view.cpp"
    "process_heaps_statistic_bucket_view.h"
    "process_heaps_statistic_view.cpp"
    "process_heaps_statistic_view.h"
    "process_heaps_statistics.cpp"
    "process_heaps_statistics.h"
)
source_group("Heap\\ProcessHeaps" FILES ${Heap__ProcessHeaps})

set(Heap__ProcessHeaps__StatisticViews
    "allocation_stack_trace_helper.cpp"
    "allocation_stack_trace_helper.h"
    "by_application_callsite_frequency_view.cpp"
    "by_application_callsite_frequency_view.h"
    "by_size_frequency_view.cpp"
    "by_size_frequency_view.h"
    "by_size_ranges_frequency_view.cpp"
    "by_size_ranges_frequency_view.h"
    "by_stacktrace_frequency_view.cpp"
    "by_stacktrace_frequency_view.h"
    "statistic_view_options.h"
    "system_module_list.cpp"
    "system_module_list.h"
)
source_group("Heap\\ProcessHeaps\\StatisticViews" FILES ${Heap__ProcessHeaps__StatisticViews})

set(Heap__SegmentHeap
    "heap_lfh_affinity_slot.cpp"
    "heap_lfh_affinity_slot.h"
    "heap_lfh_bucket.cpp"
    "heap_lfh_bucket.h"
    "heap_lfh_context.cpp"
    "heap_lfh_context.h"
    "heap_lfh_entry.cpp"
    "heap_lfh_entry.h"
    "heap_lfh_subsegment.cpp"
    "heap_lfh_subsegment.h"
    "heap_page_segment.cpp"
    "heap_page_segment.h"
    "heap_segment_context.cpp"
    "heap_segment_context.h"
    "heap_vs_context.cpp"
    "heap_vs_context.h"
    "heap_vs_entry.cpp"
    "heap_vs_entry.h"
    "heap_vs_subsegment.cpp"
    "heap_vs_subsegment.h"
    "large_alloc_entry.cpp"
    "large_alloc_entry.h"
    "page_range_descriptor.cpp"
    "page_range_descriptor.h"
    "segment_heap.cpp"
    "segment_heap.h"
    "segment_heap_utils.cpp"
    "segment_heap_utils.h"
)
source_group("Heap\\SegmentHeap" FILES ${Heap__SegmentHeap})

set(MiniDump
    "mini_dump.cpp"
    "mini_dump.h"
    "mini_dump_stream_type.cpp"
    "mini_dump_stream_type.h"
    "mini_dump_type.cpp"
    "mini_dump_type.h"
)
source_group("MiniDump" FILES ${MiniDump})

set(NtdllUtilities
    "list_entry_walker.cpp"
    "list_entry_walker.h"
    "rtl_balanced_links_walker.cpp"
    "rtl_balanced_links_walker.h"
    "rtl_rb_tree_walker.cpp"
    "rtl_rb_tree_walker.h"
)
source_group("NtdllUtilities" FILES ${NtdllUtilities})

set(PeUtilities
    "pe_file.cpp"
    "pe_file.h"
    "pe_file_memory_mapping.cpp"
    "pe_file_memory_mapping.h"
)
source_group("PeUtilities" FILES ${PeUtilities})

set(Process
    "global_variable.cpp"
    "global_variable.h"
    "global_variables.cpp"
    "global_variables.h"
    "process_environment_block.cpp"
    "process_environment_block.h"
    "process_parameters.cpp"
    "process_parameters.h"
)
source_group("Process" FILES ${Process})

set(StreamUtilities
    "common_symbol_names.cpp"
    "common_symbol_names.h"
    "common_symbol_utils.cpp"
    "common_symbol_utils.h"
    "cv_info_pdb70.cpp"
    "cv_info_pdb70.h"
    "exception_utils.cpp"
    "exception_utils.h"
    "find_thread_stack.cpp"
    "find_thread_stack.h"
    "gflags_utils.cpp"
    "gflags_utils.h"
    "lfh_subsegment_location_utils.cpp"
    "lfh_subsegment_location_utils.h"
    "memory_info_utils.cpp"
    "memory_info_utils.h"
    "mini_dump_memory_stream.cpp"
    "mini_dump_memory_stream.h"
    "mini_dump_memory_walker.cpp"
    "mini_dump_memory_walker.h"
    "module_match.cpp"
    "module_match.h"
    "page_range_flags_utils.cpp"
    "page_range_flags_utils.h"
    "register_names.cpp"
    "register_names.h"
    "stream_stack_dump.cpp"
    "stream_stack_dump.h"
    "stream_utils.cpp"
    "stream_utils.h"
    "symbol_type_utils.cpp"
    "symbol_type_utils.h"
    "system_info_utils.cpp"
    "system_info_utils.h"
    "thread_info_utils.cpp"
    "thread_info_utils.h"
)
source_group("StreamUtilities" FILES ${StreamUtilities})

set(Streams
    "comment_stream_a.cpp"
    "comment_stream_a.h"
    "comment_stream_w.cpp"
    "comment_stream_w.h"
    "exception_stream.cpp"
    "exception_stream.h"
    "function_table_entry_type.h"
    "function_table_stream.cpp"
    "function_table_stream.h"
    "handle_data_stream.cpp"
    "handle_data_stream.h"
    "handle_operation_list_stream.cpp"
    "handle_operation_list_stream.h"
    "memory64_list_stream.cpp"
    "memory64_list_stream.h"
    "memory_info_list_stream.cpp"
    "memory_info_list_stream.h"
    "memory_list_stream.cpp"
    "memory_list_stream.h"
    "misc_info_stream.cpp"
    "misc_info_stream.h"
    "module_list_stream.cpp"
    "module_list_stream.h"
    "process_vm_counters_stream.cpp"
    "process_vm_counters_stream.h"
    "stream_function_descriptor.cpp"
    "stream_function_descriptor.h"
    "stream_function_entry.cpp"
    "stream_function_entry.h"
    "stream_handle.cpp"
    "stream_handle.h"
    "stream_handle_object_information.cpp"
    "stream_handle_object_information.h"
    "stream_module.cpp"
    "stream_module.h"
    "stream_module_name.cpp"
    "stream_module_name.h"
    "stream_thread.cpp"
    "stream_thread.h"
    "stream_thread_context.cpp"
    "stream_thread_context.h"
    "stream_thread_ex.cpp"
    "stream_thread_ex.h"
    "stream_thread_info.cpp"
    "stream_thread_info.h"
    "stream_thread_name.cpp"
    "stream_thread_name.h"
    "stream_token_info.cpp"
    "stream_token_info.h"
    "stream_unloaded_module.cpp"
    "stream_unloaded_module.h"
    "string_stream.cpp"
    "string_stream.h"
    "system_info_stream.cpp"
    "system_info_stream.h"
    "system_memory_info_stream.cpp"
    "system_memory_info_stream.h"
    "thread_ex_list_stream.cpp"
    "thread_ex_list_stream.h"
    "thread_info_list_stream.cpp"
    "thread_info_list_stream.h"
    "thread_list_stream.cpp"
    "thread_list_stream.h"
    "thread_names_list_stream.cpp"
    "thread_names_list_stream.h"
    "token_info_list_stream.cpp"
    "token_info_list_stream.h"
    "unloaded_module_list_stream.cpp"
    "unloaded_module_list_stream.h"
)
source_group("Streams" FILES ${Streams})

set(Utility
    "cache_manager.h"
    "chrono_unit_convert_to_string.h"
    "crc.h"
    "exit_scope.h"
    "filesystem_utils.cpp"
    "filesystem_utils.h"
    "flags_string_utils.h"
    "guid_utils.cpp"
    "guid_utils.h"
    "handles.h"
    "hash_combine.h"
    "hex_dump.cpp"
    "hex_dump.h"
    "join.h"
    "memory_range.h"
    "locale_number_formatting.cpp"
    "locale_number_formatting.h"
    "print_utils.h"
    "range_units.h"
    "size_units.cpp"
    "size_units.h"
    "stream_hex_dump.h"
    "string_compare.h"
    "string_conversation.cpp"
    "string_conversation.h"
    "string_utils.h"
    "system_time_utils.h"
    "time_units.cpp"
    "time_units.h"
    "time_utils.cpp"
    "time_utils.h"
    "vector_to_hash_set.h"
    "wide_runtime_error.h"
    "windows_error.cpp"
    "windows_error.h"
    "windows_setup.h"
)
source_group("Utility" FILES ${Utility})

set(ALL_FILES
    ${DbgHelp}
    ${Heap}
    ${Heap__Crt}
    ${Heap__DebugHeap}
    ${Heap__NtHeap}
    ${Heap__ProcessHeaps}
    ${Heap__ProcessHeaps__StatisticViews}
    ${Heap__SegmentHeap}
    ${MiniDump}
    ${NtdllUtilities}
    ${PeUtilities}
    ${Process}
    ${StreamUtilities}
    ${Streams}
    ${Utility}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE DbgHelpUtils)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/x86/${CMAKE_VS_PLATFORM_TOOLSET}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/x86/${CMAKE_VS_PLATFORM_TOOLSET}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/x64/${CMAKE_VS_PLATFORM_TOOLSET}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/x64/${CMAKE_VS_PLATFORM_TOOLSET}/$<CONFIG>/"
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${MSVC_DIA_SDK_DIR}/include"
)
################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /std:c++latest;
        /sdl;
        /W4;
        /WX;
        /MP;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /SUBSYSTEM:CONSOLE
    )
endif()

