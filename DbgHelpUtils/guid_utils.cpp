#include "guid_utils.h"

#include <rpc.h>
#include <sstream>

#include "wide_runtime_error.h"
#include "windows_error.h"

#pragma comment(lib, "rpcrt4.lib")

namespace dlg_help_utils::guid_utils
{
    std::wstring to_string(GUID const& guid)
    {
        RPC_WSTR rpc_string;
        auto const status = UuidToStringW(const_cast<UUID*>(&guid), &rpc_string);
        if (status != RPC_S_OK)
        {
            auto const ec = GetLastError();

            std::wostringstream oss;
            oss << "UuidToStringW failed: " << ec << L" - " << windows_error::get_windows_error_string(ec);
            throw exceptions::wide_runtime_error{std::move(oss).str()};
        }

        std::wstring rv(reinterpret_cast<wchar_t*>(rpc_string));
        RpcStringFreeW(&rpc_string);
        return rv;
    }
}
