#include "windows_error.h"

#include <stdexcept>
#include "handles.h"

using namespace std;

namespace dlg_help_utils::windows_error
{
    using namespace handles;

    std::wstring get_windows_error_string(DWORD const ec, std::wstring const& dll_source_path)
    {
        auto const source_module = load_library(dll_source_path);
        LPVOID buffer{nullptr};
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | (
                source_module ? FORMAT_MESSAGE_FROM_HMODULE : 0),
            source_module.get(),
            ec,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer),
            0,
            nullptr);
        if (!buffer)
        {
            return {};
        }

        auto buffer_handle = make_local_memory_handle(buffer);
        return wstring{static_cast<LPWSTR>(buffer)};
    }
}
