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
        std::vector<std::wstring> get_x86_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_x64_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_wow64_thread_context_flags(uint32_t context_flags);
        std::vector<std::wstring> get_mx_csr_register(uint32_t mx_csr);
    }
}
