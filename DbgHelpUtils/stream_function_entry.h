#pragma once
#include "windows_setup.h"

#include <cstdint>

#include "function_table_entry_type.h"

namespace dlg_help_utils
{
    class stream_function_entry
    {
    public:
        stream_function_entry(size_t index, void const* function_entry, uint32_t size_of_function_entry,
                              function_table_entry_type entry_type);

        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] void const* data() const { return function_entry_; }
        [[nodiscard]] size_t size() const { return size_of_function_entry_; }
        [[nodiscard]] function_table_entry_type entry_type() const { return entry_type_; }
        [[nodiscard]] FPO_DATA const& fpo_data() const { return *fpo_data_; }
        [[nodiscard]] IMAGE_FUNCTION_ENTRY const& image_function_entry() const { return *image_function_entry_; }

        [[nodiscard]] IMAGE_FUNCTION_ENTRY64 const& image_function_entry_64() const
        {
            return *image_function_entry_64_;
        }

    private:
        size_t index_;
        uint32_t size_of_function_entry_{0};
        void const* function_entry_{nullptr};
        function_table_entry_type entry_type_;
        FPO_DATA const* fpo_data_{nullptr};
        IMAGE_FUNCTION_ENTRY const* image_function_entry_{nullptr};
        IMAGE_FUNCTION_ENTRY64 const* image_function_entry_64_{nullptr};
    };
}
