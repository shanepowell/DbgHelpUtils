#include "segment_heap_options.h"

namespace dlg_help_utils::heap
{
    segment_heap_options::segment_heap_options(uint16_t const front_padding_windows10_min_version, uint16_t const front_padding_windows10_max_version)
        : front_padding_windows10_min_version_{front_padding_windows10_min_version}
        , front_padding_windows10_max_version_{front_padding_windows10_max_version}
    {
    }
}
