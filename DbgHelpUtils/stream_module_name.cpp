#include "stream_module_name.h"

#include <filesystem>

namespace dlg_help_utils
{
    stream_module_name::stream_module_name(std::wstring_view const& name)
        : name_{name}
    {
        std::filesystem::path const path{name_};
        auto filename_with_extension_path = path.filename();
        filename_with_extension_ = filename_with_extension_path.wstring();
        filename_without_extension_ = filename_with_extension_path.replace_extension().wstring();
    }
}
