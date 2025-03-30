#include "pch.h"
#include "ExceptionStreamPage.xaml.h"

#include "ThreadListStreamPage.h"
#include "ThreadExListStreamPage.h"

#include "DbgHelpUtils/find_thread_stack.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/thread_list_stream.h"
#include "DbgHelpUtils/thread_names_list_stream.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Models/ThreadContext.h"
#include "Models/ThreadStack.h"
#include "Models/MiniDumpException.h"
#include "Helpers/UIHelper.h"

#if __has_include("ExceptionStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ExceptionStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ExceptionStreamPage::ExceptionStreamPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"ThreadId"
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void ExceptionStreamPage::InitializeComponent()
    {
        ExceptionStreamPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void ExceptionStreamPage::ShowThreadPage([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        if (!stackInfo_.has_value())
        {
            return;
        }

        auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
        miniDumpPage->SelectNavigationItemTag(CreateFindNavigationTag());
    }

    void ExceptionStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        miniDumpPage_ = parameters.MiniDump();
        auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        exception_stream_ = dlg_help_utils::exception_stream{miniDump, parameters.StreamIndex()};

        if (!exception_stream_.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump exception stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        exception_.as<MiniDumpException>()->Set(&exception_stream_.exception().ExceptionRecord);
        exceptionParametersDataSource_.SetException(exception_);

        threadContext_.as<implementation::ThreadContext>()->Set(exception_stream_.thread_context());

        dlg_help_utils::thread_names_list_stream const thread_names_list_stream{miniDump};
        if (auto const name = thread_names_list_stream.get_thread_name_for_thread_id(exception_stream_.exception().ThreadId);
            name.is_valid())
        {
            name_ = name.name();
        }

        stackInfo_ = find_thread_stack(miniDump, exception_stream_.exception().ThreadId);

        if (stackInfo_.has_value() && stackInfo_->stream_type == ThreadListStream)
        {
            dlg_help_utils::thread_list_stream const thread_list{ miniDump, stackInfo_->list_stream_index };
            auto thread = thread_list.get_thread(stackInfo_->stream_index);
            stack_.as<ThreadStack>()->Set(std::move(thread));
            stack_.as<ThreadStack>()->LoadStack(miniDump);
        }

        RaisePropertyChanged(L"ThreadId");
        RaisePropertyChanged(L"Exception");
        RaisePropertyChanged(L"ThreadContext");
        RaisePropertyChanged(L"Stack");
    }

    void ExceptionStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(threadId(), exceptionCode(), exceptionFlags(), exceptionRecord(), exceptionAddress());
    }

    std::wstring ExceptionStreamPage::CreateFindNavigationTag() const
    {
        switch (stackInfo_->stream_type)
        {
            case ThreadListStream:  // NOLINT(bugprone-branch-clone)
                return ThreadListStreamPage::CreateFindNavigationTag(stackInfo_->list_stream_index, stackInfo_->stream_index);

            case ThreadExListStream:
                return ThreadExListStreamPage::CreateFindNavigationTag(stackInfo_->list_stream_index, stackInfo_->stream_index);

            default:
                throw std::invalid_argument("Unknown thread list type");
        }
    }
}
