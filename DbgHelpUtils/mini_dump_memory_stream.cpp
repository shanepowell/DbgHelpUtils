#include "mini_dump_memory_stream.h"

namespace dlg_help_utils
{
    mini_dump_memory_stream::mini_dump_memory_stream(std::function<void const*(uint64_t base_address, uint64_t& size, enable_module_loading_t enable_module_loading)> get_process_memory_range
        , uint64_t const base_address
        , uint64_t size
        , enable_module_loading_t const enable_module_loading)
    : get_process_memory_range_{std::move(get_process_memory_range)}
    , enable_module_loading_{enable_module_loading}
    , current_address_{base_address}
    , end_address_{base_address + size}
    , memory_{static_cast<uint8_t const*>(get_process_memory_range_(current_address_, size, enable_module_loading_))}
    , end_memory_{memory_ + size}
    {
    }

    mini_dump_memory_stream::mini_dump_memory_stream(void const* memory, uint64_t const size)
    : end_address_{size}
    , memory_{static_cast<uint8_t const*>(memory)}
    , end_memory_{memory_ + size}
    {
    }

    bool mini_dump_memory_stream::eof() const
    {
        return memory_ == nullptr || current_address_ == end_address_;
    }

    size_t mini_dump_memory_stream::read(void* buffer, size_t const length)
    {
        return process_data(length, [destination = static_cast<uint8_t*>(buffer)](uint8_t const* memory, size_t const amount) mutable
            {
                memcpy(destination, memory, amount);
                destination += amount;
                return true;
            });
    }

    size_t mini_dump_memory_stream::skip(size_t const length)
    {
        return process_data(length, []([[maybe_unused]] uint8_t const* memory, [[maybe_unused]] size_t const amount)
            {
                return true;
            });
    }

    generator<std::pair<void const*, size_t>> mini_dump_memory_stream::ranges()
    {
        while(!eof())
        {
            auto const buffer = static_cast<void const*>(memory_);
            auto const length = static_cast<size_t>(end_memory_ - memory_);
            current_address_ += length;
            memory_ += length;

            co_yield std::make_pair(buffer, length);

            if(!eof() && memory_ == end_memory_)
            {
                uint64_t size = end_address_ - current_address_;
                memory_ = static_cast<uint8_t const*>(get_process_memory_range_(current_address_, size, enable_module_loading_));
                end_memory_ = memory_ + size;
            }
        }
    }
}
