set(PROJECT_NAME MiniDumper)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "system_modules.json"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "common_symbol_lookup_utils.h"
    "dump_file_options.h"
    "dump_mini_dump.h"
    "dump_mini_dump_app_info.h"
    "dump_mini_dump_comment.h"
    "dump_mini_dump_exception.h"
    "dump_mini_dump_heap.h"
    "dump_mini_dump_info.h"
    "dump_mini_dump_memory.h"
    "dump_mini_dump_module.h"
    "dump_mini_dump_streams.h"
    "dump_mini_dump_symbols.h"
    "dump_mini_dump_thread.h"
    "resource.h"
    "symbol_engine_ui.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "common_symbol_lookup_utils.cpp"
    "dump_file_options.cpp"
    "dump_mini_dump.cpp"
    "dump_mini_dump_app_info.cpp"
    "dump_mini_dump_comment.cpp"
    "dump_mini_dump_exception.cpp"
    "dump_mini_dump_heap.cpp"
    "dump_mini_dump_info.cpp"
    "dump_mini_dump_memory.cpp"
    "dump_mini_dump_module.cpp"
    "dump_mini_dump_streams.cpp"
    "dump_mini_dump_symbols.cpp"
    "dump_mini_dump_thread.cpp"
    "MiniDumper.cpp"
    "symbol_engine_ui.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE MiniDumper)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/x86/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/x86/$<CONFIG>/"
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/lyra/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
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
    "_CONSOLE;"
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
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    DbgHelpUtils
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    DbgHelpUtils
)

