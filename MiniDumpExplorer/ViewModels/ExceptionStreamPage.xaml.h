#pragma once

#include "ExceptionStreamPage.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"
#include "DbgHelpUtils/exception_stream.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpException;
    struct ExceptionParametersDataSource;

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

        uint32_t ThreadId() const { return exception_stream_.exception().ThreadId; }
        MiniDumpExplorer::MiniDumpException Exception() const { return exception_; }

        [[nodiscard]] IDataGridDataSource ExceptionParametersSource() const noexcept { return exceptionParametersDataSource_; }

    private:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;
        void SetupFlyoutMenus();

    private:
        dlg_help_utils::exception_stream exception_stream_{};
        MiniDumpExplorer::MiniDumpException exception_{};
        MiniDumpExplorer::ExceptionParametersDataSource exceptionParametersDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ExceptionStreamPage : ExceptionStreamPageT<ExceptionStreamPage, implementation::ExceptionStreamPage>
    {
    };
}
