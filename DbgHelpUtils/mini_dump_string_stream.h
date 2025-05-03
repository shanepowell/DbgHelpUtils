#pragma once
#include "mini_dump_memory_stream.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils
{
    template<typename T>
    class mini_dump_string_stream
    {
    public:
        mini_dump_string_stream(mini_dump_memory_stream& stream, uint64_t const max_size, stop_at_null_t const stop_at_null)
            : stream_{stream}
            , size_{find_string_size(stream, max_size, stop_at_null)}
        {
            stream_ = stream_.sub_range(0, size_ * sizeof(T));
        }

        auto size() const { return size_; }

        auto begin() const { return iterator{ stream_ }; }
        auto end() const { return iterator{ {} }; }

        struct iterator
        {
            mini_dump_memory_stream stream_;
            iterator(mini_dump_memory_stream stream) : stream_(std::move(stream)) {}
            bool operator!=(iterator const& rhs) {return stream_.start_memory_raw() != rhs.stream_.start_memory_raw() && stream_.eof() != rhs.stream_.eof();}
            T operator*()
            {
                auto stream = stream_;
                T ch;
                if(stream.read(&ch, sizeof(T)) != sizeof(T))
                {
                    throw exceptions::wide_runtime_error{L"Failed to read character from stream."};
                }
                return ch;
            }
            void operator++()
            {
                stream_.skip(sizeof(T));
            }
        };

    private:
        static size_t find_string_size(mini_dump_memory_stream& stream, uint64_t const max_size, stop_at_null_t const stop_at_null)
        {
            size_t size = 0;
            for(uint64_t i = 0; i < max_size; ++i)
            {
                T ch;
                if(stream.read(&ch, sizeof(T)) != sizeof(T))
                {
                    break;
                }

                if(stop_at_null && ch == 0)
                {
                    break;
                }

                ++size;
            }

            return size;
        }

    private:
        mini_dump_memory_stream stream_;
        size_t size_;
    };
}
