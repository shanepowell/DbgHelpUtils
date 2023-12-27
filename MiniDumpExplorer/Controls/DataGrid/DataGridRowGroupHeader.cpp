#include "pch.h"
#include "DataGridRowGroupHeader.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGrid.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DataGridDataConnection.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowGroupHeaderAutomationPeer.h"
#include "DataGridRowHeader.h"
#include "DbgHelpUtils/exit_scope.h"
#include "Helpers/UIHelper.h"
#include "Helpers/VisualStates.h"

#if __has_include("DataGridRowGroupHeader.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowGroupHeader.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        hstring const BottomGridLine = L"BottomGridLine";
        hstring const ExpanderButton = L"ExpanderButton";
        hstring const IndentSpacer = L"IndentSpacer";
        hstring const ItemCountElement = L"ItemCountElement";
        hstring const PropertyNameElement = L"PropertyNameElement";
        hstring const PropertyValueElement = L"PropertyValueElement";
    }

    auto DataGridRowGroupHeader::InternalOwningGrid() const
    {
        return owningGrid_.try_as<DataGrid>();
    }

    auto DataGridRowGroupHeader::InternalHeaderElement() const
    {
        return headerElement_.as<DataGridRowHeader>();
    }


    DataGridRowGroupHeader::DataGridRowGroupHeader()
    {
        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGridRowGroupHeader"));

        AddHandler(UIElement::TappedEvent(), winrt::box_value<Input::TappedEventHandler>({ [](Windows::Foundation::IInspectable const& sender, Input::TappedRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_Tapped(e); } }), true /*handledEventsToo*/);
        AddHandler(UIElement::DoubleTappedEvent(), winrt::box_value<Input::DoubleTappedEventHandler>({ [](Windows::Foundation::IInspectable const& sender, Input::DoubleTappedRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_DoubleTapped(e); } }), true /*handledEventsToo*/);

        PointerCanceled(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerCanceled(e); } });
        PointerEntered(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerEntered(e); } });
        PointerExited(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerExited(e); } });
        PointerMoved(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerMoved(e); } });
        PointerPressed(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerPressed(e); } });
        PointerReleased(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRowGroupHeader>()->DataGridRowGroupHeader_PointerReleased(e); } });
    }

    DependencyProperty DataGridRowGroupHeader::HeaderStyleProperty()
    {
        static DependencyProperty s_headerStyleProperty = DependencyProperty::Register(
            L"HeaderStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRowGroupHeader>()->OnHeaderStylePropertyChanged(e); } });

        return s_headerStyleProperty;
    }

    DependencyProperty DataGridRowGroupHeader::ItemCountVisibilityProperty()
    {
        static DependencyProperty s_itemCountVisibilityProperty = DependencyProperty::Register(
            L"ItemCountVisibility",
            xaml_typename<Microsoft::UI::Xaml::Visibility>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(Visibility::Visible) });

        return s_itemCountVisibilityProperty;
    }

    DependencyProperty DataGridRowGroupHeader::LevelProperty()
    {
        static DependencyProperty s_levelProperty = DependencyProperty::Register(
            L"Level",
            xaml_typename<uint64_t>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(0ULL) });

        return s_levelProperty;
    }

    DependencyProperty DataGridRowGroupHeader::PropertyNameProperty()
    {
        static DependencyProperty s_propertyNameProperty = DependencyProperty::Register(
            L"PropertyName",
            xaml_typename<hstring>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(hstring{}), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRowGroupHeader>()->OnPropertyNameChanged(e); } });

        return s_propertyNameProperty;
    }

    DependencyProperty DataGridRowGroupHeader::PropertyNameVisibilityProperty()
    {
        static DependencyProperty s_propertyNameVisibilityProperty = DependencyProperty::Register(
            L"PropertyNameVisibility",
            xaml_typename<Microsoft::UI::Xaml::Visibility>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(Visibility::Visible) });

        return s_propertyNameVisibilityProperty;
    }

    DependencyProperty DataGridRowGroupHeader::PropertyValueProperty()
    {
        static DependencyProperty s_propertyValueProperty = DependencyProperty::Register(
            L"PropertyValue",
            xaml_typename<hstring>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(hstring{}), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRowGroupHeader>()->OnPropertyValueChanged(e); } });

        return s_propertyValueProperty;
    }

    DependencyProperty DataGridRowGroupHeader::SublevelIndentProperty()
    {
        static DependencyProperty s_sublevelIndentProperty = DependencyProperty::Register(
            L"SublevelIndent",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGridRowGroupHeader>(),
            PropertyMetadata{ box_value(DataGrid::DefaultRowGroupSublevelIndent), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRowGroupHeader>()->OnSublevelIndentPropertyChanged(e); }  });

        return s_sublevelIndentProperty;
    }

    Style DataGridRowGroupHeader::HeaderStyle() const
    {
        return GetValue(HeaderStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGridRowGroupHeader::HeaderStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(HeaderStyleProperty(), value);
    }

    Visibility DataGridRowGroupHeader::ItemCountVisibility() const
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(ItemCountVisibilityProperty()));
    }

    void DataGridRowGroupHeader::ItemCountVisibility(Microsoft::UI::Xaml::Visibility const& value) const
    {
        SetValue(ItemCountVisibilityProperty(), box_value(value));
    }

    uint32_t DataGridRowGroupHeader::Level() const
    {
        return unbox_value<uint32_t>(GetValue(LevelProperty()));
    }

    void DataGridRowGroupHeader::Level(uint32_t const& value) const
    {
        SetValue(LevelProperty(), box_value(value));
    }

    hstring DataGridRowGroupHeader::PropertyName() const
    {
        return unbox_value<hstring>(GetValue(PropertyNameProperty()));
    }

    void DataGridRowGroupHeader::PropertyName(hstring const& value) const
    {
        SetValue(PropertyNameProperty(), box_value(value));
    }

    Visibility DataGridRowGroupHeader::PropertyNameVisibility() const
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(PropertyNameVisibilityProperty()));
    }

    void DataGridRowGroupHeader::PropertyNameVisibility(Microsoft::UI::Xaml::Visibility const& value) const
    {
        SetValue(PropertyNameVisibilityProperty(), box_value(value));
    }

    hstring DataGridRowGroupHeader::PropertyValue() const
    {
        return unbox_value<hstring>(GetValue(PropertyValueProperty()));
    }

    void DataGridRowGroupHeader::PropertyValue(hstring const& value) const
    {
        SetValue(PropertyValueProperty(), box_value(value));
    }

    double DataGridRowGroupHeader::SublevelIndent() const
    {
        return unbox_value<double>(GetValue(SublevelIndentProperty()));
    }

    void DataGridRowGroupHeader::SublevelIndent(double const& value) const
    {
        SetValue(SublevelIndentProperty(), box_value(value));
    }

    Data::ICollectionViewGroup DataGridRowGroupHeader::CollectionViewGroup() const
    {
        return RowGroupInfo() ? RowGroupInfo()->CollectionViewGroup() : nullptr;
    }

    void DataGridRowGroupHeader::TotalIndent(double const value)
    {
        totalIndent_ = value;
        if (indentSpacer_)
        {
            indentSpacer_.Width(totalIndent_);
        }
    }

    void DataGridRowGroupHeader::ApplyHeaderState(bool const animate) const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            headerElement_ && internalOwningGrid->AreRowHeadersVisible())
        {
            InternalHeaderElement()->ApplyOwnerState(animate);
        }
    }

    void DataGridRowGroupHeader::ApplyState(bool const useTransitions) const
    {
        // Common States
        if (isPressed_)
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StatePressed, VisualStates::StatePointerOver, VisualStates::StateNormal);
        }
        else if (isPointerOver_)
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StatePointerOver, VisualStates::StateNormal);
        }
        else
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateNormal);
        }

        // Current States
        if (auto const internalOwningGrid = InternalOwningGrid();
            IsCurrent() && !internalOwningGrid->ColumnHeaderHasFocus())
        {
            if (internalOwningGrid->ContainsFocus())
            {
                VisualStates::GoToState(*this, useTransitions, VisualStates::StateCurrentWithFocus, VisualStates::StateCurrent, VisualStates::StateRegular);
            }
            else
            {
                VisualStates::GoToState(*this, useTransitions, VisualStates::StateCurrent, VisualStates::StateRegular);
            }
        }
        else
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateRegular);
        }

        // Expanded States
        if (RowGroupInfo()->CollectionViewGroup().GroupItems().Size() == 0)
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateEmpty);
        }
        else
        {
            if (RowGroupInfo()->Visibility() == Visibility::Visible)
            {
                VisualStates::GoToState(*this, useTransitions, VisualStates::StateExpanded, VisualStates::StateEmpty);
            }
            else
            {
                VisualStates::GoToState(*this, useTransitions, VisualStates::StateCollapsed, VisualStates::StateEmpty);
            }
        }
    }

    void DataGridRowGroupHeader::ClearFrozenStates() const
    {
        if (!rootElement_)
        {
            return;
        }

        for(auto const& child : rootElement_.Children())
        {
            child.RenderTransform(nullptr);
        }
    }

    void DataGridRowGroupHeader::EnsureExpanderButtonIsChecked()
    {
        if (expanderButton_ &&
            RowGroupInfo() &&
            RowGroupInfo()->CollectionViewGroup() &&
            RowGroupInfo()->CollectionViewGroup().GroupItems() &&
            RowGroupInfo()->CollectionViewGroup().GroupItems().Size() != 0)
        {
            SetIsCheckedNoCallBack(RowGroupInfo()->Visibility() == Visibility::Visible);
        }
    }

    void DataGridRowGroupHeader::EnsureHeaderStyleAndVisibility(Microsoft::UI::Xaml::Style const& previousStyle) const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            headerElement_ && internalOwningGrid)
        {
            if (internalOwningGrid->AreRowHeadersVisible())
            {
                InternalHeaderElement()->EnsureStyle(previousStyle);
                headerElement_.Visibility(Visibility::Visible);
            }
            else
            {
                headerElement_.Visibility(Visibility::Collapsed);
            }
        }
    }

    void DataGridRowGroupHeader::LoadVisualsForDisplay()
    {
        EnsureExpanderButtonIsChecked();

        EnsureHeaderStyleAndVisibility(nullptr);
        ApplyState(false /*useTransitions*/);
        ApplyHeaderState(false);
    }

    void DataGridRowGroupHeader::ToggleExpandCollapse(Microsoft::UI::Xaml::Visibility const newVisibility, bool const setCurrent)
    {
        if (RowGroupInfo()->CollectionViewGroup().GroupItems().Size() != 0)
        {
            if (auto const internalOwningGrid = InternalOwningGrid();
                !internalOwningGrid)
            {
                // Do these even if the OwningGrid is null in case it could improve the Designer experience for a standalone DataGridRowGroupHeader
                RowGroupInfo()->Visibility(newVisibility);
            }
            else
            {
                internalOwningGrid->OnRowGroupHeaderToggled(*this, newVisibility, setCurrent);
            }

            EnsureExpanderButtonIsChecked();
            ApplyState(true /*useTransitions*/);
        }
    }

    void DataGridRowGroupHeader::UpdateTitleElements() const
    {
        auto propertyName = PropertyName();
        auto const hasPropertyValue = propertyValueElement_ && !PropertyValue().empty();

        if (propertyNameElement_)
        {
            auto const internalOwningGrid = InternalOwningGrid();
            if (auto const displayName = internalOwningGrid->DataConnection().GetPropertyDisplayName(propertyName);
                !displayName.empty())
            {
                propertyName = displayName;
            }

            if (propertyName.empty())
            {
                propertyName = internalOwningGrid->RowGroupHeaderPropertyNameAlternative();
            }

            if (!propertyName.empty() && hasPropertyValue)
            {
                propertyName = std::format(L"{}:", propertyName);
            }

            if (!propertyName.empty())
            {
                propertyNameElement_.Text(propertyName);
            }
        }

        if (hasPropertyValue)
        {
            propertyValueElement_.Text(PropertyValue());
        }

        if (itemCountElement_ && RowGroupInfo() && RowGroupInfo()->CollectionViewGroup())
        {
            itemCountElement_.Text(MakeNumberOfItemsText(RowGroupInfo()->CollectionViewGroup().GroupItems().Size()));
        }
    }

    void DataGridRowGroupHeader::EnsureGridLine() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid  && bottomGridLine_)
        {
            if (auto const newVisibility = internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::Horizontal || internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::All
                ? Visibility::Visible : Visibility::Collapsed;
                newVisibility != bottomGridLine_.Visibility())
            {
                bottomGridLine_.Visibility(newVisibility);
            }

            bottomGridLine_.Fill(internalOwningGrid->HorizontalGridLinesBrush());
        }
    }

    Windows::Foundation::Size DataGridRowGroupHeader::ArrangeOverride(Windows::Foundation::Size const& finalSize) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        // ReSharper disable once CppLocalVariableMayBeConst
        auto size = base_type::ArrangeOverride(finalSize);
        if (!internalOwningGrid || !rootElement_)
        {
            return size;
        }

        if (internalOwningGrid->AreRowGroupHeadersFrozen())
        {
            for(auto const& child : rootElement_.Children())
            {
                child.Clip(nullptr);
            }
        }
        else
        {
            double frozenLeftEdge = 0;
            for(auto const& child : rootElement_.Children())
            {
                if (DataGridFrozenGrid::GetIsFrozen(child) && child.Visibility() == Visibility::Visible)
                {
                    Media::TranslateTransform transform{};

                    // Automatic layout rounding doesn't apply to transforms, so we need to Round this
                    transform.X(std::round(internalOwningGrid->HorizontalOffset()));
                    child.RenderTransform(transform);

                    auto const childLeftEdge = UIHelper::Translate(child, *this, {child.RenderSize().Width, 0}).X - transform.X();
                    frozenLeftEdge = std::max(frozenLeftEdge, childLeftEdge + internalOwningGrid->HorizontalOffset());
                }
            }

            // Clip the non-frozen elements, so they don't overlap the frozen ones
            for(auto const& child : rootElement_.Children())
            {
                if (!DataGridFrozenGrid::GetIsFrozen(child))
                {
                    EnsureChildClip(child, frozenLeftEdge);
                }
            }
        }

        return size;
    }

    void DataGridRowGroupHeader::OnApplyTemplate()
    {
        rootElement_ = GetTemplateChild(DataGridRow::ElementRoot).try_as<Controls::Panel>();

        if(expanderButtonChecked_)
        {
            expanderButton_.Checked(expanderButtonChecked_);
            expanderButtonChecked_ = {};
        }
        if(expanderButtonUnchecked_)
        {
            expanderButton_.Unchecked(expanderButtonUnchecked_);
            expanderButtonUnchecked_ = {};
        }

        bottomGridLine_ = GetTemplateChild(BottomGridLine).try_as<Shapes::Rectangle>();

        expanderButton_ = GetTemplateChild(ExpanderButton).try_as<Controls::Primitives::ToggleButton>();
        if (expanderButton_)
        {
            EnsureExpanderButtonIsChecked();
            expanderButtonChecked_ = expanderButton_.Checked({this, &DataGridRowGroupHeader::ExpanderButton_Checked});
            expanderButtonUnchecked_ = expanderButton_.Unchecked({this, &DataGridRowGroupHeader::ExpanderButton_Unchecked});
        }

        headerElement_ = GetTemplateChild(DataGridRow::ElementRowHeader).try_as<MiniDumpExplorer::DataGridRowHeader>();
        if (headerElement_)
        {
            InternalHeaderElement()->Owner(*this);
            EnsureHeaderStyleAndVisibility(nullptr);
        }

        indentSpacer_ = GetTemplateChild(IndentSpacer).try_as<FrameworkElement>();
        if (indentSpacer_)
        {
            indentSpacer_.Width(totalIndent_);
        }

        itemCountElement_ = GetTemplateChild(ItemCountElement).try_as<Controls::TextBlock>();
        propertyNameElement_ = GetTemplateChild(PropertyNameElement).try_as<Controls::TextBlock>();
        propertyValueElement_ = GetTemplateChild(PropertyValueElement).try_as<Controls::TextBlock>();
        UpdateTitleElements();
        EnsureGridLine();
    }

    Automation::Peers::AutomationPeer DataGridRowGroupHeader::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer{*this};
    }

    void DataGridRowGroupHeader::OnHeaderStylePropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        headerElement_.as<DataGridRowHeader>()->EnsureStyle(e.OldValue().as<Microsoft::UI::Xaml::Style>());
    }

    void DataGridRowGroupHeader::OnPropertyNameChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        UpdateTitleElements();
    }

    void DataGridRowGroupHeader::OnPropertyValueChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        UpdateTitleElements();
    }

    void DataGridRowGroupHeader::OnSublevelIndentPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        auto const newValue = unbox_value<double>(e.NewValue());

        // We don't need to revert to the old value if our input is bad because we never read this property value
        if (std::isnan(newValue))
        {
            throw DataGridError::ValueCannotBeSetToNAN(L"SublevelIndent");
        }

        if (std::isinf(newValue))
        {
            throw DataGridError::ValueCannotBeSetToInfinity(L"SublevelIndent");
        }

        if (newValue < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value", L"SublevelIndent", 0);
        }

        if (auto const internalOwningGrid = InternalOwningGrid(); internalOwningGrid)
        {
            internalOwningGrid->OnSublevelIndentUpdated(*this, newValue);
        }
    }

    bool DataGridRowGroupHeader::IsCurrent() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);
        return RowGroupInfo()->Slot() == internalOwningGrid->CurrentSlot();
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_Tapped(Input::TappedRoutedEventArgs const& e) const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid && !internalOwningGrid->HasColumnUserInteraction())
        {
            if (!e.Handled() && internalOwningGrid->IsTabStop())
            {
                auto const success = internalOwningGrid->Focus(FocusState::Programmatic);
                assert(success);
            }

            e.Handled(internalOwningGrid->UpdateStateOnTapped(e, internalOwningGrid->CurrentColumnIndex(), RowGroupInfo()->Slot(), false /*allowEdit*/));
        }
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_DoubleTapped(Input::DoubleTappedRoutedEventArgs const& e)
    {
        if (auto const internalOwningGrid = InternalOwningGrid(); internalOwningGrid && !internalOwningGrid->HasColumnUserInteraction() && !e.Handled())
        {
            ToggleExpandCollapse(RowGroupInfo()->Visibility() == Visibility::Visible ? Visibility::Collapsed : Visibility::Visible, true);
            e.Handled(true);
        }
    }

    void DataGridRowGroupHeader::EnsureChildClip(UIElement const& child, double const frozenLeftEdge) const
    {
        if (auto const childLeftEdge = UIHelper::Translate(child, *this, {0, 0}).X;
            frozenLeftEdge > childLeftEdge)
        {
            auto const xClip = std::round(frozenLeftEdge - childLeftEdge);
            Media::RectangleGeometry const rg{};
            rg.Rect({static_cast<float>(xClip), 0, static_cast<float>(std::max(0.0, child.RenderSize().Width - xClip)), child.RenderSize().Height});
            child.Clip(rg);
        }
        else
        {
            child.Clip(nullptr);
        }
    }

    void DataGridRowGroupHeader::ExpanderButton_Checked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (!areIsCheckedHandlersSuspended_)
        {
            ToggleExpandCollapse(Visibility::Visible, true);
        }
    }

    void DataGridRowGroupHeader::ExpanderButton_Unchecked([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (!areIsCheckedHandlersSuspended_)
        {
            ToggleExpandCollapse(Visibility::Collapsed, true);
        }
    }

    void DataGridRowGroupHeader::SetIsCheckedNoCallBack(bool const value)
    {
        if (expanderButton_ && expanderButton_.IsChecked() != Windows::Foundation::IReference{value})
        {
            areIsCheckedHandlersSuspended_ = true;
            dlg_help_utils::scope_exit const reset_areIsCheckedHandlersSuspended{[this] { areIsCheckedHandlersSuspended_ = false; }};
            expanderButton_.IsChecked(value);
        }
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerCanceled([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(false);
        UpdateIsPressed(false);
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerEntered([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerExited([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(false);
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerMoved([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerPressed([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPressed(true);
    }

    void DataGridRowGroupHeader::DataGridRowGroupHeader_PointerReleased([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPressed(false);
    }

    void DataGridRowGroupHeader::UpdateIsPointerOver(bool const isPointerOver)
    {
        if (!IsEnabled() || isPointerOver == isPointerOver_)
        {
            return;
        }

        isPointerOver_ = isPointerOver;
        ApplyState(true /*useTransitions*/);
    }

    void DataGridRowGroupHeader::UpdateIsPressed(bool const isPressed)
    {
        if (!IsEnabled() || isPressed == isPressed_)
        {
            return;
        }

        isPressed_ = isPressed;
        ApplyState(true /*useTransitions*/);
    }

    hstring DataGridRowGroupHeader::MakeNumberOfItemsText(size_t items)
    {
        if(items == 1)
        {
            return L"(1 item)";
        }
        return hstring{std::format(L"({} items)", items)};
    }
}
