#pragma once

#include "DataGridRowGroupHeader.g.h"
#include "DataGridRowGroupInfo.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridRowHeader;

    struct DataGridRowGroupHeader : DataGridRowGroupHeaderT<DataGridRowGroupHeader>
    {
        DataGridRowGroupHeader();

        static Microsoft::UI::Xaml::DependencyProperty HeaderStyleProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemCountVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty LevelProperty();
        static Microsoft::UI::Xaml::DependencyProperty PropertyNameProperty();
        static Microsoft::UI::Xaml::DependencyProperty PropertyNameVisibilityProperty();
        static Microsoft::UI::Xaml::DependencyProperty PropertyValueProperty();
        static Microsoft::UI::Xaml::DependencyProperty SublevelIndentProperty();
        
        Microsoft::UI::Xaml::Style HeaderStyle() const;
        void HeaderStyle(Microsoft::UI::Xaml::Style const& value) const;
        Microsoft::UI::Xaml::Visibility ItemCountVisibility() const;
        void ItemCountVisibility(Microsoft::UI::Xaml::Visibility const& value) const;
        uint32_t Level() const;
        void Level(uint32_t const& value) const;
        hstring PropertyName() const;
        void PropertyName(hstring const& value) const;
        Microsoft::UI::Xaml::Visibility PropertyNameVisibility() const;
        void PropertyNameVisibility(Microsoft::UI::Xaml::Visibility const& value) const;
        hstring PropertyValue() const;
        void PropertyValue(hstring const& value) const;
        double SublevelIndent() const;
        void SublevelIndent(double const& value) const;
        Microsoft::UI::Xaml::Data::ICollectionViewGroup CollectionViewGroup() const;

        MiniDumpExplorer::DataGrid const& OwningGrid() const { return owningGrid_; }
        void OwningGrid(MiniDumpExplorer::DataGrid const& value) { owningGrid_ = value; }

        std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& RowGroupInfo() const { return rowGroupInfo_; }
        void RowGroupInfo(std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> value) { rowGroupInfo_ = std::move(value); }

        MiniDumpExplorer::DataGridRowHeader const& HeaderCell() const { return headerElement_; }

        bool IsRecycled() const { return isRecycled_; }
        void IsRecycled(bool const value) { isRecycled_ = value; }

        void TotalIndent(double value);
        void ApplyHeaderState(bool animate) const;
        void ApplyState(bool useTransitions) const;

        void ClearFrozenStates() const;
        void EnsureExpanderButtonIsChecked();
        void EnsureHeaderStyleAndVisibility(Microsoft::UI::Xaml::Style const& previousStyle) const;
        void LoadVisualsForDisplay();
        void ToggleExpandCollapse(Microsoft::UI::Xaml::Visibility newVisibility, bool setCurrent);
        void UpdateTitleElements() const;
        void EnsureGridLine() const;

        // ReSharper disable CppHidingFunction
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize) const; // override
        void OnApplyTemplate(); // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override
        // ReSharper restore CppHidingFunction

    private:
        void OnHeaderStylePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnPropertyNameChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnPropertyValueChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnSublevelIndentPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        bool IsCurrent() const;
        void DataGridRowGroupHeader_Tapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e) const;
        void DataGridRowGroupHeader_DoubleTapped(Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e);
        void EnsureChildClip(Microsoft::UI::Xaml::UIElement const& child, double frozenLeftEdge) const;
        void ExpanderButton_Checked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ExpanderButton_Unchecked(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SetIsCheckedNoCallBack(bool value);
        void DataGridRowGroupHeader_PointerCanceled(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRowGroupHeader_PointerEntered(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRowGroupHeader_PointerExited(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRowGroupHeader_PointerMoved(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRowGroupHeader_PointerPressed(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridRowGroupHeader_PointerReleased(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void UpdateIsPointerOver(bool isPointerOver);
        void UpdateIsPressed(bool isPressed);
        static hstring MakeNumberOfItemsText(size_t items);

        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;
        auto InternalHeaderElement() const;
        // ReSharper restore CppFunctionIsNotImplemented

    private:
        MiniDumpExplorer::DataGrid owningGrid_{ nullptr };
        std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> rowGroupInfo_;
        MiniDumpExplorer::DataGridRowHeader headerElement_{nullptr};
        bool isPointerOver_{};
        bool isPressed_{};
        bool isRecycled_{};
        bool areIsCheckedHandlersSuspended_{};
        Microsoft::UI::Xaml::Shapes::Rectangle bottomGridLine_{nullptr};
        Microsoft::UI::Xaml::Controls::Primitives::ToggleButton expanderButton_{nullptr};
        Microsoft::UI::Xaml::FrameworkElement indentSpacer_{nullptr};
        Microsoft::UI::Xaml::Controls::TextBlock itemCountElement_{nullptr};
        Microsoft::UI::Xaml::Controls::TextBlock propertyNameElement_{nullptr};
        Microsoft::UI::Xaml::Controls::TextBlock propertyValueElement_{nullptr};
        Microsoft::UI::Xaml::Controls::Panel rootElement_{nullptr};
        double totalIndent_{};
        event_token expanderButtonChecked_{};
        event_token expanderButtonUnchecked_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowGroupHeader : DataGridRowGroupHeaderT<DataGridRowGroupHeader, implementation::DataGridRowGroupHeader>
    {
    };
}
