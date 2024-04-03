#pragma once

#include "ModuleListStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ModuleListStreamEntriesDataSource;

    struct ModuleListStreamPage : ModuleListStreamPageT<ModuleListStreamPage>, MiniDumpPageBase<ModuleListStreamPage>
    {
        ModuleListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return modulesStreamEntriesDataSource_; }

        void SelectModule(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::ModuleListStreamEntriesDataSource modulesStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ModuleListStreamPage : ModuleListStreamPageT<ModuleListStreamPage, implementation::ModuleListStreamPage>
    {
    };
}
