#pragma once
#include <cstdint>
#include <functional>

namespace dlg_help_utils
{
    class mini_dump_memory_stream
    {
    public:
        mini_dump_memory_stream(std::function<void const*(uint64_t base_address, uint64_t& size, bool enable_module_loading)> get_process_memory_range, uint64_t base_address, uint64_t size, bool enable_module_loading);

        [[nodiscard]] bool eof() const;
        [[nodiscard]] size_t read(void* buffer, size_t length);
        size_t skip(size_t length);
        [[nodiscard]] uint64_t current_address() const { return current_address_; }

    private:
        std::function<void const*(uint64_t base_address, uint64_t& size, bool enable_module_loading)> get_process_memory_range_;
        bool const enable_module_loading_;
        uint64_t current_address_;
        uint64_t const end_address_{};
        uint8_t const* memory_{nullptr};
        uint8_t const* end_memory_{nullptr};
    };
}
