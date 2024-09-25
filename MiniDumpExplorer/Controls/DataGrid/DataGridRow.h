#pragma once

#include "DataGridRow.g.h"

#include "DataGridCellCollection.h"
#include "DataGridInteractionInfo.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridCell;
    struct DataGridCellEventArgs;
    struct DataGridCellsPresenter;
    struct DataGridDetailsPresenter;
    struct DataGridRowHeader;

    struct DataGridRow : DataGridRowT<DataGridRow>
    {
        static constexpr auto InvalidIndex = int32_t{-1};

        DataGridRow();

        static constexpr int MaximumHeight = 65536;
        static constexpr double MinimumHeight = 0;
        static const std::wstring ElementRoot;
        static const std::wstring ElementRowHeader;

        static Microsoft::UI::Xaml::DependencyProperty DetailsTemplateProperty();
        static Microsoft::UI::Xaml::DependencyProperty DetailsVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeaderProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeaderStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsValidProperty();

        Microsoft::UI::Xaml::DataTemplate DetailsTemplate() const;
        void DetailsTemplate(Microsoft::UI::Xaml::DataTemplate const& value) const;
        Microsoft::UI::Xaml::Visibility DetailsVisibility() const;
        void DetailsVisibility(Microsoft::UI::Xaml::Visibility const& value) const;
        Windows::Foundation::IInspectable Header() const;
        void Header(Windows::Foundation::IInspectable const& value) const;
        Microsoft::UI::Xaml::Style HeaderStyle() const;
        void HeaderStyle(Microsoft::UI::Xaml::Style const& value) const;
        bool IsValid() const;
        void IsValid(bool value) const;

        int32_t Index() const { return index_; }
        void Index(int32_t const value) { index_ = value; }

        MiniDumpExplorer::DataGrid const& OwningGrid() const { return owningGrid_; }
        void OwningGrid(MiniDumpExplorer::DataGrid const& value) { owningGrid_ = value; }
        DataGridInternal::DataGridCellCollection const& Cells() const { return cells_; }
        DataGridInternal::DataGridCellCollection& Cells() { return cells_; }

        int32_t Slot() const { return slot_; }
        void Slot(int32_t const slot) { slot_ = slot; }
        bool IsSelected() const;

        MiniDumpExplorer::DataGridRowHeader HeaderCell() const { return headerElement_;}
        MiniDumpExplorer::DataGridCell FillerCell();

        Microsoft::UI::Xaml::Media::Brush ComputedForeground() const { return computedForeground_; }
        void ComputedForeground(Microsoft::UI::Xaml::Media::Brush const& value);

        bool IsPointerOver() const { return interactionInfo_.IsPointerOver; }
        void IsPointerOver(bool value);

        double ActualBottomGridLineHeight()  const;
        double DetailsContentHeight() const;
        bool HasBottomGridLine() const { return static_cast<bool>(bottomGridLine_); }
        bool HasHeaderCell() const { return static_cast<bool>(headerElement_); }
        bool IsEditing() const;

        bool IsRecycled() const { return isRecycled_; }
        bool IsRecyclable() const;
        Microsoft::UI::Xaml::Controls::Panel const& RootElement() const { return rootElement_; }
        double TargetHeight() const;

        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize); // override
        // ReSharper disable once CppHidingFunction
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize) const; // override
        void OnApplyTemplate(); // override

        // ReSharper disable once CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override

        void ApplyCellsState(bool animate) const;
        void ApplyDetailsTemplate(bool initializeDetailsPreferredHeight);
        void ApplyHeaderState(bool animate) const;
        void ApplyState(bool animate) const;
        void DetachFromDataGrid(bool recycle);
        void EnsureBackground() const;
        void EnsureForeground();
        void EnsureFillerVisibility() const;
        void EnsureGridLines() const;
        void EnsureHeaderStyleAndVisibility(Microsoft::UI::Xaml::Style const& previousStyle) const;
        void EnsureHeaderVisibility() const;
        void InvalidateHorizontalArrange() const;
        void SetDetailsVisibilityInternal(Microsoft::UI::Xaml::Visibility visibility,bool raiseNotification);

        static MiniDumpExplorer::DataGridRow GetRowContainingElement(Microsoft::UI::Xaml::FrameworkElement const& element);

    private:
        // ReSharper disable once CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;

        void OnCellAdded(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args) const;
        void OnCellRemoved(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args) const;
        void DataGridRow_PointerTapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e) const;
        void DataGridRow_PointerCanceled(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRow_PointerCaptureLost(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRow_PointerPressed(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRow_PointerReleased(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void DataGridRow_PointerEntered(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRow_PointerExited(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRow_PointerMoved(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void OnDependencyPropertyChanged(Microsoft::UI::Xaml::DependencyObject const& sender, Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty);
        void OnDetailsTemplatePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnDetailsVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void OnHeaderPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnHeaderStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnIsValidPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void DetailsContent_SizeChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

        void DataGridRow_DoubleTapped(Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) const;

        void CancelPointer(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void UpdateIsPointerOver(bool isPointerOver);

        Microsoft::UI::Xaml::DataTemplate ActualDetailsTemplate() const;
        Microsoft::UI::Xaml::DataTemplate GetActualDetailsTemplate(Microsoft::UI::Xaml::DataTemplate const& source) const;
        Microsoft::UI::Xaml::Visibility ActualDetailsVisibility() const;
        bool AreDetailsVisible() const { return ActualDetailsVisibility() == Microsoft::UI::Xaml::Visibility::Visible; }

        void EnsureHeaderGridLines(Microsoft::UI::Xaml::Visibility visibility) const;
        void EnsureDetailsDesiredHeight();
        void EnsureDetailsContentHeight();
        void OnRowDetailsChanged() const;
        void Recycle();
        void RemoveDetailsContentSizeChangedHandler();
        void UnloadDetailsTemplate(bool recycle, bool setDetailsVisibilityToCollapsed);

    private:
        enum class ScrollBarVisualState : uint8_t
        {
            NoIndicator,
            TouchIndicator,
            MouseIndicator,
            MouseIndicatorFull
        };

        enum class ScrollBarsSeparatorVisualState : uint8_t
        {
            SeparatorCollapsed,
            SeparatorExpanded,
            SeparatorExpandedWithoutAnimation,
            SeparatorCollapsedWithoutAnimation
        };

        // Locally cache whether details are visible, so we don't run redundant storyboards
        // The Details Template that is actually applied to the Row
        Microsoft::UI::Xaml::DataTemplate appliedDetailsTemplate_{nullptr};
        std::optional<Microsoft::UI::Xaml::Visibility> appliedDetailsVisibility_{Microsoft::UI::Xaml::Visibility::Collapsed};
        Microsoft::UI::Xaml::Shapes::Rectangle bottomGridLine_;
        MiniDumpExplorer::DataGridCellsPresenter cellsElement_{nullptr};
        Microsoft::UI::Xaml::Controls::Panel rootElement_{nullptr};

        // In the case where Details scales vertically when it's arranged at a different width, we
        // get the wrong height measurement, so we need to check it again after arrange
        bool checkDetailsContentHeight_{nullptr};

        Microsoft::UI::Xaml::Media::Brush computedForeground_{Foreground()};

        // Optimal height of the details based on the Element created by the DataTemplate
        double detailsDesiredHeight_{std::nan("")};
        bool detailsLoaded_{false};
        bool detailsVisibilityNotificationPending_{false};
        Microsoft::UI::Xaml::FrameworkElement detailsContent_{nullptr};
        MiniDumpExplorer::DataGridDetailsPresenter detailsElement_{nullptr};
        MiniDumpExplorer::DataGridCell fillerCell_{nullptr};
        MiniDumpExplorer::DataGridRowHeader headerElement_{nullptr};
        double lastHorizontalOffset_{};

        int32_t index_{InvalidIndex};
        int32_t slot_{InvalidIndex};
        bool isRecycled_{false};

        MiniDumpExplorer::DataGrid owningGrid_{ nullptr };

        DataGridInternal::DataGridCellCollection cells_{ this };
        DataGridInternal::DataGridInteractionInfo interactionInfo_{};
        event_token detailsContentSizeChangedEvent_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRow : DataGridRowT<DataGridRow, implementation::DataGridRow>
    {
    };
}
