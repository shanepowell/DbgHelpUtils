#include "pch.h"
#include "ExceptionStreamPage.xaml.h"

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

    void ExceptionStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        exception_stream_ = dlg_help_utils::exception_stream{miniDump, parameters.StreamIndex()};

        if (!exception_stream_.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump exception stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        exception_.as<MiniDumpException>()->Set(&exception_stream_.exception().ExceptionRecord);
        exceptionParametersDataSource_.SetException(exception_);

        RaisePropertyChanged(L"ThreadId");
        RaisePropertyChanged(L"Exception");
    }

    void ExceptionStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(threadId(), exceptionCode(), exceptionFlags(), exceptionRecord(), exceptionAddress());
    }
}
