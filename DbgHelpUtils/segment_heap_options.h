#pragma once
#include <cstdint>

namespace dlg_help_utils::heap
{
    class segment_heap_options
    {
    public:
        segment_heap_options(uint16_t front_padding_windows10_min_version, uint16_t front_padding_windows10_max_version);

        [[nodiscard]] uint16_t front_padding_windows10_min_version() const { return front_padding_windows10_min_version_; }
        [[nodiscard]] uint16_t front_padding_windows10_max_version() const { return front_padding_windows10_max_version_; }

    private:
        uint16_t front_padding_windows10_min_version_;
        uint16_t front_padding_windows10_max_version_;
    };
}
