#include "stream_function_entry.h"

namespace dlg_help_utils
{
    stream_function_entry::stream_function_entry(size_t const index, void const* function_entry,
                                                 uint32_t const size_of_function_entry,
                                                 function_table_entry_type const entry_type)
        : index_{index}
          , size_of_function_entry_{size_of_function_entry}
          , function_entry_{function_entry}
          , entry_type_{entry_type}
          , fpo_data_{
              entry_type == function_table_entry_type::fpo_data ? static_cast<FPO_DATA const*>(function_entry) : nullptr
          }
          , image_function_entry_{
              entry_type == function_table_entry_type::image_function_entry
                  ? static_cast<IMAGE_FUNCTION_ENTRY const*>(function_entry)
                  : nullptr
          }
          , image_function_entry_64_{entry_type == function_table_entry_type::image_function_entry_64 ? static_cast<IMAGE_FUNCTION_ENTRY64 const*>(function_entry) : nullptr}
    {
    }
}
