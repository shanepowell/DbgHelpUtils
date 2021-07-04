#include "pch.h"
#include "MiniDumpModule.h"

#include "../DbgHelpUtils/hex_dump.h"
#include "../DbgHelpUtils/stream_module.h"
#include "VsFixedFileInfo.h"
#include "CvInfoPdb70.h"

namespace MiniDumpExplorerApp
{
    MiniDumpModule::MiniDumpModule(dlg_help_utils::stream_module const& stream_module)
        : stream_module_{stream_module}
        , name_{stream_module_.name()}
        , vs_fixed_file_info_{*winrt::make_self<VsFixedFileInfo>(stream_module_->VersionInfo)}
        , cv_info_pdb70_{*winrt::make_self<CvInfoPdb70>(stream_module_.pdb_info())}
    {
        if(stream_module_->CvRecord.DataSize > 0)
        {
            std::wstringstream ss;
            dlg_help_utils::hex_dump::hex_dump(ss, stream_module_.cv_record(), stream_module_->CvRecord.DataSize, 0);
            cv_record_hex_dump_ = std::move(ss).str();
        }
        if(stream_module_->MiscRecord.DataSize > 0)
        {
            std::wstringstream ss;
            dlg_help_utils::hex_dump::hex_dump(ss, stream_module_.misc_record(), stream_module_->MiscRecord.DataSize, 0);
            misc_record_hex_dump_ = std::move(ss).str();
        }
    }

    winrt::hstring MiniDumpModule::Name() const
    {
        return name_;
    }

    uint64_t MiniDumpModule::BaseOfImage() const
    {
        return stream_module_->BaseOfImage;
    }

    uint32_t MiniDumpModule::SizeOfImage() const
    {
        return stream_module_->SizeOfImage;
    }

    uint32_t MiniDumpModule::CheckSum() const
    {
        return stream_module_->CheckSum;
    }

    winrt::clock::time_point MiniDumpModule::TimeDateStamp() const
    {
        return winrt::clock::from_time_t(stream_module_->TimeDateStamp);
    }

    winrt::MiniDumpExplorer::IVsFixedFileInfo MiniDumpModule::VersionInfo() const
    {
        return vs_fixed_file_info_;
    }

    uint32_t MiniDumpModule::CvRecordSize() const
    {
        return stream_module_->CvRecord.DataSize;
    }

    bool MiniDumpModule::HasCvRecordHexDump() const
    {
        return stream_module_->CvRecord.DataSize > 0 && !stream_module_.pdb_info().is_valid();
    }

    winrt::hstring MiniDumpModule::CvRecordHexDump() const
    {
        return cv_record_hex_dump_;
    }

    winrt::MiniDumpExplorer::ICvInfoPdb70 MiniDumpModule::PdbInfo() const
    {
        return cv_info_pdb70_;
    }

    uint32_t MiniDumpModule::MiscRecordSize() const
    {
        return stream_module_->MiscRecord.DataSize;
    }

    bool MiniDumpModule::HasMiscRecordHexDump() const
    {
        return stream_module_->MiscRecord.DataSize > 0;
    }

    winrt::hstring MiniDumpModule::MiscRecordHexDump() const
    {
        return misc_record_hex_dump_;
    }

    uint64_t MiniDumpModule::Reserved0() const
    {
        return stream_module_->Reserved0;
    }

    uint64_t MiniDumpModule::Reserved1() const
    {
        return stream_module_->Reserved1;
    }
}
