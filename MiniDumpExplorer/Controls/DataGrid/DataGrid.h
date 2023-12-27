#pragma once

#include "DataGrid.g.h"

#include <experimental/generator>
#include <functional>

#include "DataGridCellCoordinates.h"
#include "DataGridColumnCollection.h"
#include "DataGridColumnHeaderInteractionInfo.h"
#include "DataGridDataConnection.h"
#include "DataGridDisplayData.h"
#include "DataGridRowGroupInfo.h"
#include "DataGridSelectedItemsCollection.h"
#include "DataGridSelectionAction.h"
#include "Utility\IndexToValueTable.h"
#include "Utility\ValidationResult.h"
#include "Utility\WeakEventListener.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridAutoGeneratingColumnEventArgs;
    struct DataGridAutomationPeer;
    struct DataGridBeginningEditEventArgs;
    struct DataGridBoundColumn;
    struct DataGridCell;
    struct DataGridCellEditEndedEventArgs;
    struct DataGridCellEditEndingEventArgs;
    struct DataGridClipboardCellContent;
    struct DataGridColumn;
    struct DataGridColumnEventArgs;
    struct DataGridColumnHeadersPresenter;
    struct DataGridColumnReorderingEventArgs;
    struct DataGridPreparingCellForEditEventArgs;
    struct DataGridLength;
    struct DataGridRow;
    struct DataGridRowClipboardEventArgs;
    struct DataGridRowDetailsEventArgs;
    struct DataGridRowEditEndedEventArgs;
    struct DataGridRowEditEndingEventArgs;
    struct DataGridRowEventArgs;
    struct DataGridRowGroupHeader;
    struct DataGridRowGroupHeaderEventArgs;
    struct DataGridRowsPresenter;
    struct DataGridTemplateColumn;

    struct DataGrid : DataGridT<DataGrid>
    {
        static constexpr auto InvalidIndex = int32_t{-1};
        static constexpr double MaximumStarColumnWidth = 10000;
        static constexpr double MinimumStarColumnWidth = 0.001;
        static constexpr double DefaultRowGroupSublevelIndent = 20;
        static constexpr double HorizontalGridLinesThickness = 1;
        static constexpr double DefaultRowHeight = 22;

        DataGrid();

        event_token AutoGeneratingColumn(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs> const& value);
        void AutoGeneratingColumn(event_token const& value);
        event_token BeginningEdit(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridBeginningEditEventArgs> const& value);
        void BeginningEdit(event_token const& value);
        event_token CellEditEnded(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndedEventArgs> const& value);
        void CellEditEnded(event_token const& value);
        event_token CellEditEnding(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndingEventArgs> const& value);
        void CellEditEnding(event_token const& value);
        event_token ColumnDisplayIndexChanged(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value);
        void ColumnDisplayIndexChanged(event_token const& value);
        event_token ColumnHeaderDragCompleted(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragCompletedEventArgs> const& value);
        void ColumnHeaderDragCompleted(event_token const& value);
        event_token ColumnHeaderDragDelta(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragDeltaEventArgs> const& value);
        void ColumnHeaderDragDelta(event_token const& value);
        event_token ColumnHeaderDragStarted(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragStartedEventArgs> const& value);
        void ColumnHeaderDragStarted(event_token const& value);
        event_token ColumnReordered(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value);
        void ColumnReordered(event_token const& value);
        event_token ColumnReordering(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnReorderingEventArgs> const& value);
        void ColumnReordering(event_token const& value);
        event_token CopyingRowClipboardContent(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowClipboardEventArgs> const& value);
        void CopyingRowClipboardContent(event_token const& value);
        event_token CurrentCellChanged(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs> const& value);
        void CurrentCellChanged(event_token const& value);
        event_token LoadingRow(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs> const& value);
        void LoadingRow(event_token const& value);
        event_token LoadingRowDetails(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value);
        void LoadingRowDetails(event_token const& value);
        event_token LoadingRowGroup(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs> const& value);
        void LoadingRowGroup(event_token const& value);
        event_token PreparingCellForEdit(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridPreparingCellForEditEventArgs> const& value);
        void PreparingCellForEdit(event_token const& value);
        event_token RowDetailsVisibilityChanged(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value);
        void RowDetailsVisibilityChanged(event_token const& value);
        event_token RowEditEnded(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndedEventArgs> const& value);
        void RowEditEnded(event_token const& value);
        event_token RowEditEnding(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndingEventArgs> const& value);
        void RowEditEnding(event_token const& value);
        event_token SelectionChanged(Microsoft::UI::Xaml::Controls::SelectionChangedEventHandler const& value);
        void SelectionChanged(event_token const& value);
        event_token Sorting(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs> const& value);
        void Sorting(event_token const& value);
        event_token UnloadingRow(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs> const& value);
        void UnloadingRow(event_token const& value);
        event_token UnloadingRowGroup(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs> const& value);
        void UnloadingRowGroup(event_token const& value);
        event_token UnloadingRowDetails(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value);
        void UnloadingRowDetails(event_token const& value);
        event_token RowDoubleTapped(Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs> const& value);
        void RowDoubleTapped(event_token const& value);
        
        static Microsoft::UI::Xaml::DependencyProperty AlternatingRowBackgroundProperty();
        static Microsoft::UI::Xaml::DependencyProperty AlternatingRowForegroundProperty();
        static Microsoft::UI::Xaml::DependencyProperty AreRowDetailsFrozenProperty();
        static Microsoft::UI::Xaml::DependencyProperty AreRowGroupHeadersFrozenProperty();
        static Microsoft::UI::Xaml::DependencyProperty AutoGenerateColumnsProperty();
        static Microsoft::UI::Xaml::DependencyProperty CanUserReorderColumnsProperty();
        static Microsoft::UI::Xaml::DependencyProperty CanUserResizeColumnsProperty();
        static Microsoft::UI::Xaml::DependencyProperty CanUserSortColumnsProperty();
        static Microsoft::UI::Xaml::DependencyProperty CellStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty ClipboardCopyModeProperty();
        static Microsoft::UI::Xaml::DependencyProperty ColumnHeaderHeightProperty();
        static Microsoft::UI::Xaml::DependencyProperty ColumnHeaderStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty ColumnWidthProperty();
        static Microsoft::UI::Xaml::DependencyProperty DataFetchSizeProperty();
        static Microsoft::UI::Xaml::DependencyProperty DragIndicatorStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty DropLocationIndicatorStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty FrozenColumnCountProperty();
        static Microsoft::UI::Xaml::DependencyProperty GridLinesVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeadersVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty HorizontalGridLinesBrushProperty();
        static Microsoft::UI::Xaml::DependencyProperty HorizontalScrollBarVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsReadOnlyProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsValidProperty();
        static Microsoft::UI::Xaml::DependencyProperty IncrementalLoadingThresholdProperty();
        static Microsoft::UI::Xaml::DependencyProperty IncrementalLoadingTriggerProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemsSourceProperty();
        static Microsoft::UI::Xaml::DependencyProperty MaxColumnWidthProperty();
        static Microsoft::UI::Xaml::DependencyProperty MinColumnWidthProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowBackgroundProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowDetailsTemplateProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowDetailsVisibilityModeProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowForegroundProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowHeightProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowHeaderWidthProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowHeaderStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty RowStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty SelectionModeProperty();
        static Microsoft::UI::Xaml::DependencyProperty SelectedIndexProperty();
        static Microsoft::UI::Xaml::DependencyProperty SelectedItemProperty();
        static Microsoft::UI::Xaml::DependencyProperty VerticalGridLinesBrushProperty();
        static Microsoft::UI::Xaml::DependencyProperty VerticalScrollBarVisibilityProperty();

        Microsoft::UI::Xaml::Media::Brush AlternatingRowBackground() const;
        void AlternatingRowBackground(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::Media::Brush AlternatingRowForeground() const;
        void AlternatingRowForeground(Microsoft::UI::Xaml::Media::Brush const& value) const;
        bool AreRowDetailsFrozen() const;
        void AreRowDetailsFrozen(bool value) const;
        bool AreRowGroupHeadersFrozen() const;
        void AreRowGroupHeadersFrozen(bool value) const;
        bool AutoGenerateColumns() const;
        void AutoGenerateColumns(bool value) const;
        bool CanUserReorderColumns() const;
        void CanUserReorderColumns(bool value) const;
        bool CanUserResizeColumns() const;
        void CanUserResizeColumns(bool value) const;
        bool CanUserSortColumns() const;
        void CanUserSortColumns(bool value) const;
        Microsoft::UI::Xaml::Style CellStyle() const;
        void CellStyle(Microsoft::UI::Xaml::Style const& value) const;
        DataGridClipboardCopyMode ClipboardCopyMode() const;
        void ClipboardCopyMode(DataGridClipboardCopyMode value) const;
        double ColumnHeaderHeight() const;
        void ColumnHeaderHeight(double value) const;
        Microsoft::UI::Xaml::Style ColumnHeaderStyle() const;
        void ColumnHeaderStyle(Microsoft::UI::Xaml::Style const& value) const;
        MiniDumpExplorer::DataGridLength ColumnWidth() const;
        void ColumnWidth(MiniDumpExplorer::DataGridLength const& value) const;
        double DataFetchSize() const;
        void DataFetchSize(double value) const;
        Microsoft::UI::Xaml::Style DragIndicatorStyle() const;
        void DragIndicatorStyle(Microsoft::UI::Xaml::Style const& value) const;
        Microsoft::UI::Xaml::Style DropLocationIndicatorStyle() const;
        void DropLocationIndicatorStyle(Microsoft::UI::Xaml::Style const& value) const;
        int32_t FrozenColumnCount() const;
        void FrozenColumnCount(int32_t value) const;
        DataGridGridLinesVisibility GridLinesVisibility() const;
        void GridLinesVisibility(DataGridGridLinesVisibility value) const;
        DataGridHeadersVisibility HeadersVisibility() const;
        void HeadersVisibility(DataGridHeadersVisibility value) const;
        Microsoft::UI::Xaml::Media::Brush HorizontalGridLinesBrush() const;
        void HorizontalGridLinesBrush(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::Controls::ScrollBarVisibility HorizontalScrollBarVisibility() const;
        void HorizontalScrollBarVisibility(Microsoft::UI::Xaml::Controls::ScrollBarVisibility value) const;
        bool IsReadOnly() const;
        void IsReadOnly(bool value) const;
        bool IsValid() const;
        void IsValid(bool value) const;
        double IncrementalLoadingThreshold() const;
        void IncrementalLoadingThreshold(double value) const;
        Microsoft::UI::Xaml::Controls::IncrementalLoadingTrigger IncrementalLoadingTrigger() const;
        void IncrementalLoadingTrigger(Microsoft::UI::Xaml::Controls::IncrementalLoadingTrigger value) const;
        IDataGridDataSource ItemsSource() const;
        void ItemsSource(IDataGridDataSource const& value) const;
        double MaxColumnWidth() const;
        void MaxColumnWidth(double value) const;
        double MinColumnWidth() const;
        void MinColumnWidth(double value) const;
        Microsoft::UI::Xaml::Media::Brush RowBackground() const;
        void RowBackground(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::DataTemplate RowDetailsTemplate() const;
        void RowDetailsTemplate(Microsoft::UI::Xaml::DataTemplate const& value) const;
        DataGridRowDetailsVisibilityMode RowDetailsVisibilityMode() const;
        void RowDetailsVisibilityMode(DataGridRowDetailsVisibilityMode value) const;
        Microsoft::UI::Xaml::Media::Brush RowForeground() const;
        void RowForeground(Microsoft::UI::Xaml::Media::Brush const& value) const;
        double RowHeight() const;
        void RowHeight(double value) const;
        double RowHeaderWidth() const;
        void RowHeaderWidth(double value) const;
        Microsoft::UI::Xaml::Style RowHeaderStyle() const;
        void RowHeaderStyle(Microsoft::UI::Xaml::Style const& value) const;
        Microsoft::UI::Xaml::Style RowStyle() const;
        void RowStyle(Microsoft::UI::Xaml::Style const& value) const;
        DataGridSelectionMode SelectionMode() const;
        void SelectionMode(DataGridSelectionMode value) const;
        int32_t SelectedIndex() const;
        void SelectedIndex(int32_t value) const;
        Windows::Foundation::IInspectable SelectedItem() const;
        void SelectedItem(Windows::Foundation::IInspectable const& value) const;
        Microsoft::UI::Xaml::Media::Brush VerticalGridLinesBrush() const;
        void VerticalGridLinesBrush(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::Controls::ScrollBarVisibility VerticalScrollBarVisibility() const;
        void VerticalScrollBarVisibility(Microsoft::UI::Xaml::Controls::ScrollBarVisibility value) const;

        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> Columns() const;
        MiniDumpExplorer::DataGridColumn CurrentColumn() const;
        void CurrentColumn(MiniDumpExplorer::DataGridColumn const& value);
        hstring RowGroupHeaderPropertyNameAlternative() const { return rowGroupHeaderPropertyNameAlternative_; }
        void RowGroupHeaderPropertyNameAlternative(hstring const& value) { rowGroupHeaderPropertyNameAlternative_ = value; }
        Windows::Foundation::Collections::IObservableVector<Microsoft::UI::Xaml::Style> RowGroupHeaderStyles() const { return rowGroupHeaderStyles_; }
        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> SelectedItems() const;

        // internal

        int32_t AnchorSlot() const { return anchorSlot_; }
        double ActualRowHeaderWidth() const;
        double ActualRowsPresenterHeight() const;
        bool AllowsManipulation() const;
        bool AreColumnHeadersVisible() const;
        bool AreRowHeadersVisible() const;
        bool AutoSizingColumns() const { return autoSizingColumns_; }
        void AutoSizingColumns(bool value);
        double AvailableSlotElementRoom() const { return availableSlotElementRoom_; }
        void AvailableSlotElementRoom(double const value) { availableSlotElementRoom_ = value; }
        double CellsHeight() const;
        double CellsWidth() const;
        Microsoft::UI::Xaml::Controls::ContentControl const& ClipboardContentControl() const { return clipboardContentControl_; }
        bool ColumnHeaderHasFocus() const { return columnHeaderHasFocus_; }
        void ColumnHeaderHasFocus(bool value);
        DataGridInternal::DataGridColumnHeaderInteractionInfo const& ColumnHeaderInteractionInfo() const { return columnHeaderInteractionInfo_; }
        DataGridInternal::DataGridColumnHeaderInteractionInfo& ColumnHeaderInteractionInfo() { return columnHeaderInteractionInfo_; }
        MiniDumpExplorer::DataGridColumnHeadersPresenter const& ColumnHeaders() const { return columnHeadersPresenter_; }
        DataGridInternal::DataGridColumnCollection const& ColumnsInternal() const { return columnsInternal_; }
        DataGridInternal::DataGridColumnCollection& ColumnsInternal() { return columnsInternal_; }
        std::vector<MiniDumpExplorer::DataGridColumn> const& ColumnsItemsInternal() const;
        auto ContainsFocus() const { return containsFocus_; }
        int32_t CurrentColumnIndex() const;
        int32_t CurrentSlot() const;
        DataGridInternal::DataGridDataConnection const& DataConnection() const { return dataConnection_; }
        DataGridInternal::DataGridDataConnection& DataConnection() { return dataConnection_; }
        DataGridInternal::DataGridDisplayData const& DisplayData() const { return displayData_; }
        DataGridInternal::DataGridDisplayData& DisplayData() { return displayData_; }
        auto EditingColumnIndex() const { return editingColumnIndex_; }
        MiniDumpExplorer::DataGridRow const& EditingRow() const { return editingRow_; }
        auto FirstDisplayedScrollingColumnHiddenWidth() const { return negHorizontalOffset_; }
        MiniDumpExplorer::DataGridColumn const& FocusedColumn() const { return focusedColumn_; }
        void FocusedColumn(MiniDumpExplorer::DataGridColumn const& value) { focusedColumn_ = value; }
        bool HasColumnUserInteraction() const;
        auto HorizontalAdjustment() const { return horizontalAdjustment_; }
        auto HorizontalOffset() const { return horizontalOffset_; }
        void HorizontalOffset(double value);
        Microsoft::UI::Xaml::Controls::Primitives::ScrollBar const& HorizontalScrollBar() const { return hScrollBar_; }
        bool LoadingOrUnloadingRow() const { return loadingOrUnloadingRow_; }
        bool InDisplayIndexAdjustments() const { return inDisplayIndexAdjustments_; }
        void InDisplayIndexAdjustments(bool const value) { inDisplayIndexAdjustments_ = value; }
        auto NegVerticalOffset() const { return negVerticalOffset_; }
        auto NoCurrentCellChangeCount() const { return noCurrentCellChangeCount_; }
        void NoCurrentCellChangeCount(int32_t value);
        double RowDetailsHeightEstimate() const { return rowDetailsHeightEstimate_; }
        double RowHeadersDesiredWidth() const { return rowHeaderDesiredWidth_; }
        void RowHeadersDesiredWidth(double value);
        double RowGroupHeaderHeightEstimate() const { return rowGroupHeaderHeightEstimate_; }
        Utility::IndexToValueTable<std::shared_ptr<DataGridInternal::DataGridRowGroupInfo>> const& RowGroupHeadersTable() const { return rowGroupHeadersTable_; }
        Utility::IndexToValueTable<std::shared_ptr<DataGridInternal::DataGridRowGroupInfo>>& RowGroupHeadersTable() { return rowGroupHeadersTable_; }
        std::vector<double> const& RowGroupSublevelIndents() const { return rowGroupSublevelIndents_; }
        double RowHeightEstimate() const { return rowHeightEstimate_; }
        std::optional<Windows::Foundation::Size> const& RowsPresenterAvailableSize() const { return rowsPresenterAvailableSize_; }
        void RowsPresenterAvailableSize(std::optional<Windows::Foundation::Size> value);
        bool SelectionHasChanged() const { return selectionHasChanged_; }
        void SelectionHasChanged(bool const value) { selectionHasChanged_ = value; }
        auto SlotCount() const { return slotCount_; }
        bool UpdatedStateOnTapped() const { return updatedStateOnTapped_; }
        void UpdatedStateOnTapped(bool const value) { updatedStateOnTapped_ = value; }
        bool UsesStarSizing() const;
        double VerticalOffset() const { return verticalOffset_; }
        void VerticalOffset(double value);
        Microsoft::UI::Xaml::Controls::Primitives::ScrollBar const& VerticalScrollBar() const { return vScrollBar_; }
        auto VisibleSlotCount() const { return visibleSlotCount_; }
        bool BeginEdit() { return BeginEdit(nullptr); }
        bool BeginEdit(Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs);
        bool CancelEdit() { return CancelEdit(DataGridEditingUnit::Row); }
        bool CancelEdit(DataGridEditingUnit const editingUnit) { return CancelEdit(editingUnit, true /*raiseEvents*/); }
        bool CommitEdit() { return CommitEdit(DataGridEditingUnit::Row, true); }
        bool CommitEdit(DataGridEditingUnit editingUnit, bool exitEditingMode);
        Microsoft::UI::Xaml::Data::ICollectionViewGroup GetGroupFromItem(Windows::Foundation::IInspectable const& item, int32_t groupLevel);
        void ScrollIntoView(Windows::Foundation::IInspectable const& item, MiniDumpExplorer::DataGridColumn const& column);

        // overrides
        // ReSharper disable CppHidingFunction
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize); // override
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize); // override
        void OnApplyTemplate(); // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override
        void OnPointerWheelChanged(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e); // override
        // ReSharper restore CppHidingFunction

        bool CancelEdit(DataGridEditingUnit editingUnit, bool raiseEvents);
        void CoerceSelectedItem();
        std::experimental::generator<Windows::Foundation::IInspectable> GetSelectionInclusive(int32_t startRowIndex, int32_t endRowIndex) const;
        void InitializeElements(bool recycleRows);
        Windows::Foundation::IInspectable ItemFromSlot(int32_t slot, int32_t& rowIndex);
        void LoadMoreDataFromIncrementalItemsSource();
        void OnRowDetailsChanged() const;
        void OnUserSorting();
        void OnUserSorted();
        bool ProcessDownKey();
        bool ProcessEndKey();
        bool ProcessEnterKey();
        bool ProcessHomeKey();
        void ProcessHorizontalScroll(Microsoft::UI::Xaml::Controls::Primitives::ScrollEventType scrollEventType);
        bool ProcessLeftKey();
        bool ProcessNextKey();
        bool ProcessPriorKey();
        bool ProcessRightKey();
        bool ProcessScrollOffsetDelta(double offsetDelta, bool isForHorizontalScroll);
        void ProcessSelectionAndCurrency(int32_t columnIndex, Windows::Foundation::IInspectable const& item, int32_t backupSlot, DataGridInternal::DataGridSelectionAction action, bool scrollIntoView);
        bool ProcessUpKey();
        void ProcessVerticalScroll(Microsoft::UI::Xaml::Controls::Primitives::ScrollEventType scrollEventType);
        void RefreshRowsAndColumns(bool clearRows);
        void ResetColumnHeaderInteractionInfo();
        bool ScrollSlotIntoView(int32_t columnIndex, int32_t slot, bool forCurrentCellChange, bool forceHorizontalScroll);
        bool SetCurrentCellCore(int32_t columnIndex, int32_t slot);
        void UpdateHorizontalOffset(double newValue);
        bool UpdateSelectionAndCurrency(int32_t columnIndex, int32_t slot, DataGridInternal::DataGridSelectionAction action, bool scrollIntoView);
        void UpdateStateOnCurrentChanged(Windows::Foundation::IInspectable const& currentItem, int32_t currentPosition);
        bool UpdateStateOnTapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args, int32_t columnIndex, int32_t slot, bool allowEdit);
        void UpdateVerticalScrollBar();
        bool WaitForLostFocus(std::function<void()> action);

        static MiniDumpExplorer::DataGridCell GetOwningCell(Microsoft::UI::Xaml::FrameworkElement element);

        // DataGridRows.cpp
        bool AreRowBottomGridLinesRequired() const;
        int32_t FirstVisibleSlot() const;
        int32_t FrozenColumnCountWithFiller();
        int32_t LastVisibleSlot() const;

        // public
        void CollapseRowGroup(Microsoft::UI::Xaml::Data::ICollectionViewGroup const& collectionViewGroup, bool collapseAllSubgroups);
        void ExpandRowGroup(Microsoft::UI::Xaml::Data::ICollectionViewGroup const& collectionViewGroup, bool expandAllSubgroups);

        // internal
        void ClearRowSelection(bool resetAnchorSlot);
        void ClearRowSelection(int32_t slotException, bool setAnchorSlot);
        int32_t GetCollapsedSlotCount(int32_t startSlot, int32_t endSlot) const;
        int32_t GetNextVisibleSlot(int32_t slot) const;
        int32_t GetPreviousVisibleSlot(int32_t slot) const;
        Microsoft::UI::Xaml::Visibility GetRowDetailsVisibility(int32_t rowIndex) const;
        Microsoft::UI::Xaml::Visibility GetRowDetailsVisibility(int32_t rowIndex, DataGridRowDetailsVisibilityMode gridLevelRowDetailsVisibility) const;
        MiniDumpExplorer::DataGridRow GetRowFromItem(Windows::Foundation::IInspectable const& dataItem) const;
        bool GetRowSelection(int32_t slot) const;
        void InsertElementAt(
            int32_t slot,
            int32_t rowIndex,
            Windows::Foundation::IInspectable const& item,
            std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& groupInfo,
            bool isCollapsed);
        void InsertRowAt(int32_t rowIndex);
        bool IsColumnDisplayed(int32_t columnIndex) const;
        bool IsRowRecyclable(MiniDumpExplorer::DataGridRow const& row) const;
        bool IsSlotVisible(int32_t slot) const;
        void OnUnloadingRowDetails(MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::FrameworkElement const& detailsElement);
        void OnLoadingRowDetails(MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::FrameworkElement const& detailsElement);
        void OnRowDetailsVisibilityPropertyChanged(int32_t rowIndex, Microsoft::UI::Xaml::Visibility visibility);
        void OnRowGroupHeaderToggled(MiniDumpExplorer::DataGridRowGroupHeader const& groupHeader, Microsoft::UI::Xaml::Visibility newVisibility, bool setCurrent);
        void OnRowsMeasure();
        void OnSublevelIndentUpdated(MiniDumpExplorer::DataGridRowGroupHeader const& groupHeader, double newValue);
        void RefreshRows(bool recycleRows, bool clearRows);
        void RemoveRowAt(int32_t rowIndex, Windows::Foundation::IInspectable const& item);
        std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> RowGroupInfoFromCollectionViewGroup(Microsoft::UI::Xaml::Data::ICollectionViewGroup const& collectionViewGroup) const;
        int32_t RowIndexFromSlot(int32_t slot) const;
        bool ScrollSlotIntoView(int32_t slot, bool scrolledHorizontally);
        void SetRowSelection(int32_t slot, bool isSelected, bool setAnchorSlot);
        void SetRowsSelection(int32_t startSlot, int32_t endSlot, bool isSelected = true);
        int32_t SlotFromRowIndex(int32_t rowIndex) const;

        virtual void OnRowDetailsVisibilityChanged(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e);


        // DataGridColumns.cpp
        virtual void OnColumnDisplayIndexChanged(MiniDumpExplorer::DataGridColumnEventArgs const& e);
        virtual void OnColumnReordered(MiniDumpExplorer::DataGridColumnEventArgs const& e);
        virtual void OnColumnReordering(MiniDumpExplorer::DataGridColumnReorderingEventArgs const& e);
        virtual void OnColumnSorting(MiniDumpExplorer::DataGridColumnEventArgs const& e);

        static double GetEdgedColumnWidth(MiniDumpExplorer::DataGridColumn const& dataGridColumn);

        double AdjustColumnWidths(int32_t displayIndex, double amount, bool userInitiated);
        void AutoSizeColumn(MiniDumpExplorer::DataGridColumn const& column, double desiredWidth);
        bool ColumnRequiresRightGridLine(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool includeLastRightGridLineWhenPresent);
        double DecreaseColumnWidths(int32_t displayIndex, double amount, bool userInitiated);
        bool GetColumnReadOnlyState(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool isReadOnly);
        double IncreaseColumnWidths(int32_t displayIndex, double amount, bool userInitiated);
        void OnClearingColumns();
        void OnColumnCanUserResizeChanged(MiniDumpExplorer::DataGridColumn const& column);
        void OnColumnCellStyleChanged(MiniDumpExplorer::DataGridColumn const& column, Microsoft::UI::Xaml::Style const& previousStyle);
        void OnColumnCollectionChanged_PostNotification(bool columnsGrew);
        void OnColumnCollectionChanged_PreNotification(bool columnsGrew);
        void OnColumnDisplayIndexChanged(MiniDumpExplorer::DataGridColumn const& dataGridColumn);
        void OnColumnDisplayIndexChanged_PostNotification();
        void OnColumnDisplayIndexChanging(MiniDumpExplorer::DataGridColumn const& targetColumn, int32_t newDisplayIndex);
        void OnColumnBindingChanged(MiniDumpExplorer::DataGridBoundColumn const& column);
        void OnColumnElementStyleChanged(MiniDumpExplorer::DataGridBoundColumn const& column);
        void OnColumnHeaderDragStarted(Microsoft::UI::Xaml::Controls::Primitives::DragStartedEventArgs const& e);
        void OnColumnHeaderDragDelta(Microsoft::UI::Xaml::Controls::Primitives::DragDeltaEventArgs const& e);
        void OnColumnHeaderDragCompleted(Microsoft::UI::Xaml::Controls::Primitives::DragCompletedEventArgs const& e);
        void OnColumnMaxWidthChanged(MiniDumpExplorer::DataGridColumn const& column, double oldValue);
        void OnColumnMinWidthChanged(MiniDumpExplorer::DataGridColumn const& column, double oldValue);
        void OnColumnReadOnlyStateChanging(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool isReadOnly);
        void OnColumnVisibleStateChanged(MiniDumpExplorer::DataGridColumn const& updatedColumn);
        void OnColumnVisibleStateChanging(MiniDumpExplorer::DataGridColumn const& targetColumn);
        void OnColumnWidthChanged(MiniDumpExplorer::DataGridColumn const& updatedColumn);
        void OnFillerColumnWidthNeeded(double finalWidth);
        void OnInsertedColumn_PostNotification(DataGridInternal::DataGridCellCoordinates const& newCurrentCellCoordinates, int32_t newDisplayIndex);
        void OnInsertedColumn_PreNotification(MiniDumpExplorer::DataGridColumn const& insertedColumn);
        DataGridInternal::DataGridCellCoordinates OnInsertingColumn(int32_t columnIndexInserted, MiniDumpExplorer::DataGridColumn const& insertColumn);
        void OnRemovedColumn_PostNotification(DataGridInternal::DataGridCellCoordinates const& newCurrentCellCoordinates);
        void OnRemovedColumn_PreNotification(MiniDumpExplorer::DataGridColumn const& removedColumn);
        DataGridInternal::DataGridCellCoordinates OnRemovingColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumn);
        void RefreshColumnElements(MiniDumpExplorer::DataGridColumn const& dataGridColumn, std::wstring_view const& propertyName);
        std::experimental::generator<MiniDumpExplorer::DataGridRow> GetAllRows() const;
        void OnRowDoubleClicked(MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);

    protected:
        Windows::Foundation::IInspectable CurrentItem() const;

        // ReSharper disable once CppFunctionIsNotImplemented
        auto LoadingOrUnloadingRowScope();

        virtual void OnAutoGeneratingColumn(MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs const& e);
        virtual void OnBeginningEdit(MiniDumpExplorer::DataGridBeginningEditEventArgs const& e);
        virtual void OnCellEditEnded(MiniDumpExplorer::DataGridCellEditEndedEventArgs const& e);
        virtual void OnCellEditEnding(MiniDumpExplorer::DataGridCellEditEndingEventArgs const& e);
        virtual void OnCopyingRowClipboardContent(MiniDumpExplorer::DataGridRowClipboardEventArgs const& e);
        virtual void OnCurrentCellChanged(Microsoft::UI::Xaml::RoutedEventArgs const& e);
        virtual void OnLoadingRow(MiniDumpExplorer::DataGridRowEventArgs const& e);
        virtual void OnLoadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs const& e);
        virtual void OnLoadingRowDetails(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e);
        virtual void OnPreparingCellForEdit(MiniDumpExplorer::DataGridPreparingCellForEditEventArgs const& e);
        virtual void OnRowEditEnded(MiniDumpExplorer::DataGridRowEditEndedEventArgs const& e);
        virtual void OnRowEditEnding(MiniDumpExplorer::DataGridRowEditEndingEventArgs const& e);
        virtual void OnSelectionChanged(Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        virtual void OnUnloadingRow(MiniDumpExplorer::DataGridRowEventArgs const& e);
        virtual void OnUnloadingRowDetails(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e);
        virtual void OnUnloadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs const& e);
        virtual void OnRowDoubleTapped(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e);

    private:
        enum class ScrollBarVisualState
        {
            NoIndicator,
            TouchIndicator,
            MouseIndicator,
            MouseIndicatorFull
        };

        enum class ScrollBarsSeparatorVisualState
        {
            SeparatorCollapsed,
            SeparatorExpanded,
            SeparatorExpandedWithoutAnimation,
            SeparatorCollapsedWithoutAnimation
        };

        void OnAlternatingRowBackgroundPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnAlternatingRowForegroundPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnAreRowGroupHeadersFrozenPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnAutoGenerateColumnsPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnCanUserResizeColumnsPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnCellStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnColumnHeaderHeightPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnColumnHeaderStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnColumnWidthPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnDataFetchSizePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnFrozenColumnCountPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnGridLinesVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnHeadersVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnHorizontalGridLinesBrushPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnHorizontalScrollBarVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnIsReadOnlyPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnIsValidPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnIncrementalLoadingThresholdPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnItemsSourcePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnMaxColumnWidthPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnMinColumnWidthPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowBackgroundPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnRowDetailsTemplatePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowDetailsVisibilityModePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowForegroundPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnRowHeightPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowHeaderWidthPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowHeaderStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnRowStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnSelectionModePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnSelectedIndexPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnSelectedItemPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnVerticalGridLinesBrushPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnVerticalScrollBarVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void ProcessFrozenColumnCount();
        void CurrentColumnIndex(int32_t value);
        void CurrentSlot(int32_t value);
        bool AreAllScrollBarsCollapsed() const;
        bool AreBothScrollBarsVisible() const;
        int32_t FirstDisplayedNonFillerColumnIndex() const;
        bool IsHorizontalScrollBarInteracting() const { return isHorizontalScrollBarInteracting_; }
        void IsHorizontalScrollBarInteracting(bool value);
        bool IsHorizontalScrollBarOverCells() const;
        bool IsVerticalScrollBarInteracting() const { return isVerticalScrollBarInteracting_; }
        void IsVerticalScrollBarInteracting(bool value);
        bool IsVerticalScrollBarOverCells() const;
        int32_t NoSelectionChangeCount() const { return noSelectionChangeCount_; }
        void NoSelectionChangeCount(int32_t value);
        bool AddNewItem(Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs);
        void AddNewCellPrivate(MiniDumpExplorer::DataGridRow const& row, MiniDumpExplorer::DataGridColumn const& column);
        bool BeginCellEdit(Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs);
        bool BeginRowEdit(MiniDumpExplorer::DataGridRow const& dataGridRow);
        bool CancelRowEdit(bool exitEditingMode);
        bool CommitEditForOperation(int32_t columnIndex, int32_t slot, bool forCurrentCellChange);
        bool CommitRowEdit(bool exitEditingMode);
        void CompleteCellsCollection(MiniDumpExplorer::DataGridRow const& dataGridRow);
        void ComputeScrollBarsLayout();
        void EditingElement_LostFocus(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void EnsureHorizontalLayout();
        bool EnsureRowHeaderWidth();
        void EnsureRowsPresenterVisibility();
        void EnsureTopLeftCornerHeader() const;
        void ExpandRowGroupParentChain(int32_t level, int32_t slot);
        void InvalidateCellsArrange() const;
        void InvalidateColumnHeadersArrange() const;
        void InvalidateColumnHeadersMeasure() const;
        void InvalidateRowsArrange() const;
        void InvalidateRowsMeasure(bool invalidateIndividualElements) const;
        void DataGrid_GettingFocus(Microsoft::UI::Xaml::UIElement const& sender, Microsoft::UI::Xaml::Input::GettingFocusEventArgs const& e);
        void DataGrid_GotFocus(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DataGrid_IsEnabledChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void DataGrid_KeyDown(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void DataGrid_KeyUp(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void DataGrid_LostFocus(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        Windows::Foundation::IInspectable GetFocusedElement() const;
        void DataGrid_PointerEntered(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGrid_PointerExited(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGrid_PointerMoved(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGrid_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGrid_PointerReleased(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void DataGrid_Unloaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void EditingElement_Loaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        bool EndCellEdit(DataGridEditAction editAction, bool exitEditingMode, bool keepFocus, bool raiseEvents);
        bool EndRowEdit(DataGridEditAction editAction, bool exitEditingMode, bool raiseEvents);
        void EnsureColumnHeadersVisibility() const;
        void EnsureVerticalGridLines();
        void ExitEdit(bool keepFocus);
        void ExternalEditingElement_LostFocus(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void FlushCurrentCellChanged();
        void FlushSelectionChanged();
        bool FocusEditingCell(bool setFocus);
        static hstring FormatClipboardContent(MiniDumpExplorer::DataGridRowClipboardEventArgs const& e);
        double GetHorizontalSmallScrollDecrease();
        double GetHorizontalSmallScrollIncrease();
        double GetVerticalSmallScrollIncrease();
        void HideScrollBars(bool useTransitions);
        void HideScrollBarsAfterDelay();
        void HideScrollBarsTimerTick(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        void HookDataGridEvents();
        void HookHorizontalScrollBarEvents();
        void HookVerticalScrollBarEvents();
        void HorizontalScrollBar_PointerEntered(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void HorizontalScrollBar_PointerExited(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void VerticalScrollBar_PointerEntered(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void VerticalScrollBar_PointerExited(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void HorizontalScrollBar_Scroll(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::ScrollEventArgs const& e);
        void IndicatorStateStoryboard_Completed(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        bool IsColumnOutOfBounds(int32_t columnIndex) const;
        bool IsInnerCellOutOfBounds(int32_t columnIndex, int32_t slot) const;
        bool IsInnerCellOutOfSelectionBounds(int32_t columnIndex, int32_t slot) const;
        bool IsSlotOutOfBounds(int32_t slot) const;
        bool IsSlotOutOfSelectionBounds(int32_t slot) const;
        void LoadMoreDataFromIncrementalItemsSource(double totalVisibleHeight);
        void MakeFirstDisplayedCellCurrentCell();
        void NoIndicatorStateStoryboard_Completed(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
        void PopulateCellContent(bool isCellEdited, MiniDumpExplorer::DataGridColumn const& dataGridColumn, MiniDumpExplorer::DataGridRow const& dataGridRow, MiniDumpExplorer::DataGridCell const& dataGridCell);
        void PreparingCellForEditPrivate(Microsoft::UI::Xaml::FrameworkElement const& editingElement);
        bool ProcessAKey();
        bool ProcessCopyKey();
        bool ProcessDataGridKey(Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        bool ProcessDownKeyInternal(bool shift, bool ctrl);
        bool ProcessEndKey(bool shift, bool ctrl);
        bool ProcessEnterKey(bool shift, bool ctrl);
        bool ProcessEscapeKey();
        bool ProcessF2Key(Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        bool ProcessHomeKey(bool shift, bool ctrl);
        bool ProcessLeftKey(bool shift, bool ctrl);
        bool ProcessLeftMost(int32_t firstVisibleColumnIndex, int32_t firstVisibleSlot);
        bool ProcessNextKey(bool shift, bool ctrl);
        bool ProcessPriorKey(bool shift, bool ctrl);
        bool ProcessRightKey(bool shift, bool ctrl);
        bool ProcessRightMost(int32_t lastVisibleColumnIndex, int32_t firstVisibleSlot);
        bool ProcessSpaceKey();
        bool ProcessTabKey(Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        bool ProcessTabKey(Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e, bool shift, bool ctrl);
        bool ProcessUpKey(bool shift, bool ctrl);
        void RemoveDisplayedColumnHeader(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const;
        void RemoveDisplayedColumnHeaders();
        void ResetCurrentCellCore();
        void ResetEditingRow();
        void ResetFocusedRow();
        void ResetValidationStatus();
        void RowGroupHeaderStyles_CollectionChanged(Windows::Foundation::Collections::IObservableVector<Microsoft::UI::Xaml::Style> const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& event);
        void SelectAll();
        void SetAndSelectCurrentCell(int32_t columnIndex, int32_t slot, bool forceCurrentCellSelection);
        bool SetCurrentCellCore(int32_t columnIndex, int32_t slot, bool commitEdit, bool endRowEdit);
        void SetHorizontalOffset(double newHorizontalOffset);
        void SetVerticalOffset(double newVerticalOffset);
        void ShowScrollBars();
        void StopHideScrollBarsTimer() const;
        void SwitchScrollBarsVisualStates(ScrollBarVisualState scrollBarsState, ScrollBarsSeparatorVisualState separatorState, bool useTransitions);
        void UnhookHorizontalScrollBarEvents();
        void UnhookVerticalScrollBarEvents();
        void UpdateCurrentState(Microsoft::UI::Xaml::UIElement const& displayedElement, int32_t columnIndex, bool applyCellState) const;
        void UpdateDisabledVisual() const;
        void UpdateHorizontalScrollBar(bool needHorizontalScrollBar, bool forceHorizontalScrollBar, double totalVisibleWidth, double totalVisibleFrozenWidth, double cellsWidth);
        void UpdateRowDetailsVisibilityMode(DataGridRowDetailsVisibilityMode newDetailsMode);
        void UpdateRowsPresenterManipulationMode(bool horizontalMode, bool verticalMode) const;
        bool UpdateStateOnTapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args, int32_t columnIndex, int32_t slot, bool allowEdit, bool shift, bool ctrl);
        void UpdateValidationResults(std::vector<Utility::ValidationResult> const& newValidationResults, bool scrollIntoView);
        void UpdateValidationStatus() const;
        void UpdateVerticalScrollBar(bool needVertScrollBar, bool forceVertScrollBar, double totalVisibleHeight, double cellsHeight);
        bool ValidateEditingRow(bool scrollIntoView, bool wireEvents);
        void ValidateIndei(Microsoft::UI::Xaml::Data::INotifyDataErrorInfo const& indei, std::wstring const& bindingProperty, std::wstring const& bindingPath, std::wstring const& declaringPath, std::vector<Utility::ValidationResult>& validationResults, bool wireEvents);
        void ValidationItem_ErrorsChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Data::DataErrorsChangedEventArgs const& e);
        void VerticalScrollBar_Scroll(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::Primitives::ScrollEventArgs const& e);

        static void EnsureElementStyle(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Style const& oldDataGridStyle, Microsoft::UI::Xaml::Style const& newDataGridStyle);

        // DataGridRows.cpp
        double EdgedRowsHeightCalculated();

        static void CorrectRowAfterDeletion(MiniDumpExplorer::DataGridRow const& row, bool rowDeleted);
        static void CorrectRowAfterInsertion(MiniDumpExplorer::DataGridRow const& row, bool rowInserted);

        void AddSlotElement(int32_t slot, Microsoft::UI::Xaml::UIElement const& element);
        void AddSlots(int32_t totalSlots);
        void ApplyDisplayedRowsState(int32_t startSlot, int32_t endSlot);
        void ClearRowGroupHeadersTable();
        void ClearRows(bool recycle);
        void ClearShowDetailsTable();
        double CollapseSlotsInTable(int32_t startSlot, int32_t endSlot, int32_t& slotsExpanded, int32_t lastDisplayedSlot, double& heightChangeBelowLastDisplayedSlot);
        void CorrectEditingRow();
        void CorrectLastSubItemSlotsAfterInsertion(std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> subGroupInfo);
        void CorrectSlotsAfterDeletion(int32_t slotDeleted, bool wasRow);
        void CorrectSlotsAfterInsertion(int32_t slotInserted, bool isCollapsed, bool rowInserted);
        int32_t CountAndPopulateGroupHeaders(Windows::Foundation::IInspectable const& group, int32_t rootSlot, uint32_t level);
        void CollectionViewGroupItems_VectorChanged(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems, Windows::Foundation::Collections::IVectorChangedEventArgs const& e);
        void EnsureAncestorsExpanderButtonChecked(DataGridInternal::DataGridRowGroupInfo const& parentGroupInfo);
        void EnsureRowDetailsVisibility(MiniDumpExplorer::DataGridRow const& row, bool raiseNotification) const;
        std::experimental::generator<MiniDumpExplorer::DataGridRowGroupHeader> GetAllRowGroupHeaders() const;
        int32_t GetDetailsCountInclusive(int32_t lowerBound, int32_t upperBound) const;
        void EnsureRowGroupSpacerColumn();
        void EnsureRowGroupSpacerColumnWidth(uint32_t groupLevelCount);
        void EnsureRowGroupVisibility(std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& rowGroupInfo, Microsoft::UI::Xaml::Visibility visibility, bool setCurrent);
        void ExpandSlots(int32_t startSlot, int32_t endSlot, bool isHeaderDisplayed, int32_t& slotsExpanded, double& totalHeightChange);
        void GenerateEditingElements();
        MiniDumpExplorer::DataGridRow GenerateRow(int32_t rowIndex, int32_t slot);
        MiniDumpExplorer::DataGridRow GenerateRow(int32_t rowIndex, int32_t slot, Windows::Foundation::IInspectable const& dataContext);
        MiniDumpExplorer::DataGridRowGroupHeader GenerateRowGroupHeader(int32_t slot, std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& rowGroupInfo);
        double GetExactSlotElementHeight(int32_t slot);
        double GetHeightEstimate(int32_t fromSlot, int32_t toSlot) const;
        std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> GetParentGroupInfo(Windows::Foundation::IInspectable const& collection) const;
        int32_t GetRowGroupHeaderCount(int32_t startSlot, int32_t endSlot, std::optional<Microsoft::UI::Xaml::Visibility> visibility, double& headersHeight) const;
        double GetSlotElementHeight(int32_t slot) const;
        double GetSlotElementsHeight(int32_t fromSlot, int32_t toSlot) const;
        MiniDumpExplorer::DataGridRow GetGeneratedRow(Windows::Foundation::IInspectable const& dataContext) const;
        MiniDumpExplorer::DataGridRow GetLoadedRow(Windows::Foundation::IInspectable const& dataContext) const;
        Microsoft::UI::Xaml::FrameworkElement InsertDisplayedElement(int32_t slot, bool updateSlotInformation);
        void InsertDisplayedElement(int32_t slot, Microsoft::UI::Xaml::UIElement const& element, bool wasNewlyAdded, bool updateSlotInformation);
        void InsertElement(int32_t slot, Microsoft::UI::Xaml::UIElement const& element, bool updateVerticalScrollBarOnly, bool isCollapsed, bool isRow);
        void InvalidateRowHeightEstimate();
        void OnAddedElement_Phase1(int32_t slot, Microsoft::UI::Xaml::UIElement const& element);
        void OnAddedElement_Phase2(int32_t slot, bool updateVerticalScrollBarOnly);
        void OnCollectionViewGroupItemInserted(Windows::Foundation::IInspectable const& groupItems, Windows::Foundation::IInspectable const& insertedItem, int32_t insertedIndex);
        void OnCollectionViewGroupItemRemoved(Windows::Foundation::IInspectable const& groupItems, Windows::Foundation::IInspectable const& removedItem, int32_t removedIndex);
        void OnElementsChanged(bool grew);
        void OnInsertedElement_Phase1(int32_t slot, Microsoft::UI::Xaml::UIElement const& element, bool isCollapsed, bool isRow);
        void OnInsertedElement_Phase2(int32_t slot, bool updateVerticalScrollBarOnly, bool isCollapsed);
        void OnInsertingElement(int32_t slotInserted, bool firstInsertion, bool isCollapsed, bool isRow);
        void OnRemovedElement(int32_t slotDeleted, Windows::Foundation::IInspectable const& itemDeleted, bool isRow);
        void OnRemovingElement(int32_t slotDeleted);
        void LoadRowVisualsForDisplay(MiniDumpExplorer::DataGridRow const& row) const;
        void PopulateRowGroupHeadersTable();
        void HookupVectorChangedListenerToGroup(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems);
        void UnhookVectorChangedListenerFromGroup(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems, bool removeFromTable);
        void RefreshRowGroupHeaders();
        void RefreshSlotCounts();
        void RemoveDisplayedElement(int32_t slot, bool wasDeleted, bool updateSlotInformation);
        void RemoveDisplayedElement(Microsoft::UI::Xaml::UIElement const& element, int32_t slot, bool wasDeleted, bool updateSlotInformation);
        void RemoveEditingElements() const;
        void RemoveElementAt(int32_t slot, Windows::Foundation::IInspectable const& item, bool isRow);
        void RemoveNonDisplayedRows(int32_t newFirstDisplayedSlot, int32_t newLastDisplayedSlot);
        void ResetDisplayedRows();
        bool SlotIsDisplayed(int32_t slot) const;
        void ScrollSlotsByHeight(double height);
        void SelectDisplayedElement(int32_t slot);
        void SelectSlot(int32_t slot, bool isSelected);
        void SelectSlots(int32_t startSlot, int32_t endSlot, bool isSelected);
        bool ToggleRowGroup();
        void UnloadElements(bool recycle);
        void UnloadRow(MiniDumpExplorer::DataGridRow const& dataGridRow);
        void UpdateDisplayedRows(int32_t newFirstDisplayedSlot, double displayHeight);
        void UpdateDisplayedRowsFromBottom(int32_t newLastDisplayedScrollingRow);
        void UpdateRowDetailsHeightEstimate();
        double UpdateRowGroupVisibility(DataGridInternal::DataGridRowGroupInfo& targetRowGroupInfo, Microsoft::UI::Xaml::Visibility newVisibility, bool isHeaderDisplayed);
        void UpdateTablesForRemoval(int32_t slotDeleted, Windows::Foundation::IInspectable const& itemDeleted);

        // DataGridColumns.cpp
        static double DecreaseNonStarColumnWidth(MiniDumpExplorer::DataGridColumn const& column, double targetWidth, double amount);
        static MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs GenerateColumn(std::wstring_view const& typeName, std::wstring_view const& propertyName, std::wstring_view const& header);
        static MiniDumpExplorer::DataGridBoundColumn GetDataGridColumnFromType(std::wstring_view const& typeName);
        static double IncreaseNonStarColumnWidth(MiniDumpExplorer::DataGridColumn const& column, double targetWidth, double amount);
        static void RefreshCellElement(MiniDumpExplorer::DataGridColumn const& dataGridColumn, MiniDumpExplorer::DataGridRow const& dataGridRow, std::wstring_view const& propertyName);
        bool AddGeneratedColumn(MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs const& e);
        double AdjustStarColumnWidths(int32_t displayIndex, double adjustment, bool userInitiated);
        double AdjustStarColumnWidths(int32_t displayIndex, double remainingAdjustment, bool userInitiated, std::function<double (MiniDumpExplorer::DataGridColumn const&)> const& targetWidth);
        void AutoGenerateColumnsPrivate();
        bool ComputeDisplayedColumns();
        int32_t ComputeFirstVisibleScrollingColumn();
        void CorrectColumnDisplayIndexesAfterDeletion(MiniDumpExplorer::DataGridColumn const& deletedColumn);
        void CorrectColumnDisplayIndexesAfterInsertion(MiniDumpExplorer::DataGridColumn const& insertedColumn);
        void CorrectColumnFrozenStates();
        void CorrectColumnIndexesAfterDeletion(MiniDumpExplorer::DataGridColumn const& deletedColumn) const;
        void CorrectColumnIndexesAfterInsertion(MiniDumpExplorer::DataGridColumn const& insertedColumn, int32_t insertionCount) const;
        double DecreaseNonStarColumnWidths(int32_t displayIndex, std::function<double (MiniDumpExplorer::DataGridColumn const& )> const& targetWidth, double amount, bool reverse, bool affectNewColumns);
        void FlushDisplayIndexChanged(bool raiseEvent);
        void GenerateColumnsFromProperties();
        bool GetColumnEffectiveReadOnlyState(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const;
        double GetColumnXFromIndex(int32_t index);
        double GetNegHorizontalOffsetFromHorizontalOffset(double horizontalOffset);
        double IncreaseNonStarColumnWidths(int32_t displayIndex, std::function<double (MiniDumpExplorer::DataGridColumn const& )> const& targetWidth, double amount, bool reverse, bool affectNewColumns);
        void InsertDisplayedColumnHeader(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const;
        void RemoveAutoGeneratedColumns();
        bool ScrollColumnIntoView(int32_t columnIndex);
        void ScrollColumns(int32_t columns);
        void UpdateDisplayedColumns();

    private:
        Microsoft::UI::Xaml::UIElement bottomRightCorner_{nullptr};
        MiniDumpExplorer::DataGridColumnHeadersPresenter columnHeadersPresenter_{nullptr};
        Microsoft::UI::Xaml::Controls::Primitives::ScrollBar hScrollBar_{nullptr};
        MiniDumpExplorer::DataGridRowsPresenter rowsPresenter_{nullptr};
        Microsoft::UI::Xaml::Controls::Primitives::ScrollBar vScrollBar_{nullptr};

        byte autoGeneratingColumnOperationCount_{};
        bool autoSizingColumns_{};
        std::vector<Utility::ValidationResult> bindingValidationResults_;
        Microsoft::UI::Xaml::Controls::ContentControl clipboardContentControl_{};
        Utility::IndexToValueTable<Microsoft::UI::Xaml::Visibility> collapsedSlotsTable_;
        bool columnHeaderHasFocus_{};
        DataGridInternal::DataGridCellCoordinates currentCellCoordinates_{InvalidIndex, InvalidIndex};

        // used to store the current column during a Reset
        int32_t desiredCurrentColumnIndex_{};
        int32_t editingColumnIndex_{InvalidIndex};
        Microsoft::UI::Xaml::RoutedEventArgs editingEventArgs_{nullptr};
        bool executingLostFocusActions_{};
        bool flushCurrentCellChanged_{};
        bool focusEditingControl_{};
        Microsoft::UI::Xaml::Input::FocusInputDeviceKind focusInputDevice_{Microsoft::UI::Xaml::Input::FocusInputDeviceKind::None};
        Microsoft::UI::Xaml::DependencyObject focusedObject_{nullptr};
        MiniDumpExplorer::DataGridRow focusedRow_{nullptr};
        Microsoft::UI::Xaml::FrameworkElement frozenColumnScrollBarSpacer_{nullptr};
        bool hasNoIndicatorStateStoryboardCompletedHandler_{};
        Microsoft::UI::Dispatching::DispatcherQueueTimer hideScrollBarsTimer_{nullptr};

        // the sum of the widths in pixels of the scrolling columns preceding
        // the first displayed scrolling column
        double horizontalOffset_{};
        byte horizontalScrollChangesIgnored_{};
        bool ignoreNextScrollBarsLayout_{};
        std::vector<Utility::ValidationResult> indeiValidationResults_;
        bool initializingNewItem_{};

        bool isHorizontalScrollBarInteracting_{};
        bool isVerticalScrollBarInteracting_{};

        // Set to True when the pointer is over the optional scroll bars.
        bool isPointerOverHorizontalScrollBar_{};
        bool isPointerOverVerticalScrollBar_{};

        // Set to True to prevent the normal fade-out of the scroll bars.
        bool keepScrollBarsShowing_{};

        // Nth row of rows 0…N that make up the RowHeightEstimate
        int32_t lastEstimatedRow_{InvalidIndex};
        std::vector<MiniDumpExplorer::DataGridRow> loadedRows_;

        // prevents reentry into the VerticalScroll event handler
        std::vector<std::function<void ()>> lostFocusActions_;
        event_token lostFocusEvent_{};
        bool makeFirstDisplayedCellCurrentCellPending_{};
        bool measured_{};

        // the number of pixels of the firstDisplayedScrollingCol which are not displayed
        double negHorizontalOffset_{};

        // the number of pixels of DisplayData.FirstDisplayedScrollingRow which are not displayed
        int32_t noCurrentCellChangeCount_{};
        int32_t noFocusedColumnChangeCount_{};
        int32_t noSelectionChangeCount_{};

        double oldEdgedRowsHeightCalculated_{0.0};

        // Set to True to favor mouse indicators over panning indicators for the scroll bars.
        bool preferMouseIndicators_{};

        DataGridInternal::DataGridCellCoordinates previousAutomationFocusCoordinates_{};
        MiniDumpExplorer::DataGridColumn previousCurrentColumn_{nullptr};
        Windows::Foundation::IInspectable previousCurrentItem_{nullptr};
        std::vector<Utility::ValidationResult> propertyValidationResults_;
        ScrollBarVisualState proposedScrollBarsState_{ScrollBarVisualState::NoIndicator};
        ScrollBarsSeparatorVisualState proposedScrollBarsSeparatorState_{ScrollBarsSeparatorVisualState::SeparatorCollapsed};
        hstring rowGroupHeaderPropertyNameAlternative_;
        Windows::Foundation::Collections::IObservableVector<Microsoft::UI::Xaml::Style> rowGroupHeaderStyles_{ single_threaded_observable_vector<Microsoft::UI::Xaml::Style>() };

        // To figure out what the old RowGroupHeaderStyle was for each level, we need to keep a copy
        // of the list.  The old style important so we don't blow away styles set directly on the RowGroupHeader
        std::vector<Microsoft::UI::Xaml::Style> rowGroupHeaderStylesOld_;
        std::vector<double> rowGroupHeightsByLevel_;
        double rowHeaderDesiredWidth_{0};
        std::optional<Windows::Foundation::Size> rowsPresenterAvailableSize_;
        bool scrollingByHeight_{};
        DataGridInternal::DataGridSelectedItemsCollection selectedItems_{this};
        Utility::IndexToValueTable<Microsoft::UI::Xaml::Visibility> showDetailsTable_;

        // Set to True when the mouse scroll bars are currently showing.
        bool showingMouseIndicators_;
        bool successfullyUpdatedSelection_;
        bool temporarilyResetCurrentCell_;
        bool isUserSorting_; // True if we're currently in a user invoked sorting operation
        Microsoft::UI::Xaml::Controls::ContentControl topLeftCornerHeader_;
        Microsoft::UI::Xaml::Controls::ContentControl topRightCornerHeader_;
        Windows::Foundation::IInspectable uneditedValue_{nullptr}; // Represents the original current cell value at the time it enters editing mode.
        hstring updateSourcePath_;
        struct ValidationItem
        {
            std::wstring path;
            event_token token;
        };

        std::map<Microsoft::UI::Xaml::Data::INotifyDataErrorInfo, ValidationItem> validationItems_;
        std::vector<Utility::ValidationResult> validationResults_;
        byte verticalScrollChangesIgnored_{};
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> topLevelGroup_{nullptr};

        // An approximation of the sum of the heights in pixels of the scrolling rows preceding
        // the first displayed scrolling row.  Since the scrolled off rows are discarded, the grid
        // does not know their actual height. The heights used for the approximation are the ones
        // set as the rows were scrolled off.
        double verticalOffset_{};

        double rowHeightEstimate_;
        double rowDetailsHeightEstimate_{0};
        double rowGroupHeaderHeightEstimate_;
        double availableSlotElementRoom_{};
        int32_t anchorSlot_{InvalidIndex};
        Windows::System::VirtualKey lastHandledKeyDown_{Windows::System::VirtualKey::None};

        MiniDumpExplorer::DataGridRow editingRow_{nullptr};
        MiniDumpExplorer::DataGridColumn focusedColumn_{nullptr};
        double horizontalAdjustment_{};
        bool loadingOrUnloadingRow_{};
        bool inDisplayIndexAdjustments_{};
        double negVerticalOffset_{};
        Utility::IndexToValueTable<std::shared_ptr<DataGridInternal::DataGridRowGroupInfo>> rowGroupHeadersTable_;
        std::vector<double> rowGroupSublevelIndents_;
        bool selectionHasChanged_{};
        int32_t slotCount_{};
        bool updatedStateOnTapped_{};
        int32_t visibleSlotCount_{};
        bool containsFocus_{};
        std::unordered_map<Microsoft::UI::Xaml::FrameworkElement, event_token> editingElementLoadedEvent_{};

        event_token dataGridIsEnabledChangedEvent_{};
        event_token dataGridKeyDownEvent_{};
        event_token dataGridKeyUpEvent_{};
        event_token dataGridGettingFocusEvent_{};
        event_token dataGridGotFocusEvent_{};
        event_token dataGridLostFocusEvent_{};
        event_token dataGridPointerEnteredEvent_{};
        event_token dataGridPointerExitedEvent_{};
        event_token dataGridPointerMovedEvent_{};
        event_token dataGridPointerPressedEvent_{};
        event_token dataGridPointerReleasedEvent_{};
        event_token dataGridUnloadedEvent_{};

        event_token hScrollBarScrollEvent_{};
        event_token hScrollBarPointerEnteredEvent_{};
        event_token hScrollBarPointerExitedEvent_{};

        event_token vScrollBarScrollEvent_{};
        event_token vScrollBarPointerEnteredEvent_{};
        event_token vScrollBarPointerExitedEvent_{};

        // Cache event listeners for VectorChanged events from ICollectionViewGroup's GroupItems
        std::map<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>, std::shared_ptr<Utility::WeakEventListener<MiniDumpExplorer::DataGrid, Windows::Foundation::IInspectable, Windows::Foundation::Collections::IVectorChangedEventArgs>>> groupsVectorChangedListenersTable_;

        DataGridInternal::DataGridColumnHeaderInteractionInfo columnHeaderInteractionInfo_{};
        DataGridInternal::DataGridDisplayData displayData_{this};
        DataGridInternal::DataGridColumnCollection columnsInternal_{this};
        DataGridInternal::DataGridDataConnection dataConnection_{this};

        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs>> autoGeneratingColumnHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridBeginningEditEventArgs>> beginningEditHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndedEventArgs>> cellEditEndedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridCellEditEndingEventArgs>> cellEditEndingHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs>> columnDisplayIndexChangedHandler_;
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragCompletedEventArgs>> columnHeaderDragCompletedHandler_;
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragDeltaEventArgs>> columnHeaderDragDeltaHandler_;
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::Controls::Primitives::DragStartedEventArgs>> columnHeaderDragStartedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs>> columnReorderedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnReorderingEventArgs>> columnReorderingHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowClipboardEventArgs>> copyingRowClipboardContentHandler_;
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs>> currentCellChangedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs>> loadingRowHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs>> loadingRowDetailsHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs>> loadingRowGroupHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridPreparingCellForEditEventArgs>> preparingCellForEditHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs>> rowDetailsVisibilityChangedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndedEventArgs>> rowEditEndedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEditEndingEventArgs>> rowEditEndingHandler_;
        event<Microsoft::UI::Xaml::Controls::SelectionChangedEventHandler> selectionChangedHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridColumnEventArgs>> sortingHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowEventArgs>> unloadingRowHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowGroupHeaderEventArgs>> unloadingRowGroupHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs>> unloadingRowDetailsHandler_;
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::DataGridRowDetailsEventArgs>> rowDoubleTappedHandler_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGrid : DataGridT<DataGrid, implementation::DataGrid>
    {
    };
}
