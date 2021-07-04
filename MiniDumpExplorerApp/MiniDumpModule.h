#pragma once

namespace dlg_help_utils
{
    class stream_module;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpModule : winrt::implements<MiniDumpModule, winrt::MiniDumpExplorer::IMiniDumpModule>
    {
        MiniDumpModule(dlg_help_utils::stream_module const& stream_module);

        [[nodiscard]] winrt::hstring Name() const;
        [[nodiscard]] uint64_t BaseOfImage() const;
        [[nodiscard]] uint32_t SizeOfImage() const;
        [[nodiscard]] uint32_t CheckSum() const;
        [[nodiscard]] winrt::clock::time_point TimeDateStamp() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IVsFixedFileInfo VersionInfo() const;
        [[nodiscard]] uint32_t CvRecordSize() const;
        [[nodiscard]] bool HasCvRecordHexDump() const;
        [[nodiscard]] winrt::hstring CvRecordHexDump() const;
        [[nodiscard]] winrt::MiniDumpExplorer::ICvInfoPdb70 PdbInfo() const;
        [[nodiscard]] uint32_t MiscRecordSize() const;
        [[nodiscard]] bool HasMiscRecordHexDump() const;
        [[nodiscard]] winrt::hstring MiscRecordHexDump() const;
        [[nodiscard]] uint64_t Reserved0() const;
        [[nodiscard]] uint64_t Reserved1() const;

    private:
        dlg_help_utils::stream_module const& stream_module_;
        winrt::hstring name_;
        winrt::hstring cv_record_hex_dump_;
        winrt::hstring misc_record_hex_dump_;
        winrt::MiniDumpExplorer::IVsFixedFileInfo vs_fixed_file_info_;
        winrt::MiniDumpExplorer::ICvInfoPdb70 cv_info_pdb70_;
    };
}
