// ReSharper disable CppUnusedIncludeDirective
#pragma once

#include "App.xaml.g.h"
#include "Triggers/ControlSizeTrigger.h"
#include "Triggers/IsEqualStateTrigger.h"
#include "Triggers/IsNullOrEmptyStateTrigger.h"
#include "StyleExtension/StyleExtensions.h"
#include "StyleExtension/StyleExtensionResourceDictionary.h"
#include "Controls/SettingsCard/SettingsCardAutomationPeer.h"
#include "Controls/SettingsCard/SettingsCard.h"
#include "Controls/SettingsExpander/SettingsExpanderAutomationPeer.h"
#include "Controls/SettingsExpander/SettingsExpanderItemStyleSelector.h"
#include "Controls/SettingsExpander/SettingsExpander.h"
#include "Converters/HexNumberConverter.h"
#include "Models/RecentFileItem.h"
#include "Controls\DataGrid\DataGridCheckBoxColumn.h"
#include "Controls\DataGrid\DataGridTextColumn.h"
#include "Controls\DataGrid\DataGridComboBoxColumn.h"
#include "Controls\DataGrid\DataGrid.h"
#include "Controls\DataGrid\DataGridColumnHeader.h"
#include "Controls\DataGrid\DataGridColumn.h"
#include "Controls\DataGrid\DataGridLength.h"
#include "Controls\DataGrid\DataGridAutoGeneratingColumnEventArgs.h"    
#include "Controls\DataGrid\DataGridBeginningEditEventArgs.h"
#include "Controls\DataGrid\DataGridRow.h"
#include "Controls\DataGrid\DataGridCellEditEndedEventArgs.h"
#include "Controls\DataGrid\DataGridCellEditEndingEventArgs.h"
#include "Controls\DataGrid\DataGridColumnEventArgs.h"
#include "Controls\DataGrid\DataGridColumnReorderingEventArgs.h"
#include "Controls\DataGrid\DataGridRowClipboardEventArgs.h"
#include "Controls\DataGrid\DataGridClipboardCellContent.h"
#include "Controls\DataGrid\DataGridRowEventArgs.h"
#include "Controls\DataGrid\DataGridRowDetailsEventArgs.h"
#include "Controls\DataGrid\DataGridRowGroupHeaderEventArgs.h"
#include "Controls\DataGrid\DataGridRowGroupHeader.h"
#include "Controls\DataGrid\DataGridPreparingCellForEditEventArgs.h"
#include "Controls\DataGrid\DataGridRowEditEndedEventArgs.h"
#include "Controls\DataGrid\DataGridRowEditEndingEventArgs.h"
#include "Controls\DataGrid\DataGridColumnHeadersPresenter.h"
#include "Controls\DataGrid\DataGridRowsPresenter.h"
#include "Controls\DataGrid\DataGridFillerColumn.h"
#include "Controls\DataGrid\DataGridCell.h"
#include "Controls\DataGrid\DataGridCellEventArgs.h"
#include "Controls\DataGrid\DataGridCellAutomationPeer.h"
#include "Controls\DataGrid\DataGridRowHeader.h"
#include "Controls\DataGrid\DataGridBoundColumn.h"
#include "Controls\DataGrid\DataGridFrozenGrid.h"
#include "Controls\DataGrid\DataGridCellsPresenter.h"
#include "Controls\DataGrid\DataGridDetailsPresenter.h"
#include "Controls\DataGrid\DataGridValueConverter.h"
#include "Controls\DataGrid\DataGridRowAutomationPeer.h"
#include "Controls\DataGrid\DataGridRowHeaderAutomationPeer.h"
#include "Controls\DataGrid\DataGridColumnHeaderAutomationPeer.h"
#include "Controls\DataGrid\DataGridAutomationPeer.h"
#include "Controls\DataGrid\DataGridGroupItemAutomationPeer.h"
#include "Controls\DataGrid\DataGridItemAutomationPeer.h"
#include "Controls\DataGrid\DataGridRowGroupHeaderAutomationPeer.h"
#include "Controls\DataGrid\DataGridColumnHeadersPresenterAutomationPeer.h"
#include "Controls\DataGrid\DataGridRowsPresenterAutomationPeer.h"
#include "Controls\DataGrid\DataGridTemplateColumn.h"
#include "Models\RecentFileItemsDataSource.h"
#include "Converters\SizeNumberConverter.h"
#include "Converters\BoolToObjectConverter.h"
#include "Converters\BoolToVisibilityConverter.h"
#include "Converters\LocalNumberConverter.h"
#include "Converters\TimeStampNumberConverter.h"
#include "Models\GlobalOptionItems.h"
#include "Converters\BoolNegationConverter.h"
#include "Models\DirectoryStreamEntry.h"
#include "Models\MiniDumpLocationDescriptor.h"
#include "Models\DirectoryStreamEntriesDataSource.h"
#include "Models\MiniDumpPageParameters.h"
#include "Models\ThreadListStreamEntriesDataSource.h"
#include "Models\ThreadListStreamEntry.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        Microsoft::UI::Xaml::Window window{ nullptr };
        Windows::System::DispatcherQueueController log_queue_;
    };
}
