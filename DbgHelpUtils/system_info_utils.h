#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace dlg_help_utils
{
    enum class dump_file_type;
}

namespace dlg_help_utils::system_info_utils
{
    std::wstring_view mini_dump_type_to_string(dump_file_type type);
    std::wstring processor_architecture_to_string(unsigned short processor_architecture);
    std::wstring product_type_to_string(unsigned char product_type);
    std::wstring platform_id_to_string(uint32_t platform_id);
    std::wstring_view windows_version_to_string(uint32_t major_version, uint32_t minor_version, uint32_t build_number,
                                                unsigned char product_type, unsigned short processor_architecture,
                                                unsigned short suite_mask);
    std::vector<std::wstring_view> suite_mask_to_strings(unsigned short suite_mask);
    std::wstring get_product_version(std::wstring const& filename);
    std::wstring_view process_integrity_level_to_string(uint32_t process_integrity_level);
    std::wstring vm_counters_2_flags_to_string(uint16_t flags);
    std::wstring_view handle_trace_operation_to_string(uint32_t type);
    std::wstring_view handle_object_information_type_to_string(uint32_t type);
    std::wstring version_info_to_string(uint32_t version_ms, uint32_t version_ls);

    std::filesystem::path const& get_running_application_path();
    std::filesystem::path const& get_run_from_directory();
    std::filesystem::path const& get_temp_directory();
}
