#include "guid_utils.h"

#include <rpc.h>
#include <sstream>

#include "wide_runtime_error.h"
#include "windows_error.h"

#pragma comment(lib, "rpcrt4.lib")

using namespace std::string_view_literals;

namespace dlg_help_utils::guid_utils
{
    std::wstring to_string(GUID const& guid)
    {
        RPC_WSTR rpc_string;
        if (auto const status = UuidToStringW(&guid, &rpc_string); status != RPC_S_OK)
        {
            windows_error::throw_windows_api_error(L"UuidToStringW"sv);
        }

        std::wstring rv(reinterpret_cast<wchar_t*>(rpc_string));
        RpcStringFreeW(&rpc_string);
        return rv;
    }
}
