#include "mini_dump_type.h"

using namespace std::string_view_literals;

namespace dlg_help_utils::mini_dump_type
{
    std::vector<std::wstring_view> to_strings(MINIDUMP_TYPE const type)
    {
        std::vector<std::wstring_view> rv;

        if (type == MiniDumpNormal)
        {
            rv.emplace_back(L"MiniDumpNormal"sv);
        }
        else
        {
            if (type & MiniDumpWithDataSegs)
            {
                // ReSharper disable once StringLiteralTypo
                rv.emplace_back(L"MiniDumpWithDataSegs"sv);
            }

            if (type & MiniDumpWithFullMemory)
            {
                rv.emplace_back(L"MiniDumpWithFullMemory"sv);
            }

            if (type & MiniDumpWithHandleData)
            {
                rv.emplace_back(L"MiniDumpWithHandleData"sv);
            }

            if (type & MiniDumpFilterMemory)
            {
                rv.emplace_back(L"MiniDumpFilterMemory"sv);
            }

            if (type & MiniDumpScanMemory)
            {
                rv.emplace_back(L"MiniDumpScanMemory"sv);
            }

            if (type & MiniDumpWithUnloadedModules)
            {
                rv.emplace_back(L"MiniDumpWithUnloadedModules"sv);
            }

            if (type & MiniDumpWithIndirectlyReferencedMemory)
            {
                rv.emplace_back(L"MiniDumpWithIndirectlyReferencedMemory"sv);
            }

            if (type & MiniDumpFilterModulePaths)
            {
                rv.emplace_back(L"MiniDumpFilterModulePaths"sv);
            }

            if (type & MiniDumpWithProcessThreadData)
            {
                rv.emplace_back(L"MiniDumpWithProcessThreadData"sv);
            }

            if (type & MiniDumpWithPrivateReadWriteMemory)
            {
                rv.emplace_back(L"MiniDumpWithPrivateReadWriteMemory"sv);
            }

            if (type & MiniDumpWithoutOptionalData)
            {
                rv.emplace_back(L"MiniDumpWithoutOptionalData"sv);
            }

            if (type & MiniDumpWithFullMemoryInfo)
            {
                rv.emplace_back(L"MiniDumpWithFullMemoryInfo"sv);
            }

            if (type & MiniDumpWithThreadInfo)
            {
                rv.emplace_back(L"MiniDumpWithThreadInfo"sv);
            }

            if (type & MiniDumpWithCodeSegs)
            {
                // ReSharper disable once StringLiteralTypo
                rv.emplace_back(L"MiniDumpWithCodeSegs"sv);
            }

            if (type & MiniDumpWithoutAuxiliaryState)
            {
                rv.emplace_back(L"MiniDumpWithoutAuxiliaryState"sv);
            }

            if (type & MiniDumpWithFullAuxiliaryState)
            {
                rv.emplace_back(L"MiniDumpWithFullAuxiliaryState"sv);
            }

            if (type & MiniDumpWithPrivateWriteCopyMemory)
            {
                rv.emplace_back(L"MiniDumpWithPrivateWriteCopyMemory"sv);
            }

            if (type & MiniDumpIgnoreInaccessibleMemory)
            {
                rv.emplace_back(L"MiniDumpIgnoreInaccessibleMemory"sv);
            }

            if (type & MiniDumpWithTokenInformation)
            {
                rv.emplace_back(L"MiniDumpWithTokenInformation"sv);
            }

            if (type & MiniDumpWithModuleHeaders)
            {
                rv.emplace_back(L"MiniDumpWithModuleHeaders"sv);
            }

            if (type & MiniDumpFilterTriage)
            {
                rv.emplace_back(L"MiniDumpFilterTriage"sv);
            }

            if (type & MiniDumpWithAvxXStateContext)
            {
                rv.emplace_back(L"MiniDumpWithAvxXStateContext"sv);
            }

            if (type & MiniDumpWithIptTrace)
            {
                rv.emplace_back(L"MiniDumpWithIptTrace"sv);
            }

            if ((type & ~MiniDumpValidTypeFlags) != 0)
            {
                rv.emplace_back(L"Invalid Flags Enabled"sv);
            }
        }
        return rv;
    }
}
