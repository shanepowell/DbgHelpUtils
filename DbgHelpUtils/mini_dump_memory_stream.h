#pragma once
#include <cstdint>
#include <functional>
#include <string>

#include "generator.h"
#include "enable_module_loading.h"
#include "tagged_bool.h"

namespace dlg_help_utils
{
    using stop_at_null_t = tagged_bool<struct stop_at_null_type>;

    class mini_dump_memory_stream
    {
    public:
        mini_dump_memory_stream() = default;
        mini_dump_memory_stream(std::function<void const*(uint64_t base_address, uint64_t& size, enable_module_loading_t enable_module_loading)> get_process_memory_range
            , uint64_t base_address
            , uint64_t size
            , enable_module_loading_t enable_module_loading);
        mini_dump_memory_stream(void const* memory, uint64_t size);

        [[nodiscard]] bool eof() const;
        [[nodiscard]] size_t read(void* buffer, size_t length);
        size_t skip(size_t length);
        [[nodiscard]] uint64_t current_address() const { return current_address_; }
        [[nodiscard]] uint64_t length() const { return end_address_ - current_address_; }

        [[nodiscard]] generator<std::pair<void const*,size_t>> ranges();

        [[nodiscard]] void const* start_memory_raw() const { return memory_; }
        [[nodiscard]] void const* end_memory_raw() const { return end_memory_; }

        template<typename T>
        [[nodiscard]] bool find_pattern(std::function<bool(T, size_t, size_t&)> const& check_data, std::function<bool(size_t)> const& is_found)
        {
            auto reset_stream = *this;
            size_t index = 0;
            while(!eof() && !is_found(index))
            {
                T check;
                auto const before_stream = *this;
                if(read(&check, sizeof(T)) != sizeof(T))
                {
                    break;
                }

                size_t jump_amount{std::numeric_limits<size_t>::max()};
                auto const keep_processing_index = check_data(check, index, jump_amount);

                if(jump_amount == std::numeric_limits<size_t>::max())
                {
                    jump_amount = keep_processing_index ? sizeof(T) : 1;
                }

                // increment by the jump amount…
                if(jump_amount != sizeof(T))
                {
                    *this = before_stream;
                    skip(jump_amount);
                }

                if(keep_processing_index)
                {
                    if(index == 0)
                    {
                        reset_stream = *this;
                    }
                    index++;
                    if(is_found(index))
                    {
                        return true;
                    }
                }
                else
                {
                    if(index > 0)
                    {
                        *this = reset_stream;
                    }
                    index = 0;
                }
            }

            return is_found(index);
        }


        template<typename T>
        [[nodiscard]] std::basic_string_view<T, std::char_traits<T>> read_string_view(uint64_t const max_size, stop_at_null_t const stop_at_null)
        {
            if (eof())
            {
                return {};
            }

            T const* start = reinterpret_cast<T const*>(memory_);
            auto const length = process_data(sizeof(T), [max_size, stop_at_null](uint8_t const* memory, size_t const amount)
            {
                T const* current = reinterpret_cast<T const*>(memory);
                return  (stop_at_null ? *current != 0 : true) && amount < max_size;
            });

            if (length < max_size)
            {
                // move past the null terminator
                skip(sizeof(T));
            }

            return { start, length / sizeof(T) };
        }

    private:
        template<typename T>
        [[nodiscard]] size_t process_data(size_t length, T op)
        {
            if(eof()) return 0;

            size_t read_length{0};
            while(length > 0 && !eof())
            {
                auto const copy_length = std::min(length, static_cast<size_t>(end_memory_ - memory_));
                auto const continue_op = op(memory_, copy_length);

                length -= copy_length;
                current_address_ += copy_length;
                memory_ += copy_length;
                read_length += copy_length;

                if (!continue_op)
                {
                    break;
                }

                if(!eof() && memory_ == end_memory_)
                {
                    uint64_t size = end_address_ - current_address_;
                    memory_ = static_cast<uint8_t const*>(get_process_memory_range_(current_address_, size, enable_module_loading_));
                    end_memory_ = memory_ + size;
                }
            }

            return read_length;
        }

    private:
        std::function<void const*(uint64_t base_address, uint64_t& size, enable_module_loading_t enable_module_loading)> get_process_memory_range_;
        enable_module_loading_t enable_module_loading_{false};
        uint64_t current_address_{};
        uint64_t end_address_{};
        uint8_t const* memory_{nullptr};
        uint8_t const* end_memory_{nullptr};
    };
}
