#include "string_stream.h"
#include "mini_dump.h"

namespace dlg_help_utils::string_stream
{
    std::wstring_view to_string(mini_dump const& dump, RVA const rva)
    {
        if (rva == 0) return {};
        auto const* name = static_cast<MINIDUMP_STRING const*>(dump.rva32(rva));
        return std::wstring_view{name->Buffer, name->Length / 2};
    }

    std::wstring_view to_string(mini_dump const& dump, RVA64 const rva)
    {
        if (rva == 0) return {};
        auto const* name = static_cast<MINIDUMP_STRING const*>(dump.rva64(rva));
        return std::wstring_view{name->Buffer, name->Length / 2};
    }
}
