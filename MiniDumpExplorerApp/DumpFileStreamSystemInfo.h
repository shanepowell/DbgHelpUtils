#pragma once
#include "../DbgHelpUtils/mini_dump.h"
#include "../DbgHelpUtils/system_info_stream.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamSystemInfo : winrt::implements<DumpFileStreamSystemInfo, winrt::MiniDumpExplorer::IDumpFileStreamSystemInfo>
    {
        DumpFileStreamSystemInfo(size_t index, dlg_help_utils::mini_dump const& mini_dump);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpSystemInfo SystemInfo() const;
        [[nodiscard]] winrt::hstring CsdVersion() const;
        [[nodiscard]] winrt::hstring VendorId() const;
        [[nodiscard]] bool IsIntel() const;
        [[nodiscard]] bool IsAmd() const;
        [[nodiscard]] bool IsX86() const;
        [[nodiscard]] bool IsX64() const;
        [[nodiscard]] uint16_t ProcessorModel() const;
        [[nodiscard]] uint16_t ProcessorStepping() const;

    private:
        dlg_help_utils::system_info_stream system_info_;
        winrt::MiniDumpExplorer::IMiniDumpSystemInfo info_;
        winrt::hstring csd_version_;
        winrt::hstring vendor_id_;
    };
}

