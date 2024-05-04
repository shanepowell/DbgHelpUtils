#include "pch.h"
#include "UnsupportedStreamPage.xaml.h"

#include "MiniDumpPage.xaml.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "Utility/logger.h"

#if __has_include("UnsupportedStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "UnsupportedStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    void UnsupportedStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        auto const* header = miniDump.header();
        auto const* directory = miniDump.directory();
        index_ = parameters.StreamIndex();

        if(directory == nullptr || parameters.StreamIndex() >= header->NumberOfStreams)
        {
            logger::Log().LogMessage(log_level::error
                , std::format("failed to load mini dump stream data header:[{}], stream index:[{}], header NumberOfStreams:[{}]"
                    , dlg_help_utils::string_conversation::wstring_to_utf8(dlg_help_utils::stream_hex_dump::to_hex(header))
                    , parameters.StreamIndex()
                    , header->NumberOfStreams));
            return;
        }

        auto const& stream_entry = directory[parameters.StreamIndex()];

        std::wstringstream ss;
        dlg_help_utils::hex_dump::hex_dump(ss, miniDump.rva32(stream_entry.Location), stream_entry.Location.DataSize, 1);
        streamDataHexDump_ = std::move(ss).str();
        RaisePropertyChanged(L"HexDump");
    }
}
