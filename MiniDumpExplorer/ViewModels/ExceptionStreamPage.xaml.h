#pragma once

#include "ExceptionStreamPage.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"
#include "DbgHelpUtils/exception_stream.h"
#include "DbgHelpUtils/find_thread_stack.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ExceptionParametersDataSource;
    struct MiniDumpException;
    struct ThreadContext;
    struct ThreadListStreamEntry;

    struct ExceptionStreamPage : ExceptionStreamPageT<ExceptionStreamPage>, GlobalOptionsNotifyPropertyChangedBase<ExceptionStreamPage>, MiniDumpPageBase<ExceptionStreamPage>
    {
        ExceptionStreamPage();


        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint32_t Index() const { return static_cast<uint32_t>(exception_stream_.index()); }
        uint32_t ThreadId() const { return exception_stream_.exception().ThreadId; }
        hstring ThreadName() const { return name_; }
        MiniDumpExplorer::MiniDumpException Exception() const { return exception_; }

        [[nodiscard]] IDataGridDataSource ExceptionParametersSource() const noexcept { return exceptionParametersDataSource_; }

        MiniDumpExplorer::ThreadContext ThreadContext() const { return threadContext_; }

        void ShowThreadPage(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;
        void SetupFlyoutMenus();
        std::wstring CreateFindNavigationTag() const;

    private:
        dlg_help_utils::exception_stream exception_stream_{};
        hstring name_;
        MiniDumpExplorer::MiniDumpException exception_{};
        MiniDumpExplorer::ExceptionParametersDataSource exceptionParametersDataSource_{};
        MiniDumpExplorer::ThreadContext threadContext_{};
        std::optional<dlg_help_utils::thread_stack> stackInfo_;
        MiniDumpExplorer::MiniDumpPage miniDumpPage_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ExceptionStreamPage : ExceptionStreamPageT<ExceptionStreamPage, implementation::ExceptionStreamPage>
    {
    };
}
