#include "windows_error.h"

#include <format>
#include <stdexcept>

#include "handles.h"
#include "stream_hex_dump.h"
#include "wide_runtime_error.h"

using namespace std;

namespace dlg_help_utils::windows_error
{
    using namespace handles;

    void throw_windows_api_error(std::wstring_view const& api, DWORD const ec)
    {
        throw exceptions::wide_runtime_error{std::format(L"{0} failed. Error: {1} - {2}", api, stream_hex_dump::to_hex(ec), get_windows_error_string(ec))};
    }

    void throw_windows_api_error(std::wstring_view const& api, std::wstring_view const& optional_data, DWORD const ec)
    {
        throw exceptions::wide_runtime_error{std::format(L"{0} failed ({1}). Error: {2} - {3}", api, optional_data, stream_hex_dump::to_hex(ec), get_windows_error_string(ec))};
    }

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
