#include "pch.h"
#include "CommentStreamPage.xaml.h"

#include "DbgHelpUtils/comment_stream_a.h"
#include "DbgHelpUtils/comment_stream_w.h"

#if __has_include("CommentStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "CommentStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    CommentStreamPage::CommentStreamPage() = default;

    void CommentStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        if (dlg_help_utils::comment_stream_w const comment{miniDump, parameters.StreamIndex()};
            comment.found())
        {
            comment_ = comment.comment();
            RaisePropertyChanged(L"Comment");
            return;
        }

        if (dlg_help_utils::comment_stream_a const comment{miniDump, parameters.StreamIndex()};
            comment.found())
        {
            comment_ = comment.comment();
            RaisePropertyChanged(L"Comment");
            return;
        }

        logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump comment stream index:[{}]", parameters.StreamIndex()));
    }
}
