#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace dlg_help_utils
{
    enum class dump_file_type : uint16_t;
}

namespace dlg_help_utils::system_info_utils
{
    std::wstring mini_dump_type_to_string(dump_file_type type);
    std::wstring processor_architecture_to_string(unsigned short processor_architecture);
    std::wstring product_type_to_string(unsigned char product_type);
    std::wstring platform_id_to_string(uint32_t platform_id);
    std::wstring windows_version_to_string(uint32_t major_version, uint32_t minor_version, uint32_t build_number,
                                                unsigned char product_type, unsigned short processor_architecture,
                                                unsigned short suite_mask);
    std::vector<std::wstring_view> suite_mask_to_strings(unsigned short suite_mask);
    std::wstring process_integrity_level_to_string(uint32_t process_integrity_level);
    std::wstring vm_counters_2_flags_to_string(uint16_t flags);
    std::vector<std::wstring_view> vm_counters_2_flags_to_strings(uint16_t flags);
    std::wstring handle_trace_operation_to_string(uint32_t type);
    std::wstring handle_object_information_type_to_string(uint32_t type);
    std::wstring version_info_to_string(uint32_t version);
    std::wstring version_info_to_string(uint32_t version_ms, uint32_t version_ls);
    std::vector<std::wstring_view> process_execute_flags_to_strings(uint32_t flags);
    std::wstring version_file_flags_to_string(uint32_t flags, uint32_t mask);
    std::vector<std::wstring_view> version_file_flags_to_strings(uint32_t flags, uint32_t mask);
    std::wstring version_file_os_to_string(uint32_t file_os);
    std::vector<std::wstring_view> version_file_os_to_strings(uint32_t file_os);
    std::wstring version_file_type_to_string(uint32_t file_type, uint32_t file_sub_type);
    std::wstring sym_type_to_string(uint32_t type);

    std::filesystem::path const& get_running_application_path();
    std::filesystem::path const& get_run_from_directory();
    std::filesystem::path const& get_temp_directory();
}
