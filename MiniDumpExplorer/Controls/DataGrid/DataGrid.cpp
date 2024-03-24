#include "pch.h"
#include "DataGrid.h"

#include <ranges>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGridAutomationPeer.h"
#include "DataGridBoundColumn.h"
#include "DataGridCell.h"
#include "DataGridColumn.h"
#include "DataGridColumnHeader.h"
#include "DataGridColumnHeadersPresenter.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"
#include "DataGridRowsPresenter.h"
#include "DbgHelpUtils/double_utils.h"
#include "DbgHelpUtils/exit_scope.h"
#include "Helpers/ControlHelpers.h"
#include "Helpers/TypeHelper.h"
#include "Helpers/UIHelper.h"
#include "Helpers/UISettingsHelper.h"
#include "Helpers/VisualStates.h"
#include "Utility/KeyboardHelper.h"
#include "Utility/StringHelper.h"
#include "Utility/SuspendedHandlers.h"

#if __has_include("DataGrid.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGrid.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        const std::wstring ElementRootName = L"Root"s;
        const std::wstring ElementRowsPresenterName = L"RowsPresenter"s;
        const std::wstring ElementColumnHeadersPresenterName = L"ColumnHeadersPresenter"s;
        const std::wstring ElementFrozenColumnScrollBarSpacerName = L"FrozenColumnScrollBarSpacer"s;
        const std::wstring ElementHorizontalScrollBarName = L"HorizontalScrollBar"s;
        const std::wstring ElementTopLeftCornerHeaderName = L"TopLeftCornerHeader"s;
        const std::wstring ElementTopRightCornerHeaderName = L"TopRightCornerHeader"s;
        const std::wstring ElementBottomRightCornerHeaderName = L"BottomRightCorner"s;
        const std::wstring ElementVerticalScrollBarName = L"VerticalScrollBar"s;

        constexpr bool DefaultAutoGenerateColumns = true;
        constexpr bool DefaultCanUserReorderColumns = true;
        constexpr bool DefaultCanUserResizeColumns = true;
        constexpr bool DefaultCanUserSortColumns = true;
        constexpr DataGridGridLinesVisibility DefaultGridLinesVisibility = DataGridGridLinesVisibility::None;
        constexpr DataGridHeadersVisibility DefaultHeadersVisibility = DataGridHeadersVisibility::Column;
        constexpr DataGridRowDetailsVisibilityMode DefaultRowDetailsVisibility = DataGridRowDetailsVisibilityMode::VisibleWhenSelected;
        constexpr DataGridSelectionMode DefaultSelectionMode = DataGridSelectionMode::Extended;
        constexpr Controls::ScrollBarVisibility DefaultScrollBarVisibility = Controls::ScrollBarVisibility::Auto;

        constexpr double MinimumRowHeaderWidth = 4;
        constexpr double MinimumColumnHeaderHeight = 4;
        constexpr double MouseWheelDeltaDivider = 4.0;
        constexpr double MaxHeadersThickness = 32768;

        constexpr double DefaultMinColumnWidth = 20;
        const double DefaultMaxColumnWidth = std::numeric_limits<double>::infinity();

        constexpr double DefaultIncrementalLoadingThreshold = 3.0;
        constexpr double DefaultDataFetchSize = 3.0;

        // 2 seconds delay used to hide the scroll bars for example when OS animations are turned off.
        constexpr int NoScrollBarCountdownMs = 2000;

        /*
        const std::wstring ElementRowHeadersPresenterName = L"RowHeadersPresenter"s;
        /// <summary>
        /// The default order to use for columns when there is no <see cref="DisplayAttribute.Order"/>
        /// value available for the property.
        /// </summary>
        /// <remarks>
        /// The value of 10,000 comes from the DataAnnotations spec, allowing
        /// some properties to be ordered at the beginning and some at the end.
        /// </remarks>
        */
    }

    DataGrid::DataGrid()
        : rowHeightEstimate_{ DefaultRowHeight }
        , rowGroupHeaderHeightEstimate_{ DefaultRowHeight }
    {
        TabNavigation(Input::KeyboardNavigationMode::Once);

        rowGroupHeaderPropertyNameAlternative_ = L"Group";
        [[maybe_unused]] auto const rowGroupHeaderStylesCollectionChangedEvent = rowGroupHeaderStyles_.VectorChanged({ this, &DataGrid::RowGroupHeaderStyles_CollectionChanged });

        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGrid"));

        HookDataGridEvents();
    }

    event_token DataGrid::AutoGeneratingColumn(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs> const& value)
    {
        return autoGeneratingColumnHandler_.add(value);
    }

    void DataGrid::AutoGeneratingColumn(event_token const& value)
    {
        autoGeneratingColumnHandler_.remove(value);
    }

    event_token DataGrid::BeginningEdit(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridBeginningEditEventArgs> const& value)
    {
        return beginningEditHandler_.add(value);
    }

    void DataGrid::BeginningEdit(event_token const& value)
    {
        beginningEditHandler_.remove(value);
    }

    event_token DataGrid::CellEditEnded(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndedEventArgs> const& value)
    {
        return cellEditEndedHandler_.add(value);
    }

    void DataGrid::CellEditEnded(event_token const& value)
    {
        cellEditEndedHandler_.remove(value);
    }

    event_token DataGrid::CellEditEnding(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndingEventArgs> const& value)
    {
        return cellEditEndingHandler_.add(value);
    }

    void DataGrid::CellEditEnding(event_token const& value)
    {
        cellEditEndingHandler_.remove(value);
    }

    event_token DataGrid::ColumnDisplayIndexChanged(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value)
    {
        return columnDisplayIndexChangedHandler_.add(value);
    }

    void DataGrid::ColumnDisplayIndexChanged(event_token const& value)
    {
        columnDisplayIndexChangedHandler_.remove(value);
    }

    event_token DataGrid::ColumnHeaderDragCompleted(Windows::Foundation::EventHandler<Controls::Primitives::DragCompletedEventArgs> const& value)
    {
        return columnHeaderDragCompletedHandler_.add(value);
    }

    void DataGrid::ColumnHeaderDragCompleted(event_token const& value)
    {
        columnHeaderDragCompletedHandler_.remove(value);
    }

    event_token DataGrid::ColumnHeaderDragDelta(Windows::Foundation::EventHandler<Controls::Primitives::DragDeltaEventArgs> const& value)
    {
        return columnHeaderDragDeltaHandler_.add(value);
    }

    void DataGrid::ColumnHeaderDragDelta(event_token const& value)
    {
        columnHeaderDragDeltaHandler_.remove(value);
    }

    event_token DataGrid::ColumnHeaderDragStarted(Windows::Foundation::EventHandler<Controls::Primitives::DragStartedEventArgs> const& value)
    {
        return columnHeaderDragStartedHandler_.add(value);
    }

    void DataGrid::ColumnHeaderDragStarted(event_token const& value)
    {
        columnHeaderDragStartedHandler_.remove(value);
    }

    event_token DataGrid::ColumnReordered(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value)
    {
        return columnReorderedHandler_.add(value);
    }

    void DataGrid::ColumnReordered(event_token const& value)
    {
        columnReorderedHandler_.remove(value);
    }

    event_token DataGrid::ColumnReordering(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnReorderingEventArgs> const& value)
    {
        return columnReorderingHandler_.add(value);
    }

    void DataGrid::ColumnReordering(event_token const& value)
    {
        columnReorderingHandler_.remove(value);
    }

    event_token DataGrid::CopyingRowClipboardContent(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowClipboardEventArgs> const& value)
    {
        return copyingRowClipboardContentHandler_.add(value);
    }

    void DataGrid::CopyingRowClipboardContent(event_token const& value)
    {
        copyingRowClipboardContentHandler_.remove(value);
    }

    event_token DataGrid::CurrentCellChanged(Windows::Foundation::EventHandler<RoutedEventArgs> const& value)
    {
        return currentCellChangedHandler_.add(value);
    }

    void DataGrid::CurrentCellChanged(event_token const& value)
    {
        currentCellChangedHandler_.remove(value);
    }

    event_token DataGrid::LoadingRow(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs> const& value)
    {
        return loadingRowHandler_.add(value);
    }

    void DataGrid::LoadingRow(event_token const& value)
    {
        loadingRowHandler_.remove(value);
    }

    event_token DataGrid::LoadingRowDetails(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value)
    {
        return loadingRowDetailsHandler_.add(value);
    }

    void DataGrid::LoadingRowDetails(event_token const& value)
    {
        loadingRowDetailsHandler_.remove(value);
    }

    event_token DataGrid::LoadingRowGroup(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs> const& value)
    {
        return loadingRowGroupHandler_.add(value);
    }

    void DataGrid::LoadingRowGroup(event_token const& value)
    {
        loadingRowGroupHandler_.remove(value);
    }

    event_token DataGrid::PreparingCellForEdit(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridPreparingCellForEditEventArgs> const& value)
    {
        return preparingCellForEditHandler_.add(value);
    }

    void DataGrid::PreparingCellForEdit(event_token const& value)
    {
        preparingCellForEditHandler_.remove(value);
    }

    event_token DataGrid::RowDetailsVisibilityChanged(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value)
    {
        return rowDetailsVisibilityChangedHandler_.add(value);
    }

    void DataGrid::RowDetailsVisibilityChanged(event_token const& value)
    {
        rowDetailsVisibilityChangedHandler_.remove(value);
    }

    event_token DataGrid::RowEditEnded(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndedEventArgs> const& value)
    {
        return rowEditEndedHandler_.add(value);
    }

    void DataGrid::RowEditEnded(event_token const& value)
    {
        rowEditEndedHandler_.remove(value);
    }

    event_token DataGrid::RowEditEnding(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndingEventArgs> const& value)
    {
        return rowEditEndingHandler_.add(value);
    }

    void DataGrid::RowEditEnding(event_token const& value)
    {
        rowEditEndingHandler_.remove(value);
    }

    event_token DataGrid::SelectionChanged(Controls::SelectionChangedEventHandler const& value)
    {
        return selectionChangedHandler_.add(value);
    }

    void DataGrid::SelectionChanged(event_token const& value)
    {
        selectionChangedHandler_.remove(value);
    }

    event_token DataGrid::Sorting(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value)
    {
        return sortingHandler_.add(value);
    }

    void DataGrid::Sorting(event_token const& value)
    {
        sortingHandler_.remove(value);
    }

    event_token DataGrid::UnloadingRow(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs> const& value)
    {
        return unloadingRowHandler_.add(value);
    }

    void DataGrid::UnloadingRow(event_token const& value)
    {
        unloadingRowHandler_.remove(value);
    }

    event_token DataGrid::UnloadingRowGroup(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs> const& value)
    {
        return unloadingRowGroupHandler_.add(value);
    }

    void DataGrid::UnloadingRowGroup(event_token const& value)
    {
        unloadingRowGroupHandler_.remove(value);
    }

    event_token DataGrid::UnloadingRowDetails(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value)
    {
        return unloadingRowDetailsHandler_.add(value);
    }

    void DataGrid::UnloadingRowDetails(event_token const& value)
    {
        unloadingRowDetailsHandler_.remove(value);
    }

    event_token DataGrid::RowDoubleTapped(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value)
    {
        return rowDoubleTappedHandler_.add(value);
    }

    void DataGrid::RowDoubleTapped(event_token const& value)
    {
        rowDoubleTappedHandler_.remove(value);
    }

    DependencyProperty DataGrid::AlternatingRowBackgroundProperty()
    {
        static DependencyProperty s_AlternatingRowBackgroundProperty = DependencyProperty::Register(
            L"AlternatingRowBackground",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnAlternatingRowBackgroundPropertyChanged(e); } });
        return s_AlternatingRowBackgroundProperty;
    }

    DependencyProperty DataGrid::AlternatingRowForegroundProperty()
    {
        static DependencyProperty s_AlternatingRowForegroundProperty = DependencyProperty::Register(
            L"AlternatingRowForeground",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnAlternatingRowForegroundPropertyChanged(e); } });
        return s_AlternatingRowForegroundProperty;
    }

    DependencyProperty DataGrid::AreRowDetailsFrozenProperty()
    {
        static DependencyProperty s_AreRowDetailsFrozenProperty = DependencyProperty::Register(
            L"AreRowDetailsFrozen",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(false) });
        return s_AreRowDetailsFrozenProperty;
    }

    DependencyProperty DataGrid::AreRowGroupHeadersFrozenProperty()
    {
        static DependencyProperty s_AreRowGroupHeadersFrozenProperty = DependencyProperty::Register(
            L"AreRowGroupHeadersFrozen",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(true), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnAreRowGroupHeadersFrozenPropertyChanged(e); } });
        return s_AreRowGroupHeadersFrozenProperty;
    }

    DependencyProperty DataGrid::AutoGenerateColumnsProperty()
    {
        static DependencyProperty s_AutoGenerateColumnsProperty = DependencyProperty::Register(
            L"AutoGenerateColumns",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultAutoGenerateColumns), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnAutoGenerateColumnsPropertyChanged(e); } });
        return s_AutoGenerateColumnsProperty;
    }

    DependencyProperty DataGrid::CanUserReorderColumnsProperty()
    {
        static DependencyProperty s_CanUserReorderColumnsProperty = DependencyProperty::Register(
            L"CanUserReorderColumns",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultCanUserReorderColumns) });
        return s_CanUserReorderColumnsProperty;
    }

    DependencyProperty DataGrid::CanUserResizeColumnsProperty()
    {
        static DependencyProperty s_CanUserResizeColumnsProperty = DependencyProperty::Register(
            L"CanUserResizeColumns",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultCanUserResizeColumns), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnCanUserResizeColumnsPropertyChanged(e); } });
        return s_CanUserResizeColumnsProperty;
    }

    DependencyProperty DataGrid::CanUserSortColumnsProperty()
    {
        static DependencyProperty s_CanUserSortColumnsProperty = DependencyProperty::Register(
            L"CanUserSortColumns",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultCanUserSortColumns) });
        return s_CanUserSortColumnsProperty;
    }

    DependencyProperty DataGrid::CellStyleProperty()
    {
        static DependencyProperty s_CellStyleProperty = DependencyProperty::Register(
            L"CellStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnCellStylePropertyChanged(e); } });
        return s_CellStyleProperty;
    }

    DependencyProperty DataGrid::ClipboardCopyModeProperty()
    {
        static DependencyProperty s_ClipboardCopyModeProperty = DependencyProperty::Register(
            L"ClipboardCopyMode",
            xaml_typename<DataGridClipboardCopyMode>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DataGridClipboardCopyMode::ExcludeHeader) });
        return s_ClipboardCopyModeProperty;
    }

    DependencyProperty DataGrid::ColumnHeaderHeightProperty()
    {
        static DependencyProperty s_ColumnHeaderHeightProperty = DependencyProperty::Register(
            L"ColumnHeaderHeight",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(std::nan("")), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnColumnHeaderHeightPropertyChanged(e); } });
        return s_ColumnHeaderHeightProperty;
    }

    DependencyProperty DataGrid::ColumnHeaderStyleProperty()
    {
        static DependencyProperty s_ColumnHeaderStyleProperty = DependencyProperty::Register(
            L"ColumnHeaderStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnColumnHeaderStylePropertyChanged(e); } });
        return s_ColumnHeaderStyleProperty;
    }

    DependencyProperty DataGrid::ColumnWidthProperty()
    {
        static DependencyProperty s_ColumnWidthProperty = DependencyProperty::Register(
            L"ColumnWidth",
            xaml_typename<MiniDumpExplorer::DataGridLength>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ MiniDumpExplorer::DataGridLength::Auto(), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnColumnWidthPropertyChanged(e); } });
        return s_ColumnWidthProperty;
    }

    DependencyProperty DataGrid::DataFetchSizeProperty()
    {
        static DependencyProperty s_DataFetchSizeProperty = DependencyProperty::Register(
            L"DataFetchSize",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultDataFetchSize), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnDataFetchSizePropertyChanged(e); } });
        return s_DataFetchSizeProperty;
    }

    DependencyProperty DataGrid::DragIndicatorStyleProperty()
    {
        static DependencyProperty s_DragIndicatorStyleProperty = DependencyProperty::Register(
            L"DragIndicatorStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr });
        return s_DragIndicatorStyleProperty;
    }

    DependencyProperty DataGrid::DropLocationIndicatorStyleProperty()
    {
        static DependencyProperty s_DropLocationIndicatorStyleProperty = DependencyProperty::Register(
            L"DropLocationIndicatorStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr });
        return s_DropLocationIndicatorStyleProperty;
    }

    DependencyProperty DataGrid::FrozenColumnCountProperty()
    {
        static DependencyProperty s_FrozenColumnCountProperty = DependencyProperty::Register(
            L"FrozenColumnCount",
            xaml_typename<int32_t>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(0), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnFrozenColumnCountPropertyChanged(e); } });
        return s_FrozenColumnCountProperty;
    }

    DependencyProperty DataGrid::GridLinesVisibilityProperty()
    {
        static DependencyProperty s_GridLinesVisibilityProperty = DependencyProperty::Register(
            L"GridLinesVisibility",
            xaml_typename<DataGridGridLinesVisibility>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultGridLinesVisibility), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnGridLinesVisibilityPropertyChanged(e); } });
        return s_GridLinesVisibilityProperty;
    }

    DependencyProperty DataGrid::HeadersVisibilityProperty()
    {
        static DependencyProperty s_HeadersVisibilityProperty = DependencyProperty::Register(
            L"HeadersVisibility",
            xaml_typename<DataGridHeadersVisibility>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultHeadersVisibility), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnHeadersVisibilityPropertyChanged(e); } });
        return s_HeadersVisibilityProperty;
    }

    DependencyProperty DataGrid::HorizontalGridLinesBrushProperty()
    {
        static DependencyProperty s_HorizontalGridLinesBrushProperty = DependencyProperty::Register(
            L"HorizontalGridLinesBrush",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnHorizontalGridLinesBrushPropertyChanged(e); } });
        return s_HorizontalGridLinesBrushProperty;
    }

    DependencyProperty DataGrid::HorizontalScrollBarVisibilityProperty()
    {
        static DependencyProperty s_HorizontalScrollBarVisibilityProperty = DependencyProperty::Register(
            L"HorizontalScrollBarVisibility",
            xaml_typename<Controls::ScrollBarVisibility>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultScrollBarVisibility), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnHorizontalScrollBarVisibilityPropertyChanged(e); } });
        return s_HorizontalScrollBarVisibilityProperty;
    }

    DependencyProperty DataGrid::IsReadOnlyProperty()
    {
        static DependencyProperty s_IsReadOnlyProperty = DependencyProperty::Register(
            L"IsReadOnly",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(false), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnIsReadOnlyPropertyChanged(e); } });
        return s_IsReadOnlyProperty;
    }

    DependencyProperty DataGrid::IsValidProperty()
    {
        static DependencyProperty s_IsValidProperty = DependencyProperty::Register(
            L"IsValid",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(true), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnIsValidPropertyChanged(e); } });
        return s_IsValidProperty;
    }

    DependencyProperty DataGrid::IncrementalLoadingThresholdProperty()
    {
        static DependencyProperty s_IncrementalLoadingThresholdProperty = DependencyProperty::Register(
            L"IncrementalLoadingThreshold",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultIncrementalLoadingThreshold), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnIncrementalLoadingThresholdPropertyChanged(e); } });
        return s_IncrementalLoadingThresholdProperty;
    }

    DependencyProperty DataGrid::IncrementalLoadingTriggerProperty()
    {
        static DependencyProperty s_IncrementalLoadingTriggerProperty = DependencyProperty::Register(
            L"IncrementalLoadingTrigger",
            xaml_typename<Controls::IncrementalLoadingTrigger>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(Controls::IncrementalLoadingTrigger::Edge) });
        return s_IncrementalLoadingTriggerProperty;
    }

    DependencyProperty DataGrid::ItemsSourceProperty()
    {
        static DependencyProperty s_ItemsSourceProperty = DependencyProperty::Register(
            L"ItemsSource",
            xaml_typename<IDataGridDataSource>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnItemsSourcePropertyChanged(e); } });
        return s_ItemsSourceProperty;
    }

    DependencyProperty DataGrid::MaxColumnWidthProperty()
    {
        static DependencyProperty s_MaxColumnWidthProperty = DependencyProperty::Register(
            L"MaxColumnWidth",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultMaxColumnWidth), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnMaxColumnWidthPropertyChanged(e); } });
        return s_MaxColumnWidthProperty;
    }

    DependencyProperty DataGrid::MinColumnWidthProperty()
    {
        static DependencyProperty s_MinColumnWidthProperty = DependencyProperty::Register(
            L"MinColumnWidth",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultMinColumnWidth), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnMinColumnWidthPropertyChanged(e); } });
        return s_MinColumnWidthProperty;
    }

    DependencyProperty DataGrid::RowBackgroundProperty()
    {
        static DependencyProperty s_RowBackgroundProperty = DependencyProperty::Register(
            L"RowBackground",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowBackgroundPropertyChanged(e); } });
        return s_RowBackgroundProperty;
    }

    DependencyProperty DataGrid::RowDetailsTemplateProperty()
    {
        static DependencyProperty s_RowDetailsTemplateProperty = DependencyProperty::Register(
            L"RowDetailsTemplate",
            xaml_typename<DataTemplate>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowDetailsTemplatePropertyChanged(e); } });
        return s_RowDetailsTemplateProperty;
    }

    DependencyProperty DataGrid::RowDetailsVisibilityModeProperty()
    {
        static DependencyProperty s_RowDetailsVisibilityModeProperty = DependencyProperty::Register(
            L"RowDetailsVisibilityMode",
            xaml_typename<DataGridRowDetailsVisibilityMode>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultRowDetailsVisibility), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowDetailsVisibilityModePropertyChanged(e); } });
        return s_RowDetailsVisibilityModeProperty;
    }

    DependencyProperty DataGrid::RowForegroundProperty()
    {
        static DependencyProperty s_RowForegroundProperty = DependencyProperty::Register(
            L"RowForeground",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowForegroundPropertyChanged(e); } });
        return s_RowForegroundProperty;
    }

    DependencyProperty DataGrid::RowHeightProperty()
    {
        static DependencyProperty s_RowHeightProperty = DependencyProperty::Register(
            L"RowHeight",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(std::nan("")), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowHeightPropertyChanged(e); } });
        return s_RowHeightProperty;
    }

    DependencyProperty DataGrid::RowHeaderWidthProperty()
    {
        static DependencyProperty s_RowHeaderWidthProperty = DependencyProperty::Register(
            L"RowHeaderWidth",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(std::nan("")), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowHeaderWidthPropertyChanged(e); } });
        return s_RowHeaderWidthProperty;
    }

    DependencyProperty DataGrid::RowHeaderStyleProperty()
    {
        static DependencyProperty s_RowHeaderStyleProperty = DependencyProperty::Register(
            L"RowHeaderStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowHeaderStylePropertyChanged(e); } });
        return s_RowHeaderStyleProperty;
    }

    DependencyProperty DataGrid::RowStyleProperty()
    {
        static DependencyProperty s_RowStyleProperty = DependencyProperty::Register(
            L"RowStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnRowStylePropertyChanged(e); } });
        return s_RowStyleProperty;
    }

    DependencyProperty DataGrid::SelectionModeProperty()
    {
        static DependencyProperty s_SelectionModeProperty = DependencyProperty::Register(
            L"SelectionMode",
            xaml_typename<DataGridSelectionMode>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultSelectionMode), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnSelectionModePropertyChanged(e); } });
        return s_SelectionModeProperty;
    }

    DependencyProperty DataGrid::SelectedIndexProperty()
    {
        static DependencyProperty s_SelectedIndexProperty = DependencyProperty::Register(
            L"SelectedIndex",
            xaml_typename<int32_t>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(InvalidIndex), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnSelectedIndexPropertyChanged(e); } });
        return s_SelectedIndexProperty;
    }

    DependencyProperty DataGrid::SelectedItemProperty()
    {
        static DependencyProperty s_SelectedItemProperty = DependencyProperty::Register(
            L"SelectedItem",
            xaml_typename<IInspectable>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnSelectedItemPropertyChanged(e); } });
        return s_SelectedItemProperty;
    }

    DependencyProperty DataGrid::VerticalGridLinesBrushProperty()
    {
        static DependencyProperty s_VerticalGridLinesBrushProperty = DependencyProperty::Register(
            L"VerticalGridLinesBrush",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnVerticalGridLinesBrushPropertyChanged(e); } });
        return s_VerticalGridLinesBrushProperty;
    }

    DependencyProperty DataGrid::VerticalScrollBarVisibilityProperty()
    {
        static DependencyProperty s_VerticalScrollBarVisibilityProperty = DependencyProperty::Register(
            L"VerticalScrollBarVisibility",
            xaml_typename<Controls::ScrollBarVisibility>(),
            xaml_typename<MiniDumpExplorer::DataGrid>(),
            PropertyMetadata{ box_value(DefaultScrollBarVisibility), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGrid>()->OnVerticalScrollBarVisibilityPropertyChanged(e); } });
        return s_VerticalScrollBarVisibilityProperty;
    }

    Media::Brush DataGrid::AlternatingRowBackground() const
    {
        return GetValue(AlternatingRowBackgroundProperty()).as<Media::Brush>();
    }

    void DataGrid::AlternatingRowBackground(Media::Brush const& value) const
    {
        SetValue(AlternatingRowBackgroundProperty(), value);
    }

    Media::Brush DataGrid::AlternatingRowForeground() const
    {
        return GetValue(AlternatingRowForegroundProperty()).as<Media::Brush>();
    }

    void DataGrid::AlternatingRowForeground(Media::Brush const& value) const
    {
        SetValue(AlternatingRowForegroundProperty(), value);
    }

    bool DataGrid::AreRowDetailsFrozen() const
    {
        return unbox_value<bool>(GetValue(AreRowDetailsFrozenProperty()));
    }

    void DataGrid::AreRowDetailsFrozen(bool const value) const
    {
        SetValue(AreRowDetailsFrozenProperty(), box_value(value));
    }

    bool DataGrid::AreRowGroupHeadersFrozen() const
    {
        return unbox_value<bool>(GetValue(AreRowGroupHeadersFrozenProperty()));
    }

    void DataGrid::AreRowGroupHeadersFrozen(bool const value) const
    {
        SetValue(AreRowGroupHeadersFrozenProperty(), box_value(value));
    }

    bool DataGrid::AutoGenerateColumns() const
    {
        return unbox_value<bool>(GetValue(AutoGenerateColumnsProperty()));
    }

    void DataGrid::AutoGenerateColumns(bool const value) const
    {
        SetValue(AutoGenerateColumnsProperty(), box_value(value));
    }

    bool DataGrid::CanUserReorderColumns() const
    {
        return unbox_value<bool>(GetValue(CanUserReorderColumnsProperty()));
    }

    void DataGrid::CanUserReorderColumns(bool const value) const
    {
        SetValue(CanUserReorderColumnsProperty(), box_value(value));
    }

    bool DataGrid::CanUserResizeColumns() const
    {
        return unbox_value<bool>(GetValue(CanUserResizeColumnsProperty()));
    }

    void DataGrid::CanUserResizeColumns(bool const value) const
    {
        SetValue(CanUserResizeColumnsProperty(), box_value(value));
    }

    bool DataGrid::CanUserSortColumns() const
    {
        return unbox_value<bool>(GetValue(CanUserSortColumnsProperty()));
    }

    void DataGrid::CanUserSortColumns(bool const value) const
    {
        SetValue(CanUserSortColumnsProperty(), box_value(value));
    }

    Style DataGrid::CellStyle() const
    {
        return GetValue(CellStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::CellStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(CellStyleProperty(), value);
    }

    DataGridClipboardCopyMode DataGrid::ClipboardCopyMode() const
    {
        return unbox_value<DataGridClipboardCopyMode>(GetValue(ClipboardCopyModeProperty()));
    }

    void DataGrid::ClipboardCopyMode(DataGridClipboardCopyMode const value) const
    {
        SetValue(ClipboardCopyModeProperty(), box_value(value));
    }

    double DataGrid::ColumnHeaderHeight() const
    {
        return unbox_value<double>(GetValue(ColumnHeaderHeightProperty()));
    }

    void DataGrid::ColumnHeaderHeight(double const value) const
    {
        SetValue(ColumnHeaderHeightProperty(), box_value(value));
    }

    Style DataGrid::ColumnHeaderStyle() const
    {
        return GetValue(ColumnHeaderStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::ColumnHeaderStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(ColumnHeaderStyleProperty(), value);
    }

    MiniDumpExplorer::DataGridLength DataGrid::ColumnWidth() const
    {
        return GetValue(ColumnWidthProperty()).as<MiniDumpExplorer::DataGridLength>();
    }

    void DataGrid::ColumnWidth(MiniDumpExplorer::DataGridLength const& value) const
    {
        SetValue(ColumnWidthProperty(), value);
    }

    double DataGrid::DataFetchSize() const
    {
        return unbox_value<double>(GetValue(DataFetchSizeProperty()));
    }

    void DataGrid::DataFetchSize(double const value) const
    {
        SetValue(DataFetchSizeProperty(), box_value(value));
    }

    Style DataGrid::DragIndicatorStyle() const
    {
        return GetValue(DragIndicatorStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::DragIndicatorStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(DragIndicatorStyleProperty(), value);
    }

    Style DataGrid::DropLocationIndicatorStyle() const
    {
        return GetValue(DropLocationIndicatorStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::DropLocationIndicatorStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(DropLocationIndicatorStyleProperty(), value);
    }

    int32_t DataGrid::FrozenColumnCount() const
    {
        return unbox_value<int32_t>(GetValue(FrozenColumnCountProperty()));
    }

    void DataGrid::FrozenColumnCount(int32_t const value) const
    {
        SetValue(FrozenColumnCountProperty(), box_value(value));
    }

    DataGridGridLinesVisibility DataGrid::GridLinesVisibility() const
    {
        return unbox_value<DataGridGridLinesVisibility>(GetValue(GridLinesVisibilityProperty()));
    }

    void DataGrid::GridLinesVisibility(DataGridGridLinesVisibility const value) const
    {
        SetValue(GridLinesVisibilityProperty(), box_value(value));
    }

    DataGridHeadersVisibility DataGrid::HeadersVisibility() const
    {
        return unbox_value<DataGridHeadersVisibility>(GetValue(HeadersVisibilityProperty()));
    }

    void DataGrid::HeadersVisibility(DataGridHeadersVisibility const value) const
    {
        SetValue(HeadersVisibilityProperty(), box_value(value));
    }

    Media::Brush DataGrid::HorizontalGridLinesBrush() const
    {
        return GetValue(HorizontalGridLinesBrushProperty()).as<Media::Brush>();
    }

    void DataGrid::HorizontalGridLinesBrush(Media::Brush const& value) const
    {
        SetValue(HorizontalGridLinesBrushProperty(), value);
    }

    Controls::ScrollBarVisibility DataGrid::HorizontalScrollBarVisibility() const
    {
        return unbox_value<Controls::ScrollBarVisibility>(GetValue(HorizontalScrollBarVisibilityProperty()));
    }

    void DataGrid::HorizontalScrollBarVisibility(Controls::ScrollBarVisibility const value) const
    {
        SetValue(HorizontalScrollBarVisibilityProperty(), box_value(value));
    }

    bool DataGrid::IsReadOnly() const
    {
        return unbox_value<bool>(GetValue(IsReadOnlyProperty()));
    }

    void DataGrid::IsReadOnly(bool const value) const
    {
        SetValue(IsReadOnlyProperty(), box_value(value));
    }

    bool DataGrid::IsValid() const
    {
        return unbox_value<bool>(GetValue(IsValidProperty()));
    }

    void DataGrid::IsValid(bool const value) const
    {
        if (value != IsValid())
        {
            if (value)
            {
                VisualStates::GoToState(*this, true, VisualStates::StateValid);
            }
            else
            {
                VisualStates::GoToState(*this, true, VisualStates::StateInvalid, VisualStates::StateValid);
            }

            SuspendedHandlers::SetValueNoCallback(*this, IsValidProperty(), box_value(value));
        }
    }

    double DataGrid::IncrementalLoadingThreshold() const
    {
        return unbox_value<double>(GetValue(IncrementalLoadingThresholdProperty()));
    }

    void DataGrid::IncrementalLoadingThreshold(double const value) const
    {
        SetValue(IncrementalLoadingThresholdProperty(), box_value(value));
    }

    Controls::IncrementalLoadingTrigger DataGrid::IncrementalLoadingTrigger() const
    {
        return unbox_value<Controls::IncrementalLoadingTrigger>(GetValue(IncrementalLoadingTriggerProperty()));
    }

    void DataGrid::IncrementalLoadingTrigger(Controls::IncrementalLoadingTrigger const value) const
    {
        SetValue(IncrementalLoadingTriggerProperty(), box_value(value));
    }

    IDataGridDataSource DataGrid::ItemsSource() const
    {
        return GetValue(ItemsSourceProperty()).as<IDataGridDataSource>();
    }

    void DataGrid::ItemsSource(IDataGridDataSource const& value) const
    {
        SetValue(ItemsSourceProperty(), value);
    }

    double DataGrid::MaxColumnWidth() const
    {
        return unbox_value<double>(GetValue(MaxColumnWidthProperty()));
    }

    void DataGrid::MaxColumnWidth(double const value) const
    {
        SetValue(MaxColumnWidthProperty(), box_value(value));
    }

    double DataGrid::MinColumnWidth() const
    {
        return unbox_value<double>(GetValue(MinColumnWidthProperty()));
    }

    void DataGrid::MinColumnWidth(double const value) const
    {
        SetValue(MinColumnWidthProperty(), box_value(value));
    }

    Media::Brush DataGrid::RowBackground() const
    {
        return GetValue(RowBackgroundProperty()).as<Media::Brush>();
    }

    void DataGrid::RowBackground(Media::Brush const& value) const
    {
        SetValue(RowBackgroundProperty(), value);
    }

    DataTemplate DataGrid::RowDetailsTemplate() const
    {
        return GetValue(RowDetailsTemplateProperty()).as<DataTemplate>();
    }

    void DataGrid::RowDetailsTemplate(DataTemplate const& value) const
    {
        SetValue(RowDetailsTemplateProperty(), value);
    }

    DataGridRowDetailsVisibilityMode DataGrid::RowDetailsVisibilityMode() const
    {
        return unbox_value<DataGridRowDetailsVisibilityMode>(GetValue(RowDetailsVisibilityModeProperty()));
    }

    void DataGrid::RowDetailsVisibilityMode(DataGridRowDetailsVisibilityMode const value) const
    {
        SetValue(RowDetailsVisibilityModeProperty(), box_value(value));
    }

    Media::Brush DataGrid::RowForeground() const
    {
        return GetValue(RowForegroundProperty()).as<Media::Brush>();
    }

    void DataGrid::RowForeground(Media::Brush const& value) const
    {
        SetValue(RowForegroundProperty(), value);
    }

    double DataGrid::RowHeight() const
    {
        return unbox_value<double>(GetValue(RowHeightProperty()));
    }

    void DataGrid::RowHeight(double const value) const
    {
        SetValue(RowHeightProperty(), box_value(value));
    }

    double DataGrid::RowHeaderWidth() const
    {
        return unbox_value<double>(GetValue(RowHeaderWidthProperty()));
    }

    void DataGrid::RowHeaderWidth(double const value) const
    {
        SetValue(RowHeaderWidthProperty(), box_value(value));
    }

    Style DataGrid::RowHeaderStyle() const
    {
        return GetValue(RowHeaderStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::RowHeaderStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(RowHeaderStyleProperty(), value);
    }

    Style DataGrid::RowStyle() const
    {
        return GetValue(RowStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGrid::RowStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(RowStyleProperty(), value);
    }

    DataGridSelectionMode DataGrid::SelectionMode() const
    {
        return unbox_value<DataGridSelectionMode>(GetValue(SelectionModeProperty()));
    }

    void DataGrid::SelectionMode(DataGridSelectionMode const value) const
    {
        SetValue(SelectionModeProperty(), box_value(value));
    }

    int32_t DataGrid::SelectedIndex() const
    {
        return unbox_value<int32_t>(GetValue(SelectedIndexProperty()));
    }

    void DataGrid::SelectedIndex(int32_t const value) const
    {
        SetValue(SelectedIndexProperty(), box_value(value));
    }

    Windows::Foundation::IInspectable DataGrid::SelectedItem() const
    {
        return GetValue(SelectedItemProperty());
    }

    void DataGrid::SelectedItem(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(SelectedItemProperty(), value);
    }

    Media::Brush DataGrid::VerticalGridLinesBrush() const
    {
        return GetValue(VerticalGridLinesBrushProperty()).as<Media::Brush>();
    }

    void DataGrid::VerticalGridLinesBrush(Media::Brush const& value) const
    {
        SetValue(VerticalGridLinesBrushProperty(), value);
    }

    Controls::ScrollBarVisibility DataGrid::VerticalScrollBarVisibility() const
    {
        return unbox_value<Controls::ScrollBarVisibility>(GetValue(VerticalScrollBarVisibilityProperty()));
    }

    void DataGrid::VerticalScrollBarVisibility(Controls::ScrollBarVisibility const value) const
    {
        SetValue(VerticalScrollBarVisibilityProperty(), box_value(value));
    }

    Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> DataGrid::Columns() const
    {
        return ColumnsInternal().ObservableItems();
    }

    MiniDumpExplorer::DataGridColumn DataGrid::CurrentColumn() const
    {
        if (CurrentColumnIndex() == InvalidIndex)
        {
            return nullptr;
        }

        assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
        return ColumnsItemsInternal()[CurrentColumnIndex()];
    }

    void DataGrid::CurrentColumn(MiniDumpExplorer::DataGridColumn const& value)
    {
        auto const dataGridColumn = value.try_as<DataGridColumn>();
        if (!dataGridColumn)
        {
            throw DataGridError::DataGrid::ValueCannotBeSetToNull(L"value", L"CurrentColumn");
        }

        if (CurrentColumn() != value)
        {
            if (dataGridColumn->OwningGrid() != *this)
            {
                // Provided column does not belong to this DataGrid
                throw DataGridError::DataGrid::ColumnNotInThisDataGrid();
            }

            if (dataGridColumn->Visibility() == Visibility::Collapsed)
            {
                // CurrentColumn cannot be set to an invisible column
                throw DataGridError::DataGrid::ColumnCannotBeCollapsed();
            }

            if (CurrentSlot() == InvalidIndex)
            {
                // There is no current row so the current column cannot be set
                throw DataGridError::DataGrid::NoCurrentRow();
            }

            auto const beginEdit = editingColumnIndex_ != InvalidIndex;
            if (!EndCellEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, ContainsFocus() /*keepFocus*/, true /*raiseEvents*/))
            {
                // Edited value couldn't be committed or aborted
                return;
            }

            if (noFocusedColumnChangeCount_ == 0)
            {
                ColumnHeaderHasFocus(false);
            }

            UpdateSelectionAndCurrency(dataGridColumn->Index(), CurrentSlot(), DataGridInternal::DataGridSelectionAction::None, false /*scrollIntoView*/);
            assert(successfullyUpdatedSelection_);
            if (beginEdit &&
                editingColumnIndex_ == InvalidIndex &&
                CurrentSlot() != InvalidIndex &&
                CurrentColumnIndex() != InvalidIndex &&
                CurrentColumnIndex() == dataGridColumn->Index() &&
                dataGridColumn->OwningGrid() == *this &&
                !GetColumnEffectiveReadOnlyState(value))
            {
                // Returning to editing mode since the grid was in that mode prior to the EndCellEdit call above.
                BeginCellEdit(RoutedEventArgs{});
            }
        }
    }

    Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> DataGrid::SelectedItems() const
    {
        auto selectedItems = single_threaded_vector<Windows::Foundation::IInspectable>();

        for (auto const& item : selectedItems_.Items())
        {
            selectedItems.Append(item);
        }

        return selectedItems;
    }

    double DataGrid::ActualRowHeaderWidth() const
    {
        if (!AreRowHeadersVisible())
        {
            return 0;
        }

        return !std::isnan(RowHeaderWidth()) ? RowHeaderWidth() : RowHeadersDesiredWidth();
    }

    double DataGrid::ActualRowsPresenterHeight() const
    {
        if (rowsPresenter_)
        {
            return rowsPresenter_.ActualHeight();
        }

        return 0;
    }

    bool DataGrid::AllowsManipulation() const
    {
        return rowsPresenter_ &&
            (static_cast<uint32_t>(rowsPresenter_.ManipulationMode()) & (static_cast<uint32_t>(Input::ManipulationModes::TranslateX) | static_cast<uint32_t>(Input::ManipulationModes::TranslateY))) != static_cast<uint32_t>(Input::ManipulationModes::None);
    }

    bool DataGrid::AreColumnHeadersVisible() const
    {
        return static_cast<DataGridHeadersVisibility>(static_cast<uint32_t>(HeadersVisibility()) & static_cast<uint32_t>(DataGridHeadersVisibility::Column)) == DataGridHeadersVisibility::Column;
    }

    bool DataGrid::AreRowHeadersVisible() const
    {
        return static_cast<DataGridHeadersVisibility>(static_cast<uint32_t>(HeadersVisibility()) & static_cast<uint32_t>(DataGridHeadersVisibility::Row)) == DataGridHeadersVisibility::Row;
    }

    void DataGrid::AutoSizingColumns(bool const value)
    {
        if (autoSizingColumns_ && value == false)
        {
            auto const adjustment = CellsWidth() - ColumnsInternal().VisibleEdgedColumnsWidth();
            AdjustColumnWidths(0, adjustment, false);
            for (auto const& column : ColumnsInternal().GetVisibleColumns())
            {
                column.as<DataGridColumn>()->IsInitialDesiredWidthDetermined(true);
            }

            ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
            ComputeScrollBarsLayout();
            InvalidateColumnHeadersMeasure();
            InvalidateRowsMeasure(true);
        }

        autoSizingColumns_ = value;
    }

    double DataGrid::CellsHeight() const
    {
        return RowsPresenterAvailableSize().has_value() ? RowsPresenterAvailableSize()->Height : 0;
    }

    double DataGrid::CellsWidth() const
    {
        if (RowsPresenterAvailableSize().has_value())
        {
            return std::max(0.0, RowsPresenterAvailableSize()->Width - ActualRowHeaderWidth());
        }

        return ColumnsInternal().VisibleEdgedColumnsWidth();
    }

    void DataGrid::ColumnHeaderHasFocus(bool const value)
    {
        assert(!value || (ColumnHeaders() && AreColumnHeadersVisible()));

        if (columnHeaderHasFocus_ != value)
        {
            columnHeaderHasFocus_ = value;
            auto const currentColumn = CurrentColumn();

            if (currentColumn && IsSlotVisible(CurrentSlot()))
            {
                UpdateCurrentState(DisplayData().GetDisplayedElement(CurrentSlot()), CurrentColumnIndex(), true /*applyCellState*/);
            }

            auto const oldFocusedColumn = FocusedColumn();
            auto const internalOldFocusedColumn = oldFocusedColumn.try_as<DataGridColumn>();
            FocusedColumn(nullptr);

            if (columnHeaderHasFocus_)
            {
                FocusedColumn(!currentColumn ? ColumnsInternal().FirstVisibleNonFillerColumn() : currentColumn);
            }

            if (internalOldFocusedColumn && internalOldFocusedColumn->HasHeaderCell())
            {
                internalOldFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }

            if (auto const internalFocusedColumn = FocusedColumn().try_as<DataGridColumn>();
                internalFocusedColumn && internalFocusedColumn->HasHeaderCell())
            {
                internalFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
                ScrollColumnIntoView(internalFocusedColumn->Index());
            }
        }
    }

    std::vector<MiniDumpExplorer::DataGridColumn> const& DataGrid::ColumnsItemsInternal() const
    {
        return ColumnsInternal().ItemsInternal();
    }

    int32_t DataGrid::CurrentColumnIndex() const
    {
        return currentCellCoordinates_.ColumnIndex;
    }

    int32_t DataGrid::CurrentSlot() const
    {
        return currentCellCoordinates_.Slot;
    }

    bool DataGrid::HasColumnUserInteraction() const
    {
        return columnHeaderInteractionInfo_.HasUserInteraction();
    }

    void DataGrid::HorizontalOffset(double value)
    {
        if (value < 0)
        {
            value = 0;
        }

        if (auto const widthNotVisible = std::max(0.0, ColumnsInternal().VisibleEdgedColumnsWidth() - CellsWidth());
            value > widthNotVisible)
        {
            value = widthNotVisible;
        }

        if (value == horizontalOffset_)
        {
            return;
        }

        SetHorizontalOffset(value);

        horizontalOffset_ = value;

        DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());

        // update the lastTotallyDisplayedScrollingCol
        ComputeDisplayedColumns();
    }

    void DataGrid::NoCurrentCellChangeCount(int32_t const value)
    {
        noCurrentCellChangeCount_ = value;
        if (value == 0)
        {
            FlushCurrentCellChanged();
        }
    }

    void DataGrid::RowHeadersDesiredWidth(double const value)
    {
        // We only auto grow
        if (rowHeaderDesiredWidth_ < value)
        {
            auto const oldActualRowHeaderWidth = ActualRowHeaderWidth();
            rowHeaderDesiredWidth_ = value;
            if (oldActualRowHeaderWidth != ActualRowHeaderWidth())
            {
                // If we didn't invalidate in Ensure, and we have star columns, force the column widths to be recomputed here.
                if (auto const invalidated = EnsureRowHeaderWidth();
                    !invalidated && ColumnsInternal().VisibleStarColumnCount() > 0)
                {
                    ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
                    InvalidateMeasure();
                }
            }
        }
    }

    void DataGrid::RowsPresenterAvailableSize(std::optional<Windows::Foundation::Size> const value)
    {
        if (rowsPresenterAvailableSize_.has_value() && value.has_value() && value->Width > rowsPresenterAvailableSize_->Width)
        {
            // When the available cells width increases, the horizontal offset can be incorrect.
            // Store away an adjustment to use during the CellsPresenter's measure, so that the
            // ShouldDisplayCell method correctly determines if a cell will be in view.
            //
            //     |   h. offset   |       new available cells width          |
            //     |-------------->|----------------------------------------->|
            //      __________________________________________________        |
            //     |           |           |             |            |       |
            //     |  column0  |  column1  |   column2   |  column3   |<----->|
            //     |           |           |             |            |  adj. |
            auto const adjustment = horizontalOffset_ + value->Width - ColumnsInternal().VisibleEdgedColumnsWidth();
            horizontalAdjustment_ = std::min(HorizontalOffset(), std::max(0.0, adjustment));
        }
        else
        {
            horizontalAdjustment_ = 0;
        }

        auto const loadMoreDataFromIncrementalItemsSource = rowsPresenterAvailableSize_.has_value() && value.has_value() && value->Height > rowsPresenterAvailableSize_->Height;

        rowsPresenterAvailableSize_ = value;

        if (loadMoreDataFromIncrementalItemsSource)
        {
            LoadMoreDataFromIncrementalItemsSource();
        }
    }

    bool DataGrid::UsesStarSizing() const
    {
        return ColumnsInternal().VisibleStarColumnCount() > 0 &&
            (!rowsPresenterAvailableSize_.has_value() || !std::isinf(rowsPresenterAvailableSize_->Width));
    }

    void DataGrid::VerticalOffset(double const value)
    {
        auto const loadMoreDataFromIncrementalItemsSource = verticalOffset_ < value;

        verticalOffset_ = value;

        if (loadMoreDataFromIncrementalItemsSource)
        {
            LoadMoreDataFromIncrementalItemsSource();
        }
    }

    bool DataGrid::BeginEdit(RoutedEventArgs const& editingEventArgs)
    {
        if (CurrentColumnIndex() == InvalidIndex || !GetRowSelection(CurrentSlot()))
        {
            return false;
        }

        assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
        assert(CurrentSlot() < SlotCount());
        assert(!EditingRow() || EditingRow().as<DataGridRow>()->Slot() == CurrentSlot());

        if (GetColumnEffectiveReadOnlyState(CurrentColumn()))
        {
            // Current column is read-only
            return false;
        }

        return BeginCellEdit(editingEventArgs);
    }

    bool DataGrid::CommitEdit(DataGridEditingUnit const editingUnit, bool const exitEditingMode)
    {
        if (!EndCellEdit(DataGridEditAction::Commit, editingUnit == DataGridEditingUnit::Cell ? exitEditingMode : true, ContainsFocus() /*keepFocus*/, true /*raiseEvents*/))
        {
            return false;
        }

        if (editingUnit == DataGridEditingUnit::Row)
        {
            return EndRowEdit(DataGridEditAction::Commit, exitEditingMode, true /*raiseEvents*/);
        }

        return true;
    }

    Data::ICollectionViewGroup DataGrid::GetGroupFromItem(Windows::Foundation::IInspectable const& item, int32_t const groupLevel)
    {
        auto const itemIndex = DataConnection().IndexOf(item);
        if (itemIndex == InvalidIndex)
        {
            return nullptr;
        }

        auto groupHeaderSlot = RowGroupHeadersTable().GetPreviousIndex(SlotFromRowIndex(itemIndex));
        auto rowGroupInfo = RowGroupHeadersTable().GetValueAt(groupHeaderSlot);
        while (rowGroupInfo && rowGroupInfo->Level() != groupLevel)
        {
            groupHeaderSlot = RowGroupHeadersTable().GetPreviousIndex(rowGroupInfo->Slot());
            rowGroupInfo = RowGroupHeadersTable().GetValueAt(groupHeaderSlot);
        }

        return !rowGroupInfo ? nullptr : rowGroupInfo->CollectionViewGroup();
    }

    void DataGrid::ScrollIntoView(Windows::Foundation::IInspectable const& item, MiniDumpExplorer::DataGridColumn const& column)
    {
        auto const internalColumn = column.try_as<DataGridColumn>();
        if ((!internalColumn && (!item || FirstDisplayedNonFillerColumnIndex() == InvalidIndex)) ||
            (internalColumn && internalColumn->OwningGrid() != *this))
        {
            // no-op
            return;
        }

        if (!item)
        {
            // scroll column into view
            ScrollSlotIntoView(internalColumn->Index(), DisplayData().FirstScrollingSlot(), false /*forCurrentCellChange*/, true /*forceHorizontalScroll*/);
        }
        else
        {
            int32_t slot;
            std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> rowGroupInfo;

            if (auto const collectionViewGroup = item.try_as<Data::ICollectionViewGroup>();
                collectionViewGroup)
            {
                rowGroupInfo = RowGroupInfoFromCollectionViewGroup(collectionViewGroup);
                if (!rowGroupInfo)
                {
                    return;
                }

                slot = rowGroupInfo->Slot();
            }
            else
            {
                // the row index will be set to InvalidIndex if the item is null or not in the list
                auto const rowIndex = DataConnection().IndexOf(item);
                if (rowIndex == InvalidIndex || (IsReadOnly() && rowIndex == DataConnection().NewItemPlaceholderIndex()))
                {
                    return;
                }

                slot = SlotFromRowIndex(rowIndex);
            }

            auto const columnIndex = !internalColumn ? FirstDisplayedNonFillerColumnIndex() : internalColumn->Index();

            if (collapsedSlotsTable_.Contains(slot))
            {
                // We need to expand all parent RowGroups so that the slot is visible
                if (rowGroupInfo)
                {
                    ExpandRowGroupParentChain(rowGroupInfo->Level() - 1, rowGroupInfo->Slot());
                }
                else
                {
                    rowGroupInfo = RowGroupHeadersTable().GetValueAt(RowGroupHeadersTable().GetPreviousIndex(slot));
                    assert(rowGroupInfo);
                    if (rowGroupInfo)
                    {
                        ExpandRowGroupParentChain(rowGroupInfo->Level(), rowGroupInfo->Slot());
                    }
                }

                // Update ScrollBar and display information
                negVerticalOffset_ = 0;
                SetVerticalOffset(0);
                ResetDisplayedRows();
                DisplayData().FirstScrollingSlot(0);
                ComputeScrollBarsLayout();
            }

            ScrollSlotIntoView(columnIndex, slot, true /*forCurrentCellChange*/, true /*forceHorizontalScroll*/);
        }
    }

    Windows::Foundation::Size DataGrid::ArrangeOverride(Windows::Foundation::Size const& finalSize)
    {
        if (makeFirstDisplayedCellCurrentCellPending_)
        {
            MakeFirstDisplayedCellCurrentCell();
        }

        if (ActualWidth() != finalSize.Width)
        {
            // If our final width has changed, we might need to update the filler
            InvalidateColumnHeadersArrange();
            InvalidateCellsArrange();
        }

        return base_type::ArrangeOverride(finalSize);
    }

    Windows::Foundation::Size DataGrid::MeasureOverride(Windows::Foundation::Size const& availableSize)
    {
        // Delay layout until after the initial measure to avoid invalid calculations when the
        // DataGrid is not part of the visual tree
        if (!measured_)
        {
            measured_ = true;

            // We don't need to clear the rows because it was already done when the ItemsSource changed
            RefreshRowsAndColumns(false /*clearRows*/);

            // Update our estimates now that the DataGrid has all the information necessary
            UpdateRowDetailsHeightEstimate();

            // Update frozen columns to account for columns added prior to loading or auto-generated columns
            if (FrozenColumnCountWithFiller() > 0)
            {
                ProcessFrozenColumnCount();
            }
        }

        Windows::Foundation::Size desiredSize;

        // This is a shortcut to skip layout if we don't have any columns
        if (ColumnsInternal().ObservableItems().Size() == 0)
        {
            if (hScrollBar_ && hScrollBar_.Visibility() != Visibility::Collapsed)
            {
                hScrollBar_.Visibility(Visibility::Collapsed);
            }

            if (vScrollBar_ && vScrollBar_.Visibility() != Visibility::Collapsed)
            {
                vScrollBar_.Visibility(Visibility::Collapsed);
            }

            desiredSize = base_type::MeasureOverride(availableSize);
        }
        else
        {
            if (rowsPresenter_)
            {
                rowsPresenter_.InvalidateMeasure();
            }

            InvalidateColumnHeadersMeasure();

            desiredSize = base_type::MeasureOverride(availableSize);

            ComputeScrollBarsLayout();
        }

        return desiredSize;
    }

    void DataGrid::OnApplyTemplate()
    {
        // The template has changed, so we need to refresh the visuals
        measured_ = false;

        hasNoIndicatorStateStoryboardCompletedHandler_ = false;
        keepScrollBarsShowing_ = false;

        if (columnHeadersPresenter_)
        {
            // If we're applying a new template, we want to remove the old column headers first
            columnHeadersPresenter_.Children().Clear();
        }

        columnHeadersPresenter_ = GetTemplateChild(ElementColumnHeadersPresenterName).try_as<MiniDumpExplorer::DataGridColumnHeadersPresenter>();
        if (columnHeadersPresenter_)
        {
            if (auto const fillerColumn = ColumnsInternal().FillerColumn().try_as<DataGridFillerColumn>())
            {
                fillerColumn->IsRepresented(false);
            }

            columnHeadersPresenter_.as<DataGridColumnHeadersPresenter>()->OwningGrid(*this);

            // Columns were added before our Template was applied, add the ColumnHeaders now
            auto sortedInternal = ColumnsItemsInternal();
            std::ranges::sort(sortedInternal, [](auto const& lhs, auto const& rhs) { return lhs.template as<DataGridColumn>()->DisplayIndexWithFiller() < rhs.template as<DataGridColumn>()->DisplayIndexWithFiller(); });
            for (auto const& column : sortedInternal)
            {
                InsertDisplayedColumnHeader(column);
            }
        }

        if (rowsPresenter_)
        {
            // If we're applying a new template, we want to remove the old rows first
            UnloadElements(false /*recycle*/);
        }

        rowsPresenter_ = GetTemplateChild(ElementRowsPresenterName).try_as<MiniDumpExplorer::DataGridRowsPresenter>();
        if (rowsPresenter_)
        {
            rowsPresenter_.as<DataGridRowsPresenter>()->OwningGrid(*this);
            InvalidateRowHeightEstimate();
            UpdateRowDetailsHeightEstimate();
            UpdateRowsPresenterManipulationMode(true /*horizontalMode*/, true /*verticalMode*/);
        }

        frozenColumnScrollBarSpacer_ = GetTemplateChild(ElementFrozenColumnScrollBarSpacerName).try_as<FrameworkElement>();

        if (hScrollBar_)
        {
            isHorizontalScrollBarInteracting_ = false;
            isPointerOverHorizontalScrollBar_ = false;
            UnhookHorizontalScrollBarEvents();
        }

        hScrollBar_ = GetTemplateChild(ElementHorizontalScrollBarName).try_as<Controls::Primitives::ScrollBar>();
        if (hScrollBar_)
        {
            hScrollBar_.IsTabStop(false);
            hScrollBar_.Maximum(0.0);
            hScrollBar_.Orientation(Controls::Orientation::Horizontal);
            hScrollBar_.Visibility(Visibility::Collapsed);
            HookHorizontalScrollBarEvents();
        }

        if (vScrollBar_)
        {
            isVerticalScrollBarInteracting_ = false;
            isPointerOverVerticalScrollBar_ = false;
            UnhookVerticalScrollBarEvents();
        }

        vScrollBar_ = GetTemplateChild(ElementVerticalScrollBarName).try_as<Controls::Primitives::ScrollBar>();
        if (vScrollBar_)
        {
            vScrollBar_.IsTabStop(false);
            vScrollBar_.Maximum(0.0);
            vScrollBar_.Orientation(Controls::Orientation::Vertical);
            vScrollBar_.Visibility(Visibility::Collapsed);
            HookVerticalScrollBarEvents();
        }

        topLeftCornerHeader_ = GetTemplateChild(ElementTopLeftCornerHeaderName).try_as<Controls::ContentControl>();
        EnsureTopLeftCornerHeader(); // EnsureTopLeftCornerHeader checks for a null _topLeftCornerHeader;
        topRightCornerHeader_ = GetTemplateChild(ElementTopRightCornerHeaderName).try_as<Controls::ContentControl>();
        bottomRightCorner_ = GetTemplateChild(ElementBottomRightCornerHeaderName).try_as<UIElement>();

        if (auto const root = GetTemplateChild(ElementRootName).try_as<FrameworkElement>();
            root)
        {
            if (auto const rootVisualStateGroups = VisualStateManager::GetVisualStateGroups(root);
                rootVisualStateGroups)
            {
                auto const groupCount = rootVisualStateGroups.Size();

                for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
                {
                    if (auto const group = rootVisualStateGroups.GetAt(groupIndex);
                        group)
                    {
                        if (auto const visualStates = group.States();
                            visualStates)
                        {
                            auto const stateCount = visualStates.Size();

                            for (uint32_t stateIndex = 0; stateIndex < stateCount; ++stateIndex)
                            {
                                if (auto const state = visualStates.GetAt(stateIndex);
                                    state)
                                {
                                    auto const stateName = state.Name();


                                    if (auto const stateStoryboard = state.Storyboard();
                                        stateStoryboard)
                                    {
                                        if (stateName == VisualStates::StateNoIndicator)
                                        {
                                            [[maybe_unused]] auto const event = stateStoryboard.Completed({ this, &DataGrid::NoIndicatorStateStoryboard_Completed });

                                            hasNoIndicatorStateStoryboardCompletedHandler_ = true;
                                        }
                                        else if (stateName == VisualStates::StateTouchIndicator || stateName == VisualStates::StateMouseIndicator || stateName == VisualStates::StateMouseIndicatorFull)
                                        {
                                            [[maybe_unused]] auto const event = stateStoryboard.Completed({ this, &DataGrid::IndicatorStateStoryboard_Completed });
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        HideScrollBars(false /*useTransitions*/);

        UpdateDisabledVisual();
    }

    Automation::Peers::AutomationPeer DataGrid::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridAutomationPeer{ *this };
    }

    void DataGrid::OnPointerWheelChanged(Input::PointerRoutedEventArgs const& e)
    {
        base_type::OnPointerWheelChanged(e);
        if (e.Handled())
        {
            return;
        }

        auto const pointerPoint = e.GetCurrentPoint(*this);

        // A horizontal scroll happens if the mouse has a horizontal wheel OR if the horizontal scrollbar is not disabled AND the vertical scrollbar IS disabled
        auto const isForHorizontalScroll = pointerPoint.Properties().IsHorizontalMouseWheel() ||
            (HorizontalScrollBarVisibility() != Controls::ScrollBarVisibility::Disabled && VerticalScrollBarVisibility() == Controls::ScrollBarVisibility::Disabled);

        if ((isForHorizontalScroll && HorizontalScrollBarVisibility() == Controls::ScrollBarVisibility::Disabled) ||
            (!isForHorizontalScroll && VerticalScrollBarVisibility() == Controls::ScrollBarVisibility::Disabled))
        {
            return;
        }

        auto offsetDelta = -pointerPoint.Properties().MouseWheelDelta() / MouseWheelDeltaDivider;
        if (isForHorizontalScroll && pointerPoint.Properties().IsHorizontalMouseWheel())
        {
            offsetDelta *= -1.0;
        }

        e.Handled(ProcessScrollOffsetDelta(offsetDelta, isForHorizontalScroll));
    }

    bool DataGrid::CancelEdit(DataGridEditingUnit const editingUnit, bool const raiseEvents)
    {
        if (!EndCellEdit(DataGridEditAction::Cancel, true, ContainsFocus() /*keepFocus*/, raiseEvents))
        {
            return false;
        }

        if (editingUnit == DataGridEditingUnit::Row)
        {
            return EndRowEdit(DataGridEditAction::Cancel, true /*exitEditingMode*/, raiseEvents);
        }

        return true;
    }

    void DataGrid::CoerceSelectedItem()
    {
        Windows::Foundation::IInspectable selectedItem{ nullptr };

        if (SelectionMode() == DataGridSelectionMode::Extended &&
            CurrentSlot() != InvalidIndex &&
            selectedItems_.ContainsSlot(CurrentSlot()))
        {
            selectedItem = CurrentItem();
        }
        else if (selectedItems_.Count() > 0)
        {
            selectedItem = selectedItems_[0];
        }

        if (SelectedItem() != selectedItem)
        {
            SuspendedHandlers::SetValueNoCallback(*this, SelectedItemProperty(), selectedItem);
        }

        // Update the SelectedIndex
        auto newIndex = InvalidIndex;
        if (selectedItem)
        {
            newIndex = DataConnection().IndexOf(selectedItem);
        }

        if (SelectedIndex() != newIndex)
        {
            SuspendedHandlers::SetValueNoCallback(*this, SelectedIndexProperty(), box_value(newIndex));
        }
    }

    std::experimental::generator<Windows::Foundation::IInspectable> DataGrid::GetSelectionInclusive(int32_t const startRowIndex, int32_t const endRowIndex) const
    {
        auto const endSlot = SlotFromRowIndex(endRowIndex);
        for (auto const slot : selectedItems_.GetSlots(SlotFromRowIndex(startRowIndex)))
        {
            if (slot > endSlot)
            {
                break;
            }

            co_yield DataConnection().GetDataItem(RowIndexFromSlot(slot));
        }
    }

    void DataGrid::InitializeElements(bool const recycleRows)
    {
        ++noCurrentCellChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noCurrentCellChangeCount_;
            });

        // The underlying collection has changed and our editing row (if there is one)
        // is no longer relevant, so we should force a cancel edit.
        CancelEdit(DataGridEditingUnit::Row, false /*raiseEvents*/);

        // We want to persist selection throughout a reset, so store away the selected items
        auto const selectedItemsCache = selectedItems_.SelectedItemsCache();

        if (recycleRows)
        {
            RefreshRows(recycleRows /*recycleRows*/, true /*clearRows*/);
        }
        else
        {
            RefreshRowsAndColumns(true /*clearRows*/);
        }

        // Re-select the old items
        selectedItems_.SelectedItemsCache() = std::move(selectedItemsCache);
        CoerceSelectedItem();
        if (RowDetailsVisibilityMode() != DataGridRowDetailsVisibilityMode::Collapsed)
        {
            UpdateRowDetailsVisibilityMode(RowDetailsVisibilityMode());
        }

        // The currently displayed rows may have incorrect visual states because of the selection change
        ApplyDisplayedRowsState(DisplayData().FirstScrollingSlot(), DisplayData().LastScrollingSlot());
    }

    Windows::Foundation::IInspectable DataGrid::ItemFromSlot(int32_t const slot, int32_t& rowIndex)
    {
        if (RowGroupHeadersTable().Contains(slot))
        {
            if (auto const groupInfo = RowGroupHeadersTable().GetValueAt(slot);
                groupInfo)
            {
                return groupInfo->CollectionViewGroup();
            }
        }
        else
        {
            rowIndex = RowIndexFromSlot(slot);
            return DataConnection().GetDataItem(rowIndex);
        }

        return nullptr;
    }

    void DataGrid::LoadMoreDataFromIncrementalItemsSource()
    {
        LoadMoreDataFromIncrementalItemsSource(EdgedRowsHeightCalculated());
    }

    void DataGrid::OnRowDetailsChanged() const
    {
        if (!scrollingByHeight_)
        {
            // Update layout when RowDetails are expanded or collapsed, just updating the vertical scroll bar is not enough
            // since rows could be added or removed.
            InvalidateMeasure();
        }
    }

    void DataGrid::OnUserSorting()
    {
        isUserSorting_ = true;
    }

    void DataGrid::OnUserSorted()
    {
        isUserSorting_ = false;
    }

    bool DataGrid::ProcessDownKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessDownKeyInternal(shift, ctrl);
    }

    bool DataGrid::ProcessEndKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessEndKey(shift, ctrl);
    }

    bool DataGrid::ProcessEnterKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessEnterKey(shift, ctrl);
    }

    bool DataGrid::ProcessHomeKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessHomeKey(shift, ctrl);
    }

    void DataGrid::ProcessHorizontalScroll(Controls::Primitives::ScrollEventType const scrollEventType)
    {
        if (scrollEventType == Controls::Primitives::ScrollEventType::EndScroll)
        {
            IsHorizontalScrollBarInteracting(false);
        }
        else if (scrollEventType == Controls::Primitives::ScrollEventType::ThumbTrack)
        {
            IsHorizontalScrollBarInteracting(true);
        }

        if (horizontalScrollChangesIgnored_ > 0)
        {
            return;
        }

        // If the user scrolls with the buttons, we need to update the new value of the scroll bar since we delay
        // this calculation.  If they scroll in another other way, the scroll bar's correct value has already been set
        auto scrollBarValueDifference = 0.0;
        if (scrollEventType == Controls::Primitives::ScrollEventType::SmallIncrement)
        {
            scrollBarValueDifference = GetHorizontalSmallScrollIncrease();
        }
        else if (scrollEventType == Controls::Primitives::ScrollEventType::SmallDecrement)
        {
            scrollBarValueDifference = -GetHorizontalSmallScrollDecrease();
        }

        {
            ++horizontalScrollChangesIgnored_;
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --horizontalScrollChangesIgnored_;
                });
            if (scrollBarValueDifference != 0)
            {
                assert(horizontalOffset_ + scrollBarValueDifference >= 0);
                SetHorizontalOffset(horizontalOffset_ + scrollBarValueDifference);
            }

            UpdateHorizontalOffset(hScrollBar_.Value());
        }

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer)
        {
            peer->RaiseAutomationScrollEvents();
        }
    }

    bool DataGrid::ProcessLeftKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessLeftKey(shift, ctrl);
    }

    bool DataGrid::ProcessNextKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessNextKey(shift, ctrl);
    }

    bool DataGrid::ProcessPriorKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessPriorKey(shift, ctrl);
    }

    bool DataGrid::ProcessRightKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessRightKey(shift, ctrl);
    }

    bool DataGrid::ProcessScrollOffsetDelta(double offsetDelta, bool const isForHorizontalScroll)
    {
        if (IsEnabled() && DisplayData().NumDisplayedScrollingElements() > 0)
        {
            if (isForHorizontalScroll)
            {
                auto newHorizontalOffset = HorizontalOffset() + offsetDelta;
                if (newHorizontalOffset < 0)
                {
                    newHorizontalOffset = 0;
                }

                if (auto const maxHorizontalOffset = std::max(0.0, ColumnsInternal().VisibleEdgedColumnsWidth() - CellsWidth());
                    newHorizontalOffset > maxHorizontalOffset)
                {
                    newHorizontalOffset = maxHorizontalOffset;
                }

                if (newHorizontalOffset != HorizontalOffset())
                {
                    UpdateHorizontalOffset(newHorizontalOffset);
                    return true;
                }
            }
            else
            {
                if (offsetDelta < 0)
                {
                    offsetDelta = std::max(-verticalOffset_, offsetDelta);
                }
                else if (offsetDelta > 0)
                {
                    if (vScrollBar_ && VerticalScrollBarVisibility() == Controls::ScrollBarVisibility::Visible)
                    {
                        offsetDelta = std::min(std::max(0.0, vScrollBar_.Maximum() - verticalOffset_), offsetDelta);
                    }
                    else
                    {
                        auto const maximum = EdgedRowsHeightCalculated() - CellsHeight();
                        offsetDelta = std::min(std::max(0.0, maximum - verticalOffset_), offsetDelta);
                    }
                }

                if (offsetDelta != 0)
                {
                    DisplayData().PendingVerticalScrollHeight(offsetDelta);
                    InvalidateRowsMeasure(false /*invalidateIndividualRows*/);
                    return true;
                }
            }
        }

        return false;
    }

    void DataGrid::ProcessSelectionAndCurrency(int32_t columnIndex, Windows::Foundation::IInspectable const& item, int32_t const backupSlot, DataGridInternal::DataGridSelectionAction const action, bool const scrollIntoView)
    {
        ++noSelectionChangeCount_;
        ++noCurrentCellChangeCount_;

        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
                --noCurrentCellChangeCount_;
            });

        int32_t slot = InvalidIndex;

        if (auto const group = item.try_as<Data::ICollectionViewGroup>();
            group)
        {
            if (auto const groupInfo = RowGroupInfoFromCollectionViewGroup(group);
                groupInfo)
            {
                slot = groupInfo->Slot();
            }
        }
        else
        {
            slot = SlotFromRowIndex(DataConnection().IndexOf(item));
        }

        if (slot == InvalidIndex)
        {
            slot = backupSlot;
        }

        if (slot == InvalidIndex || slot > SlotCount())
        {
            return;
        }

        switch (action)
        {
        case DataGridInternal::DataGridSelectionAction::AddCurrentToSelection:
            SetRowSelection(slot, true /*isSelected*/, true /*setAnchorIndex*/);
            break;
        case DataGridInternal::DataGridSelectionAction::RemoveCurrentFromSelection:
            SetRowSelection(slot, false /*isSelected*/, false /*setAnchorRowIndex*/);
            break;
        case DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent:
            if (SelectionMode() == DataGridSelectionMode::Extended && AnchorSlot() != InvalidIndex)
            {
                auto const anchorSlot = AnchorSlot();
                ClearRowSelection(slot /*slotException*/, false /*resetAnchorSlot*/);
                if (slot <= anchorSlot)
                {
                    SetRowsSelection(slot, anchorSlot);
                }
                else
                {
                    SetRowsSelection(anchorSlot, slot);
                }
                break;
            }
            // fallthrough

        case DataGridInternal::DataGridSelectionAction::SelectCurrent:
            ClearRowSelection(slot /*rowIndexException*/, true /*setAnchorRowIndex*/);
            break;
        case DataGridInternal::DataGridSelectionAction::None:
            break;
        }

        if (CurrentSlot() != slot || (CurrentColumnIndex() != columnIndex && columnIndex != InvalidIndex))
        {
            if (columnIndex == InvalidIndex)
            {
                if (CurrentColumnIndex() != InvalidIndex)
                {
                    columnIndex = CurrentColumnIndex();
                }
                else if (auto const firstVisibleColumn = ColumnsInternal().FirstVisibleNonFillerColumn();
                    firstVisibleColumn)
                {
                    columnIndex = firstVisibleColumn.as<DataGridColumn>()->Index();
                }
            }

            if (columnIndex != InvalidIndex)
            {
                if (!SetCurrentCellCore(columnIndex, slot, true /*commitEdit*/, SlotFromRowIndex(SelectedIndex()) != slot /*endRowEdit*/)
                    || (scrollIntoView && !ScrollSlotIntoView(columnIndex, slot, true /*forCurrentCellChange*/, false /*forceHorizontalScroll*/)))
                {
                    return;
                }
            }
        }

        successfullyUpdatedSelection_ = true;
    }

    bool DataGrid::ProcessUpKey()
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessUpKey(shift, ctrl);
    }

    void DataGrid::ProcessVerticalScroll(Controls::Primitives::ScrollEventType const scrollEventType)
    {
        if (scrollEventType == Controls::Primitives::ScrollEventType::EndScroll)
        {
            IsVerticalScrollBarInteracting(false);
        }
        else if (scrollEventType == Controls::Primitives::ScrollEventType::ThumbTrack)
        {
            IsVerticalScrollBarInteracting(true);
        }

        if (verticalScrollChangesIgnored_ > 0)
        {
            return;
        }

        assert(double_utils::less_than_or_close(vScrollBar_.Value(), vScrollBar_.Maximum()));

        ++verticalScrollChangesIgnored_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --verticalScrollChangesIgnored_;
            });

        assert(vScrollBar_);
        if (scrollEventType == Controls::Primitives::ScrollEventType::SmallIncrement)
        {
            DisplayData().PendingVerticalScrollHeight(GetVerticalSmallScrollIncrease());

            if (auto const newVerticalOffset = verticalOffset_ + DisplayData().PendingVerticalScrollHeight();
                newVerticalOffset > vScrollBar_.Maximum())
            {
                DisplayData().PendingVerticalScrollHeight(DisplayData().PendingVerticalScrollHeight() - newVerticalOffset - vScrollBar_.Maximum());
            }
        }
        else if (scrollEventType == Controls::Primitives::ScrollEventType::SmallDecrement)
        {
            if (double_utils::greater_than(NegVerticalOffset(), 0.0))
            {
                DisplayData().PendingVerticalScrollHeight(DisplayData().PendingVerticalScrollHeight() - NegVerticalOffset());
            }
            else
            {

                if (auto const previousScrollingSlot = GetPreviousVisibleSlot(DisplayData().FirstScrollingSlot());
                    previousScrollingSlot != InvalidIndex)
                {
                    ScrollSlotIntoView(previousScrollingSlot, false /*scrolledHorizontally*/);
                }

                return;
            }
        }
        else
        {
            DisplayData().PendingVerticalScrollHeight(vScrollBar_.Value() - verticalOffset_);
        }

        if (!double_utils::is_zero(DisplayData().PendingVerticalScrollHeight()))
        {
            // Invalidate so the scroll happens on idle
            InvalidateRowsMeasure(false /*invalidateIndividualElements*/);
        }
    }

    void DataGrid::RefreshRowsAndColumns(bool const clearRows)
    {
        if (measured_)
        {
            ++noCurrentCellChangeCount_;
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noCurrentCellChangeCount_;
                });

            if (clearRows)
            {
                ClearRows(false);
                ClearRowGroupHeadersTable();
                PopulateRowGroupHeadersTable();
                RefreshSlotCounts();
            }

            if (AutoGenerateColumns())
            {
                // Column auto-generation refreshes the rows too
                AutoGenerateColumnsPrivate();
            }

            for (auto const& column : ColumnsItemsInternal())
            {
                // We don't need to refresh the state of AutoGenerated column headers because they're up-to-date
                if (auto const internalColumn = column.as<DataGridColumn>();
                    !internalColumn->IsAutoGenerated() && internalColumn->HasHeaderCell())
                {
                    internalColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(false);
                }
            }

            RefreshRows(false /*recycleRows*/, false /*clearRows*/);

            if (Columns().Size() > 0 && CurrentColumnIndex() == InvalidIndex)
            {
                MakeFirstDisplayedCellCurrentCell();
            }
            else
            {
                makeFirstDisplayedCellCurrentCellPending_ = false;
                desiredCurrentColumnIndex_ = InvalidIndex;
                FlushCurrentCellChanged();
            }
        }
        else
        {
            if (clearRows)
            {
                ClearRows(false /*recycle*/);
            }

            ClearRowGroupHeadersTable();
            PopulateRowGroupHeadersTable();
            RefreshSlotCounts();
        }
    }

    void DataGrid::ResetColumnHeaderInteractionInfo()
    {
        columnHeaderInteractionInfo_.CapturedPointer = nullptr;
        columnHeaderInteractionInfo_.DragMode = DataGridInternal::DataGridColumnHeaderInteractionInfo::drag_mode::None;
        columnHeaderInteractionInfo_.DragPointerId = 0;
        columnHeaderInteractionInfo_.DragColumn = nullptr;
        columnHeaderInteractionInfo_.DragStart = std::nullopt;
        columnHeaderInteractionInfo_.PressedPointerPositionHeaders = std::nullopt;
        columnHeaderInteractionInfo_.LastPointerPositionHeaders = std::nullopt;

        if (auto const columnHeaders = ColumnHeaders().try_as<DataGridColumnHeadersPresenter>();
            columnHeaders)
        {
            columnHeaders->DragColumn(nullptr);
            columnHeaders->DragIndicator(nullptr);
            columnHeaders->DropLocationIndicator(nullptr);
        }
    }

    bool DataGrid::ScrollSlotIntoView(int32_t const columnIndex, int32_t const slot, bool const forCurrentCellChange, bool const forceHorizontalScroll)
    {
        assert(columnIndex != InvalidIndex);
        assert(columnIndex < ColumnsItemsInternal().size());
        assert(DisplayData().FirstDisplayedScrollingCol() != InvalidIndex);
        assert(DisplayData().FirstDisplayedScrollingCol() < ColumnsItemsInternal().size());
        assert(DisplayData().LastTotallyDisplayedScrollingCol() != InvalidIndex);
        assert(DisplayData().LastTotallyDisplayedScrollingCol() < ColumnsItemsInternal().size());
        assert(!IsSlotOutOfBounds(slot));
        assert(DisplayData().FirstScrollingSlot() != InvalidIndex);
        assert(DisplayData().FirstScrollingSlot() < SlotCount());
        assert(ColumnsItemsInternal()[columnIndex].as<DataGridColumn>()->IsVisible());

        if (CurrentColumnIndex() != InvalidIndex &&
            (CurrentColumnIndex() != columnIndex || CurrentSlot() != slot))
        {
            if (!CommitEditForOperation(columnIndex, slot, forCurrentCellChange) || IsInnerCellOutOfBounds(columnIndex, slot))
            {
                return false;
            }
        }

        auto const oldHorizontalOffset = HorizontalOffset();

        // scroll horizontally unless we're on a RowGroupHeader and we're not forcing horizontal scrolling
        if (auto const rowGroupHeadersTableContainsSlot = RowGroupHeadersTable().Contains(slot);
            (forceHorizontalScroll || (slot != InvalidIndex && !rowGroupHeadersTableContainsSlot)) &&
            !ScrollColumnIntoView(columnIndex))
        {
            return false;
        }

        // scroll vertically
        if (!ScrollSlotIntoView(slot, oldHorizontalOffset != HorizontalOffset() /*scrolledHorizontally*/))
        {
            return false;
        }

        // Scrolling horizontally or vertically could cause fewer rows to be displayed
        DisplayData().FullyRecycleElements();

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer)
        {
            peer->RaiseAutomationScrollEvents();
        }

        return true;
    }

    bool DataGrid::SetCurrentCellCore(int32_t const columnIndex, int32_t const slot)
    {
        return SetCurrentCellCore(columnIndex, slot, true /*commitEdit*/, true /*endRowEdit*/);
    }

    void DataGrid::UpdateHorizontalOffset(double const newValue)
    {
        if (HorizontalOffset() != newValue)
        {
            HorizontalOffset(newValue);

            InvalidateColumnHeadersMeasure();
            InvalidateRowsMeasure(true);
        }
    }

    bool DataGrid::UpdateSelectionAndCurrency(int32_t columnIndex, int32_t const slot, DataGridInternal::DataGridSelectionAction const action, bool const scrollIntoView)
    {
        successfullyUpdatedSelection_ = false;

        ++noSelectionChangeCount_;
        ++noCurrentCellChangeCount_;

        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
                --noCurrentCellChangeCount_;
            });

        if (auto const internalRowGroupSpacerColumn = ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>();
            internalRowGroupSpacerColumn->IsRepresented() &&
            columnIndex == internalRowGroupSpacerColumn->Index())
        {
            columnIndex = InvalidIndex;
        }

        if (IsSlotOutOfSelectionBounds(slot) || (columnIndex != InvalidIndex && IsColumnOutOfBounds(columnIndex)))
        {
            return false;
        }

        auto newCurrentPosition = InvalidIndex;
        auto const item = ItemFromSlot(slot, newCurrentPosition);
        if (newCurrentPosition == DataConnection().NewItemPlaceholderIndex())
        {
            newCurrentPosition = InvalidIndex;
        }

        if (auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            internalEditingRow && slot != internalEditingRow->Slot() && !CommitEdit(DataGridEditingUnit::Row, true))
        {
            return false;
        }

        if (auto const collectionView = DataConnection().CollectionView();
            collectionView &&
            collectionView.CurrentPosition() != newCurrentPosition)
        {
            DataConnection().MoveCurrentTo(item, slot, columnIndex, action, scrollIntoView);
        }
        else
        {
            ProcessSelectionAndCurrency(columnIndex, item, slot, action, scrollIntoView);
        }

        return successfullyUpdatedSelection_;
    }

    void DataGrid::UpdateStateOnCurrentChanged(Windows::Foundation::IInspectable const& currentItem, int32_t const currentPosition)
    {
        if (currentItem == CurrentItem() &&
            (isUserSorting_ || (currentItem == SelectedItem() && currentPosition == SelectedIndex())))
        {
            // The DataGrid's CurrentItem is already up-to-date, so we don't need to do anything.
            // In the sorting case, we receive a CurrentChanged notification if the current item
            // changes position in the CollectionView.  However, our CurrentItem is already
            // in the correct position in this case, and we do not want to update the selection so
            // we no-op here.
            return;
        }

        auto columnIndex = CurrentColumnIndex();
        if (columnIndex == InvalidIndex)
        {
            if (auto const internalRowGroupSpacerColumn = ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>();
                IsColumnOutOfBounds(desiredCurrentColumnIndex_) ||
                (internalRowGroupSpacerColumn->IsRepresented() && desiredCurrentColumnIndex_ == internalRowGroupSpacerColumn->Index()))
            {
                columnIndex = FirstDisplayedNonFillerColumnIndex();
            }
            else
            {
                columnIndex = desiredCurrentColumnIndex_;
            }
        }

        // The CollectionView will potentially raise multiple CurrentChanged events during a single
        // add operation, so we should avoid resetting our desired column index until it's committed.
        if (!DataConnection().IsAddingNew())
        {
            desiredCurrentColumnIndex_ = InvalidIndex;
        }

        ++noSelectionChangeCount_;
        ++noCurrentCellChangeCount_;

        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
                --noCurrentCellChangeCount_;
            });

        if (!CommitEdit())
        {
            CancelEdit(DataGridEditingUnit::Row, false);
        }

        ClearRowSelection(true);
        if (!currentItem)
        {
            SetCurrentCellCore(InvalidIndex, InvalidIndex);
        }
        else
        {
            auto const slot = SlotFromRowIndex(currentPosition);
            ProcessSelectionAndCurrency(columnIndex, currentItem, slot, DataGridInternal::DataGridSelectionAction::SelectCurrent, false);
        }
    }

    bool DataGrid::UpdateStateOnTapped(Input::TappedRoutedEventArgs const& args, int32_t const columnIndex, int32_t const slot, bool const allowEdit)
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return UpdateStateOnTapped(args, columnIndex, slot, allowEdit, shift, ctrl);
    }

    void DataGrid::UpdateVerticalScrollBar()
    {
        if (vScrollBar_ && vScrollBar_.Visibility() == Visibility::Visible)
        {
            auto const cellsHeight = CellsHeight();
            auto const edgedRowsHeightCalculated = EdgedRowsHeightCalculated();
            UpdateVerticalScrollBar(
                edgedRowsHeightCalculated > cellsHeight /*needVertScrollBar*/,
                VerticalScrollBarVisibility() == Controls::ScrollBarVisibility::Visible /*forceVertScrollBar*/,
                edgedRowsHeightCalculated,
                cellsHeight);
        }
    }

    bool DataGrid::WaitForLostFocus(std::function<void()> action)
    {
        if (EditingRow() && EditingColumnIndex() != InvalidIndex && !executingLostFocusActions_)
        {
            auto const editingColumn = ColumnsItemsInternal()[EditingColumnIndex()];
            if (auto const editingElement = editingColumn.GetCellContent(EditingRow());
                editingElement && UIHelper::ContainsChild(editingElement, focusedObject_))
            {
                lostFocusActions_.emplace_back(std::move(action));
                lostFocusEvent_ = editingElement.LostFocus({ this, &DataGrid::EditingElement_LostFocus });
                IsTabStop(true);
                [[maybe_unused]] auto const result = Focus(FocusState::Programmatic);
                return true;
            }
        }

        return false;
    }

    MiniDumpExplorer::DataGridCell DataGrid::GetOwningCell(FrameworkElement element)
    {
        assert(!element);
        auto cell = element.try_as<MiniDumpExplorer::DataGridCell>();
        while (element && !cell)
        {
            element = element.Parent().try_as<FrameworkElement>();
            cell = element.try_as<MiniDumpExplorer::DataGridCell>();
        }

        return cell;
    }

    Windows::Foundation::IInspectable DataGrid::CurrentItem() const
    {
        if (CurrentSlot() == InvalidIndex ||
            RowGroupHeadersTable().Contains(CurrentSlot()) ||
            !ItemsSource())
        {
            return nullptr;
        }

        return DataConnection().GetDataItem(RowIndexFromSlot(CurrentSlot()));
    }

    auto DataGrid::LoadingOrUnloadingRowScope()
    {
        loadingOrUnloadingRow_ = true;
        return dlg_help_utils::make_scope_exit([this]()
            {
                loadingOrUnloadingRow_ = false;
            });
    }

    void DataGrid::OnAutoGeneratingColumn(MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs const& e)
    {
        autoGeneratingColumnHandler_(*this, e);
    }

    void DataGrid::OnBeginningEdit(MiniDumpExplorer::DataGridBeginningEditEventArgs const& e)
    {
        beginningEditHandler_(*this, e);
    }

    void DataGrid::OnCellEditEnded(MiniDumpExplorer::DataGridCellEditEndedEventArgs const& e)
    {
        cellEditEndedHandler_(*this, e);

        // Raise the automation invoke event for the cell that just ended edit

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::InvokePatternOnInvoked))
        {
            peer->RaiseAutomationInvokeEvents(DataGridEditingUnit::Cell, e.Column(), e.Row());
        }
    }

    void DataGrid::OnCellEditEnding(MiniDumpExplorer::DataGridCellEditEndingEventArgs const& e)
    {
        cellEditEndingHandler_(*this, e);
    }

    void DataGrid::OnCopyingRowClipboardContent(MiniDumpExplorer::DataGridRowClipboardEventArgs const& e)
    {
        copyingRowClipboardContentHandler_(*this, e);
    }

    void DataGrid::OnCurrentCellChanged(RoutedEventArgs const& e)
    {
        currentCellChangedHandler_(*this, e);

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementSelected))
        {
            peer->RaiseAutomationCellSelectedEvent(CurrentSlot(), CurrentColumnIndex());
        }
    }

    void DataGrid::OnLoadingRow(MiniDumpExplorer::DataGridRowEventArgs const& e)
    {
        assert(std::ranges::find(loadedRows_, e.Row()) == loadedRows_.end());
        loadedRows_.push_back(e.Row());
        auto handle = LoadingOrUnloadingRowScope();
        auto loadingRowHandle = dlg_help_utils::make_scope_exit([this, &e]()
            {
                auto const it = std::ranges::find(loadedRows_, e.Row());
                assert(it != loadedRows_.end());
                loadedRows_.erase(it);
            });

        loadingRowHandler_(*this, e);
    }

    void DataGrid::OnLoadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        loadingRowGroupHandler_(*this, e);
    }

    void DataGrid::OnLoadingRowDetails(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        loadingRowDetailsHandler_(*this, e);
    }

    void DataGrid::OnPreparingCellForEdit(MiniDumpExplorer::DataGridPreparingCellForEditEventArgs const& e)
    {
        preparingCellForEditHandler_(*this, e);

        // Raise the automation invoke event for the cell that just began edit because now
        // its editable content has been loaded
        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::InvokePatternOnInvoked))
        {
            peer->RaiseAutomationInvokeEvents(DataGridEditingUnit::Cell, e.Column(), e.Row());
        }
    }

    void DataGrid::OnRowEditEnded(MiniDumpExplorer::DataGridRowEditEndedEventArgs const& e)
    {
        rowEditEndedHandler_(*this, e);

        // Raise the automation invoke event for the row that just ended edit because the edits
        // to its associated item have either been committed or reverted
        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::InvokePatternOnInvoked))
        {
            peer->RaiseAutomationInvokeEvents(DataGridEditingUnit::Row, nullptr, e.Row());
        }
    }

    void DataGrid::OnRowEditEnding(MiniDumpExplorer::DataGridRowEditEndingEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        rowEditEndingHandler_(*this, e);
    }

    void DataGrid::OnSelectionChanged(Controls::SelectionChangedEventArgs const& e)
    {
        selectionChangedHandler_(*this, e);

        if (Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementSelected) ||
            Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementAddedToSelection) ||
            Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementRemovedFromSelection))
        {
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer)
            {
                peer->RaiseAutomationSelectionEvents(e);
            }
        }
    }

    void DataGrid::OnUnloadingRow(MiniDumpExplorer::DataGridRowEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        unloadingRowHandler_(*this, e);
    }

    void DataGrid::OnUnloadingRowDetails(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        unloadingRowDetailsHandler_(*this, e);
    }

    void DataGrid::OnUnloadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs const& e)
    {
        auto handle = LoadingOrUnloadingRowScope();
        unloadingRowGroupHandler_(*this, e);
    }

    void DataGrid::OnRowDoubleTapped(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e)
    {
        rowDoubleTappedHandler_(*this, e);
    }

    void DataGrid::OnAlternatingRowBackgroundPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->EnsureBackground();
        }
    }

    void DataGrid::OnAlternatingRowForegroundPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->EnsureForeground();
        }
    }

    void DataGrid::OnAreRowGroupHeadersFrozenPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        ProcessFrozenColumnCount();

        // Update elements in the RowGroupHeader that were previously frozen.
        if (unbox_value<bool>(e.NewValue()))
        {
            if (rowsPresenter_)
            {
                for (auto const& element : rowsPresenter_.Children())
                {
                    if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                        groupHeader)
                    {
                        groupHeader->ClearFrozenStates();
                    }
                }
            }
        }
    }

    void DataGrid::OnAutoGenerateColumnsPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (unbox_value<bool>(e.NewValue()))
        {
            InitializeElements(false /*recycleRows*/);
        }
        else
        {
            RemoveAutoGeneratedColumns();
        }
    }

    void DataGrid::OnCanUserResizeColumnsPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e)
    {
        EnsureHorizontalLayout();
    }

    void DataGrid::OnCellStylePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        auto const previousStyle = e.OldValue().as<Microsoft::UI::Xaml::Style>();
        for (auto const& row : GetAllRows())
        {
            auto const internalRow = row.as<DataGridRow>();
            for (auto const& cell : internalRow->Cells().GetCells())
            {
                auto const internalCell = cell.as<DataGridCell>();
                internalCell->EnsureStyle(previousStyle);
            }

            internalRow->FillerCell().as<DataGridCell>()->EnsureStyle(previousStyle);
        }

        InvalidateRowHeightEstimate();
    }

    void DataGrid::OnColumnHeaderHeightPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const value = unbox_value<double>(e.NewValue());
            if (value < MinimumColumnHeaderHeight)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"ColumnHeaderHeight", MinimumColumnHeaderHeight);
            }

            if (value > MaxHeadersThickness)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeLessThanOrEqualTo(L"value", L"ColumnHeaderHeight", MaxHeadersThickness);
            }

            InvalidateMeasure();
        }
    }

    void DataGrid::OnColumnHeaderStylePropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        // TODO: ColumnHeaderStyle should be applied to the TopLeftCorner and the TopRightCorner as well
        auto const previousStyle = e.OldValue().as<Microsoft::UI::Xaml::Style>();
        for (auto const& column : Columns())
        {
            auto const columnInternal = column.as<DataGridColumn>();
            columnInternal->HeaderCell().as<DataGridColumnHeader>()->EnsureStyle(previousStyle);
        }

        if (auto const fillerColumn = ColumnsInternal().FillerColumn())
        {
            auto const columnInternal = fillerColumn.as<DataGridColumn>();
            columnInternal->HeaderCell().as<DataGridColumnHeader>()->EnsureStyle(previousStyle);
        }
    }

    void DataGrid::OnColumnWidthPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e)
    {
        for (auto const& column : ColumnsInternal().GetDisplayedColumns())
        {

            if (auto const columnInternal = column.as<DataGridColumn>();
                columnInternal->InheritsWidth())
            {
                columnInternal->SetWidthInternalNoCallback(ColumnWidth());
            }
        }

        EnsureHorizontalLayout();
    }

    void DataGrid::OnDataFetchSizePropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const newValue = unbox_value<double>(e.NewValue());

            if (std::isnan(newValue))
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueCannotBeSetToNAN(L"DataFetchSize");
            }

            if (newValue < 0)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"DataFetchSize", 0);
            }
        }
    }

    void DataGrid::OnFrozenColumnCountPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            if (unbox_value<int32_t>(e.NewValue()) == -1)
            {
                SuspendedHandlers::SetValueNoCallback(*this, FrozenColumnCountProperty(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"FrozenColumnCount", 0);
            }

            ProcessFrozenColumnCount();
        }
    }

    void DataGrid::OnGridLinesVisibilityPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        for (auto const& row : GetAllRows())
        {
            auto const internalRow = row.as<DataGridRow>();
            internalRow->EnsureGridLines();
            internalRow->InvalidateHorizontalArrange();
        }

        for (auto const& rowGroupHeader : GetAllRowGroupHeaders())
        {
            auto const internalRowGroupHeader = rowGroupHeader.as<DataGridRowGroupHeader>();
            internalRowGroupHeader->EnsureGridLine();
        }
    }

    void DataGrid::OnHeadersVisibilityPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        auto const newValue = unbox_value<DataGridHeadersVisibility>(e.NewValue());
        auto const oldValue = unbox_value<DataGridHeadersVisibility>(e.OldValue());

        auto const hasFlags = [](DataGridHeadersVisibility value, DataGridHeadersVisibility flags) { return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flags)) == static_cast<uint32_t>(flags); };

        auto const newValueCols = hasFlags(newValue, DataGridHeadersVisibility::Column);
        auto const newValueRows = hasFlags(newValue, DataGridHeadersVisibility::Row);
        auto const oldValueCols = hasFlags(oldValue, DataGridHeadersVisibility::Column);
        auto const oldValueRows = hasFlags(oldValue, DataGridHeadersVisibility::Row);

        // Columns
        if (newValueCols != oldValueCols)
        {
            if (columnHeadersPresenter_)
            {
                EnsureColumnHeadersVisibility();
                if (!newValueCols)
                {
                    columnHeadersPresenter_.Measure({ 0.0, 0.0 });
                }
                else
                {
                    EnsureVerticalGridLines();
                }

                InvalidateMeasure();
            }
        }

        // Rows
        if (newValueRows != oldValueRows && rowsPresenter_)
        {
            for (auto const& element : rowsPresenter_.Children())
            {

                if (auto const row = element.try_as<DataGridRow>();
                    row)
                {
                    row->EnsureHeaderStyleAndVisibility(nullptr);
                    if (newValueRows)
                    {
                        row->ApplyState(false /*animate*/);
                        row->EnsureHeaderVisibility();
                    }
                }
                else if (auto const rowGroupHeader = element.try_as<DataGridRowGroupHeader>();
                    rowGroupHeader)
                {
                    rowGroupHeader->EnsureHeaderStyleAndVisibility(nullptr);
                }
            }

            InvalidateRowHeightEstimate();
            InvalidateRowsMeasure(true /*invalidateIndividualElements*/);
        }

        // TODO: This isn't necessary if the TopLeftCorner and the TopRightCorner Autosize to 0.
        // See if their templates can be changed to do that.
        if (topLeftCornerHeader_)
        {
            topLeftCornerHeader_.Visibility(newValueRows && newValueCols ? Visibility::Visible : Visibility::Collapsed);
            if (topLeftCornerHeader_.Visibility() == Visibility::Collapsed)
            {
                topLeftCornerHeader_.Measure({ 0.0, 0.0 });
            }
        }
    }

    void DataGrid::OnHorizontalGridLinesBrushPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()) && rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                auto const internalRow = row.as<DataGridRow>();
                internalRow->EnsureGridLines();
            }

            for (auto const& rowGroupHeader : GetAllRowGroupHeaders())
            {
                auto const internalRowGroupHeader = rowGroupHeader.as<DataGridRowGroupHeader>();
                internalRowGroupHeader->EnsureGridLine();
            }
        }
    }

    void DataGrid::OnHorizontalScrollBarVisibilityPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()) && unbox_value<Controls::ScrollBarVisibility>(e.NewValue()) != unbox_value<Controls::ScrollBarVisibility>(e.OldValue()))
        {
            UpdateRowsPresenterManipulationMode(true /*horizontalMode*/, false /*verticalMode*/);

            if (hScrollBar_)
            {
                if (IsHorizontalScrollBarOverCells())
                {
                    ComputeScrollBarsLayout();
                }
                else
                {
                    InvalidateMeasure();
                }
            }
        }
    }

    void DataGrid::OnIsReadOnlyPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            if (unbox_value<bool>(e.NewValue()) && !CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/))
            {
                CancelEdit(DataGridEditingUnit::Row, false /*raiseEvents*/);
            }
        }
    }

    void DataGrid::OnIsValidPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            SuspendedHandlers::SetValueNoCallback(*this, IsValidProperty(), e.OldValue());
            throw DataGridError::DataGrid::UnderlyingPropertyIsReadOnly(L"IsValid");
        }
    }

    void DataGrid::OnIncrementalLoadingThresholdPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const newValue = unbox_value<double>(e.NewValue());

            if (std::isnan(newValue))
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueCannotBeSetToNAN(L"IncrementalLoadingThreshold");
            }

            if (newValue < 0)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"IncrementalLoadingThreshold", 0);
            }

            if (newValue > unbox_value<double>(e.OldValue()))
            {
                LoadMoreDataFromIncrementalItemsSource();
            }
        }
    }

    void DataGrid::OnItemsSourcePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            if (LoadingOrUnloadingRow())
            {
                SuspendedHandlers::SetValueNoCallback(*this, ItemsSourceProperty(), e.OldValue());
                throw DataGridError::DataGrid::CannotChangeItemsWhenLoadingRows();
            }

            // Try to commit edit on the old DataSource, but force a cancel if it fails.
            if (!CommitEdit())
            {
                CancelEdit(DataGridEditingUnit::Row, false);
            }

            DataConnection().UnWireEvents(DataConnection().DataSource());
            DataConnection().ClearDataProperties();

            ClearRowGroupHeadersTable();

            // The old selected indexes are no longer relevant. There's a perf benefit from
            // updating the selected indexes with a null DataSource, because we know that all the
            // previously selected indexes have been removed from selection.
            DataConnection().DataSource(nullptr);
            selectedItems_.UpdateIndexes();
            CoerceSelectedItem();

            // Wrap an IEnumerable in an ICollectionView if it's not already one.
            auto const newItemsSource = e.NewValue().try_as<IDataGridDataSource>();
            DataConnection().DataSource(newItemsSource);

            if (newItemsSource)
            {
                // Set up the column headers.
                if (DataConnection().HasDataType())
                {
                    for (auto const& boundColumn : ColumnsInternal().GetDisplayedColumns([](auto const& column) { return static_cast<bool>(column.template try_as<MiniDumpExplorer::DataGridBoundColumn>()); }))
                    {
                        boundColumn.as<DataGridBoundColumn>()->SetHeaderFromBinding();
                    }
                }

                DataConnection().WireEvents(newItemsSource);
            }

            // Wait for the current cell to be set before we raise any SelectionChanged events.
            makeFirstDisplayedCellCurrentCellPending_ = true;

            // Clear out the old rows and remove the generated columns.
            ClearRows(false /*recycle*/);
            RemoveAutoGeneratedColumns();

            // Set the SlotCount (from the data count and number of row group headers) before we make the default selection.
            PopulateRowGroupHeadersTable();
            RefreshSlotCounts();

            SelectedItem(nullptr);
            if (auto const collectionView = DataConnection().CollectionView())
            {
                SelectedItem(collectionView.CurrentItem());
            }

            // Treat this like the DataGrid has never been measured because all calculations at
            // this point are invalid until the next layout cycle.  For instance, the ItemsSource
            // can be set when the DataGrid is not part of the visual tree.
            measured_ = false;
            InvalidateMeasure();
        }
    }

    void DataGrid::OnMaxColumnWidthPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const newValue = unbox_value<double>(e.NewValue());

            if (std::isnan(newValue))
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueCannotBeSetToNAN(L"MaxColumnWidth");
            }

            if (newValue < 0)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"MaxColumnWidth", 0);
            }

            if (MinColumnWidth() > newValue)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"MaxColumnWidth", L"MinColumnWidth"sv);
            }

            auto const oldValue = unbox_value<double>(e.OldValue());
            for (auto const& column : ColumnsInternal().GetDisplayedColumns())
            {
                OnColumnMaxWidthChanged(column, std::min(column.MaxWidth(), oldValue));
            }
        }
    }

    void DataGrid::OnMinColumnWidthPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const newValue = unbox_value<double>(e.NewValue());

            if (std::isnan(newValue))
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueCannotBeSetToNAN(L"MinColumnWidth");
            }

            if (newValue < 0)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"MinColumnWidth", 0);
            }

            if (MinColumnWidth() > newValue)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"MinColumnWidth", L"MaxColumnWidth"sv);
            }

            auto const oldValue = unbox_value<double>(e.OldValue());
            for (auto const& column : ColumnsInternal().GetDisplayedColumns())
            {
                OnColumnMinWidthChanged(column, std::max(column.MinWidth(), oldValue));
            }
        }
    }

    void DataGrid::OnRowBackgroundPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        // Go through the Displayed rows and update the background
        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->EnsureBackground();
        }
    }

    void DataGrid::OnRowDetailsTemplatePropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e)
    {
        // Update the RowDetails templates if necessary
        if (rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                if (GetRowDetailsVisibility(row.Index()) == Visibility::Visible)
                {
                    // DetailsPreferredHeight is initialized when the DetailsElement's size changes.
                    row.as<DataGridRow>()->ApplyDetailsTemplate(false /*initializeDetailsPreferredHeight*/);
                }
            }
        }

        UpdateRowDetailsHeightEstimate();
        InvalidateMeasure();
    }

    void DataGrid::OnRowDetailsVisibilityModePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        UpdateRowDetailsVisibilityMode(unbox_value<DataGridRowDetailsVisibilityMode>(e.NewValue()));
    }

    void DataGrid::OnRowForegroundPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        // Go through the Displayed rows and update the foreground
        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->EnsureForeground();
        }
    }

    void DataGrid::OnRowHeightPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const value = unbox_value<double>(e.NewValue());

            if (value < DataGridRow::MinimumHeight)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"RowHeight", DataGridRow::MinimumHeight);
            }

            if (value > DataGridRow::MaximumHeight)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeLessThanOrEqualTo(L"value", L"RowHeight", DataGridRow::MaximumHeight);
            }

            InvalidateRowHeightEstimate();

            // Re-measure all the rows due to the Height change
            InvalidateRowsMeasure(true);

            // DataGrid needs to update the layout information and the ScrollBars
            InvalidateMeasure();
        }
    }

    void DataGrid::OnRowHeaderWidthPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const value = unbox_value<double>(e.NewValue());

            if (value < MinimumRowHeaderWidth)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"RowHeaderWidth", MinimumRowHeaderWidth);
            }

            if (value > MaxHeadersThickness)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                throw DataGridError::ValueMustBeLessThanOrEqualTo(L"value", L"RowHeaderWidth", MaxHeadersThickness);
            }

            EnsureRowHeaderWidth();
        }
    }

    void DataGrid::OnRowHeaderStylePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (rowsPresenter_)
        {
            // Set HeaderStyle for displayed rows
            auto const previousStyle = e.OldValue().as<Microsoft::UI::Xaml::Style>();
            for (auto const& element : rowsPresenter_.Children())
            {

                if (auto const row = element.try_as<DataGridRow>();
                    row)
                {
                    row->EnsureHeaderStyleAndVisibility(previousStyle);
                }
                else if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                    groupHeader)
                {
                    groupHeader->EnsureHeaderStyleAndVisibility(previousStyle);
                }
            }

            InvalidateRowHeightEstimate();
        }
    }

    void DataGrid::OnRowStylePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (rowsPresenter_)
        {
            // Set the style for displayed rows if it has not already been set
            for (auto const& row : GetAllRows())
            {
                EnsureElementStyle(row, e.OldValue().as<Microsoft::UI::Xaml::Style>(), e.NewValue().as<Microsoft::UI::Xaml::Style>());
            }
        }

        InvalidateRowHeightEstimate();
    }

    void DataGrid::OnSelectionModePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            ClearRowSelection(true /*resetAnchorSlot*/);
        }
    }

    void DataGrid::OnSelectedIndexPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const index = unbox_value<int>(e.NewValue());

            // GetDataItem returns null if index is >= Count, we do not check newValue
            // against Count here to avoid enumerating through an Enumerable twice
            // Setting SelectedItem coerces the final value of the SelectedIndex
            auto const newSelectedItem = index == InvalidIndex ? nullptr : DataConnection().GetDataItem(index);
            SelectedItem(newSelectedItem);
            if (SelectedItem() != newSelectedItem)
            {
                SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
            }
        }
    }

    void DataGrid::OnSelectedItemPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            auto const selectedItem = e.NewValue();
            if (auto const rowIndex = !selectedItem ? InvalidIndex : DataConnection().IndexOf(selectedItem);
                rowIndex == InvalidIndex)
            {
                // If the Item is null, or it's not found, clear the Selection
                if (!CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/))
                {
                    // Edited value couldn't be committed or aborted
                    SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                    return;
                }

                // Clear all row selections
                ClearRowSelection(true /*resetAnchorSlot*/);
            }
            else
            {
                auto const slot = SlotFromRowIndex(rowIndex);
                if (slot != CurrentSlot())
                {
                    if (!CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/))
                    {
                        // Edited value couldn't be committed or aborted
                        SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                        return;
                    }

                    if (slot >= SlotCount() && slot != InvalidIndex)
                    {
                        if (DataConnection().CollectionView())
                        {
                            [[maybe_unused]] auto result = DataConnection().CollectionView().MoveCurrentToPosition(static_cast<int32_t>(rowIndex));
                        }
                    }
                }

                auto const oldSelectedIndex = SelectedIndex();
                if (oldSelectedIndex != rowIndex)
                {
                    SuspendedHandlers::SetValueNoCallback(*this, SelectedIndexProperty(), box_value(rowIndex));
                }

                ++noSelectionChangeCount_;
                auto noSelectionChangeCountHandle = dlg_help_utils::make_scope_exit([this]()
                    {
                        --noSelectionChangeCount_;
                    });


                auto columnIndex = CurrentColumnIndex();
                if (columnIndex == InvalidIndex)
                {
                    columnIndex = FirstDisplayedNonFillerColumnIndex();
                }

                if (IsSlotOutOfSelectionBounds(slot))
                {
                    ClearRowSelection(slot /*slotException*/, true /*resetAnchorSlot*/);
                    return;
                }

                UpdateSelectionAndCurrency(columnIndex, slot, DataGridInternal::DataGridSelectionAction::SelectCurrent, false /*scrollIntoView*/);

                if (!successfullyUpdatedSelection_)
                {
                    SuspendedHandlers::SetValueNoCallback(*this, SelectedIndexProperty(), box_value(oldSelectedIndex));
                    SuspendedHandlers::SetValueNoCallback(*this, e.Property(), e.OldValue());
                }
            }
        }
    }

    void DataGrid::OnVerticalGridLinesBrushPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        if (rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                row.as<DataGridRow>()->EnsureGridLines();
            }
        }
    }

    void DataGrid::OnVerticalScrollBarVisibilityPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()) && unbox_value<Controls::ScrollBarVisibility>(e.NewValue()) != unbox_value<Controls::ScrollBarVisibility>(e.OldValue()))
        {
            UpdateRowsPresenterManipulationMode(false /*horizontalMode*/, true /*verticalMode*/);

            if (vScrollBar_)
            {
                if (IsVerticalScrollBarOverCells())
                {
                    ComputeScrollBarsLayout();
                }
                else
                {
                    InvalidateMeasure();
                }
            }
        }
    }

    void DataGrid::ProcessFrozenColumnCount()
    {
        CorrectColumnFrozenStates();
        ComputeScrollBarsLayout();

        InvalidateColumnHeadersArrange();
        InvalidateCellsArrange();
    }

    void DataGrid::CurrentColumnIndex(int32_t const value)
    {
        currentCellCoordinates_.ColumnIndex = value;
    }

    void DataGrid::CurrentSlot(int32_t const value)
    {
        currentCellCoordinates_.Slot = value;
    }

    bool DataGrid::AreAllScrollBarsCollapsed() const
    {
        return (!hScrollBar_ || hScrollBar_.Visibility() == Visibility::Collapsed) &&
            (!vScrollBar_ || vScrollBar_.Visibility() == Visibility::Collapsed);
    }

    bool DataGrid::AreBothScrollBarsVisible() const
    {
        return hScrollBar_ && hScrollBar_.Visibility() == Visibility::Visible &&
            vScrollBar_ && vScrollBar_.Visibility() == Visibility::Visible;
    }

    int32_t DataGrid::FirstDisplayedNonFillerColumnIndex() const
    {

        if (auto const column = ColumnsInternal().FirstVisibleNonFillerColumn().try_as<DataGridColumn>();
            column)
        {
            if (column->IsFrozen())
            {
                return column->Index();
            }

            if (DisplayData().FirstDisplayedScrollingCol() >= static_cast<int32_t>(column->Index()))
            {
                return DisplayData().FirstDisplayedScrollingCol();
            }

            return column->Index();
        }

        return InvalidIndex;
    }

    void DataGrid::IsHorizontalScrollBarInteracting(bool const value)
    {
        if (isHorizontalScrollBarInteracting_ != value)
        {
            isHorizontalScrollBarInteracting_ = value;

            if (hScrollBar_)
            {
                if (isHorizontalScrollBarInteracting_)
                {
                    // Prevent the vertical scroll bar from fading out while the user is interacting with the horizontal one.
                    keepScrollBarsShowing_ = true;

                    ShowScrollBars();
                }
                else
                {
                    // Make the scroll bars fade out, after the normal delay.
                    keepScrollBarsShowing_ = false;

                    HideScrollBars(true /*useTransitions*/);
                }
            }
        }
    }

    bool DataGrid::IsHorizontalScrollBarOverCells() const
    {
        return columnHeadersPresenter_ && Controls::Grid::GetColumnSpan(columnHeadersPresenter_) == 2;
    }

    void DataGrid::IsVerticalScrollBarInteracting(bool const value)
    {
        if (isVerticalScrollBarInteracting_ != value)
        {
            isVerticalScrollBarInteracting_ = value;

            if (vScrollBar_)
            {
                if (isVerticalScrollBarInteracting_)
                {
                    // Prevent the horizontal scroll bar from fading out while the user is interacting with the vertical one.
                    keepScrollBarsShowing_ = true;

                    ShowScrollBars();
                }
                else
                {
                    // Make the scroll bars fade out, after the normal delay.
                    keepScrollBarsShowing_ = false;

                    HideScrollBars(true /*useTransitions*/);
                }
            }
        }
    }

    bool DataGrid::IsVerticalScrollBarOverCells() const
    {
        return rowsPresenter_ && Controls::Grid::GetRowSpan(rowsPresenter_) == 2;
    }

    void DataGrid::NoSelectionChangeCount(int32_t const value)
    {
        noSelectionChangeCount_ = value;
        if (value == 0)
        {
            FlushSelectionChanged();
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGrid::AddNewItem([[maybe_unused]] RoutedEventArgs const& editingEventArgs)
    {
        return false;
    }

    void DataGrid::AddNewCellPrivate(MiniDumpExplorer::DataGridRow const& row, MiniDumpExplorer::DataGridColumn const& column)
    {
        MiniDumpExplorer::DataGridCell const newCell{};
        PopulateCellContent(false /*isCellEdited*/, column, row, newCell);

        auto const internalRow = row.as<DataGridRow>();
        auto const internalColumn = column.as<DataGridColumn>();
        auto const internalCell = newCell.as<DataGridCell>();

        if (internalRow->OwningGrid())
        {
            internalCell->OwningColumn(column);
            newCell.Visibility(column.Visibility());
        }

        if (column.try_as<MiniDumpExplorer::DataGridFillerColumn>())
        {
            Automation::AutomationProperties::SetAccessibilityView(
                newCell,
                Automation::Peers::AccessibilityView::Raw);
        }

        internalCell->EnsureStyle(nullptr);
        internalRow->Cells().Insert(internalColumn->Index(), newCell);
    }

    bool DataGrid::BeginCellEdit(RoutedEventArgs const& editingEventArgs)
    {
        if (CurrentColumnIndex() == InvalidIndex || !GetRowSelection(CurrentSlot()))
        {
            return false;
        }

        assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
        assert(CurrentSlot() < SlotCount());
        assert(!EditingRow() || EditingRow().as<DataGridRow>()->Slot() == CurrentSlot());
        assert(!GetColumnEffectiveReadOnlyState(CurrentColumn()));
        assert(CurrentColumn().as<DataGridColumn>()->IsVisible());

        if (editingColumnIndex_ != InvalidIndex)
        {
            // Current cell is already in edit mode
            assert(editingColumnIndex_ == CurrentColumnIndex());
            return true;
        }

        // When we begin edit on the NewItemPlaceHolder row, we should try to add a new item.
        if (CurrentSlot() == SlotFromRowIndex(DataConnection().NewItemPlaceholderIndex()))
        {
            return AddNewItem(editingEventArgs);
        }

        // Get or generate the editing row if it doesn't exist
        auto dataGridRow = EditingRow();
        if (!dataGridRow)
        {
            assert(!RowGroupHeadersTable().Contains(CurrentSlot()));

            if (IsSlotVisible(CurrentSlot()))
            {
                dataGridRow = DisplayData().GetDisplayedElement(CurrentSlot()).try_as<MiniDumpExplorer::DataGridRow>();
                assert(dataGridRow);
            }
            else
            {
                dataGridRow = GenerateRow(RowIndexFromSlot(CurrentSlot()), CurrentSlot());
                dataGridRow.Clip({});
            }

            if (DataConnection().IsAddingNew())
            {
                // We just began editing the new item row, so set a flag that prevents us from running
                // full entity validation until the user explicitly attempts to end editing the row.
                initializingNewItem_ = true;
            }
        }

        assert(dataGridRow);

        // Cache these to see if they change later
        auto const currentRowIndex = CurrentSlot();
        auto const currentColumnIndex = CurrentColumnIndex();

        // Raise the BeginningEdit event
        auto const dataGridCell = dataGridRow.as<DataGridRow>()->Cells()[CurrentColumnIndex()];
        MiniDumpExplorer::DataGridBeginningEditEventArgs const e{ CurrentColumn(), dataGridRow, editingEventArgs };
        OnBeginningEdit(e);
        auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
        if (e.Cancel() ||
            currentRowIndex != CurrentSlot() ||
            currentColumnIndex != CurrentColumnIndex() ||
            !GetRowSelection(CurrentSlot()) ||
            (!internalEditingRow && !BeginRowEdit(dataGridRow)))
        {
            // If either BeginningEdit was canceled, currency/selection was changed in the event handler,
            // or we failed opening the row for edit, then we can no longer continue BeginCellEdit
            return false;
        }

        if (!internalEditingRow || internalEditingRow->Slot() != CurrentSlot())
        {
            // This check was added to safeguard against a ListCollectionView bug where the collection changed currency
            // during a CommitNew operation but failed to raise a CurrentChanged event.
            return false;
        }

        // Finally, we can prepare the cell for editing
        editingColumnIndex_ = CurrentColumnIndex();
        editingEventArgs_ = editingEventArgs;
        internalEditingRow->Cells()[CurrentColumnIndex()].as<DataGridCell>()->ApplyCellState(true /*animate*/);
        PopulateCellContent(true /*isCellEdited*/, CurrentColumn(), dataGridRow, dataGridCell);
        return true;
    }

    bool DataGrid::BeginRowEdit(MiniDumpExplorer::DataGridRow const& dataGridRow)
    {
        assert(!EditingRow());
        assert(dataGridRow);

        assert(CurrentSlot() != InvalidIndex);
        assert(CurrentSlot() < SlotCount());

        if (DataConnection().BeginEdit(dataGridRow.DataContext()))
        {
            editingRow_ = dataGridRow;
            GenerateEditingElements();
            ValidateEditingRow(false /*scrollIntoView*/, true /*wireEvents*/);

            // Raise the automation invoke event for the row that just began edit
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::InvokePatternOnInvoked))
            {
                peer->RaiseAutomationInvokeEvents(DataGridEditingUnit::Row, nullptr, dataGridRow);
            }

            return true;
        }

        return false;
    }

    bool DataGrid::CancelRowEdit(bool const exitEditingMode)
    {
        if (!EditingRow())
        {
            return true;
        }

        auto const internalEditingRow = EditingRow().as<DataGridRow>();
        assert(internalEditingRow);
        assert(internalEditingRow->Index() != InvalidIndex);
        assert(internalEditingRow->Slot() < SlotCount());
        assert(CurrentColumn());

        if (!DataConnection().CancelEdit(EditingRow().DataContext()))
        {
            return false;
        }

        for (auto const& column : Columns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            if (!exitEditingMode && internalColumn->Index() == editingColumnIndex_ && column.try_as<DataGridBoundColumn>())
            {
                continue;
            }

            PopulateCellContent(!exitEditingMode && internalColumn->Index() == editingColumnIndex_ /*isCellEdited*/, column, EditingRow(), internalEditingRow->Cells()[internalColumn->Index()]);
        }

        return true;
    }

    bool DataGrid::CommitEditForOperation(int32_t const columnIndex, int32_t const slot, bool const forCurrentCellChange)
    {
        if (forCurrentCellChange)
        {
            if (!EndCellEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, true /*keepFocus*/, true /*raiseEvents*/))
            {
                return false;
            }

            if (CurrentSlot() != slot &&
                !EndRowEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, true /*raiseEvents*/))
            {
                return false;
            }
        }

        if (IsColumnOutOfBounds(columnIndex))
        {
            return false;
        }

        if (slot >= SlotCount())
        {
            // Current cell was reset because the commit deleted row(s).
            // Since the user wants to change the current cell, we don't
            // want to end up with no current cell. We pick the last row
            // in the grid which may be the 'new row'.
            if (auto const lastSlot = LastVisibleSlot();
                forCurrentCellChange &&
                CurrentColumnIndex() == InvalidIndex &&
                lastSlot != InvalidIndex)
            {
                SetAndSelectCurrentCell(columnIndex, lastSlot, false /*forceCurrentCellSelection (unused here)*/);
            }

            // Interrupt operation because it has become invalid.
            return false;
        }

        return true;
    }

    bool DataGrid::CommitRowEdit(bool const exitEditingMode)
    {
        if (!EditingRow())
        {
            return true;
        }

        auto const internalEditingRow = EditingRow().as<DataGridRow>();
        assert(internalEditingRow);
        assert(internalEditingRow->Index() != InvalidIndex);
        assert(internalEditingRow->Slot() < SlotCount());

        if (!ValidateEditingRow(true /*scrollIntoView*/, false /*wireEvents*/))
        {
            return false;
        }

        DataConnection().EndEdit(internalEditingRow->DataContext());

        if (!exitEditingMode)
        {
            DataConnection().BeginEdit(internalEditingRow->DataContext());
        }

        return true;
    }

    void DataGrid::CompleteCellsCollection(MiniDumpExplorer::DataGridRow const& dataGridRow)
    {
        assert(dataGridRow);
        if (auto const cellsInCollection = dataGridRow.as<DataGridRow>()->Cells().Count();
            ColumnsItemsInternal().size() > cellsInCollection)
        {
            for (auto columnIndex = cellsInCollection; columnIndex < ColumnsItemsInternal().size(); ++columnIndex)
            {
                AddNewCellPrivate(dataGridRow, ColumnsItemsInternal()[columnIndex]);
            }
        }
    }

    void DataGrid::ComputeScrollBarsLayout()
    {
        if (ignoreNextScrollBarsLayout_)
        {
            ignoreNextScrollBarsLayout_ = false;

            // TODO: This optimization is causing problems with initial layout:
            //       Investigate why horizontal ScrollBar sometimes has incorrect thumb size when
            //       it first appears after adding a row when this perf improvement is turned on.
            // return;
        }

        auto const isHorizontalScrollBarOverCells = IsHorizontalScrollBarOverCells();
        auto const isVerticalScrollBarOverCells = IsVerticalScrollBarOverCells();

        auto cellsWidth = CellsWidth();
        auto cellsHeight = CellsHeight();

        auto allowHorizontalScrollBar = false;
        auto forceHorizontalScrollBar = false;
        auto horizontalScrollBarHeight = 0.0;
        if (hScrollBar_)
        {
            forceHorizontalScrollBar = HorizontalScrollBarVisibility() == Controls::ScrollBarVisibility::Visible;
            allowHorizontalScrollBar = forceHorizontalScrollBar || (ColumnsInternal().VisibleColumnCount() > 0 &&
                HorizontalScrollBarVisibility() != Controls::ScrollBarVisibility::Disabled &&
                HorizontalScrollBarVisibility() != Controls::ScrollBarVisibility::Hidden);

            // Compensate if the horizontal scrollbar is already taking up space
            if (!forceHorizontalScrollBar && hScrollBar_.Visibility() == Visibility::Visible)
            {
                if (!isHorizontalScrollBarOverCells)
                {
                    cellsHeight += hScrollBar_.DesiredSize().Height;
                }
            }

            if (!isHorizontalScrollBarOverCells)
            {
                horizontalScrollBarHeight = hScrollBar_.Height() + hScrollBar_.Margin().Top + hScrollBar_.Margin().Bottom;
            }
        }

        auto allowVertScrollBar = false;
        auto forceVertScrollBar = false;
        auto vertScrollBarWidth = 0.0;
        if (vScrollBar_)
        {
            forceVertScrollBar = VerticalScrollBarVisibility() == Controls::ScrollBarVisibility::Visible;
            allowVertScrollBar = forceVertScrollBar || (ColumnsItemsInternal().size() > 0 &&
                VerticalScrollBarVisibility() != Controls::ScrollBarVisibility::Disabled &&
                VerticalScrollBarVisibility() != Controls::ScrollBarVisibility::Hidden);

            // Compensate if the vertical scrollbar is already taking up space
            if (!forceVertScrollBar && vScrollBar_.Visibility() == Visibility::Visible)
            {
                if (!isVerticalScrollBarOverCells)
                {
                    cellsWidth += vScrollBar_.DesiredSize().Width;
                }
            }

            if (!isVerticalScrollBarOverCells)
            {
                vertScrollBarWidth = vScrollBar_.Width() + vScrollBar_.Margin().Left + vScrollBar_.Margin().Right;
            }
        }

        // Now cellsWidth is the width potentially available for displaying data cells.
        // Now cellsHeight is the height potentially available for displaying data cells.
        auto needHorizontalScrollBar = false;
        auto needVertScrollBar = false;

        auto const totalVisibleWidth = ColumnsInternal().VisibleEdgedColumnsWidth();
        auto const totalVisibleFrozenWidth = ColumnsInternal().GetVisibleFrozenEdgedColumnsWidth();

        UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), CellsHeight());
        auto const totalVisibleHeight = EdgedRowsHeightCalculated();

        if (!forceHorizontalScrollBar && !forceVertScrollBar)
        {
            auto needHorizontalScrollBarWithoutVertScrollBar = false;

            if (allowHorizontalScrollBar &&
                double_utils::greater_than(totalVisibleWidth, cellsWidth) &&
                double_utils::less_than(totalVisibleFrozenWidth, cellsWidth) &&
                double_utils::less_than_or_close(horizontalScrollBarHeight, cellsHeight))
            {
                auto const oldDataHeight = cellsHeight;
                cellsHeight -= horizontalScrollBarHeight;
                assert(cellsHeight >= 0);
                needHorizontalScrollBarWithoutVertScrollBar = needHorizontalScrollBar = true;

                if (vertScrollBarWidth > 0 &&
                    allowVertScrollBar &&
                    (double_utils::less_than_or_close(totalVisibleWidth - cellsWidth, vertScrollBarWidth) || double_utils::less_than_or_close(cellsWidth - totalVisibleFrozenWidth, vertScrollBarWidth)))
                {
                    // Would we still need a horizontal scrollbar without the vertical one?
                    UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), cellsHeight);
                    if (DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount())
                    {
                        needHorizontalScrollBar = double_utils::less_than(totalVisibleFrozenWidth, cellsWidth - vertScrollBarWidth);
                    }

                    if (!needHorizontalScrollBar)
                    {
                        // Restore old data height because turns out a horizontal scroll bar wouldn't make sense
                        cellsHeight = oldDataHeight;
                    }
                }
            }

            // Store the current FirstScrollingSlot because removing the horizontal scrollbar could scroll
            // the DataGrid up; however, if we realize later that we need to keep the horizontal scrollbar
            // then we should use the first slot stored here which is not scrolled.
            auto const firstScrollingSlot = DisplayData().FirstScrollingSlot();

            UpdateDisplayedRows(firstScrollingSlot, cellsHeight);
            if (allowVertScrollBar &&
                double_utils::greater_than(cellsHeight, 0) &&
                double_utils::less_than_or_close(vertScrollBarWidth, cellsWidth) &&
                DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount())
            {
                cellsWidth -= vertScrollBarWidth;
                assert(cellsWidth >= 0);
                needVertScrollBar = true;
            }

            DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());

            // We compute the number of visible columns only after we set up the vertical scroll bar.
            ComputeDisplayedColumns();

            if ((vertScrollBarWidth > 0 || horizontalScrollBarHeight > 0) &&
                allowHorizontalScrollBar &&
                needVertScrollBar && !needHorizontalScrollBar &&
                double_utils::greater_than(totalVisibleWidth, cellsWidth) &&
                double_utils::less_than(totalVisibleFrozenWidth, cellsWidth) &&
                double_utils::less_than_or_close(horizontalScrollBarHeight, cellsHeight))
            {
                cellsWidth += vertScrollBarWidth;
                cellsHeight -= horizontalScrollBarHeight;
                assert(cellsHeight >= 0);
                needVertScrollBar = false;

                UpdateDisplayedRows(firstScrollingSlot, cellsHeight);
                if (cellsHeight > 0 &&
                    vertScrollBarWidth <= cellsWidth &&
                    DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount())
                {
                    cellsWidth -= vertScrollBarWidth;
                    assert(cellsWidth >= 0);
                    needVertScrollBar = true;
                }

                if (needVertScrollBar)
                {
                    needHorizontalScrollBar = true;
                }
                else
                {
                    needHorizontalScrollBar = needHorizontalScrollBarWithoutVertScrollBar;
                }
            }
        }
        else if (forceHorizontalScrollBar && !forceVertScrollBar)
        {
            if (allowVertScrollBar)
            {
                if (cellsHeight > 0 &&
                    double_utils::less_than_or_close(vertScrollBarWidth, cellsWidth) &&
                    DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount())
                {
                    cellsWidth -= vertScrollBarWidth;
                    assert(cellsWidth >= 0);
                    needVertScrollBar = true;
                }

                DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());
                ComputeDisplayedColumns();
            }

            needHorizontalScrollBar = totalVisibleWidth > cellsWidth && totalVisibleFrozenWidth < cellsWidth;
        }
        else if (!forceHorizontalScrollBar && forceVertScrollBar)
        {
            if (allowHorizontalScrollBar)
            {
                if (cellsWidth > 0 &&
                    double_utils::less_than_or_close(horizontalScrollBarHeight, cellsHeight) &&
                    double_utils::greater_than(totalVisibleWidth, cellsWidth) &&
                    double_utils::less_than(totalVisibleFrozenWidth, cellsWidth))
                {
                    cellsHeight -= horizontalScrollBarHeight;
                    assert(cellsHeight >= 0);
                    needHorizontalScrollBar = true;
                    UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), cellsHeight);
                }

                DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());
                ComputeDisplayedColumns();
            }

            needVertScrollBar = DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount();
        }
        else
        {
            assert(forceHorizontalScrollBar);
            assert(forceVertScrollBar);
            assert(allowHorizontalScrollBar);
            assert(allowVertScrollBar);
            DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());
            ComputeDisplayedColumns();
            needVertScrollBar = DisplayData().NumTotallyDisplayedScrollingElements() != VisibleSlotCount();
            needHorizontalScrollBar = totalVisibleWidth > cellsWidth && totalVisibleFrozenWidth < cellsWidth;
        }

        UpdateHorizontalScrollBar(needHorizontalScrollBar, forceHorizontalScrollBar, totalVisibleWidth, totalVisibleFrozenWidth, cellsWidth);
        UpdateVerticalScrollBar(needVertScrollBar, forceVertScrollBar, totalVisibleHeight, cellsHeight);

        if (topRightCornerHeader_)
        {
            // Show the TopRightHeaderCell based on vertical ScrollBar visibility
            topRightCornerHeader_.Visibility(
                AreColumnHeadersVisible() &&
                vScrollBar_ &&
                vScrollBar_.Visibility() == Visibility::Visible
                ? Visibility::Visible
                : Visibility::Collapsed);
        }

        if (bottomRightCorner_)
        {
            // Show the BottomRightCorner when both scrollbars are visible.
            bottomRightCorner_.Visibility(
                hScrollBar_ && hScrollBar_.Visibility() == Visibility::Visible &&
                vScrollBar_ && vScrollBar_.Visibility() == Visibility::Visible ?
                Visibility::Visible : Visibility::Collapsed);
        }

        DisplayData().FullyRecycleElements();
    }

    void DataGrid::EditingElement_LostFocus(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        auto const editingElement = sender.try_as<FrameworkElement>();
        if (!editingElement)
        {
            return;
        }

        if (lostFocusEvent_)
        {
            editingElement.LostFocus(lostFocusEvent_);
            lostFocusEvent_ = {};
        }

        if (EditingRow() && EditingColumnIndex() != InvalidIndex)
        {
            FocusEditingCell(true);
        }

        executingLostFocusActions_ = true;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                executingLostFocusActions_ = false;
                lostFocusActions_.clear();
            });

        for (auto const& action : lostFocusActions_)
        {
            action();
        }
    }

    void DataGrid::EnsureHorizontalLayout()
    {
        ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        InvalidateColumnHeadersMeasure();
        InvalidateRowsMeasure(true);
        InvalidateMeasure();
    }

    bool DataGrid::EnsureRowHeaderWidth()
    {
        auto invalidated = false;
        if (AreRowHeadersVisible())
        {
            if (AreColumnHeadersVisible())
            {
                EnsureTopLeftCornerHeader();
            }

            if (rowsPresenter_)
            {
                for (auto const& element : rowsPresenter_.Children())
                {
                    if (auto const row = element.try_as<DataGridRow>();
                        row)
                    {
                        // If the RowHeader resulted in a different width the last time it was measured, we need
                        // to re-measure it
                        if (row->HeaderCell() && row->HeaderCell().DesiredSize().Width != ActualRowHeaderWidth())
                        {
                            row->HeaderCell().InvalidateMeasure();
                            invalidated = true;
                        }
                    }
                    else if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                        groupHeader && groupHeader->HeaderCell() && groupHeader->HeaderCell().DesiredSize().Width != ActualRowHeaderWidth())
                    {
                        groupHeader->HeaderCell().InvalidateMeasure();
                        invalidated = true;
                    }
                }

                if (invalidated)
                {
                    // We need to update the width of the horizontal scrollbar if the rowHeaders width actually changed
                    if (ColumnsInternal().VisibleStarColumnCount() > 0)
                    {
                        ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
                    }

                    InvalidateMeasure();
                }
            }
        }

        return invalidated;
    }

    void DataGrid::EnsureRowsPresenterVisibility()
    {
        if (rowsPresenter_)
        {
            // RowCount doesn't need to be considered, doing so might cause extra Visibility changes
            rowsPresenter_.Visibility(!ColumnsInternal().FirstVisibleNonFillerColumn() ? Visibility::Collapsed : Visibility::Visible);
        }
    }

    void DataGrid::EnsureTopLeftCornerHeader() const
    {
        if (topLeftCornerHeader_)
        {
            topLeftCornerHeader_.Visibility(HeadersVisibility() == DataGridHeadersVisibility::All ? Visibility::Visible : Visibility::Collapsed);

            if (topLeftCornerHeader_.Visibility() == Visibility::Visible)
            {
                if (!std::isnan(RowHeaderWidth()))
                {  // NOLINT(bugprone-branch-clone)
                    // RowHeaderWidth is set explicitly, so we should use that
                    topLeftCornerHeader_.Width(RowHeaderWidth());
                }
                else if (VisibleSlotCount() > 0)
                {
                    // RowHeaders AutoSize and we have at least 1 row so take the desired width
                    topLeftCornerHeader_.Width(RowHeadersDesiredWidth());
                }
            }
        }
    }

    void DataGrid::ExpandRowGroupParentChain(int32_t const level, int32_t const slot)
    {
        if (level == InvalidIndex)
        {
            return;
        }

        auto previousHeaderSlot = RowGroupHeadersTable().GetPreviousIndex(slot + 1);
        while (previousHeaderSlot != InvalidIndex)
        {
            auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(previousHeaderSlot);
            assert(rowGroupInfo);
            if (rowGroupInfo && level == rowGroupInfo->Level())
            {
                if (collapsedSlotsTable_.Contains(rowGroupInfo->Slot()))
                {
                    // Keep going up the chain
                    ExpandRowGroupParentChain(level - 1, rowGroupInfo->Slot() - 1);
                }

                if (rowGroupInfo->Visibility() != Visibility::Visible)
                {
                    EnsureRowGroupVisibility(rowGroupInfo, Visibility::Visible, false);
                }

                return;
            }

            previousHeaderSlot = RowGroupHeadersTable().GetPreviousIndex(previousHeaderSlot);
        }
    }

    void DataGrid::InvalidateCellsArrange() const
    {
        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->InvalidateHorizontalArrange();
        }
    }

    void DataGrid::InvalidateColumnHeadersArrange() const
    {
        if (columnHeadersPresenter_)
        {
            columnHeadersPresenter_.InvalidateArrange();
        }
    }

    void DataGrid::InvalidateColumnHeadersMeasure() const
    {
        if (columnHeadersPresenter_)
        {
            EnsureColumnHeadersVisibility();
            columnHeadersPresenter_.InvalidateMeasure();
        }
    }

    void DataGrid::InvalidateRowsArrange() const
    {
        if (rowsPresenter_)
        {
            rowsPresenter_.InvalidateArrange();
        }
    }

    void DataGrid::InvalidateRowsMeasure(bool const invalidateIndividualElements) const
    {
        if (rowsPresenter_)
        {
            rowsPresenter_.InvalidateMeasure();

            if (invalidateIndividualElements)
            {
                for (auto const& element : rowsPresenter_.Children())
                {
                    element.InvalidateMeasure();
                }
            }
        }
    }

    void DataGrid::DataGrid_GettingFocus([[maybe_unused]] UIElement const& sender, Input::GettingFocusEventArgs const& e)
    {
        focusInputDevice_ = e.InputDevice();
    }

    void DataGrid::DataGrid_GotFocus([[maybe_unused]] Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e)
    {
        if (!ContainsFocus())
        {
            containsFocus_ = true;
            ApplyDisplayedRowsState(DisplayData().FirstScrollingSlot(), DisplayData().LastScrollingSlot());
            if (CurrentColumnIndex() != InvalidIndex && IsSlotVisible(CurrentSlot()))
            {
                UpdateCurrentState(DisplayData().GetDisplayedElement(CurrentSlot()), CurrentColumnIndex(), true /*applyCellState*/);
            }
        }

        auto focusedElement = e.OriginalSource().try_as<DependencyObject>();
        focusedObject_ = focusedElement;
        while (focusedElement)
        {
            // Keep track of which row contains the newly focused element
            auto const focusedRow = focusedElement.try_as<MiniDumpExplorer::DataGridRow>();
            if (auto const internalFocusedRow = focusedElement.try_as<DataGridRow>();
                focusedRow && internalFocusedRow->OwningGrid() == *this && focusedRow_ != focusedRow)
            {
                ResetFocusedRow();
                focusedRow_ = focusedRow.Visibility() == Visibility::Visible ? focusedRow : nullptr;
                break;
            }

            focusedElement = Media::VisualTreeHelper::GetParent(focusedElement);
        }

        preferMouseIndicators_ = focusInputDevice_ == Input::FocusInputDeviceKind::Mouse || focusInputDevice_ == Input::FocusInputDeviceKind::Pen;

        ShowScrollBars();

        // If the DataGrid itself got focus, we actually want the automation focus to be on the current element
        if (e.OriginalSource() == *this && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::AutomationFocusChanged))
        {
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer)
            {
                peer->RaiseAutomationFocusChangedEvent(CurrentSlot(), CurrentColumnIndex());
            }
        }
    }

    void DataGrid::DataGrid_IsEnabledChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DependencyPropertyChangedEventArgs const& e)
    {
        UpdateDisabledVisual();

        if (!IsEnabled())
        {
            HideScrollBars(true /*useTransitions*/);
        }
    }

    void DataGrid::DataGrid_KeyDown([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e)
    {
        if (!e.Handled())
        {
            e.Handled(ProcessDataGridKey(e));
            lastHandledKeyDown_ = e.Handled() ? e.Key() : Windows::System::VirtualKey::None;
        }
    }

    void DataGrid::DataGrid_KeyUp([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e)
    {
        if (e.Key() == Windows::System::VirtualKey::Tab && e.OriginalSource() == *this)
        {
            if (CurrentColumnIndex() == InvalidIndex)
            {
                if (ColumnHeaders() && AreColumnHeadersVisible() && !ColumnHeaderHasFocus())
                {
                    ColumnHeaderHasFocus(true);
                }
            }
            else
            {
                if (ColumnHeaders() && AreColumnHeadersVisible())
                {
                    if ([[maybe_unused]] auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
                        shift && lastHandledKeyDown_ != Windows::System::VirtualKey::Tab)
                    {
                        // assert(!ColumnHeaderHasFocus());

                        // Show currency on the current column's header as focus is entering the DataGrid backwards.
                        ColumnHeaderHasFocus(true);
                    }
                }

                [[maybe_unused]] auto const success = ScrollSlotIntoView(CurrentColumnIndex(), CurrentSlot(), false /*forCurrentCellChange*/, true /*forceHorizontalScroll*/);
                assert(success);
                if (CurrentColumnIndex() != InvalidIndex && !SelectedItem())
                {
                    SetRowSelection(CurrentSlot(), true /*isSelected*/, true /*setAnchorSlot*/);
                }
            }
        }
    }

    void DataGrid::DataGrid_LostFocus([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        focusedObject_ = nullptr;
        if (ContainsFocus())
        {
            auto focusLeftDataGrid = true;
            auto dataGridWillReceiveRoutedEvent = true;
            MiniDumpExplorer::DataGridColumn editingColumn{ nullptr };

            // Walk up the visual tree of the newly focused element
            // to determine if focus is still within DataGrid.
            auto const focusedObject = GetFocusedElement();
            auto focusedDependencyObject = focusedObject.try_as<DependencyObject>();

            while (focusedDependencyObject)
            {
                if (focusedDependencyObject == *this)
                {
                    focusLeftDataGrid = false;
                    break;
                }

                // Walk up the visual tree. Try using the framework element's
                // parent.  We do this because Popups behave differently with respect to the visual tree,
                // and it could have a parent even if the VisualTreeHelper doesn't find it.
                DependencyObject parent{ nullptr };
                if (auto const element = focusedDependencyObject.try_as<FrameworkElement>();
                    element)
                {
                    parent = Media::VisualTreeHelper::GetParent(focusedDependencyObject);
                }
                else
                {
                    parent = element.Parent();
                    if (!parent)
                    {
                        parent = Media::VisualTreeHelper::GetParent(focusedDependencyObject);
                    }
                    else
                    {
                        dataGridWillReceiveRoutedEvent = false;
                    }
                }

                focusedDependencyObject = parent;
            }

            if (EditingRow() && EditingColumnIndex() != InvalidIndex)
            {
                editingColumn = ColumnsItemsInternal()[EditingColumnIndex()];

                if (focusLeftDataGrid && editingColumn.try_as<MiniDumpExplorer::DataGridTemplateColumn>())
                {
                    dataGridWillReceiveRoutedEvent = false;
                }
            }

            if (focusLeftDataGrid && !editingColumn.try_as<MiniDumpExplorer::DataGridTemplateColumn>())
            {
                containsFocus_ = false;
                if (EditingRow())
                {
                    CommitEdit(DataGridEditingUnit::Row, true /*exitEditingMode*/);
                }

                ResetFocusedRow();
                ApplyDisplayedRowsState(DisplayData().FirstScrollingSlot(), DisplayData().LastScrollingSlot());
                if (ColumnHeaderHasFocus())
                {
                    ColumnHeaderHasFocus(false);
                }
                else if (CurrentColumnIndex() != InvalidIndex && IsSlotVisible(CurrentSlot()))
                {
                    UpdateCurrentState(DisplayData().GetDisplayedElement(CurrentSlot()), CurrentColumnIndex(), true /*applyCellState*/);
                }
            }
            else if (!dataGridWillReceiveRoutedEvent)
            {
                if (auto const focusedElement = focusedObject.try_as<FrameworkElement>();
                    focusedElement)
                {
                    lostFocusEvent_ = focusedElement.LostFocus({ this, &DataGrid::ExternalEditingElement_LostFocus });
                }
            }
        }
    }

    Windows::Foundation::IInspectable DataGrid::GetFocusedElement() const
    {
        if (ControlHelpers::IsXamlRootAvailable() && XamlRoot())
        {
            return Input::FocusManager::GetFocusedElement(XamlRoot());
        }
        else
        {
            return Input::FocusManager::GetFocusedElement();
        }
    }

    void DataGrid::DataGrid_PointerEntered([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        if (e.Pointer().PointerDeviceType() != Microsoft::UI::Input::PointerDeviceType::Touch)
        {
            // Mouse/Pen inputs dominate. If touch panning indicators are shown, switch to mouse indicators.
            preferMouseIndicators_ = true;
            ShowScrollBars();
        }
    }

    void DataGrid::DataGrid_PointerExited([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        if (e.Pointer().PointerDeviceType() != Microsoft::UI::Input::PointerDeviceType::Touch)
        {
            // Mouse/Pen inputs dominate. If touch panning indicators are shown, switch to mouse indicators.
            isPointerOverHorizontalScrollBar_ = false;
            isPointerOverVerticalScrollBar_ = false;
            preferMouseIndicators_ = true;
            ShowScrollBars();
            HideScrollBarsAfterDelay();
        }
    }

    void DataGrid::DataGrid_PointerMoved([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        // Don't process if this is a generated replay of the event.
        if (e.IsGenerated())
        {
            return;
        }

        if (e.Pointer().PointerDeviceType() != Microsoft::UI::Input::PointerDeviceType::Touch)
        {
            // Mouse/Pen inputs dominate. If touch panning indicators are shown, switch to mouse indicators.
            preferMouseIndicators_ = true;
            ShowScrollBars();

            if (!UISettingsHelper::AreSettingsEnablingAnimations() &&
                hideScrollBarsTimer_ &&
                (isPointerOverHorizontalScrollBar_ || isPointerOverVerticalScrollBar_))
            {
                StopHideScrollBarsTimer();
            }
        }
    }

    void DataGrid::DataGrid_PointerPressed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        if (e.Handled())
        {
            return;
        }

        // Show the scroll bars as soon as a pointer is pressed on the DataGrid.
        ShowScrollBars();
    }

    void DataGrid::DataGrid_PointerReleased([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) const
    {
        if (CurrentColumnIndex() != InvalidIndex && CurrentSlot() != InvalidIndex)
        {
            e.Handled(true);
        }
    }

    void DataGrid::DataGrid_Unloaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        showingMouseIndicators_ = false;
        keepScrollBarsShowing_ = false;
    }

    void DataGrid::EditingElement_Loaded(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        auto const element = sender.try_as<FrameworkElement>();
        if (element)
        {
            if (auto const it = editingElementLoadedEvent_.find(element);
                it != editingElementLoadedEvent_.end())
            {
                editingElementLoadedEvent_.erase(it);
                element.Loaded(it->second);
            }
        }

        PreparingCellForEditPrivate(element);
    }

    bool DataGrid::EndCellEdit(DataGridEditAction const editAction, bool const exitEditingMode, bool const keepFocus, bool const raiseEvents)
    {
        if (editingColumnIndex_ == InvalidIndex)
        {
            return true;
        }

        auto internalEditingRow = EditingRow().try_as<DataGridRow>();
        assert(internalEditingRow);
        assert(internalEditingRow->Slot() == CurrentSlot());
        assert(editingColumnIndex_ != InvalidIndex);
        assert(editingColumnIndex_ < ColumnsItemsInternal().size());
        assert(editingColumnIndex_ == CurrentColumnIndex());

        // Cache these to see if they change later
        auto const currentSlot = CurrentSlot();
        auto const currentColumnIndex = CurrentColumnIndex();

        // We're ready to start ending, so raise the event
        auto const editingCell = internalEditingRow->Cells()[editingColumnIndex_];
        auto const editingElement = editingCell.Content().try_as<FrameworkElement>();
        if (!editingElement)
        {
            return false;
        }

        if (raiseEvents)
        {
            MiniDumpExplorer::DataGridCellEditEndingEventArgs const e{ CurrentColumn(), EditingRow(), editingElement, editAction };
            OnCellEditEnding(e);
            if (e.Cancel())
            {
                // CellEditEnding has been canceled
                return false;
            }

            // Ensure that the current cell wasn't changed in the user's CellEditEnding handler
            if (editingColumnIndex_ == InvalidIndex ||
                currentSlot != CurrentSlot() ||
                currentColumnIndex != CurrentColumnIndex())
            {
                return true;
            }

            internalEditingRow = EditingRow().try_as<DataGridRow>();
            assert(internalEditingRow);
            assert(internalEditingRow->Slot() == currentSlot);
            assert(editingColumnIndex_ != InvalidIndex);
            assert(editingColumnIndex_ == CurrentColumnIndex());
        }

        bindingValidationResults_.clear();

        // If we're canceling, let the editing column repopulate its old value if it wants
        if (editAction == DataGridEditAction::Cancel)
        {
            CurrentColumn().as<DataGridColumn>()->CancelCellEditInternal(editingElement, uneditedValue_);

            // Ensure that the current cell wasn't changed in the user column's CancelCellEdit
            if (editingColumnIndex_ == InvalidIndex ||
                currentSlot != CurrentSlot() ||
                currentColumnIndex != CurrentColumnIndex())
            {
                return true;
            }

            internalEditingRow = EditingRow().try_as<DataGridRow>();
            assert(internalEditingRow);
            assert(internalEditingRow->Slot() == currentSlot);
            assert(editingColumnIndex_ != InvalidIndex);
            assert(editingColumnIndex_ == CurrentColumnIndex());

            // Re-validate
            ValidateEditingRow(true /*scrollIntoView*/, false /*wireEvents*/);
        }

        // If we're committing, explicitly update the source but watch out for any validation errors
        if (editAction == DataGridEditAction::Commit)
        {
            for (auto const& bindingData : CurrentColumn().as<DataGridColumn>()->GetInputBindings(editingElement, CurrentItem()))
            {
                assert(bindingData.BindingExpression().ParentBinding());
                updateSourcePath_ = bindingData.BindingExpression().ParentBinding().Path() ? bindingData.BindingExpression().ParentBinding().Path().Path() : hstring{};
                bindingData.BindingExpression().UpdateSource();
            }

            // Re-validate
            ValidateEditingRow(true /*scrollIntoView*/, false /*wireEvents*/);

            if (!bindingValidationResults_.empty())
            {
                ScrollSlotIntoView(CurrentColumnIndex(), CurrentSlot(), false /*forCurrentCellChange*/, true /*forceHorizontalScroll*/);
                return false;
            }
        }

        if (exitEditingMode)
        {
            editingColumnIndex_ = InvalidIndex;
            editingCell.as<DataGridCell>()->ApplyCellState(true /*animate*/);

            // TODO: Figure out if we should restore a cached this.IsTabStop.
            IsTabStop(true);
            if (keepFocus && UIHelper::ContainsFocusedElement(editingElement, *this))
            {
                [[maybe_unused]] auto const rv = Focus(FocusState::Programmatic);
            }

            PopulateCellContent(!exitEditingMode /*isCellEdited*/, CurrentColumn(), EditingRow(), editingCell);
        }

        // We're done, so raise the CellEditEnded event
        if (raiseEvents)
        {
            OnCellEditEnded(MiniDumpExplorer::DataGridCellEditEndedEventArgs{ CurrentColumn(), EditingRow(), editAction });
        }

        // There's a chance that somebody reopened this cell for edit within the CellEditEnded handler,
        // so we should return false if we were supposed to exit editing mode, but we didn't
        return !(exitEditingMode && currentColumnIndex == editingColumnIndex_);
    }

    bool DataGrid::EndRowEdit(DataGridEditAction const editAction, bool const exitEditingMode, bool const raiseEvents)
    {
        // Explicit row end edit has been triggered, so we can no longer be initializing a new item.
        initializingNewItem_ = false;

        if (!EditingRow() || DataConnection().EndingEdit())
        {
            return true;
        }

        if (editingColumnIndex_ != InvalidIndex || (editAction == DataGridEditAction::Cancel && raiseEvents &&
            !(DataConnection().CanCancelEdit() || EditingRow().DataContext().try_as<IEditableObject>() || DataConnection().IsAddingNew())))
        {
            // Ending the row edit will fail immediately under the following conditions:
            // 1. We haven't ended the cell edit yet.
            // 2. We're trying to cancel edit when the underlying DataType is not an IEditableObject,
            //    because we have no way to properly restore the old value.  We will only allow this to occur if:
            //    a. raiseEvents == false, which means we're internally forcing a cancel or
            //    b. we're canceling a new item addition.
            return false;
        }

        auto const editingRow = EditingRow();

        if (raiseEvents)
        {
            MiniDumpExplorer::DataGridRowEditEndingEventArgs const e{ EditingRow(), editAction };
            OnRowEditEnding(e);
            if (e.Cancel())
            {
                // RowEditEnding has been canceled
                return false;
            }

            // Editing states might have been changed in the RowEditEnding handlers
            if (editingColumnIndex_ != InvalidIndex)
            {
                return false;
            }

            if (editingRow != EditingRow())
            {
                return true;
            }
        }

        // Call the appropriate commit or cancel methods
        if (editAction == DataGridEditAction::Commit)
        {
            if (!CommitRowEdit(exitEditingMode))
            {
                return false;
            }
        }
        else
        {
            if (!CancelRowEdit(exitEditingMode) && raiseEvents)
            {
                // We failed to cancel edit, so we should abort unless we're forcing a cancel
                return false;
            }
        }

        ResetValidationStatus();

        // Update the previously edited row's state
        if (exitEditingMode && editingRow == EditingRow())
        {
            // Un-wire the INDEI event handlers
            for (const auto& [indei, item] : validationItems_)
            {
                indei.ErrorsChanged(item.token);
            }

            validationItems_.clear();
            RemoveEditingElements();
            ResetEditingRow();
        }

        if (CurrentSlot() == InvalidIndex && DataConnection().CollectionView() && DataConnection().CollectionView().CurrentItem())
        {
            // Some EditableCollectionViews (ListCollectionView in particular) do not raise CurrentChanged when CommitEdit
            // changes the position of the CurrentItem.  Instead, they raise a PropertyChanged event for PositionChanged.
            // We recognize that case here and setup the CurrentItem again if one exists, but it was removed and re-added
            // during Commit.  This is better than reacting to PositionChanged which would double the work in most cases
            // and likely introduce regressions.
            UpdateStateOnCurrentChanged(DataConnection().CollectionView().CurrentItem(), DataConnection().CollectionView().CurrentPosition());
        }

        // Raise the RowEditEnded event
        if (raiseEvents)
        {
            OnRowEditEnded(MiniDumpExplorer::DataGridRowEditEndedEventArgs{ editingRow, editAction });
        }

        return true;
    }

    void DataGrid::EnsureColumnHeadersVisibility() const
    {
        if (columnHeadersPresenter_)
        {
            columnHeadersPresenter_.Visibility(AreColumnHeadersVisible() ? Visibility::Visible : Visibility::Collapsed);
        }
    }

    void DataGrid::EnsureVerticalGridLines()
    {
        if (AreColumnHeadersVisible())
        {
            auto totalColumnsWidth = 0.0;
            for (auto const& column : ColumnsInternal().ObservableItems())
            {
                auto const& internalColumn = column.as<DataGridColumn>();
                totalColumnsWidth += internalColumn->ActualWidth();

                internalColumn->HeaderCell().SeparatorVisibility(column != ColumnsInternal().LastVisibleColumn() || totalColumnsWidth < CellsWidth() ?
                    Visibility::Visible : Visibility::Collapsed);
            }
        }

        for (auto const& row : GetAllRows())
        {
            row.as<DataGridRow>()->EnsureGridLines();
        }
    }

    void DataGrid::ExitEdit(bool const keepFocus)
    {
        // We're exiting editing mode, so we can no longer be initializing a new item.
        initializingNewItem_ = false;

        if (!EditingRow() || DataConnection().EndingEdit())
        {
            assert(editingColumnIndex_ == InvalidIndex);
            return;
        }

        auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
        if (editingColumnIndex_ != InvalidIndex)
        {
            assert(internalEditingRow);
            assert(internalEditingRow->Slot() == CurrentSlot());
            assert(editingColumnIndex_ != InvalidIndex);
            assert(editingColumnIndex_ < ColumnsItemsInternal().size());
            assert(editingColumnIndex_ == CurrentColumnIndex());

            editingColumnIndex_ = InvalidIndex;
            internalEditingRow->Cells()[CurrentColumnIndex()].as<DataGridCell>()->ApplyCellState(false /*animate*/);
        }

        // TODO: Figure out if we should restore a cached this.IsTabStop.
        IsTabStop(true);
        if (IsSlotVisible(internalEditingRow->Slot()))
        {
            internalEditingRow->ApplyState(true /*animate*/);
        }

        ResetEditingRow();
        if (keepFocus)
        {
            [[maybe_unused]] auto const success = Focus(FocusState::Programmatic);
            assert(success);
        }
    }

    void DataGrid::ExternalEditingElement_LostFocus(Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e)
    {
        if (auto const element = sender.try_as<FrameworkElement>();
            element && lostFocusEvent_)
        {
            element.LostFocus(lostFocusEvent_);
            lostFocusEvent_ = {};
            DataGrid_LostFocus(sender, e);
        }
    }

    void DataGrid::FlushCurrentCellChanged()
    {
        if (makeFirstDisplayedCellCurrentCellPending_)
        {
            return;
        }

        if (SelectionHasChanged())
        {
            // selection is changing, don't raise CurrentCellChanged until it's done
            flushCurrentCellChanged_ = true;
            FlushSelectionChanged();
            return;
        }

        // We don't want to expand all intermediate currency positions, so we only expand
        // the last current item before we flush the event
        if (collapsedSlotsTable_.Contains(CurrentSlot()) && CurrentSlot() != SlotFromRowIndex(DataConnection().NewItemPlaceholderIndex()))
        {
            auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(RowGroupHeadersTable().GetPreviousIndex(CurrentSlot()));
            assert(rowGroupInfo);
            if (rowGroupInfo)
            {
                ExpandRowGroupParentChain(rowGroupInfo->Level(), rowGroupInfo->Slot());
            }
        }

        if (CurrentColumn() != previousCurrentColumn_ || CurrentItem() != previousCurrentItem_)
        {
            CoerceSelectedItem();
            previousCurrentColumn_ = CurrentColumn();
            previousCurrentItem_ = CurrentItem();

            OnCurrentCellChanged({});
        }

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer && currentCellCoordinates_ != previousAutomationFocusCoordinates_)
        {
            previousAutomationFocusCoordinates_ = DataGridInternal::DataGridCellCoordinates{ currentCellCoordinates_ };

            // If the DataGrid itself has focus, we want to move automation focus to the new current element

            if (auto const focusedObject = GetFocusedElement();
                focusedObject == *this && Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::AutomationFocusChanged))
            {
                peer->RaiseAutomationFocusChangedEvent(CurrentSlot(), CurrentColumnIndex());
            }
        }

        flushCurrentCellChanged_ = false;
    }

    void DataGrid::FlushSelectionChanged()
    {
        if (SelectionHasChanged() && noSelectionChangeCount_ == 0 && !makeFirstDisplayedCellCurrentCellPending_)
        {
            CoerceSelectedItem();
            if (NoCurrentCellChangeCount() != 0)
            {
                // current cell is changing, don't raise SelectionChanged until it's done
                return;
            }

            SelectionHasChanged(false);

            if (flushCurrentCellChanged_)
            {
                FlushCurrentCellChanged();
            }

            if (auto const e = selectedItems_.GetSelectionChangedEventArgs();
                e.AddedItems().Size() > 0 || e.RemovedItems().Size() > 0)
            {
                OnSelectionChanged(e);
            }
        }
    }

    bool DataGrid::FocusEditingCell(bool const setFocus)
    {
        auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
        assert(CurrentColumnIndex() != InvalidIndex);
        assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
        assert(CurrentSlot() != InvalidIndex);
        assert(CurrentSlot() < SlotCount());
        assert(internalEditingRow);
        assert(internalEditingRow->Slot() == CurrentSlot());
        assert(editingColumnIndex_ != InvalidIndex);

        // TODO: Figure out if we should cache this.IsTabStop in order to restore
        //       it later instead of setting it back to true unconditionally.
        IsTabStop(false);
        focusEditingControl_ = false;

        auto success = false;
        if (auto const dataGridCell = internalEditingRow->Cells()[editingColumnIndex_];
            setFocus)
        {
            if (UIHelper::ContainsFocusedElement(dataGridCell, *this))
            {
                success = true;
            }
            else
            {
                success = dataGridCell.Focus(FocusState::Programmatic);
            }

            focusEditingControl_ = !success;
        }

        return success;
    }

    hstring DataGrid::FormatClipboardContent(MiniDumpExplorer::DataGridRowClipboardEventArgs const& e)
    {
        std::wstringstream text;
        auto const clipboardRowContent = e.ClipboardRowContent();
        for (uint32_t cellIndex = 0; cellIndex < clipboardRowContent.Size(); ++cellIndex)
        {
            if (auto const cellContent = clipboardRowContent.GetAt(cellIndex);
                cellContent)
            {
                text << UIHelper::ObjectToString(cellContent.Content());
            }

            if (cellIndex < clipboardRowContent.Size() - 1)
            {
                text << L'\t';
            }
            else
            {
                text << L'\r';
                text << L'\n';
            }
        }

        return hstring{ text.str() };
    }

    double DataGrid::GetHorizontalSmallScrollDecrease()
    {
        // If the first column is covered up, scroll to the start of it when the user clicks the left button
        if (negHorizontalOffset_ > 0)
        {
            return negHorizontalOffset_;
        }
        else
        {
            // The entire first column is displayed, show the entire previous column when the user clicks
            // the left button

            if (auto const previousColumn = ColumnsInternal().GetPreviousVisibleScrollingColumn(ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()]);
                previousColumn)
            {
                return GetEdgedColumnWidth(previousColumn);
            }
            else
            {
                // There's no previous column so don't move
                return 0;
            }
        }
    }

    double DataGrid::GetHorizontalSmallScrollIncrease()
    {
        if (DisplayData().FirstDisplayedScrollingCol() != InvalidIndex)
        {
            return GetEdgedColumnWidth(ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()]) - negHorizontalOffset_;
        }

        return 0.0;
    }

    double DataGrid::GetVerticalSmallScrollIncrease()
    {
        if (DisplayData().FirstScrollingSlot() != InvalidIndex)
        {
            return GetExactSlotElementHeight(DisplayData().FirstScrollingSlot()) - NegVerticalOffset();
        }

        return 0;
    }

    void DataGrid::HideScrollBars(bool const useTransitions)
    {
        if (!keepScrollBarsShowing_)
        {
            proposedScrollBarsState_ = ScrollBarVisualState::NoIndicator;
            proposedScrollBarsSeparatorState_ = UISettingsHelper::AreSettingsEnablingAnimations() ? ScrollBarsSeparatorVisualState::SeparatorCollapsed : ScrollBarsSeparatorVisualState::SeparatorCollapsedWithoutAnimation;
            if (UISettingsHelper::AreSettingsAutoHidingScrollBars())
            {
                SwitchScrollBarsVisualStates(proposedScrollBarsState_, proposedScrollBarsSeparatorState_, useTransitions);
            }
        }
    }

    void DataGrid::HideScrollBarsAfterDelay()
    {
        if (!keepScrollBarsShowing_)
        {
            Microsoft::UI::Dispatching::DispatcherQueueTimer hideScrollBarsTimer{ nullptr };

            if (hideScrollBarsTimer_)
            {
                hideScrollBarsTimer = hideScrollBarsTimer_;
                if (hideScrollBarsTimer.IsRunning())
                {
                    hideScrollBarsTimer.Stop();
                }
            }
            else
            {
                hideScrollBarsTimer = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread().CreateTimer();
                hideScrollBarsTimer.Interval(std::chrono::milliseconds(NoScrollBarCountdownMs));
                [[maybe_unused]] auto const token = hideScrollBarsTimer.Tick({ this, &DataGrid::HideScrollBarsTimerTick });
                hideScrollBarsTimer_ = hideScrollBarsTimer;
            }

            hideScrollBarsTimer.Start();
        }
    }

    void DataGrid::HideScrollBarsTimerTick([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Windows::Foundation::IInspectable const& e)
    {
        StopHideScrollBarsTimer();
        HideScrollBars(true /*useTransitions*/);
    }

    void DataGrid::HookDataGridEvents()
    {
        dataGridIsEnabledChangedEvent_ = IsEnabledChanged({ this, &DataGrid::DataGrid_IsEnabledChanged });
        dataGridKeyDownEvent_ = KeyDown({ this, &DataGrid::DataGrid_KeyDown });
        dataGridKeyUpEvent_ = KeyUp({ this, &DataGrid::DataGrid_KeyUp });
        dataGridGettingFocusEvent_ = GettingFocus({ this, &DataGrid::DataGrid_GettingFocus });
        dataGridGotFocusEvent_ = GotFocus({ this, &DataGrid::DataGrid_GotFocus });
        dataGridLostFocusEvent_ = LostFocus({ this, &DataGrid::DataGrid_LostFocus });
        dataGridPointerEnteredEvent_ = PointerEntered({ this, &DataGrid::DataGrid_PointerEntered });
        dataGridPointerExitedEvent_ = PointerExited({ this, &DataGrid::DataGrid_PointerExited });
        dataGridPointerMovedEvent_ = PointerMoved({ this, &DataGrid::DataGrid_PointerMoved });
        dataGridPointerPressedEvent_ = PointerPressed({ this, &DataGrid::DataGrid_PointerPressed });
        dataGridPointerReleasedEvent_ = PointerReleased({ this, &DataGrid::DataGrid_PointerReleased });
        dataGridUnloadedEvent_ = Unloaded({ this, &DataGrid::DataGrid_Unloaded });
    }

    void DataGrid::HookHorizontalScrollBarEvents()
    {
        if (hScrollBar_)
        {
            hScrollBarScrollEvent_ = hScrollBar_.Scroll({ this, &DataGrid::HorizontalScrollBar_Scroll });
            hScrollBarPointerEnteredEvent_ = hScrollBar_.PointerEntered({ this, &DataGrid::HorizontalScrollBar_PointerEntered });
            hScrollBarPointerExitedEvent_ = hScrollBar_.PointerExited({ this, &DataGrid::HorizontalScrollBar_PointerExited });
        }
    }

    void DataGrid::HookVerticalScrollBarEvents()
    {
        if (vScrollBar_)
        {
            vScrollBarScrollEvent_ = vScrollBar_.Scroll({ this, &DataGrid::VerticalScrollBar_Scroll });
            vScrollBarPointerEnteredEvent_ = vScrollBar_.PointerEntered({ this, &DataGrid::VerticalScrollBar_PointerEntered });
            vScrollBarPointerExitedEvent_ = vScrollBar_.PointerExited({ this, &DataGrid::VerticalScrollBar_PointerExited });
        }
    }

    void DataGrid::HorizontalScrollBar_PointerEntered([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        isPointerOverHorizontalScrollBar_ = true;

        if (!UISettingsHelper::AreSettingsEnablingAnimations())
        {
            HideScrollBarsAfterDelay();
        }
    }

    void DataGrid::HorizontalScrollBar_PointerExited([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        isPointerOverHorizontalScrollBar_ = false;
        HideScrollBarsAfterDelay();
    }

    void DataGrid::VerticalScrollBar_PointerEntered([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        isPointerOverVerticalScrollBar_ = true;

        if (!UISettingsHelper::AreSettingsEnablingAnimations())
        {
            HideScrollBarsAfterDelay();
        }
    }

    void DataGrid::VerticalScrollBar_PointerExited([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        isPointerOverVerticalScrollBar_ = false;
        HideScrollBarsAfterDelay();
    }

    void DataGrid::HorizontalScrollBar_Scroll([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Controls::Primitives::ScrollEventArgs const& e)
    {
        ProcessHorizontalScroll(e.ScrollEventType());
    }

    void DataGrid::IndicatorStateStoryboard_Completed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Windows::Foundation::IInspectable const& e)
    {
        // If the cursor is currently directly over either scroll bar then do not automatically hide the indicators.
        if (!keepScrollBarsShowing_ &&
            !isPointerOverVerticalScrollBar_ &&
            !isPointerOverHorizontalScrollBar_)
        {
            // Go to the NoIndicator state using transitions.
            if (UISettingsHelper::AreSettingsEnablingAnimations())
            {
                // By default, there is a delay before the NoIndicator state actually shows.
                HideScrollBars(true /*useTransitions*/);
            }
            else
            {
                // Since OS animations are turned off, use a timer to delay the scroll bars' hiding.
                HideScrollBarsAfterDelay();
            }
        }
    }

    bool DataGrid::IsColumnOutOfBounds(int32_t const columnIndex) const
    {
        return columnIndex >= ColumnsItemsInternal().size() || columnIndex == InvalidIndex;
    }

    bool DataGrid::IsInnerCellOutOfBounds(int32_t const columnIndex, int32_t const slot) const
    {
        return IsColumnOutOfBounds(columnIndex) || IsSlotOutOfBounds(slot);
    }

    bool DataGrid::IsInnerCellOutOfSelectionBounds(int32_t const columnIndex, int32_t const slot) const
    {
        return IsColumnOutOfBounds(columnIndex) || IsSlotOutOfSelectionBounds(slot);
    }

    bool DataGrid::IsSlotOutOfBounds(int32_t const slot) const
    {
        return slot >= SlotCount() || slot == InvalidIndex || collapsedSlotsTable_.Contains(slot);
    }

    bool DataGrid::IsSlotOutOfSelectionBounds(int32_t const slot) const
    {
        if (RowGroupHeadersTable().Contains(slot))
        {
            assert(slot != InvalidIndex);
            assert(slot < SlotCount());
            return false;
        }

        auto const rowIndex = RowIndexFromSlot(slot);
        return rowIndex == InvalidIndex || rowIndex >= DataConnection().Count();
    }

    void DataGrid::LoadMoreDataFromIncrementalItemsSource(double const totalVisibleHeight)
    {
        if (IncrementalLoadingTrigger() == Controls::IncrementalLoadingTrigger::Edge && DataConnection().IsDataSourceIncremental() && DataConnection().HasMoreItems() && !DataConnection().IsLoadingMoreItems())
        {

            if (auto const bottomScrolledOffHeight = std::max(0.0, totalVisibleHeight - CellsHeight() - VerticalOffset());
                IncrementalLoadingThreshold() * CellsHeight() >= bottomScrolledOffHeight)
            {
                auto const numberOfRowsToLoad = std::max(1, static_cast<int32_t>(DataFetchSize() * CellsHeight() / RowHeightEstimate()));

                DataConnection().LoadMoreItems(static_cast<int32_t>(numberOfRowsToLoad));
            }
        }
    }

    void DataGrid::MakeFirstDisplayedCellCurrentCell()
    {
        if (CurrentColumnIndex() != InvalidIndex)
        {
            makeFirstDisplayedCellCurrentCellPending_ = false;
            desiredCurrentColumnIndex_ = InvalidIndex;
            FlushCurrentCellChanged();
            return;
        }

        if (SlotCount() != SlotFromRowIndex(DataConnection().Count()))
        {
            makeFirstDisplayedCellCurrentCellPending_ = true;
            return;
        }

        // No current cell, therefore no selection either - try to set the current cell to the
        // ItemsSource's ICollectionView.CurrentItem if it exists, otherwise use the first displayed cell.
        int32_t slot;
        if (auto const collectionView = DataConnection().CollectionView();
            collectionView)
        {
            if (collectionView.IsCurrentBeforeFirst() ||
                collectionView.IsCurrentAfterLast())
            {
                slot = RowGroupHeadersTable().Contains(0) ? 0 : InvalidIndex;
            }
            else
            {
                slot = SlotFromRowIndex(collectionView.CurrentPosition());
            }
        }
        else
        {
            if (SelectedIndex() == InvalidIndex)
            {
                // Try to default to the first row
                slot = SlotFromRowIndex(0);
                if (!IsSlotVisible(slot))
                {
                    slot = InvalidIndex;
                }
            }
            else
            {
                slot = SlotFromRowIndex(SelectedIndex());
            }
        }

        auto columnIndex = FirstDisplayedNonFillerColumnIndex();
        if (desiredCurrentColumnIndex_ != InvalidIndex && desiredCurrentColumnIndex_ < ColumnsItemsInternal().size())
        {
            columnIndex = desiredCurrentColumnIndex_;
        }

        SetAndSelectCurrentCell(
            columnIndex,
            slot,
            false /*forceCurrentCellSelection*/);
        anchorSlot_ = slot;
        makeFirstDisplayedCellCurrentCellPending_ = false;
        desiredCurrentColumnIndex_ = InvalidIndex;
        FlushCurrentCellChanged();
    }

    void DataGrid::NoIndicatorStateStoryboard_Completed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Windows::Foundation::IInspectable const& e)
    {
        assert(hasNoIndicatorStateStoryboardCompletedHandler_);

        showingMouseIndicators_ = false;
    }

    void DataGrid::PopulateCellContent(bool const isCellEdited, MiniDumpExplorer::DataGridColumn const& dataGridColumn, MiniDumpExplorer::DataGridRow const& dataGridRow, MiniDumpExplorer::DataGridCell const& dataGridCell)
    {
        assert(dataGridColumn);
        assert(dataGridRow);
        assert(dataGridCell);

        auto const internalDataGridColumn = dataGridColumn.try_as<DataGridColumn>();
        auto const dataGridBoundColumn = dataGridColumn.try_as<DataGridBoundColumn>();

        FrameworkElement element{nullptr};

        if (isCellEdited)
        {
            // Generate EditingElement and apply column style if available
            element = internalDataGridColumn->GenerateEditingElementInternal(dataGridCell, dataGridRow.DataContext());
            if (element)
            {
                if (dataGridBoundColumn && dataGridBoundColumn->EditingElementStyle())
                {
                    UIHelper::SetStyleWithType(element, dataGridBoundColumn->EditingElementStyle());
                }

                // Subscribe to the new element's events
                editingElementLoadedEvent_[element] = element.Loaded({ this, &DataGrid::EditingElement_Loaded });
            }
        }
        else
        {
            // Generate Element and apply column style if available
            element = internalDataGridColumn->GenerateElementInternal(dataGridCell, dataGridRow.DataContext());
            if (element)
            {
                if (dataGridBoundColumn && dataGridBoundColumn->ElementStyle())
                {
                    UIHelper::SetStyleWithType(element, dataGridBoundColumn->ElementStyle());
                }
            }

        }

        if(dataGridCell.Content() != element)
        {
            dataGridCell.Content(element);
        }
    }

    void DataGrid::PreparingCellForEditPrivate(FrameworkElement const& editingElement)
    {
        auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
        assert(internalEditingRow);

        if (editingColumnIndex_ == InvalidIndex ||
            CurrentColumnIndex() == InvalidIndex ||
            internalEditingRow->Cells()[CurrentColumnIndex()].Content() != editingElement)
        {
            // The current cell has changed since the call to BeginCellEdit, so the fact
            // that this element has loaded is no longer relevant
            return;
        }

        assert(internalEditingRow->Slot() == CurrentSlot());
        assert(editingColumnIndex_ != InvalidIndex);
        assert(editingColumnIndex_ < ColumnsItemsInternal().size());
        assert(editingColumnIndex_ == CurrentColumnIndex());

        FocusEditingCell(ContainsFocus() || focusEditingControl_ /*setFocus*/);

        // Prepare the cell for editing and raise the PreparingCellForEdit event for all columns
        auto const dataGridColumn = CurrentColumn();
        auto const dataGridColumnInternal = dataGridColumn.as<DataGridColumn>();
        uneditedValue_ = dataGridColumnInternal->PrepareCellForEditInternal(editingElement, editingEventArgs_);
        OnPreparingCellForEdit(MiniDumpExplorer::DataGridPreparingCellForEditEventArgs{ dataGridColumn, EditingRow(), editingEventArgs_, editingElement });
    }

    bool DataGrid::ProcessAKey()
    {
        if (auto const [ctrl, shift, alt] = KeyboardHelper::GetMetaKeyState2();
            ctrl && !shift && !alt && SelectionMode() == DataGridSelectionMode::Extended)
        {
            SelectAll();
            return true;
        }

        return false;
    }

    bool DataGrid::ProcessCopyKey()
    {
        if (auto const [ctrl, shift, alt] = KeyboardHelper::GetMetaKeyState2();
            ctrl &&
            !shift &&
            !alt &&
            ClipboardCopyMode() != DataGridClipboardCopyMode::None &&
            SelectedItems().Size() > 0 &&
            editingColumnIndex_ != CurrentColumnIndex())
        {
            std::wstringstream textBuilder;

            if (ClipboardCopyMode() == DataGridClipboardCopyMode::IncludeHeader)
            {
                MiniDumpExplorer::DataGridRowClipboardEventArgs const headerArgs{ nullptr, true };
                for (auto const& column : ColumnsInternal().GetVisibleColumns())
                {
                    headerArgs.ClipboardRowContent().Append(MiniDumpExplorer::DataGridClipboardCellContent{ nullptr, column, column.Header() });
                }

                OnCopyingRowClipboardContent(headerArgs);
                textBuilder << FormatClipboardContent(headerArgs);
            }

            for (auto const& item : SelectedItems())
            {
                MiniDumpExplorer::DataGridRowClipboardEventArgs itemArgs{ item, false };
                for (auto const& column : ColumnsInternal().GetVisibleColumns())
                {
                    auto const columnInternal = column.as<DataGridColumn>();
                    auto const content = columnInternal->GetCellValue(item, column.ClipboardContentBinding());
                    itemArgs.ClipboardRowContent().Append(MiniDumpExplorer::DataGridClipboardCellContent{ item, column, content });
                }

                OnCopyingRowClipboardContent(itemArgs);
                textBuilder << FormatClipboardContent(itemArgs);
            }



            if (auto const text = hstring{ std::move(textBuilder).str() };
                !text.empty())
            {
                try
                {
                    Windows::ApplicationModel::DataTransfer::DataPackage const content{};
                    content.SetText(text);
                    Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(content);
                }
                catch (hresult_error const& ex)
                {
                    // We will get a SecurityException if the user does not allow access to the clipboard.
                    auto const errorMessage = std::format(L"Error sending Text format to Clipboard: {}", ex.message());
                    OutputDebugStringW(errorMessage.c_str());
                }

                return true;
            }
        }

        return false;
    }

    bool DataGrid::ProcessDataGridKey(Input::KeyRoutedEventArgs const& e)
    {
        using Windows::System::VirtualKey;
        auto focusDataGrid = false;

        switch (e.Key())
        {
        case VirtualKey::Tab:
            return ProcessTabKey(e);

        case VirtualKey::Up:
            focusDataGrid = ProcessUpKey();
            break;

        case VirtualKey::Down:
            focusDataGrid = ProcessDownKey();
            break;

        case VirtualKey::PageDown:
            focusDataGrid = ProcessNextKey();
            break;

        case VirtualKey::PageUp:
            focusDataGrid = ProcessPriorKey();
            break;

        case VirtualKey::Left:
            focusDataGrid = FlowDirection() == FlowDirection::LeftToRight ? ProcessLeftKey() : ProcessRightKey();
            break;

        case VirtualKey::Right:
            focusDataGrid = FlowDirection() == FlowDirection::LeftToRight ? ProcessRightKey() : ProcessLeftKey();
            break;

        case VirtualKey::F2:
            return ProcessF2Key(e);

        case VirtualKey::Home:
            focusDataGrid = ProcessHomeKey();
            break;

        case VirtualKey::End:
            focusDataGrid = ProcessEndKey();
            break;

        case VirtualKey::Enter:
            focusDataGrid = ProcessEnterKey();
            break;

        case VirtualKey::Escape:
            return ProcessEscapeKey();

        case VirtualKey::A:
            return ProcessAKey();

        case VirtualKey::C:
        case VirtualKey::Insert:
            return ProcessCopyKey();

        case VirtualKey::Space:
            return ProcessSpaceKey();

        default:
            break;
        }

        if (focusDataGrid && IsTabStop())
        {
            [[maybe_unused]] auto const result = Focus(FocusState::Programmatic);
        }

        return focusDataGrid;
    }

    bool DataGrid::ProcessDownKeyInternal(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().FirstVisibleColumn().try_as<DataGridColumn>();
        auto const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        auto const lastSlot = LastVisibleSlot();
        if (firstVisibleColumnIndex == InvalidIndex || lastSlot == InvalidIndex)
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessDownKeyInternal(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto nextSlot = InvalidIndex;
        if (CurrentSlot() != InvalidIndex)
        {
            nextSlot = GetNextVisibleSlot(CurrentSlot());
            if (nextSlot >= SlotCount())
            {
                nextSlot = InvalidIndex;
            }
        }

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        int32_t desiredSlot;
        int32_t columnIndex;
        DataGridInternal::DataGridSelectionAction action;

        if (ColumnHeaderHasFocus())
        {
            if (ctrl || shift)
            {
                return false;
            }

            if (CurrentSlot() == FirstVisibleSlot())
            {
                ColumnHeaderHasFocus(false);
                return true;
            }

            assert(CurrentColumnIndex() != InvalidIndex);
            desiredSlot = FirstVisibleSlot();
            columnIndex = CurrentColumnIndex();
            action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }
        else if (CurrentColumnIndex() == InvalidIndex)
        {
            desiredSlot = FirstVisibleSlot();
            columnIndex = firstVisibleColumnIndex;
            action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }
        else if (ctrl)
        {
            if (shift)
            {
                // Both Ctrl and Shift
                desiredSlot = lastSlot;
                columnIndex = CurrentColumnIndex();
                action = SelectionMode() == DataGridSelectionMode::Extended
                    ? DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                    : DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
            else
            {
                // Ctrl without Shift
                desiredSlot = lastSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
        }
        else
        {
            if (nextSlot == InvalidIndex)
            {
                return true;
            }

            if (shift)
            {
                // Shift without Ctrl
                desiredSlot = nextSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent;
            }
            else
            {
                // Neither Ctrl nor Shift
                desiredSlot = nextSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
        }

        UpdateSelectionAndCurrency(columnIndex, desiredSlot, action, true /*scrollIntoView*/);

        return successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessEndKey(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().LastVisibleColumn().try_as<DataGridColumn>();
        auto const lastVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        auto const firstVisibleSlot = FirstVisibleSlot();
        auto const lastVisibleSlot = LastVisibleSlot();
        if (lastVisibleColumnIndex == InvalidIndex || (firstVisibleSlot == InvalidIndex && !ColumnHeaderHasFocus()))
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessEndKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });
        if (!ctrl)
        {
            return ProcessRightMost(lastVisibleColumnIndex, firstVisibleSlot);
        }
        else if (firstVisibleSlot != InvalidIndex)
        {
            auto const action = shift && SelectionMode() == DataGridSelectionMode::Extended
                ? DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                : DataGridInternal::DataGridSelectionAction::SelectCurrent;
            UpdateSelectionAndCurrency(lastVisibleColumnIndex, lastVisibleSlot, action, true /*scrollIntoView*/);
        }

        return successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessEnterKey(bool const shift, bool const ctrl)
    {
        auto const oldCurrentSlot = CurrentSlot();

        if (!ctrl)
        {
            if (ColumnHeaderHasFocus())
            {
                CurrentColumn().as<DataGridColumn>()->HeaderCell().as<DataGridColumnHeader>()->InvokeProcessSort();
                return true;
            }

            if (FirstVisibleSlot() != InvalidIndex && RowGroupHeadersTable().Contains(CurrentSlot()) && ToggleRowGroup())
            {
                return true;
            }

            // If Enter was used by a TextBox, we shouldn't handle the key
            if (auto const focusedTextBox = GetFocusedElement().try_as<Controls::TextBox>();
                focusedTextBox && focusedTextBox.AcceptsReturn())
            {
                return false;
            }

            if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
                {
                    if (auto const self = weakPtr.get())
                    {
                        self->ProcessEnterKey(shift, ctrl);
                    }
                }))
            {
                return true;
            }

            // Enter behaves like down arrow - it commits the potential editing and goes down one cell.
            if (!ProcessDownKeyInternal(false, ctrl))
            {
                return false;
            }
        }
        else if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessEnterKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        // Try to commit the potential editing
        if (oldCurrentSlot == CurrentSlot() && EndCellEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, true /*keepFocus*/, true /*raiseEvents*/) && EditingRow())
        {
            EndRowEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, true /*raiseEvents*/);
            ScrollIntoView(CurrentItem(), CurrentColumn());
        }

        return true;
    }

    bool DataGrid::ProcessEscapeKey()
    {
        if (WaitForLostFocus([weakPtr = get_weak()]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessEscapeKey();
                }
            }))
        {
            return true;
        }

        if (editingColumnIndex_ != InvalidIndex)
        {
            // Revert the potential cell editing and exit cell editing.
            EndCellEdit(DataGridEditAction::Cancel, true /*exitEditingMode*/, true /*keepFocus*/, true /*raiseEvents*/);
            return true;
        }

        if (EditingRow())
        {
            // Revert the potential row editing and exit row editing.
            EndRowEdit(DataGridEditAction::Cancel, true /*exitEditingMode*/, true /*raiseEvents*/);
            return true;
        }

        return false;
    }

    bool DataGrid::ProcessF2Key(Input::KeyRoutedEventArgs const& e)
    {
        if (auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
            !shift && !ctrl &&
            editingColumnIndex_ == InvalidIndex && CurrentColumnIndex() != InvalidIndex && GetRowSelection(CurrentSlot()) &&
            !GetColumnEffectiveReadOnlyState(CurrentColumn()))
        {
            if (ScrollSlotIntoView(CurrentColumnIndex(), CurrentSlot(), false /*forCurrentCellChange*/, true /*forceHorizontalScroll*/))
            {
                BeginCellEdit(e);
            }

            return true;
        }

        return false;
    }

    bool DataGrid::ProcessHomeKey(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().FirstVisibleNonFillerColumn().try_as<DataGridColumn>();
        int32_t const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        int32_t const firstVisibleSlot = FirstVisibleSlot();
        if (firstVisibleColumnIndex == InvalidIndex || (firstVisibleSlot == InvalidIndex && !ColumnHeaderHasFocus()))
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessHomeKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        if (!ctrl)
        {
            return ProcessLeftMost(firstVisibleColumnIndex, firstVisibleSlot);
        }

        if (firstVisibleSlot != InvalidIndex)
        {
            auto const action = shift && SelectionMode() == DataGridSelectionMode::Extended
                ? DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                : DataGridInternal::DataGridSelectionAction::SelectCurrent;
            UpdateSelectionAndCurrency(firstVisibleColumnIndex, firstVisibleSlot, action, true /*scrollIntoView*/);
        }

        return successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessLeftKey(bool const shift, bool const ctrl)
    {
        auto dataGridColumn = ColumnsInternal().FirstVisibleNonFillerColumn().try_as<DataGridColumn>();
        auto const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        auto const firstVisibleSlot = FirstVisibleSlot();
        if (firstVisibleColumnIndex == InvalidIndex || (firstVisibleSlot == InvalidIndex && !ColumnHeaderHasFocus()))
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessLeftKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto previousVisibleColumnIndex = InvalidIndex;
        if (CurrentColumnIndex() != InvalidIndex)
        {
            dataGridColumn = ColumnsInternal().GetPreviousVisibleNonFillerColumn(ColumnsItemsInternal()[CurrentColumnIndex()]).try_as<DataGridColumn>();;
            if (dataGridColumn)
            {
                previousVisibleColumnIndex = dataGridColumn->Index();
            }
        }

        const auto oldFocusedColumn = FocusedColumn();

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });
        if (ctrl)
        {
            return ProcessLeftMost(firstVisibleColumnIndex, firstVisibleSlot);
        }

        if (firstVisibleSlot != InvalidIndex && (!RowGroupHeadersTable().Contains(CurrentSlot()) || ColumnHeaderHasFocus()))
        {
            if (CurrentColumnIndex() == InvalidIndex)
            {
                UpdateSelectionAndCurrency(firstVisibleColumnIndex, firstVisibleSlot, DataGridInternal::DataGridSelectionAction::SelectCurrent, true /*scrollIntoView*/);
            }
            else
            {
                if (previousVisibleColumnIndex == InvalidIndex)
                {
                    return true;
                }

                ++noFocusedColumnChangeCount_;
                auto handle2 = dlg_help_utils::make_scope_exit([this]()
                    {
                        --noFocusedColumnChangeCount_;
                    });
                UpdateSelectionAndCurrency(previousVisibleColumnIndex, CurrentSlot(), DataGridInternal::DataGridSelectionAction::None, true /*scrollIntoView*/);
            }
        }

        if (ColumnHeaderHasFocus())
        {
            if (!CurrentColumn())
            {
                dataGridColumn = ColumnsInternal().GetPreviousVisibleNonFillerColumn(FocusedColumn()).try_as<DataGridColumn>();;
                if (dataGridColumn)
                {
                    FocusedColumn(dataGridColumn.as<MiniDumpExplorer::DataGridColumn>());
                }
            }
            else
            {
                FocusedColumn(CurrentColumn());
            }

            if (firstVisibleSlot == InvalidIndex && FocusedColumn())
            {
                ScrollColumnIntoView(FocusedColumn().as<DataGridColumn>()->Index());
            }
        }

        auto const focusedColumnChanged = ColumnHeaderHasFocus() && oldFocusedColumn != FocusedColumn();

        if (focusedColumnChanged)
        {
            if (auto const internalOldFocusedColumn = oldFocusedColumn.try_as<DataGridColumn>();
                internalOldFocusedColumn && internalOldFocusedColumn->HasHeaderCell())
            {
                internalOldFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }

            if (auto const internalFocusedColumn = FocusedColumn().try_as<DataGridColumn>();
                internalFocusedColumn && internalFocusedColumn->HasHeaderCell())
            {
                internalFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }
        }

        return focusedColumnChanged || successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessLeftMost(int32_t const firstVisibleColumnIndex, int32_t const firstVisibleSlot)
    {
        auto const oldFocusedColumn = FocusedColumn();

        ++noSelectionChangeCount_;
        {
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });
            int32_t desiredSlot;
            DataGridInternal::DataGridSelectionAction action;
            if (CurrentColumnIndex() == InvalidIndex)
            {
                desiredSlot = firstVisibleSlot;
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
                assert(selectedItems_.Count() == 0);
            }
            else
            {
                desiredSlot = CurrentSlot();
                action = DataGridInternal::DataGridSelectionAction::None;
            }

            ++noFocusedColumnChangeCount_;
            auto handle2 = dlg_help_utils::make_scope_exit([this]()
                {
                    --noFocusedColumnChangeCount_;
                });
            UpdateSelectionAndCurrency(firstVisibleColumnIndex, desiredSlot, action, true /*scrollIntoView*/);
        }

        if (ColumnHeaderHasFocus())
        {
            if (!CurrentColumn())
            {
                FocusedColumn(ColumnsInternal().FirstVisibleColumn());
            }
            else
            {
                FocusedColumn(CurrentColumn());
            }

            if (firstVisibleSlot == InvalidIndex && FocusedColumn())
            {
                ScrollColumnIntoView(FocusedColumn().as<DataGridColumn>()->Index());
            }
        }

        auto const focusedColumnChanged = ColumnHeaderHasFocus() && oldFocusedColumn != FocusedColumn();
        if (focusedColumnChanged)
        {
            if (auto const internalOldFocusedColumn = oldFocusedColumn.try_as<DataGridColumn>();
                internalOldFocusedColumn && internalOldFocusedColumn->HasHeaderCell())
            {
                internalOldFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }

            if (auto const internalFocusedColumn = FocusedColumn().try_as<DataGridColumn>();
                internalFocusedColumn && internalFocusedColumn->HasHeaderCell())
            {
                internalFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }
        }

        return focusedColumnChanged || successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessNextKey(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().FirstVisibleNonFillerColumn();
        auto const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn.as<DataGridColumn>()->Index();
        if (firstVisibleColumnIndex == InvalidIndex || DisplayData().FirstScrollingSlot() == InvalidIndex)
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessNextKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto nextPageSlot = CurrentSlot() == InvalidIndex ? DisplayData().FirstScrollingSlot() : CurrentSlot();
        assert(nextPageSlot != InvalidIndex);
        auto slot = GetNextVisibleSlot(nextPageSlot);

        auto scrollCount = DisplayData().NumTotallyDisplayedScrollingElements();
        while (scrollCount > 0 && slot < SlotCount())
        {
            nextPageSlot = slot;
            --scrollCount;
            slot = GetNextVisibleSlot(slot);
        }

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        DataGridInternal::DataGridSelectionAction action;
        int32_t columnIndex;
        if (CurrentColumnIndex() == InvalidIndex)
        {
            columnIndex = firstVisibleColumnIndex;
            action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }
        else
        {
            columnIndex = CurrentColumnIndex();
            action = shift && SelectionMode() == DataGridSelectionMode::Extended
                ? action = DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                : action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }

        UpdateSelectionAndCurrency(columnIndex, nextPageSlot, action, true /*scrollIntoView*/);

        return successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessPriorKey(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().FirstVisibleNonFillerColumn();
        auto const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn.as<DataGridColumn>()->Index();
        if (firstVisibleColumnIndex == InvalidIndex || DisplayData().FirstScrollingSlot() == InvalidIndex)
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessPriorKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto previousPageSlot = CurrentSlot() == InvalidIndex ? DisplayData().FirstScrollingSlot() : CurrentSlot();
        assert(previousPageSlot != InvalidIndex);

        auto scrollCount = DisplayData().NumTotallyDisplayedScrollingElements();
        auto slot = GetPreviousVisibleSlot(previousPageSlot);
        while (scrollCount > 0 && slot != InvalidIndex)
        {
            previousPageSlot = slot;
            --scrollCount;
            slot = GetPreviousVisibleSlot(slot);
        }

        assert(previousPageSlot != InvalidIndex);

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        int32_t columnIndex;
        DataGridInternal::DataGridSelectionAction action;
        if (CurrentColumnIndex() == InvalidIndex)
        {
            columnIndex = firstVisibleColumnIndex;
            action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }
        else
        {
            columnIndex = CurrentColumnIndex();
            action = shift && SelectionMode() == DataGridSelectionMode::Extended
                ? DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                : DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }

        UpdateSelectionAndCurrency(columnIndex, previousPageSlot, action, true /*scrollIntoView*/);

        return successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessRightKey(bool const shift, bool const ctrl)
    {
        auto dataGridColumn = ColumnsInternal().LastVisibleColumn().as<DataGridColumn>();
        int32_t const lastVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        int32_t const firstVisibleSlot = FirstVisibleSlot();
        if (lastVisibleColumnIndex == InvalidIndex || (firstVisibleSlot == InvalidIndex && !ColumnHeaderHasFocus()))
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessRightKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto nextVisibleColumnIndex = InvalidIndex;
        if (CurrentColumnIndex() != InvalidIndex)
        {
            dataGridColumn = ColumnsInternal().GetNextVisibleColumn(ColumnsItemsInternal()[CurrentColumnIndex()]).try_as<DataGridColumn>();
            if (dataGridColumn)
            {
                nextVisibleColumnIndex = dataGridColumn->Index();
            }
        }

        auto const oldFocusedColumn = FocusedColumn();

        ++noSelectionChangeCount_;
        {
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });
            if (ctrl)
            {
                return ProcessRightMost(lastVisibleColumnIndex, firstVisibleSlot);
            }

            if (firstVisibleSlot != InvalidIndex && (!RowGroupHeadersTable().Contains(CurrentSlot()) || ColumnHeaderHasFocus()))
            {
                if (CurrentColumnIndex() == InvalidIndex)
                {
                    auto const firstVisibleColumnIndex = !ColumnsInternal().FirstVisibleColumn() ? InvalidIndex : ColumnsInternal().FirstVisibleColumn().as<DataGridColumn>()->Index();
                    UpdateSelectionAndCurrency(firstVisibleColumnIndex, firstVisibleSlot, DataGridInternal::DataGridSelectionAction::SelectCurrent, true /*scrollIntoView*/);
                }
                else
                {
                    if (nextVisibleColumnIndex == InvalidIndex)
                    {
                        return true;
                    }

                    ++noFocusedColumnChangeCount_;
                    auto handle2 = dlg_help_utils::make_scope_exit([this]()
                        {
                            --noFocusedColumnChangeCount_;
                        });
                    UpdateSelectionAndCurrency(nextVisibleColumnIndex, CurrentSlot(), DataGridInternal::DataGridSelectionAction::None, true /*scrollIntoView*/);
                }
            }
        }

        if (ColumnHeaderHasFocus())
        {
            if (!CurrentColumn())
            {
                if (auto const nextDataGridColumn = ColumnsInternal().GetNextVisibleColumn(FocusedColumn());
                    nextDataGridColumn)
                {
                    FocusedColumn(nextDataGridColumn);
                }
            }
            else
            {
                FocusedColumn(CurrentColumn());
            }

            if (firstVisibleSlot == InvalidIndex && !FocusedColumn())
            {
                ScrollColumnIntoView(FocusedColumn().as<DataGridColumn>()->Index());
            }
        }

        auto const focusedColumnChanged = ColumnHeaderHasFocus() && oldFocusedColumn != FocusedColumn();

        if (focusedColumnChanged)
        {
            if (auto const internalOldFocusedColumn = oldFocusedColumn.try_as<DataGridColumn>();
                internalOldFocusedColumn && internalOldFocusedColumn->HasHeaderCell())
            {
                internalOldFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }

            if (auto const internalFocusedColumn = FocusedColumn().try_as<DataGridColumn>();
                internalFocusedColumn && internalFocusedColumn->HasHeaderCell())
            {
                internalFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }
        }

        return focusedColumnChanged || successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessRightMost(int32_t const lastVisibleColumnIndex, int32_t const firstVisibleSlot)
    {
        auto const oldFocusedColumn = FocusedColumn();

        ++noSelectionChangeCount_;
        {
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });
            int32_t desiredSlot;
            DataGridInternal::DataGridSelectionAction action;
            if (CurrentColumnIndex() == InvalidIndex)
            {
                desiredSlot = firstVisibleSlot;
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
            else
            {
                desiredSlot = CurrentSlot();
                action = DataGridInternal::DataGridSelectionAction::None;
            }

            ++noFocusedColumnChangeCount_;
            auto handle2 = dlg_help_utils::make_scope_exit([this]()
                {
                    --noFocusedColumnChangeCount_;
                });
            UpdateSelectionAndCurrency(lastVisibleColumnIndex, desiredSlot, action, true /*scrollIntoView*/);
        }

        if (ColumnHeaderHasFocus())
        {
            if (!CurrentColumn())
            {
                FocusedColumn(ColumnsInternal().LastVisibleColumn());
            }
            else
            {
                FocusedColumn(CurrentColumn());
            }

            if (firstVisibleSlot == InvalidIndex && !FocusedColumn())
            {
                ScrollColumnIntoView(FocusedColumn().as<DataGridColumn>()->Index());
            }
        }

        auto const focusedColumnChanged = ColumnHeaderHasFocus() && oldFocusedColumn != FocusedColumn();

        if (focusedColumnChanged)
        {
            if (auto const internalOldFocusedColumn = oldFocusedColumn.try_as<DataGridColumn>();
                internalOldFocusedColumn && internalOldFocusedColumn->HasHeaderCell())
            {
                internalOldFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }

            if (auto const internalFocusedColumn = FocusedColumn().try_as<DataGridColumn>();
                internalFocusedColumn && internalFocusedColumn->HasHeaderCell())
            {
                internalFocusedColumn->HeaderCell().as<DataGridColumnHeader>()->ApplyState(true);
            }
        }

        return focusedColumnChanged || successfullyUpdatedSelection_;
    }

    bool DataGrid::ProcessSpaceKey()
    {
        return ToggleRowGroup();
    }

    bool DataGrid::ProcessTabKey(Input::KeyRoutedEventArgs const& e)
    {
        auto const [ctrl, shift] = KeyboardHelper::GetMetaKeyState1();
        return ProcessTabKey(e, shift, ctrl);
    }

    bool DataGrid::ProcessTabKey(Input::KeyRoutedEventArgs const& e, bool const shift, bool const ctrl)
    {
        if (ctrl || editingColumnIndex_ == InvalidIndex || IsReadOnly())
        {
            // Go to the next/previous control on the page or the column header when
            // - Ctrl key is used
            // - Potential current cell is not edited, or the DataGrid is read-only.
            if (!shift && ColumnHeaders() && AreColumnHeadersVisible() && !ColumnHeaderHasFocus())
            {
                // Show focus on the current column's header.
                ColumnHeaderHasFocus(true);
                return true;
            }

            if (shift && ColumnHeaderHasFocus())
            {
                ColumnHeaderHasFocus(false);
                return CurrentColumnIndex() != InvalidIndex;
            }

            ColumnHeaderHasFocus(false);
            return false;
        }

        // Try to locate a writable cell before/after the current cell
        assert(CurrentColumnIndex() != InvalidIndex);
        assert(CurrentSlot() != InvalidIndex);

        int32_t neighborSlot;
        MiniDumpExplorer::DataGridColumn dataGridColumn{nullptr};
        if (shift)
        {
            dataGridColumn = ColumnsInternal().GetPreviousVisibleWritableColumn(ColumnsItemsInternal()[CurrentColumnIndex()]);
            neighborSlot = GetPreviousVisibleSlot(CurrentSlot());
            if (EditingRow())
            {
                while (neighborSlot != InvalidIndex && RowGroupHeadersTable().Contains(neighborSlot))
                {
                    neighborSlot = GetPreviousVisibleSlot(neighborSlot);
                }
            }
        }
        else
        {
            dataGridColumn = ColumnsInternal().GetNextVisibleWritableColumn(ColumnsItemsInternal()[CurrentColumnIndex()]);
            neighborSlot = GetNextVisibleSlot(CurrentSlot());
            if (EditingRow())
            {
                while (neighborSlot < SlotCount() && RowGroupHeadersTable().Contains(neighborSlot))
                {
                    neighborSlot = GetNextVisibleSlot(neighborSlot);
                }
            }
        }

        auto const neighborVisibleWritableColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn.as<DataGridColumn>()->Index();

        if (neighborVisibleWritableColumnIndex == InvalidIndex && (neighborSlot == InvalidIndex || neighborSlot >= SlotCount()))
        {
            // There is no previous/next row and no previous/next writable cell on the current row
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), e, shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessTabKey(e, shift, ctrl);
                }
            }))
        {
            return true;
        }

        int32_t targetSlot;
        --noSelectionChangeCount_;
        {
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });

            int32_t targetColumnIndex;
            if (neighborVisibleWritableColumnIndex == InvalidIndex)
            {
                targetSlot = neighborSlot;
                if (shift)
                {
                    assert(ColumnsInternal().LastVisibleWritableColumn());
                    targetColumnIndex = ColumnsInternal().LastVisibleWritableColumn().as<DataGridColumn>()->Index();
                }
                else
                {
                    assert(ColumnsInternal().FirstVisibleWritableColumn());
                    targetColumnIndex = ColumnsInternal().FirstVisibleWritableColumn().as<DataGridColumn>()->Index();
                }
            }
            else
            {
                targetSlot = CurrentSlot();
                targetColumnIndex = neighborVisibleWritableColumnIndex;
            }

            DataGridInternal::DataGridSelectionAction action;
            if (targetSlot != CurrentSlot() || (SelectionMode() == DataGridSelectionMode::Extended))
            {
                if (IsSlotOutOfBounds(targetSlot))
                {
                    return true;
                }

                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
            else
            {
                action = DataGridInternal::DataGridSelectionAction::None;
            }

            UpdateSelectionAndCurrency(targetColumnIndex, targetSlot, action, true /*scrollIntoView*/);
        }

        if (successfullyUpdatedSelection_ && !RowGroupHeadersTable().Contains(targetSlot))
        {
            BeginCellEdit(e);
        }

        // Return true to say we handled the key event even if the operation was unsuccessful. If we don't
        // say we handled this event, the framework will continue to process the tab key and change focus.
        return true;
    }

    bool DataGrid::ProcessUpKey(bool const shift, bool const ctrl)
    {
        auto const dataGridColumn = ColumnsInternal().FirstVisibleNonFillerColumn().try_as<DataGridColumn>();
        auto const firstVisibleColumnIndex = !dataGridColumn ? InvalidIndex : dataGridColumn->Index();
        auto const firstVisibleSlot = FirstVisibleSlot();
        if (firstVisibleColumnIndex == InvalidIndex || firstVisibleSlot == InvalidIndex)
        {
            return false;
        }

        if (WaitForLostFocus([weakPtr = get_weak(), shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ProcessUpKey(shift, ctrl);
                }
            }))
        {
            return true;
        }

        auto const previousVisibleSlot = CurrentSlot() != InvalidIndex ? GetPreviousVisibleSlot(CurrentSlot()) : InvalidIndex;

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        int32_t slot;
        int32_t columnIndex;
        DataGridInternal::DataGridSelectionAction action;
        if (CurrentColumnIndex() == InvalidIndex)
        {
            slot = firstVisibleSlot;
            columnIndex = firstVisibleColumnIndex;
            action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
        }
        else if (ctrl)
        {
            if (shift)
            {
                // Both Ctrl and Shift
                slot = firstVisibleSlot;
                columnIndex = CurrentColumnIndex();
                action = SelectionMode() == DataGridSelectionMode::Extended
                    ? DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent
                    : DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
            else
            {
                // Ctrl without Shift
                slot = firstVisibleSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
        }
        else
        {
            if (previousVisibleSlot == InvalidIndex)
            {
                return true;
            }

            if (shift)
            {
                // Shift without Ctrl
                slot = previousVisibleSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent;
            }
            else
            {
                // Neither Shift nor Ctrl
                slot = previousVisibleSlot;
                columnIndex = CurrentColumnIndex();
                action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
            }
        }

        UpdateSelectionAndCurrency(columnIndex, slot, action, true /*scrollIntoView*/);

        return successfullyUpdatedSelection_;
    }

    void DataGrid::RemoveDisplayedColumnHeader(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const
    {
        if (uint32_t index{};
            columnHeadersPresenter_ && columnHeadersPresenter_.Children().IndexOf(dataGridColumn.as<DataGridColumn>()->HeaderCell(), index))
        {
            columnHeadersPresenter_.Children().RemoveAt(index);
        }
    }

    void DataGrid::RemoveDisplayedColumnHeaders()
    {
        if (columnHeadersPresenter_)
        {
            columnHeadersPresenter_.Children().Clear();
        }

        ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->IsRepresented(false);
    }

    void DataGrid::ResetCurrentCellCore()
    {
        if(CurrentColumnIndex() != InvalidIndex)
        {
            SetCurrentCellCore(InvalidIndex, InvalidIndex);
        }
    }

    void DataGrid::ResetEditingRow()
    {
        auto const oldEditingRow = EditingRow();
        auto const internalOldEditingRow = oldEditingRow.try_as<DataGridRow>();
        if (oldEditingRow &&
            oldEditingRow != focusedRow_ &&
            !IsSlotVisible(internalOldEditingRow->Slot()))
        {
            // Unload the old editing row if it's off-screen
            oldEditingRow.Clip(nullptr);
            UnloadRow(oldEditingRow);
            DisplayData().FullyRecycleElements();
        }

        editingRow_ = nullptr;
        if (oldEditingRow && IsSlotVisible(internalOldEditingRow->Slot()))
        {
            // If the row is no longer editing, then its visuals need to change.
            internalOldEditingRow->ApplyState(true /*animate*/);
        }
    }

    void DataGrid::ResetFocusedRow()
    {
        if (focusedRow_ &&
            focusedRow_ != EditingRow() &&
            !IsSlotVisible(focusedRow_.as<DataGridRow>()->Slot()))
        {
            // Unload the old focused row if it's off-screen
            focusedRow_.Clip(nullptr);
            UnloadRow(focusedRow_);
            DisplayData().FullyRecycleElements();
        }

        focusedRow_ = nullptr;
    }

    void DataGrid::ResetValidationStatus()
    {
        // Clear the invalid status of the Cell, Row and DataGrid
        if (EditingRow())
        {
            auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            internalEditingRow->IsValid(true);
            if (internalEditingRow->Index() != InvalidIndex)
            {
                for(auto const& cell : internalEditingRow->Cells().GetCells())
                {
                    if (!cell.IsValid())
                    {
                        auto const internalCell = cell.as<DataGridCell>();
                        internalCell->IsValid(true);
                        internalCell->ApplyCellState(true);
                    }
                }

                internalEditingRow->ApplyState(true);
            }
        }

        IsValid(true);

        // Clear the previous validation results
        validationResults_.clear();
    }

    void DataGrid::RowGroupHeaderStyles_CollectionChanged([[maybe_unused]] Windows::Foundation::Collections::IObservableVector<Microsoft::UI::Xaml::Style> const& sender, [[maybe_unused]] Windows::Foundation::Collections::IVectorChangedEventArgs const& event)
    {
        if (rowsPresenter_)
        {
            auto const oldLastStyle = rowGroupHeaderStylesOld_.size() > 0 ? rowGroupHeaderStylesOld_[rowGroupHeaderStylesOld_.size() - 1] : nullptr;
            while (rowGroupHeaderStylesOld_.size() < rowGroupHeaderStyles_.Size())
            {
                rowGroupHeaderStylesOld_.push_back(oldLastStyle);
            }

            auto const lastStyle = rowGroupHeaderStyles_.Size() > 0 ? rowGroupHeaderStyles_.GetAt(rowGroupHeaderStyles_.Size() - 1) : nullptr;
            for(auto const& element : rowsPresenter_.Children())
            {
                if (auto const groupHeader = element.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
                    groupHeader)
                {
                    auto const oldStyle = groupHeader.Level() < rowGroupHeaderStylesOld_.size() ? rowGroupHeaderStylesOld_[groupHeader.Level()] : oldLastStyle;
                    auto const newStyle = groupHeader.Level() < rowGroupHeaderStyles_.Size() ? rowGroupHeaderStyles_.GetAt(groupHeader.Level()) : lastStyle;
                    EnsureElementStyle(groupHeader, oldStyle, newStyle);
                }
            }
        }

        rowGroupHeaderStylesOld_.clear();
        for(auto const& style : rowGroupHeaderStyles_)
        {
            rowGroupHeaderStylesOld_.push_back(style);
        }
    }

    void DataGrid::SelectAll()
    {
        SetRowsSelection(0, SlotCount() - 1);
    }

    void DataGrid::SetAndSelectCurrentCell(int32_t const columnIndex, int32_t const slot, bool const forceCurrentCellSelection)
    {
        auto const action = forceCurrentCellSelection ? DataGridInternal::DataGridSelectionAction::SelectCurrent : DataGridInternal::DataGridSelectionAction::None;
        UpdateSelectionAndCurrency(columnIndex, slot, action, false /*scrollIntoView*/);
    }

    // columnIndex = 2, rowIndex = InvalidIndex --> current cell belongs to the 'new row'.
    // columnIndex = 2, rowIndex = 2 --> current cell is an inner cell
    // columnIndex = InvalidIndex, rowIndex = InvalidIndex --> current cell is reset
    // columnIndex = InvalidIndex, rowIndex = 2 --> Unexpected
    bool DataGrid::SetCurrentCellCore(int32_t const columnIndex, int32_t slot, bool const commitEdit, bool const endRowEdit)
    {
        assert(columnIndex == InvalidIndex || columnIndex < ColumnsItemsInternal().size());
        assert(slot == InvalidIndex || slot < SlotCount());
        assert(columnIndex == InvalidIndex || ColumnsItemsInternal()[columnIndex].as<DataGridColumn>()->IsVisible());
        assert(columnIndex <= InvalidIndex || slot != InvalidIndex);

        if (columnIndex == CurrentColumnIndex() &&
            slot == CurrentSlot())
        {
            assert(editingColumnIndex_ == InvalidIndex || editingColumnIndex_ == CurrentColumnIndex());
            assert(!EditingRow() || EditingRow().as<DataGridRow>()->Slot() == CurrentSlot() || DataConnection().EndingEdit());
            return true;
        }

        UIElement oldDisplayedElement{nullptr};
        auto const oldCurrentCell = currentCellCoordinates_;
        Windows::Foundation::IInspectable newCurrentItem{nullptr};

        if (!RowGroupHeadersTable().Contains(slot))
        {
            if (auto const rowIndex = RowIndexFromSlot(slot);
                rowIndex != InvalidIndex && rowIndex < DataConnection().Count())
            {
                newCurrentItem = DataConnection().GetDataItem(rowIndex);
            }
        }

        if (CurrentColumnIndex() != InvalidIndex)
        {
            assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
            assert(CurrentSlot() < SlotCount());

            if (!IsInnerCellOutOfBounds(oldCurrentCell.ColumnIndex, oldCurrentCell.Slot) &&
                IsSlotVisible(oldCurrentCell.Slot))
            {
                oldDisplayedElement = DisplayData().GetDisplayedElement(oldCurrentCell.Slot);
            }

            if (!RowGroupHeadersTable().Contains(oldCurrentCell.Slot) && !temporarilyResetCurrentCell_)
            {
                auto const keepFocus = ContainsFocus();
                if (commitEdit)
                {
                    if (!EndCellEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, keepFocus, true /*raiseEvents*/))
                    {
                        return false;
                    }

                    // Resetting the current cell: setting it to (InvalidIndex, InvalidIndex) is not considered setting it out of bounds
                    if ((columnIndex != InvalidIndex && slot != InvalidIndex && IsInnerCellOutOfSelectionBounds(columnIndex, slot)) ||
                        IsInnerCellOutOfSelectionBounds(oldCurrentCell.ColumnIndex, oldCurrentCell.Slot))
                    {
                        return false;
                    }

                    if (endRowEdit && !EndRowEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, true /*raiseEvents*/))
                    {
                        return false;
                    }
                }
                else
                {
                    CancelEdit(DataGridEditingUnit::Row, false);
                    ExitEdit(keepFocus);
                }
            }
        }

        if (newCurrentItem)
        {
            slot = SlotFromRowIndex(DataConnection().IndexOf(newCurrentItem));
        }

        if (slot == InvalidIndex && columnIndex != InvalidIndex)
        {
            return false;
        }

        if (noFocusedColumnChangeCount_ == 0)
        {
            ColumnHeaderHasFocus(false);
        }

        CurrentColumnIndex(columnIndex);
        CurrentSlot(slot);

        if (temporarilyResetCurrentCell_)
        {
            if (columnIndex != InvalidIndex)
            {
                temporarilyResetCurrentCell_ = false;
            }
        }

        if (!temporarilyResetCurrentCell_ && editingColumnIndex_ != InvalidIndex)
        {
            editingColumnIndex_ = columnIndex;
        }

        if (oldDisplayedElement)
        {
            if (auto const row = oldDisplayedElement.try_as<DataGridRow>();
                row)
            {
                // Don't reset the state of the current cell if we're editing it because that would put it in an invalid state
                UpdateCurrentState(oldDisplayedElement, oldCurrentCell.ColumnIndex, !(temporarilyResetCurrentCell_ && row->IsEditing() && editingColumnIndex_ == oldCurrentCell.ColumnIndex));
            }
            else
            {
                UpdateCurrentState(oldDisplayedElement, oldCurrentCell.ColumnIndex, false /*applyCellState*/);
            }
        }

        if (CurrentColumnIndex() != InvalidIndex)
        {
            assert(CurrentSlot() != InvalidIndex);
            assert(CurrentColumnIndex() < ColumnsItemsInternal().size());
            assert(CurrentSlot() < SlotCount());
            if (IsSlotVisible(CurrentSlot()))
            {
                UpdateCurrentState(DisplayData().GetDisplayedElement(CurrentSlot()), CurrentColumnIndex(), true /*applyCellState*/);
            }
        }

        return true;
    }

    void DataGrid::SetHorizontalOffset(double const newHorizontalOffset)
    {
        if (hScrollBar_ && hScrollBar_.Value() != newHorizontalOffset)
        {
            hScrollBar_.Value(newHorizontalOffset);

            // Unless the control is still loading, show the scroll bars when an offset changes. Keep the existing indicator type.
            if (Media::VisualTreeHelper::GetParent(*this))
            {
                ShowScrollBars();
            }
        }
    }

    void DataGrid::SetVerticalOffset(double const newVerticalOffset)
    {
        VerticalOffset(newVerticalOffset);

        if (vScrollBar_ && !double_utils::are_close(newVerticalOffset, vScrollBar_.Value()))
        {
            vScrollBar_.Value(verticalOffset_);

            // Unless the control is still loading, show the scroll bars when an offset changes. Keep the existing indicator type.
            if (Media::VisualTreeHelper::GetParent(*this))
            {
                ShowScrollBars();
            }
        }
    }

    void DataGrid::ShowScrollBars()
    {
        if (AreAllScrollBarsCollapsed())
        {
            proposedScrollBarsState_ = ScrollBarVisualState::NoIndicator;
            proposedScrollBarsSeparatorState_ = ScrollBarsSeparatorVisualState::SeparatorCollapsedWithoutAnimation;
            SwitchScrollBarsVisualStates(proposedScrollBarsState_, proposedScrollBarsSeparatorState_, false /*useTransitions*/);
        }
        else
        {
            if (hideScrollBarsTimer_ && hideScrollBarsTimer_.IsRunning())
            {
                hideScrollBarsTimer_.Stop();
                hideScrollBarsTimer_.Start();
            }

            // Mouse indicators dominate if they are already showing or if we have set the flag to prefer them.
            if (preferMouseIndicators_ || showingMouseIndicators_)
            {
                if (AreBothScrollBarsVisible() && (isPointerOverHorizontalScrollBar_ || isPointerOverVerticalScrollBar_))
                {
                    proposedScrollBarsState_ = ScrollBarVisualState::MouseIndicatorFull;
                }
                else
                {
                    proposedScrollBarsState_ = ScrollBarVisualState::MouseIndicator;
                }

                showingMouseIndicators_ = true;
            }
            else
            {
                proposedScrollBarsState_ = ScrollBarVisualState::TouchIndicator;
            }

            // Select the proper state for the scroll bars separator square within the GroupScrollBarsSeparator group:
            if (UISettingsHelper::AreSettingsEnablingAnimations())
            {
                // When OS animations are turned on, show the square when a scroll bar is shown unless the DataGrid is disabled, using an animation.
                proposedScrollBarsSeparatorState_ =
                    IsEnabled() &&
                    proposedScrollBarsState_ == ScrollBarVisualState::MouseIndicatorFull ?
                    ScrollBarsSeparatorVisualState::SeparatorExpanded : ScrollBarsSeparatorVisualState::SeparatorCollapsed;
            }
            else
            {
                // OS animations are turned off. Show or hide the square depending on the presence of a scroll bars, without an animation.
                // When the DataGrid is disabled, hide the square in sync with the scroll bar(s).
                if (proposedScrollBarsState_ == ScrollBarVisualState::MouseIndicatorFull)
                {
                    proposedScrollBarsSeparatorState_ = IsEnabled() ? ScrollBarsSeparatorVisualState::SeparatorExpandedWithoutAnimation : ScrollBarsSeparatorVisualState::SeparatorCollapsed;
                }
                else
                {
                    proposedScrollBarsSeparatorState_ = IsEnabled() ? ScrollBarsSeparatorVisualState::SeparatorCollapsedWithoutAnimation : ScrollBarsSeparatorVisualState::SeparatorCollapsed;
                }
            }

            if (!UISettingsHelper::AreSettingsAutoHidingScrollBars())
            {
                if (AreBothScrollBarsVisible())
                {
                    if (UISettingsHelper::AreSettingsEnablingAnimations())
                    {
                        SwitchScrollBarsVisualStates(ScrollBarVisualState::MouseIndicatorFull, IsEnabled() ? ScrollBarsSeparatorVisualState::SeparatorExpanded : ScrollBarsSeparatorVisualState::SeparatorCollapsed, true /*useTransitions*/);
                    }
                    else
                    {
                        SwitchScrollBarsVisualStates(ScrollBarVisualState::MouseIndicatorFull, IsEnabled() ? ScrollBarsSeparatorVisualState::SeparatorExpandedWithoutAnimation : ScrollBarsSeparatorVisualState::SeparatorCollapsed, true /*useTransitions*/);
                    }
                }
                else
                {
                    if (UISettingsHelper::AreSettingsEnablingAnimations())
                    {
                        SwitchScrollBarsVisualStates(ScrollBarVisualState::MouseIndicator, ScrollBarsSeparatorVisualState::SeparatorCollapsed, true /*useTransitions*/);
                    }
                    else
                    {
                        SwitchScrollBarsVisualStates(ScrollBarVisualState::MouseIndicator, IsEnabled() ? ScrollBarsSeparatorVisualState::SeparatorCollapsedWithoutAnimation : ScrollBarsSeparatorVisualState::SeparatorCollapsed, true /*useTransitions*/);
                    }
                }
            }
            else
            {
                SwitchScrollBarsVisualStates(proposedScrollBarsState_, proposedScrollBarsSeparatorState_, true /*useTransitions*/);
            }
        }
    }

    void DataGrid::StopHideScrollBarsTimer() const
    {
        if (hideScrollBarsTimer_ && hideScrollBarsTimer_.IsRunning())
        {
            hideScrollBarsTimer_.Stop();
        }
    }

    void DataGrid::SwitchScrollBarsVisualStates(ScrollBarVisualState const scrollBarsState, ScrollBarsSeparatorVisualState const separatorState, bool const useTransitions)
    {
        switch (scrollBarsState)
        {
        case ScrollBarVisualState::NoIndicator:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateNoIndicator);

            if (!hasNoIndicatorStateStoryboardCompletedHandler_)
            {
                showingMouseIndicators_ = false;
            }

            break;
        case ScrollBarVisualState::TouchIndicator:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateTouchIndicator);
            break;
        case ScrollBarVisualState::MouseIndicator:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateMouseIndicator);
            break;
        case ScrollBarVisualState::MouseIndicatorFull:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateMouseIndicatorFull);
            break;
        }

        switch (separatorState)
        {
        case ScrollBarsSeparatorVisualState::SeparatorCollapsed:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateSeparatorCollapsed);
            break;
        case ScrollBarsSeparatorVisualState::SeparatorExpanded:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateSeparatorExpanded);
            break;
        case ScrollBarsSeparatorVisualState::SeparatorExpandedWithoutAnimation:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateSeparatorExpandedWithoutAnimation);
            break;
        case ScrollBarsSeparatorVisualState::SeparatorCollapsedWithoutAnimation:
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateSeparatorCollapsedWithoutAnimation);
            break;
        }
    }

    void DataGrid::UnhookHorizontalScrollBarEvents()
    {
        if (hScrollBar_ && hScrollBarScrollEvent_)
        {
            hScrollBar_.Scroll(hScrollBarScrollEvent_);
            hScrollBarScrollEvent_ = {};
            hScrollBar_.PointerEntered(hScrollBarPointerEnteredEvent_);
            hScrollBarPointerEnteredEvent_ = {};
            hScrollBar_.PointerExited(hScrollBarPointerExitedEvent_);
            hScrollBarPointerExitedEvent_ = {};
        }
    }

    void DataGrid::UnhookVerticalScrollBarEvents()
    {
        if (vScrollBar_ && vScrollBarScrollEvent_)
        {
            vScrollBar_.Scroll(vScrollBarScrollEvent_);
            vScrollBarScrollEvent_ = {};
            vScrollBar_.PointerEntered(vScrollBarPointerEnteredEvent_);
            vScrollBarPointerEnteredEvent_ = {};
            vScrollBar_.PointerExited(vScrollBarPointerExitedEvent_);
            vScrollBarPointerExitedEvent_ = {};
        }
    }

    void DataGrid::UpdateCurrentState(UIElement const& displayedElement, int32_t const columnIndex, bool const applyCellState) const
    {
        if (auto const row = displayedElement.try_as<DataGridRow>();
            row)
        {
            if (AreRowHeadersVisible())
            {
                row->ApplyHeaderState(true /*animate*/);
            }

            if (applyCellState)
            {
                auto const cell = row->Cells()[columnIndex].as<DataGridCell>();
                cell->ApplyCellState(true /*animate*/);
            }
        }
        else if (auto const groupHeader = displayedElement.try_as<DataGridRowGroupHeader>();
                groupHeader)
        {
            groupHeader->ApplyState(true /*useTransitions*/);
            if (AreRowHeadersVisible())
            {
                groupHeader->ApplyHeaderState(true /*animate*/);
            }
        }
    }

    void DataGrid::UpdateDisabledVisual() const
    {
        if (IsEnabled())
        {
            VisualStates::GoToState(*this, true, VisualStates::StateNormal);
        }
        else
        {
            VisualStates::GoToState(*this, true, VisualStates::StateDisabled, VisualStates::StateNormal);
        }
    }

    void DataGrid::UpdateHorizontalScrollBar(bool const needHorizontalScrollBar, bool const forceHorizontalScrollBar, double const totalVisibleWidth, double const totalVisibleFrozenWidth, double const cellsWidth)
    {
        if (hScrollBar_)
        {
            if (needHorizontalScrollBar || forceHorizontalScrollBar)
            {
                // ..........viewportSize
                //         v---v
                // |<|_____|###|>|
                //   ^     ^
                //   min   max

                // we want to make the relative size of the thumb reflect the relative size of the viewing area
                // viewportSize / (max + viewportSize) = cellsWidth / max
                // -> viewportSize = max * cellsWidth / (max - cellsWidth)

                // always zero
                hScrollBar_.Minimum(0);
                if (needHorizontalScrollBar)
                {
                    // maximum travel distance -- not the total width
                    hScrollBar_.Maximum(totalVisibleWidth - cellsWidth);
                    assert(totalVisibleFrozenWidth >= 0);
                    if (frozenColumnScrollBarSpacer_)
                    {
                        frozenColumnScrollBarSpacer_.Width(totalVisibleFrozenWidth);
                    }

                    assert(hScrollBar_.Maximum() >= 0);

                    // width of the scrollable viewing area
                    auto const viewPortSize = std::max(0.0, cellsWidth - totalVisibleFrozenWidth);
                    hScrollBar_.ViewportSize(viewPortSize);
                    hScrollBar_.LargeChange(viewPortSize);

                    // The ScrollBar should be in sync with HorizontalOffset at this point.  There's a resize case
                    // where the ScrollBar will coerce an old value here, but we don't want that.
                    SetHorizontalOffset(horizontalOffset_);

                    hScrollBar_.IsEnabled(true);
                }
                else
                {
                    hScrollBar_.Maximum(0);
                    hScrollBar_.ViewportSize(0);
                    hScrollBar_.IsEnabled(false);
                }

                if (hScrollBar_.Visibility() != Visibility::Visible)
                {
                    // This will trigger a call to this method via Cells_SizeChanged for which no processing is needed.
                    hScrollBar_.Visibility(Visibility::Visible);
                    ignoreNextScrollBarsLayout_ = true;

                    if (!IsHorizontalScrollBarOverCells() && hScrollBar_.DesiredSize().Height == 0)
                    {
                        // We need to know the height for the rest of layout to work correctly so measure it now
                        hScrollBar_.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
                    }
                }
            }
            else
            {
                hScrollBar_.Maximum(0);
                if (hScrollBar_.Visibility() != Visibility::Collapsed)
                {
                    // This will trigger a call to this method via Cells_SizeChanged for which no processing is needed.
                    hScrollBar_.Visibility(Visibility::Collapsed);
                    ignoreNextScrollBarsLayout_ = true;
                }
            }

            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer)
            {
                peer->RaiseAutomationScrollEvents();
            }
        }
    }

    void DataGrid::UpdateRowDetailsVisibilityMode(DataGridRowDetailsVisibilityMode const newDetailsMode)
    {
        if (rowsPresenter_ && DataConnection().Count() > 0)
        {
            auto newDetailsVisibility = Visibility::Collapsed;

            switch (newDetailsMode)
            {
            case DataGridRowDetailsVisibilityMode::Visible:
                newDetailsVisibility = Visibility::Visible;
                break;
            case DataGridRowDetailsVisibilityMode::Collapsed:
                newDetailsVisibility = Visibility::Collapsed;
                break;
            case DataGridRowDetailsVisibilityMode::VisibleWhenSelected:
                break;
            }

            ClearShowDetailsTable();

            auto updated = false;
            for(auto const& row : GetAllRows())
            {
                if (row.Visibility() == Visibility::Visible)
                {
                    auto const internalRow = row.as<DataGridRow>();
                    if (newDetailsMode == DataGridRowDetailsVisibilityMode::VisibleWhenSelected)
                    {
                        // For VisibleWhenSelected, we need to calculate the value for each individual row
                        newDetailsVisibility = selectedItems_.ContainsSlot(internalRow->Slot()) && row.Index() != DataConnection().NewItemPlaceholderIndex() ? Visibility::Visible : Visibility::Collapsed;
                    }

                    if (row.DetailsVisibility() != newDetailsVisibility)
                    {
                        updated = true;
                        internalRow->SetDetailsVisibilityInternal(
                            newDetailsVisibility,
                            true /*raiseNotification*/);
                    }
                }
            }

            if (updated)
            {
                UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), CellsHeight());
                InvalidateRowsMeasure(false /*invalidateIndividualElements*/);
            }
        }
    }

    void DataGrid::UpdateRowsPresenterManipulationMode(bool const horizontalMode, bool const verticalMode) const
    {
        if (rowsPresenter_)
        {
            auto manipulationMode = rowsPresenter_.ManipulationMode();

            if (horizontalMode)
            {
                if (HorizontalScrollBarVisibility() != Controls::ScrollBarVisibility::Disabled)
                {
                    manipulationMode |= Input::ManipulationModes::TranslateX | Input::ManipulationModes::TranslateInertia;
                }
                else
                {
                    manipulationMode &= ~(Input::ManipulationModes::TranslateX | Input::ManipulationModes::TranslateRailsX);
                }
            }

            if (verticalMode)
            {
                if (VerticalScrollBarVisibility() != Controls::ScrollBarVisibility::Disabled)
                {
                    manipulationMode |= Input::ManipulationModes::TranslateY | Input::ManipulationModes::TranslateInertia;
                }
                else
                {
                    manipulationMode &= ~(Input::ManipulationModes::TranslateY | Input::ManipulationModes::TranslateRailsY);
                }
            }

            if ((manipulationMode & (Input::ManipulationModes::TranslateX | Input::ManipulationModes::TranslateY)) == (Input::ManipulationModes::TranslateX | Input::ManipulationModes::TranslateY))
            {
                manipulationMode |= Input::ManipulationModes::TranslateRailsX | Input::ManipulationModes::TranslateRailsY;
            }

            if ((manipulationMode & (Input::ManipulationModes::TranslateX | Input::ManipulationModes::TranslateRailsX | Input::ManipulationModes::TranslateY | Input::ManipulationModes::TranslateRailsY)) ==
                Input::ManipulationModes::None)
            {
                manipulationMode &= ~Input::ManipulationModes::TranslateInertia;
            }

            rowsPresenter_.ManipulationMode(manipulationMode);
        }
    }

    bool DataGrid::UpdateStateOnTapped(Input::TappedRoutedEventArgs const& args, int32_t const columnIndex, int32_t const slot, bool const allowEdit, bool const shift, bool const ctrl)
    {
        assert(slot != InvalidIndex);

        // Before changing selection, check if the current cell needs to be committed, and
        // check if the current row needs to be committed. If any of those two operations are required and fail,
        // do not change selection, and do not change current cell.
        auto const wasInEdit = EditingColumnIndex() != InvalidIndex;

        if (IsSlotOutOfBounds(slot))
        {
            return true;
        }

        if (wasInEdit && (columnIndex != EditingColumnIndex() || slot != CurrentSlot()) &&
            WaitForLostFocus([weakPtr = get_weak(), args, columnIndex, slot, allowEdit, shift, ctrl]
            {
                if (auto const self = weakPtr.get())
                {
                    self->UpdateStateOnTapped(args, columnIndex, slot, allowEdit, shift, ctrl);
                }
            }))
        {
            return true;
        }

        bool beginEdit;
        {
            ++noSelectionChangeCount_;
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });

            beginEdit = allowEdit &&
                CurrentSlot() == slot &&
                columnIndex != InvalidIndex &&
                (wasInEdit || CurrentColumnIndex() == columnIndex) &&
                !GetColumnEffectiveReadOnlyState(ColumnsItemsInternal()[columnIndex]);

            DataGridInternal::DataGridSelectionAction action;
            if (SelectionMode() == DataGridSelectionMode::Extended && shift)
            {
                // Shift select multiple rows.
                action = DataGridInternal::DataGridSelectionAction::SelectFromAnchorToCurrent;
            }
            else if (GetRowSelection(slot))
            {
                // Unselecting single row or Selecting a previously multi-selected row.
                if (!ctrl && SelectionMode() == DataGridSelectionMode::Extended && selectedItems_.Count() != 0)
                {
                    // Unselect everything except the row that was clicked on.
                    action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
                }
                else if (ctrl && !EditingRow())
                {
                    action = DataGridInternal::DataGridSelectionAction::RemoveCurrentFromSelection;
                }
                else
                {
                    action = DataGridInternal::DataGridSelectionAction::None;
                }
            }
            else
            {
                // Selecting a single row or multi-selecting with Ctrl.
                if (SelectionMode() == DataGridSelectionMode::Single || !ctrl)
                {
                    // Unselect the correctly selected rows except the new selected row.
                    action = DataGridInternal::DataGridSelectionAction::SelectCurrent;
                }
                else
                {
                    action = DataGridInternal::DataGridSelectionAction::AddCurrentToSelection;
                }
            }

            UpdateSelectionAndCurrency(columnIndex, slot, action, false /*scrollIntoView*/);
        }

        if (successfullyUpdatedSelection_ && beginEdit && BeginCellEdit(args))
        {
            FocusEditingCell(true /*setFocus*/);
        }

        return true;
    }

    void DataGrid::UpdateValidationResults(std::vector<Utility::ValidationResult> const& newValidationResults, bool const scrollIntoView)
    {
        auto validationResultsChanged = false;
        assert(EditingRow());

        // Remove the validation results that have been fixed
        std::vector<int32_t> removedValidationResults;
        for(int32_t index = 0; index < validationResults_.size(); ++index)
        {
            if (auto const& oldValidationResult = validationResults_[index];
                !ContainsEqualValidationResult(newValidationResults, oldValidationResult))
            {
                removedValidationResults.push_back(index);
                validationResultsChanged = true;
            }
        }

        for(auto const index : removedValidationResults | std::views::reverse)
        {
            validationResults_.erase(validationResults_.begin() + index);
        }

        // Add any validation results that were just introduced
        for(auto const& newValidationResult : newValidationResults)
        {
            if (!ContainsEqualValidationResult(validationResults_, newValidationResult))
            {
                validationResults_.push_back(newValidationResult);
                validationResultsChanged = true;
            }
        }

        if (validationResultsChanged)
        {
            UpdateValidationStatus();
        }

        if (!IsValid() && scrollIntoView)
        {
            // Scroll the row with the error into view.
            auto const editingRowSlot = EditingRow().as<DataGridRow>()->Slot();
            ScrollSlotIntoView(editingRowSlot, false /*scrolledHorizontally*/);
        }
    }

    void DataGrid::UpdateValidationStatus() const
    {
        if (auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            internalEditingRow)
        {
            for(auto const& cell : internalEditingRow->Cells().GetCells())
            {
                auto const internalCell = cell.as<DataGridCell>();
                auto isCellValid = true;

                assert(internalCell->OwningColumn());
                if (!internalCell->OwningColumn().IsReadOnly())
                {
                    for(auto const& validationResult : validationResults_)
                    {
                        if (ContainsEqualValidationResult(propertyValidationResults_, validationResult))
                        {
                            for(auto const& bindingPath : validationResult.MemberNames())
                            {
                                if (auto const bindingPaths = internalCell->OwningColumn().as<DataGridColumn>()->BindingPaths();
                                    std::ranges::find(bindingPaths, bindingPath) != bindingPaths.end())
                                {
                                    isCellValid = false;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (cell.IsValid() != isCellValid)
                {
                    internalCell->IsValid(isCellValid);
                    internalCell->ApplyCellState(true /*animate*/);
                }
            }

            auto const isRowValid = validationResults_.empty();
            if (internalEditingRow->IsValid() != isRowValid)
            {
                internalEditingRow->IsValid(isRowValid);
                internalEditingRow->ApplyState(true /*animate*/);
            }

            IsValid(isRowValid);
        }
        else
        {
            IsValid(true);
        }
    }

    void DataGrid::UpdateVerticalScrollBar(bool const needVertScrollBar, bool const forceVertScrollBar, double const totalVisibleHeight, double const cellsHeight)
    {
        if (vScrollBar_)
        {
            if (needVertScrollBar || forceVertScrollBar)
            {
                // ..........viewportSize
                //         v---v
                // |<|_____|###|>|
                //   ^     ^
                //   min   max

                // we want to make the relative size of the thumb reflect the relative size of the viewing area
                // viewportSize / (max + viewportSize) = cellsWidth / max
                // -> viewportSize = max * cellsHeight / (totalVisibleHeight - cellsHeight)
                // ->              = max * cellsHeight / (totalVisibleHeight - cellsHeight)
                // ->              = max * cellsHeight / max
                // ->              = cellsHeight

                // always zero
                vScrollBar_.Minimum(0);
                if (needVertScrollBar && !std::isinf(cellsHeight))
                {
                    // maximum travel distance -- not the total height
                    assert(totalVisibleHeight >= cellsHeight);
                    vScrollBar_.Maximum(totalVisibleHeight - cellsHeight);
                    assert(vScrollBar_.Maximum() >= 0);

                    // total height of the display area
                    vScrollBar_.ViewportSize(cellsHeight);
                    vScrollBar_.LargeChange(cellsHeight);
                    vScrollBar_.IsEnabled(true);
                }
                else
                {
                    vScrollBar_.Maximum(0);
                    vScrollBar_.ViewportSize(0);
                    vScrollBar_.IsEnabled(false);
                }

                if (vScrollBar_.Visibility() != Visibility::Visible)
                {
                    // This will trigger a call to this method via Cells_SizeChanged for which no processing is needed.
                    vScrollBar_.Visibility(Visibility::Visible);
                    ignoreNextScrollBarsLayout_ = true;

                    if (!IsVerticalScrollBarOverCells() && vScrollBar_.DesiredSize().Width == 0)
                    {
                        // We need to know the width for the rest of layout to work correctly so measure it now.
                        vScrollBar_.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
                    }
                }
            }
            else
            {
                vScrollBar_.Maximum(0);
                if (vScrollBar_.Visibility() != Visibility::Collapsed)
                {
                    // This will trigger a call to this method via Cells_SizeChanged for which no processing is needed.
                    vScrollBar_.Visibility(Visibility::Collapsed);
                    ignoreNextScrollBarsLayout_ = true;
                }
            }

            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer)
            {
                peer->RaiseAutomationScrollEvents();
            }
        }
    }

    bool DataGrid::ValidateEditingRow(bool const scrollIntoView, bool const wireEvents)
    {
        std::vector<Utility::ValidationResult> validationResults;
        if (initializingNewItem_)
        {
            // We only want to run property validation if we're initializing a new item. Instead of
            // clearing all the errors, we will only remove those associated with the current column.
            validationResults = validationResults_;
        }
        else
        {
            // We're going to run full entity-level validation, so throw away the
            // old errors since they will be recreated if they're still active.
            propertyValidationResults_.clear();
            indeiValidationResults_.clear();
        }

        if (EditingRow())
        {
            auto const dataItem = EditingRow().DataContext();
            assert(dataItem);

            if (!initializingNewItem_)
            {
                // INDEI entity validation.
                ValidateIndei(dataItem.try_as<Data::INotifyDataErrorInfo>(), {}, {}, {}, validationResults, wireEvents);
            }

            // IDEI and INDEI property validation.
            for(auto const& column : ColumnsInternal().GetDisplayedColumns([](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->IsVisible() && !c.IsReadOnly(); }))
            {
                if (!initializingNewItem_ || column == CurrentColumn())
                {
                    for(auto const& bindingPath : column.as<DataGridColumn>()->BindingPaths())
                    {
                        std::wstring declaringPath;
                        auto declaringItem = dataItem;
                        auto bindingProperty = bindingPath;

                        // Check for nested paths.
                        if (auto const lastIndexOfSeparator = StringHelper::LastIndexOfAny(bindingPath, TypeHelper::PropertyNameSeparator, TypeHelper::LeftIndexerToken);
                            lastIndexOfSeparator != std::wstring::npos)
                        {
                            declaringPath = bindingPath.substr(0, lastIndexOfSeparator);
                            declaringItem = TypeHelper::GetNestedDependencyPropertyValue(dataItem, declaringPath);
                            if (bindingProperty[lastIndexOfSeparator] == TypeHelper::LeftIndexerToken)
                            {
                                bindingProperty = TypeHelper::PrependDefaultMemberName(declaringItem, bindingPath.substr(lastIndexOfSeparator));
                            }
                            else
                            {
                                bindingProperty = bindingPath.substr(lastIndexOfSeparator + 1);
                            }
                        }

                        if (initializingNewItem_)
                        {
                            // We're only re-validating the current column, so remove its old errors
                            // because we're about to check if they're still relevant.
                            for(auto const& oldValidationResult : validationResults_)
                            {
                                if (ContainsMemberName(oldValidationResult, bindingPath))
                                {
                                    Erase(validationResults, oldValidationResult);
                                    Erase(indeiValidationResults_, oldValidationResult);
                                    Erase(propertyValidationResults_, oldValidationResult);
                                }
                            }
                        }

                        // INDEI property validation.
                        ValidateIndei(declaringItem.try_as<Data::INotifyDataErrorInfo>(), bindingProperty, bindingPath, declaringPath, validationResults, wireEvents);
                    }
                }
            }

            // Add any existing exception errors (in case we're editing a cell).
            // Note: these errors will only be displayed in the ValidationSummary if the
            // editing data item implements IDEI or INDEI.
            for(auto const& validationResult : bindingValidationResults_)
            {
                AddIfNew(validationResults, validationResult);
                AddIfNew(propertyValidationResults_, validationResult);
            }

            // Merge the new validation results with the existing ones.
            UpdateValidationResults(validationResults, scrollIntoView);

            // Return false if there are validation errors.
            if (!IsValid())
            {
                return false;
            }
        }

        // Return true if there are no errors or there is no editing row.
        ResetValidationStatus();
        return true;
    }

    void DataGrid::ValidateIndei(Data::INotifyDataErrorInfo const& indei, std::wstring const& bindingProperty, std::wstring const& bindingPath, std::wstring const& declaringPath, std::vector<Utility::ValidationResult>& validationResults, bool const wireEvents)
    {
        if (indei)
        {
            if (indei.HasErrors())
            {
                Windows::Foundation::Collections::IIterable<Windows::Foundation::IInspectable> errors{nullptr};
                try
                {
                    errors = indei.GetErrors(bindingProperty);
                }
                catch (hresult_error const& ex)
                {
                    // We will get a SecurityException if the user does not allow access to the clipboard.
                    auto const errorMessage = std::format(L"GetErrors failed: {}", ex.message());
                    OutputDebugStringW(errorMessage.c_str());
                }
                if (errors)
                {
                    for(auto const& errorItem : errors)
                    {
                        if (errorItem)
                        {
                            std::wstring errorString;
                            try
                            {
                                errorString = UIHelper::ObjectToString(errorItem);
                            }
                            catch (hresult_error const& ex)
                            {
                                // We will get a SecurityException if the user does not allow access to the clipboard.
                                auto const errorMessage = std::format(L"ObjectToString failed: {}", ex.message());
                                OutputDebugStringW(errorMessage.c_str());
                            }

                            if (!errorString.empty())
                            {
                                Utility::ValidationResult validationResult;
                                if (!bindingProperty.empty())
                                {
                                    validationResult = Utility::ValidationResult{errorString, bindingPath};
                                    propertyValidationResults_.push_back(validationResult);
                                }
                                else
                                {
                                    assert(bindingPath.empty());
                                    validationResult = Utility::ValidationResult{errorString};
                                }

                                AddIfNew(validationResults, validationResult);
                                AddIfNew(indeiValidationResults_, validationResult);
                            }
                        }
                    }
                }
            }

            if (wireEvents && validationItems_.contains(indei))
            {
                auto const event = indei.ErrorsChanged({this, &DataGrid::ValidationItem_ErrorsChanged});
                validationItems_.insert(std::make_pair(indei, ValidationItem{declaringPath, event}));
            }
        }
    }

    void DataGrid::ValidationItem_ErrorsChanged(Windows::Foundation::IInspectable const& sender, Data::DataErrorsChangedEventArgs const& e)
    {
        auto const indei = sender.try_as<Data::INotifyDataErrorInfo>();
        if (auto const it = validationItems_.find(indei);
            it != validationItems_.end())
        {
            assert(EditingRow());

            // Determine the binding path.
            auto bindingPath = it->second.path;
            auto const propertyName = std::wstring{e.PropertyName()};
            if (bindingPath.empty())
            {
                bindingPath = propertyName;
            }
            else if (!propertyName.empty() && propertyName.find_first_of(TypeHelper::LeftIndexerToken) != std::wstring::npos)
            {
                bindingPath += TypeHelper::RemoveDefaultMemberName(propertyName);
            }
            else
            {
                bindingPath += TypeHelper::PropertyNameSeparator + propertyName;
            }

            // Remove the old errors.
            std::vector<Utility::ValidationResult> validationResults;
            for(auto const& validationResult : validationResults_)
            {
                
                if (auto oldValidationResult = FindEqualValidationResult(indeiValidationResults_, validationResult);
                    oldValidationResult.has_value() && ContainsMemberName(oldValidationResult.value(), bindingPath))
                {
                    Erase(indeiValidationResults_, oldValidationResult.value());
                }
                else
                {
                    validationResults.push_back(validationResult);
                }
            }

            // Find any new errors and update the visuals.
            ValidateIndei(indei, propertyName, bindingPath, {}, validationResults, false /*wireEvents*/);
            UpdateValidationResults(validationResults, false /*scrollIntoView*/);

            // If we're valid now then reset our status.
            if (IsValid())
            {
                ResetValidationStatus();
            }
        }
    }

    void DataGrid::VerticalScrollBar_Scroll([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Controls::Primitives::ScrollEventArgs const& e)
    {
        ProcessVerticalScroll(e.ScrollEventType());
    }

    void DataGrid::EnsureElementStyle(FrameworkElement const& element, Microsoft::UI::Xaml::Style const& oldDataGridStyle, Microsoft::UI::Xaml::Style const& newDataGridStyle)
    {
        assert(element);

        // Apply the DataGrid style if the row was using the old DataGridRowStyle before
        if (element && (!element.Style() || element.Style() == oldDataGridStyle))
        {
            UIHelper::SetStyleWithType(element, newDataGridStyle);
        }
    }
}
