#include "Pch.h"
#include "DumpFile.h"

#include "DumpFileHeader.h"
#include "DumpFileStream.h"
#include "../DbgHelpUtils/wide_runtime_error.h"

namespace MiniDumpExplorerApp
{
    DumpFile::DumpFile(winrt::hstring path, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
        : dump_file_{std::wstring{path}}
        , symbol_engine_{symbol_engine}
    {
        try
        {
            dump_file_.open_mini_dump();
        }
        catch ([[maybe_unused]] dlg_help_utils::exceptions::wide_runtime_error const& e)
        {
            return;
        }

        if (dump_file_.header() == nullptr)
        {
            return;
        }

        switch (dump_file_.type())
        {
        case dlg_help_utils::dump_file_type::user_mode_dump:
            type_ = winrt::MiniDumpExplorer::DumpFileType::UserModeDump;
            LoadStreams();
            break;

        case dlg_help_utils::dump_file_type::kernel_mode_dump_x86:
            type_ = winrt::MiniDumpExplorer::DumpFileType::KernelModeDumpX86;
            break;

        case dlg_help_utils::dump_file_type::kernel_mode_dump_x64:
            type_ = winrt::MiniDumpExplorer::DumpFileType::KernelModeDumpX64;
            break;

        case dlg_help_utils::dump_file_type::invalid:
            type_ = winrt::MiniDumpExplorer::DumpFileType::Invalid;
            break;
        }
    }

    bool DumpFile::HasHeader() const
    {
        return dump_file_.header() != nullptr;
    }

    winrt::MiniDumpExplorer::IDumpFileHeader DumpFile::Header()
    {
        auto const header = winrt::make<DumpFileHeader>(dump_file_, type_);
        auto rv = header.as<winrt::MiniDumpExplorer::IDumpFileHeader>();
        return rv;
    }

    winrt::Windows::Foundation::Collections::IVector<winrt::MiniDumpExplorer::IDumpFileStream> DumpFile::Streams() const
    {
        return streams_;
    }

    void DumpFile::LoadStreams() const
    {
        auto const* header_data = dump_file_.header();
        if (header_data == nullptr)
        {
            return;
        }

        auto const* directory = dump_file_.directory();
        if (directory == nullptr)
        {
            return;
        }

        for (size_t index = 0; index < header_data->NumberOfStreams; ++index)
        {
            auto const& entry = directory[index];
            const auto stream = winrt::make_self<MiniDumpExplorerApp::DumpFileStream>(index, entry);
            streams_.Append(*stream);
        }
    }
}

