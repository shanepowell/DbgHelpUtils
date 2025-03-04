#pragma once
#include <string>
#include <vector>

namespace dlg_help_utils::context_utils
{
    namespace resources
    {
        std::wstring get_x86_thread_context_flags_to_string(uint32_t context_flags);
        std::wstring get_x64_thread_context_flags_to_string(uint32_t context_flags);
        std::wstring get_wow64_thread_context_flags_to_string(uint32_t context_flags);
        std::wstring get_mx_csr_register_to_string(uint32_t mx_csr);
        std::wstring get_flags_register_to_string(uint64_t flags);
        std::wstring get_npx_status_word_to_string(uint16_t status_word);
        std::wstring get_npx_control_word_to_string(uint16_t control_word);
        std::wstring get_npx_tag_word_to_string(uint16_t tag_word);
        std::vector<std::wstring> get_x86_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_x64_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_wow64_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_mx_csr_register(uint32_t mx_csr);
        std::vector<std::wstring> get_flags_register(uint64_t flags);
        std::vector<std::wstring> get_npx_status_word(uint16_t status_word);
        std::vector<std::wstring> get_npx_control_word(uint16_t control_word);
        std::vector<std::wstring> get_npx_tag_word(uint16_t tag_word);
    }
}
