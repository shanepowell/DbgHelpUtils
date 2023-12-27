#include "pch.h"
#include "DataGridRow.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGrid.h"
#include "DataGridCell.h"
#include "DataGridCellCollection.h"
#include "DataGridCellsPresenter.h"
#include "DataGridColumn.h"
#include "DataGridColumnCollection.h"
#include "DataGridDataConnection.h"
#include "DataGridDetailsPresenter.h"
#include "DataGridError.h"
#include "DataGridRowHeader.h"
#include "DbgHelpUtils/double_utils.h"
#include "Helpers/UIHelper.h"
#include "Helpers/VisualStates.h"
#include "Utility/SuspendedHandlers.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellInternal.h"

#if __has_include("DataGridRow.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRow.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRowAutomationPeer;

    namespace
    {
        constexpr byte DefaultMinHeight = 0;

        const hstring ElementBottomGridLine = L"BottomGridLine";
        const hstring ElementCells = L"CellsPresenter";
        const hstring ElementDetails = L"DetailsPresenter";

        const hstring StateAlternate = L"NormalAlternatingRow";
        const hstring StatePointerOver = L"PointerOver";
        const hstring StatePointerOverEditing = L"PointerOverUnfocusedEditing";
        const hstring StatePointerOverEditingFocused = L"PointerOverEditing";
        const hstring StatePointerOverSelected = L"PointerOverUnfocusedSelected";
        const hstring StatePointerOverSelectedFocused = L"PointerOverSelected";
        const hstring StateNormal = L"Normal";
        const hstring StateNormalEditing = L"UnfocusedEditing";
        const hstring StateNormalEditingFocused = L"NormalEditing";
        const hstring StateSelected = L"UnfocusedSelected";
        const hstring StateSelectedFocused = L"NormalSelected";

        constexpr byte StatePointerOverCode = 0;
        constexpr byte StatePointerOverEditingCode = 1;
        constexpr byte StatePointerOverEditingFocusedCode = 2;
        constexpr byte StatePointerOverSelectedCode = 3;
        constexpr byte StatePointerOverSelectedFocusedCode = 4;
        constexpr byte StateNormalCode = 5;
        constexpr byte StateNormalEditingCode = 6;
        constexpr byte StateNormalEditingFocusedCode = 7;
        constexpr byte StateSelectedCode = 8;
        constexpr byte StateSelectedFocusedCode = 9;
        constexpr byte StateNullCode = 255;

        // Static arrays to handle state transitions:
        constexpr std::array IdealStateMapping = 
        {
            StateNormalCode,
            StateNormalCode,
            StatePointerOverCode,
            StatePointerOverCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateSelectedCode,
            StateSelectedFocusedCode,
            StatePointerOverSelectedCode,
            StatePointerOverSelectedFocusedCode,
            StateNormalEditingCode,
            StateNormalEditingFocusedCode,
            StatePointerOverEditingCode,
            StatePointerOverEditingFocusedCode
        };

        constexpr std::array FallbackStateMapping =
        {
            StateNormalCode, // statePointerOverCode's fallback
            StatePointerOverEditingFocusedCode, // statePointerOverEditingCode's fallback
            StateNormalEditingFocusedCode, // statePointerOverEditingFocusedCode's fallback
            StatePointerOverSelectedFocusedCode, // statePointerOverSelectedCode's fallback
            StateSelectedFocusedCode, // statePointerOverSelectedFocusedCode's fallback
            StateNullCode, // stateNormalCode's fallback
            StateNormalEditingFocusedCode, // stateNormalEditingCode's fallback
            StateSelectedFocusedCode, // stateNormalEditingFocusedCode's fallback
            StateSelectedFocusedCode, // stateSelectedCode's fallback
            StateNormalCode // stateSelectedFocusedCode's fallback
        };

        constexpr std::array StateNames =
        {
            &StatePointerOver,
            &StatePointerOverEditing,
            &StatePointerOverEditingFocused,
            &StatePointerOverSelected,
            &StatePointerOverSelectedFocused,
            &StateNormal,
            &StateNormalEditing,
            &StateNormalEditingFocused,
            &StateSelected,
            &StateSelectedFocused
        };
    }

    const std::wstring DataGridRow::ElementRoot = L"RowRoot"s;
    const std::wstring DataGridRow::ElementRowHeader = L"RowHeader"s;

    auto DataGridRow::InternalOwningGrid() const
    {
        return OwningGrid().as<DataGrid>();
    }

    DataGridRow::DataGridRow()
    {
        MinHeight(DefaultMinHeight);
        IsTapEnabled(true);
        IsValid(true);
        cells_.CellAdded([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args)
        {
            if(auto const instance = weakPtr.get())
            {
                instance->OnCellAdded(sender, args);
            }
        });
        cells_.CellRemoved([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args)
        {
            if(auto const instance = weakPtr.get())
            {
                instance->OnCellRemoved(sender, args);
            }
        });

        AddHandler(UIElement::TappedEvent(), winrt::box_value<Input::TappedEventHandler>({[](Windows::Foundation::IInspectable const& sender, Input::TappedRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerTapped(e); } }), true /*handledEventsToo*/);
        AddHandler(UIElement::DoubleTappedEvent(), winrt::box_value<Input::DoubleTappedEventHandler>({ [](Windows::Foundation::IInspectable const& sender, Input::DoubleTappedRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_DoubleTapped(e); } }), true /*handledEventsToo*/);

        PointerCanceled(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerCanceled(e); } });
        PointerCaptureLost(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerCaptureLost(e); } });
        PointerPressed(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerPressed(e); } });
        PointerReleased(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerReleased(e); } });
        PointerEntered(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerEntered(e); } });
        PointerExited(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerExited(e); } });
        PointerMoved(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridRow>()->DataGridRow_PointerMoved(e); } });

        RegisterPropertyChangedCallback(Controls::Control::ForegroundProperty(), [weakPtr = get_weak()](DependencyObject const& sender, DependencyProperty const& dp)
        {
            if(auto const instance = weakPtr.get())
            {
                instance->OnDependencyPropertyChanged(sender, dp);
            }
        });

        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGridRow"));
    }

    DependencyProperty DataGridRow::DetailsTemplateProperty()
    {
        static DependencyProperty s_detailsTemplateProperty = DependencyProperty::Register(
            L"DetailsTemplate",
            xaml_typename<DataTemplate>(),
            xaml_typename<MiniDumpExplorer::DataGridRow>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRow>()->OnDetailsTemplatePropertyChanged(e); } });

        return s_detailsTemplateProperty;
    }

    DependencyProperty DataGridRow::DetailsVisibilityProperty()
    {
        static DependencyProperty s_detailsVisibilityProperty = DependencyProperty::Register(
            L"DetailsVisibility",
            xaml_typename<Microsoft::UI::Xaml::Visibility>(),
            xaml_typename<MiniDumpExplorer::DataGridRow>(),
            PropertyMetadata{ box_value(Visibility::Collapsed), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRow>()->OnDetailsVisibilityPropertyChanged(e); } });

        return s_detailsVisibilityProperty;
    }

    DependencyProperty DataGridRow::HeaderProperty()
    {
        static DependencyProperty s_headerProperty = DependencyProperty::Register(
                   L"Header",
                       xaml_typename<Windows::Foundation::IInspectable>(),
                       xaml_typename<MiniDumpExplorer::DataGridRow>(),
                   PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRow>()->OnHeaderPropertyChanged(e); } });
        return s_headerProperty;
    }

    DependencyProperty DataGridRow::HeaderStyleProperty()
    {
        static DependencyProperty s_headerStyleProperty = DependencyProperty::Register(
            L"HeaderStyle",
            xaml_typename<Microsoft::UI::Xaml::Style>(),
            xaml_typename<MiniDumpExplorer::DataGridRow>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRow>()->OnHeaderStylePropertyChanged(e); } });

        return s_headerStyleProperty;
    }

    DependencyProperty DataGridRow::IsValidProperty()
    {
        static DependencyProperty s_isValidProperty = DependencyProperty::Register(
                   L"IsValid",
                       xaml_typename<bool>(),
                       xaml_typename<MiniDumpExplorer::DataGridRow>(),
                   PropertyMetadata{ box_value(true), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridRow>()->OnIsValidPropertyChanged(e); } });

        return s_isValidProperty;
    }

    DataTemplate DataGridRow::DetailsTemplate() const
    {
        return GetValue(DetailsTemplateProperty()).as<DataTemplate>();
    }

    void DataGridRow::DetailsTemplate(DataTemplate const& value) const
    {
        SetValue(DetailsTemplateProperty(), value);
    }

    Visibility DataGridRow::DetailsVisibility() const
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(DetailsVisibilityProperty()));
    }

    void DataGridRow::DetailsVisibility(Microsoft::UI::Xaml::Visibility const& value) const
    {
        SetValue(DetailsVisibilityProperty(), box_value(value));
    }

    Windows::Foundation::IInspectable DataGridRow::Header() const
    {
        return GetValue(HeaderProperty());
    }

    void DataGridRow::Header(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(HeaderProperty(), value);
    }

    Style DataGridRow::HeaderStyle() const
    {
        return GetValue(HeaderStyleProperty()).as<Microsoft::UI::Xaml::Style>();
    }

    void DataGridRow::HeaderStyle(Microsoft::UI::Xaml::Style const& value) const
    {
        SetValue(HeaderStyleProperty(), value);
    }

    bool DataGridRow::IsValid() const
    {
        return unbox_value<bool>(GetValue(IsValidProperty()));
    }

    void DataGridRow::IsValid(bool const value) const
    {
        SetValue(IsValidProperty(), box_value(value));
    }

    bool DataGridRow::IsSelected() const
    {
        if (!owningGrid_ || Slot() == -1)
        {
            // The Slot can be -1 if we're about to reuse or recycle this row, but the layout cycle has not
            // passed, so we don't know the outcome yet.  We don't care whether it's selected in this case
            return false;
        }

        assert(Index() != -1);
        return InternalOwningGrid()->GetRowSelection(Slot());
    }

    MiniDumpExplorer::DataGridCell DataGridRow::FillerCell()
    {
        assert(OwningGrid());

        if (!fillerCell_)
        {
            fillerCell_ = MiniDumpExplorer::DataGridCell{};

            Automation::AutomationProperties::SetAccessibilityView(fillerCell_, Automation::Peers::AccessibilityView::Raw);

            fillerCell_.Visibility(Visibility::Collapsed);

            auto const internalFillerCell = fillerCell_.as<DataGridCell>();
            internalFillerCell->OwningRow(*this);
            internalFillerCell->EnsureStyle(nullptr);
            if (!cellsElement_)
            {
                cellsElement_.Children().Append(fillerCell_);
            }
        }

        return fillerCell_;
    }

    void DataGridRow::ComputedForeground(Media::Brush const& value)
    {
        if(computedForeground_ == value) return;

        computedForeground_ = value;

        for (auto const& dataGridCell : cells_.GetCells())
        {
            
            if (auto const element = dataGridCell.Content().try_as<FrameworkElement>();
                element)
            {
                dataGridCell.as<DataGridCell>()->InternalOwningColumn()->RefreshForeground(element, computedForeground_);
            }
        }
    }

    MiniDumpExplorer::DataGridRow DataGridRow::GetRowContainingElement(FrameworkElement const& element)
    {
        // Walk up the tree to find the DataGridRow that contains the element
        DependencyObject parent = element;
        auto row = parent.as<MiniDumpExplorer::DataGridRow>();
        while (parent && !row)
        {
            parent = Media::VisualTreeHelper::GetParent(parent);
            row = parent.as<MiniDumpExplorer::DataGridRow>();
        }

        return row;
    }

    void DataGridRow::IsPointerOver(bool const value)
    {
        interactionInfo_.IsPointerOver = value;
        ApplyState(true /*animate*/);
    }

    double DataGridRow::ActualBottomGridLineHeight() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            bottomGridLine_ && internalOwningGrid && internalOwningGrid->AreRowBottomGridLinesRequired())
        {
            // Unfortunately, _bottomGridLine has no size yet, so we can't get its actual height
            return DataGrid::HorizontalGridLinesThickness;
        }

        return 0;
    }

    double DataGridRow::DetailsContentHeight() const
    {
        if (detailsElement_)
        {
            return detailsElement_.ContentHeight();
        }

        return 0;
    }

    bool DataGridRow::IsEditing() const
    {
        return owningGrid_ && InternalOwningGrid()->EditingRow() == *this;
    }

    bool DataGridRow::IsRecyclable() const
    {
        if(owningGrid_)
        {
            return InternalOwningGrid()->IsRowRecyclable(*this);
        }

        return true;
    }

    double DataGridRow::TargetHeight() const
    {
        if (!std::isnan(Height()))
        {
            return Height();
        }

        UIHelper::EnsureMeasured(*this);
        if (detailsElement_ && appliedDetailsVisibility_ == Visibility::Visible && appliedDetailsTemplate_)
        {
            auto const contentHeight = detailsElement_.ContentHeight();
            assert(!std::isnan(contentHeight));
            assert(!std::isnan(detailsDesiredHeight_));
            return DesiredSize().Height + detailsDesiredHeight_ - contentHeight;
        }

        return DesiredSize().Height;
    }

    Windows::Foundation::Size DataGridRow::ArrangeOverride(Windows::Foundation::Size const& finalSize)
    {
        if (!owningGrid_)
        {
            return base_type::ArrangeOverride(finalSize);
        }

        // If the DataGrid was scrolled horizontally after our last Arrange, we need to make sure
        // the Cells and Details are Arranged again
        auto const internalOwningGrid = InternalOwningGrid();
        if (lastHorizontalOffset_ != internalOwningGrid->HorizontalOffset())
        {
            lastHorizontalOffset_ = internalOwningGrid->HorizontalOffset();
            InvalidateHorizontalArrange();
        }

        auto const size = base_type::ArrangeOverride(finalSize);

        if (checkDetailsContentHeight_)
        {
            checkDetailsContentHeight_ = false;
            EnsureDetailsContentHeight();
        }

        if (rootElement_)
        {
            for (auto const& child : rootElement_.Children())
            {
                if (DataGridFrozenGrid::GetIsFrozen(child))
                {
                    Media::TranslateTransform transform{};

                    // Automatic layout rounding doesn't apply to transforms, so we need to Round this
                    transform.X(std::round(internalOwningGrid->HorizontalOffset()));
                    child.RenderTransform(transform);
                }
            }
        }

        if (bottomGridLine_)
        {
            Media::RectangleGeometry const gridLineClipGeometry{};
            gridLineClipGeometry.Rect(
                {
                    static_cast<float>(internalOwningGrid->HorizontalOffset())
                    , 0
                    , std::max<float>(0, DesiredSize().Width - static_cast<float>(internalOwningGrid->HorizontalOffset()))
                    , bottomGridLine_.DesiredSize().Height
                });
            bottomGridLine_.Clip(gridLineClipGeometry);
        }

        return size;
    }

    Windows::Foundation::Size DataGridRow::MeasureOverride(Windows::Foundation::Size const& availableSize) const
    {
        if (!owningGrid_)
        {
            return base_type::MeasureOverride(availableSize);
        }

        // Allow the DataGrid specific components to adjust themselves based on new values
        if (headerElement_)
        {
            headerElement_.InvalidateMeasure();
        }

        if (cellsElement_)
        {
            cellsElement_.InvalidateMeasure();
        }

        if (detailsElement_)
        {
            detailsElement_.InvalidateMeasure();
        }

        // ReSharper disable once CppTooWideScopeInitStatement
        // ReSharper disable once CppLocalVariableMayBeConst
        bool currentAddItemIsDataContext = false;
        Windows::Foundation::Size desiredSize{};
        try
        {
            desiredSize = base_type::MeasureOverride(availableSize);
        }
        catch (...)  // NOLINT(bugprone-empty-catch)
        {
            // ignore
        }

        auto const internalOwningGrid = InternalOwningGrid();
        desiredSize.Width = std::max<float>(desiredSize.Width, static_cast<float>(internalOwningGrid->CellsWidth()));
        if (std::isnan(Height()) && double_utils::is_zero(MinHeight()) &&
            (Index() == internalOwningGrid->DataConnection().NewItemPlaceholderIndex() ||
            (currentAddItemIsDataContext && internalOwningGrid->DataConnection().IsAddingNew())))
        {
            // In order to avoid auto-sizing the placeholder or new item row to an unusable height, we will
            // measure it at the DataGrid's average RowHeightEstimate if its Height has not been explicitly set.
            desiredSize.Height = std::max<float>(desiredSize.Height, static_cast<float>(internalOwningGrid->RowHeightEstimate()));
        }

        return desiredSize;
    }

    void DataGridRow::OnApplyTemplate()
    {
        rootElement_ = GetTemplateChild(ElementRoot).as<Controls::Panel>();

        if (rootElement_)
        {
            EnsureBackground();
            ApplyState(false /*animate*/);
        }

        if (cellsElement_)
        {
            // If we're applying a new template, we  want to remove the cells from the previous _cellsElement
            cellsElement_.Children().Clear();
        }

        cellsElement_ = GetTemplateChild(ElementCells).as<MiniDumpExplorer::DataGridCellsPresenter>();
        if (cellsElement_)
        {
            cellsElement_.as<DataGridCellsPresenter>()->OwningRow(*this);

            // Cells that were already added before the Template was applied need to
            // be added to the Canvas
            if (Cells().Count() > 0)
            {
                auto const children = cellsElement_.Children();
                for(auto const& cell : Cells().GetCells())
                {
                    children.Append(cell);
                }
            }
        }

        detailsElement_ = GetTemplateChild(ElementDetails).as<MiniDumpExplorer::DataGridDetailsPresenter>();
        if (detailsElement_ && owningGrid_)
        {
            detailsElement_.as<DataGridDetailsPresenter>()->OwningRow(*this);
            if (ActualDetailsVisibility() == Visibility::Visible && ActualDetailsTemplate() && appliedDetailsTemplate_)
            {
                // Apply the DetailsTemplate now that the row template is applied.
                SetDetailsVisibilityInternal(
                    ActualDetailsVisibility(),
                    detailsVisibilityNotificationPending_ /*raiseNotification*/);
                detailsVisibilityNotificationPending_ = false;
            }
        }

        bottomGridLine_ = GetTemplateChild(ElementBottomGridLine).try_as<Shapes::Rectangle>();

        headerElement_ = GetTemplateChild(ElementRowHeader).as<MiniDumpExplorer::DataGridRowHeader>();
        if (headerElement_)
        {
            headerElement_.as<DataGridRowHeader>()->Owner(*this);
            if (Header())
            {
                headerElement_.Content(Header());
            }

            EnsureHeaderStyleAndVisibility(nullptr);
        }

        EnsureGridLines();
        EnsureForeground();
    }

    Automation::Peers::AutomationPeer DataGridRow::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridRowAutomationPeer{*this};
    }

    void DataGridRow::ApplyCellsState(bool const animate) const
    {
        for(auto const& dataGridCell : Cells().GetCells())
        {
            dataGridCell.as<DataGridCell>()->ApplyCellState(animate);
        }
    }

    void DataGridRow::ApplyDetailsTemplate(bool const initializeDetailsPreferredHeight)
    {
        if (detailsElement_ && AreDetailsVisible())
        {
            auto const internalOwningGrid = InternalOwningGrid();
            auto const oldDetailsTemplate = appliedDetailsTemplate_;
            if (auto const actualDetailsTemplate = ActualDetailsTemplate(); actualDetailsTemplate != appliedDetailsTemplate_)
            {
                if (!actualDetailsTemplate)
                {
                    UnloadDetailsTemplate(false /*recycle*/, false /*setDetailsVisibilityToCollapsed*/);
                }
                else
                {
                    if (detailsElement_)
                    {
                        RemoveDetailsContentSizeChangedHandler();
                        if (detailsLoaded_)
                        {
                            internalOwningGrid->OnUnloadingRowDetails(*this, detailsContent_);
                            detailsLoaded_ = false;
                        }
                        detailsElement_.Children().Clear();
                    }

                    detailsContent_ = actualDetailsTemplate.LoadContent().try_as<FrameworkElement>();
                    appliedDetailsTemplate_ = actualDetailsTemplate;

                    if (detailsContent_)
                    {
                        detailsContentSizeChangedEvent_ = detailsContent_.SizeChanged([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, SizeChangedEventArgs const& e)
                        {
                            if(auto const instance = weakPtr.get())
                            {
                                instance->DetailsContent_SizeChanged(sender, e);
                            }
                        });
                        detailsElement_.Children().Append(detailsContent_);
                    }
                }
            }

            if (detailsContent_ && !detailsLoaded_)
            {
                detailsLoaded_ = true;
                detailsContent_.DataContext(DataContext());
                internalOwningGrid->OnLoadingRowDetails(*this, detailsContent_);
            }

            if (initializeDetailsPreferredHeight && std::isnan(detailsDesiredHeight_) &&
                appliedDetailsTemplate_ && detailsElement_.Children().Size() > 0)
            {
                EnsureDetailsDesiredHeight();
            }
            else if (!oldDetailsTemplate)
            {
                detailsDesiredHeight_ = std::nan("");
                EnsureDetailsDesiredHeight();
                detailsElement_.ContentHeight(detailsDesiredHeight_);
            }
        }
    }

    void DataGridRow::ApplyHeaderState(bool const animate) const
    {
        if (headerElement_ && InternalOwningGrid()->AreRowHeadersVisible())
        {
            headerElement_.as<DataGridRowHeader>()->ApplyOwnerState(animate);
        }
    }

    void DataGridRow::ApplyState(bool const animate) const
    {
        if (!rootElement_)
        {
            return;
        }

        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid)
        {
            return;
        }

        if (Visibility() != Visibility::Visible)
        {
            return;
        }

        assert(Index() != InvalidIndex);
        byte idealStateMappingIndex = 0;
        if (IsSelected() || IsEditing())
        {
            idealStateMappingIndex += 8;
        }

        if (IsEditing())
        {
            idealStateMappingIndex += 4;
        }

        if (IsPointerOver())
        {
            idealStateMappingIndex += 2;
        }

        if (internalOwningGrid->ContainsFocus())
        {
            idealStateMappingIndex += 1;
        }

        byte stateCode = IdealStateMapping[idealStateMappingIndex];
        assert(stateCode != StateNullCode);

        hstring storyboardName;
        while (stateCode != StateNullCode)
        {
            if (stateCode == StateNormalCode)
            {
                if (Index() % 2 == 1)
                {
                    storyboardName = StateAlternate;
                }
                else
                {
                    storyboardName = StateNormal;
                }
            }
            else
            {
                storyboardName = *StateNames[stateCode];
            }

            if (VisualStateManager::GoToState(*this, storyboardName, animate))
            {
                break;
            }

            // The state wasn't implemented so fall back to the next one.
            stateCode = FallbackStateMapping[stateCode];
        }

        if (IsValid())
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateValid);
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateInvalid, VisualStates::StateValid);
        }

        ApplyHeaderState(animate);
    }

    void DataGridRow::DetachFromDataGrid(bool const recycle)
    {
        UnloadDetailsTemplate(recycle, true /*setDetailsVisibilityToCollapsed*/);

        if (recycle)
        {
            Recycle();

            if (cellsElement_)
            {
                cellsElement_.as<DataGridCellsPresenter>()->Recycle();
            }

            checkDetailsContentHeight_ = false;

            // Clear out the old Details cache, so it won't be reused for other data
            detailsDesiredHeight_ = std::nan("");
            if (detailsElement_)
            {
                detailsElement_.ClearValue(DataGridDetailsPresenter::ContentHeightProperty());
            }
        }

        Slot(InvalidIndex);
    }

    void DataGridRow::EnsureBackground() const
    {
        // Inherit the DataGrid's RowBackground properties only if this row doesn't explicitly have a background set
        if (!rootElement_ || !OwningGrid())
        {
            return;
        }

        assert(Index() != InvalidIndex);

        Media::Brush newBackground{nullptr};
        if (!Background())
        {
            auto const internalOwningGrid = InternalOwningGrid();
            if (auto const alternatingRowBackground = internalOwningGrid->AlternatingRowBackground();
                Index() % 2 == 0 || !alternatingRowBackground)
            {
                // Use OwningGrid.RowBackground if the index is even or if the OwningGrid.AlternatingRowBackground is null
                if (internalOwningGrid->RowBackground())
                {
                    newBackground = internalOwningGrid->RowBackground();
                }
            }
            else
            {
                // Alternate row
                if (alternatingRowBackground)
                {
                    newBackground = alternatingRowBackground;
                }
            }
        }
        else
        {
            newBackground = Background();
        }

        if (rootElement_.Background() != newBackground)
        {
            rootElement_.Background(newBackground);
        }
    }

    void DataGridRow::EnsureForeground()
    {
        // Inherit the DataGrid's RowForeground properties only if this row doesn't explicitly have a foreground set
        if (OwningGrid())
        {
            assert(Index() != InvalidIndex);
            auto const internalOwningGrid = InternalOwningGrid();
            auto const alternatingRowForeground = internalOwningGrid->AlternatingRowForeground();
            auto const newForeground = Index() % 2 == 0 || !alternatingRowForeground
                ? internalOwningGrid->RowForeground()
                : alternatingRowForeground;

            ComputedForeground(newForeground ? newForeground : Foreground());
        }
        else
        {
            ComputedForeground(Foreground());
        }
    }

    void DataGridRow::EnsureFillerVisibility() const
    {
        if (cellsElement_)
        {
            cellsElement_.as<DataGridCellsPresenter>()->EnsureFillerVisibility();
        }
    }

    void DataGridRow::EnsureGridLines() const
    {
        if(!OwningGrid())
        {
            return;
        }

        auto const internalOwningGrid = InternalOwningGrid();
        if (bottomGridLine_)
        {
            auto const newVisibility = internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::Horizontal || internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::All
                ? Visibility::Visible : Visibility::Collapsed;

            if (newVisibility != bottomGridLine_.Visibility())
            {
                bottomGridLine_.Visibility(newVisibility);
            }

            EnsureHeaderGridLines(newVisibility);

            bottomGridLine_.Fill(internalOwningGrid->HorizontalGridLinesBrush());
        }

        for (auto const& cell : Cells().GetCells())
        {
            cell.as<DataGridCell>()->EnsureGridLine(internalOwningGrid->ColumnsInternal().LastVisibleColumn());
        }
    }

    void DataGridRow::EnsureHeaderStyleAndVisibility(Microsoft::UI::Xaml::Style const& previousStyle) const
    {
        if(!headerElement_ || !OwningGrid())
        {
            return;
        }

        if (auto const internalOwningGrid = InternalOwningGrid();  // NOLINT(bugprone-branch-clone)
            internalOwningGrid->AreRowHeadersVisible())
        {
            headerElement_.as<DataGridRowHeader>()->EnsureStyle(previousStyle);
            headerElement_.Visibility(Visibility::Visible);
        }
        else
        {
            headerElement_.Visibility(Visibility::Collapsed);
        }
    }

    void DataGridRow::EnsureHeaderVisibility() const
    {
        if(!headerElement_ || !OwningGrid())
        {
            return;
        }

        auto const internalOwningGrid = InternalOwningGrid();
        headerElement_.Visibility(internalOwningGrid->AreRowHeadersVisible() ? Visibility::Visible : Visibility::Collapsed);
    }

    void DataGridRow::InvalidateHorizontalArrange() const
    {
        if (cellsElement_)
        {
            cellsElement_.InvalidateArrange();
        }

        if (detailsElement_)
        {
            detailsElement_.InvalidateArrange();
        }
    }

    void DataGridRow::SetDetailsVisibilityInternal(Microsoft::UI::Xaml::Visibility visibility, bool const raiseNotification)
    {
        assert(OwningGrid());
        assert(Index() != InvalidIndex);

        if (appliedDetailsVisibility_ != visibility)
        {
            if (!detailsElement_)
            {
                if (raiseNotification)
                {
                    detailsVisibilityNotificationPending_ = true;
                }

                return;
            }

            appliedDetailsVisibility_ = visibility;
            SuspendedHandlers::SetValueNoCallback(*this, DetailsVisibilityProperty(), box_value(visibility));

            // Applies a new DetailsTemplate only if it has changed either here or at the DataGrid level
            ApplyDetailsTemplate(true /*initializeDetailsPreferredHeight*/);

            // no template to show
            if (!appliedDetailsTemplate_)
            {
                if (detailsElement_.ContentHeight() > 0)
                {
                    detailsElement_.ContentHeight(0);
                }

                return;
            }

            if (AreDetailsVisible())
            {
                // Set the details height directly
                detailsElement_.ContentHeight(detailsDesiredHeight_);
                checkDetailsContentHeight_ = true;
            }
            else
            {
                detailsElement_.ContentHeight(0);
            }

            OnRowDetailsChanged();

            if (raiseNotification)
            {
                InternalOwningGrid()->OnRowDetailsVisibilityChanged(MiniDumpExplorer::DataGridRowDetailsEventArgs{*this, detailsContent_});
            }
        }
    }

    void DataGridRow::OnCellAdded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args) const
    {
        if (cellsElement_)
        {
            cellsElement_.Children().Append(args.Cell());
        }
    }

    void DataGridRow::OnCellRemoved([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEventArgs const& args) const
    {
        if (cellsElement_)
        {
            auto const children = cellsElement_.Children();
            if(uint32_t index{0};
                children.IndexOf(args.Cell(), index))
            {
                children.RemoveAt(index);
            }
        }
    }

    void DataGridRow::DataGridRow_PointerTapped(Input::TappedRoutedEventArgs const& e) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid || internalOwningGrid->HasColumnUserInteraction())
        {
            return;
        }

        if (internalOwningGrid->UpdatedStateOnTapped())
        {
            internalOwningGrid->UpdatedStateOnTapped(false);
        }
        else
        {
            e.Handled(internalOwningGrid->UpdateStateOnTapped(e, InvalidIndex, Slot(), false /*allowEdit*/));
        }
    }

    void DataGridRow::DataGridRow_PointerCanceled(Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridRow::DataGridRow_PointerCaptureLost(Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridRow::DataGridRow_PointerPressed(Input::PointerRoutedEventArgs const& e)
    {
        if (auto const internalOwnerGrid = InternalOwningGrid();
            e.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Touch &&
            internalOwnerGrid &&
            internalOwnerGrid->AllowsManipulation() &&
            interactionInfo_.CapturedPointerId == 0u &&
            CapturePointer(e.Pointer()))
        {
            interactionInfo_.CapturedPointerId = e.Pointer().PointerId();
        }
    }

    void DataGridRow::DataGridRow_PointerReleased(Input::PointerRoutedEventArgs const& e) const
    {
        if (interactionInfo_.CapturedPointerId == e.Pointer().PointerId())
        {
            ReleasePointerCapture(e.Pointer());
        }
    }

    void DataGridRow::DataGridRow_PointerEntered([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridRow::DataGridRow_PointerExited([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(false);
    }

    void DataGridRow::DataGridRow_PointerMoved([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridRow::OnDependencyPropertyChanged([[maybe_unused]] DependencyObject const& sender, DependencyProperty const& dependencyProperty)
    {
        if (dependencyProperty == Controls::Control::ForegroundProperty())
        {
            EnsureForeground();
        }
    }

    void DataGridRow::OnDetailsTemplatePropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        auto const newValue = e.NewValue().try_as<DataTemplate>();
        auto const oldValue = e.OldValue().try_as<DataTemplate>();

        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()) && OwningGrid())
        {
            // We don't always want to apply the new Template because they might have set the same one
            // we inherited from the DataGrid
            if (GetActualDetailsTemplate(newValue) != GetActualDetailsTemplate(oldValue))
            {
                ApplyDetailsTemplate(false /*initializeDetailsPreferredHeight*/);
            }
        }
    }

    void DataGridRow::OnDetailsVisibilityPropertyChanged(DependencyPropertyChangedEventArgs const& e)
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            if (!OwningGrid())
            {
                throw DataGridError::NoOwningGrid(get_class_name(*this));
            }

            if (Index() == -1)
            {
                throw DataGridError::DataGridRow::InvalidRowIndexCannotCompleteOperation();
            }

            auto const newValue = unbox_value<Microsoft::UI::Xaml::Visibility>(e.NewValue());
            InternalOwningGrid()->OnRowDetailsVisibilityPropertyChanged(Index(), newValue);
            SetDetailsVisibilityInternal(
                newValue,
                true /*raiseNotification*/);
        }
    }

    void DataGridRow::OnHeaderPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (headerElement_)
        {
            headerElement_.Content(e.NewValue());
        }
    }

    void DataGridRow::OnHeaderStylePropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (headerElement_)
        {
            headerElement_.as<DataGridRowHeader>()->EnsureStyle(e.OldValue().as<Microsoft::UI::Xaml::Style>());
        }
    }

    void DataGridRow::OnIsValidPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            SuspendedHandlers::SetValueNoCallback(*this, IsValidProperty(), e.OldValue());
            throw DataGridError::DataGrid::UnderlyingPropertyIsReadOnly(L"IsValid"sv);
        }
    }

    void DataGridRow::DetailsContent_SizeChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, SizeChangedEventArgs const& e)
    {
        if (e.NewSize().Height == e.PreviousSize().Height || e.NewSize().Height == detailsDesiredHeight_)
        {
            return;
        }

        // Update the new desired height for RowDetails
        detailsDesiredHeight_ = e.NewSize().Height;

        if (AreDetailsVisible() && appliedDetailsTemplate_)
        {
            detailsElement_.ContentHeight(e.NewSize().Height);

            // Calling this when details are not visible invalidates during layout when we have no work
            // to do.  In certain scenarios, this could cause a layout cycle
            OnRowDetailsChanged();
        }
    }

    void DataGridRow::DataGridRow_DoubleTapped(Input::DoubleTappedRoutedEventArgs const& e) const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid)
        {
            internalOwningGrid->OnRowDoubleClicked(*this, e);
        }
    }

    void DataGridRow::CancelPointer(Input::PointerRoutedEventArgs const& e)
    {
        if (interactionInfo_.CapturedPointerId == e.Pointer().PointerId())
        {
            interactionInfo_.CapturedPointerId = 0;
        }

        IsPointerOver(false);
    }

    void DataGridRow::UpdateIsPointerOver(bool const isPointerOver)
    {
        if (interactionInfo_.CapturedPointerId != 0u)
        {
            return;
        }

        IsPointerOver(isPointerOver);
    }

    DataTemplate DataGridRow::GetActualDetailsTemplate(DataTemplate const& source) const
    {
        return source ? source : InternalOwningGrid()->RowDetailsTemplate();
    }

    Visibility DataGridRow::ActualDetailsVisibility() const
    {
        if (!owningGrid_)
        {
            throw DataGridError::NoOwningGrid(get_class_name(*this));
        }

        if (Index() == InvalidIndex)
        {
            throw DataGridError::DataGridRow::InvalidRowIndexCannotCompleteOperation();
        }

        return InternalOwningGrid()->GetRowDetailsVisibility(Index());
    }

    void DataGridRow::EnsureHeaderGridLines(Microsoft::UI::Xaml::Visibility const visibility) const
    {
        if (headerElement_)
        {
            headerElement_.SeparatorVisibility(visibility);
        }
    }

    void DataGridRow::EnsureDetailsDesiredHeight()
    {
        assert(detailsElement_);
        assert(OwningGrid());

        if (detailsContent_)
        {
            assert(std::find(detailsElement_.Children().begin(), detailsElement_.Children().end(), detailsContent_) != detailsElement_.Children().end());

            detailsContent_.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
            detailsDesiredHeight_ = detailsContent_.DesiredSize().Height;
        }
        else
        {
            detailsDesiredHeight_ = 0;
        }
    }

    void DataGridRow::EnsureDetailsContentHeight()
    {
        if (detailsElement_ &&
            detailsContent_ &&
            std::isnan(detailsContent_.Height()) &&
            AreDetailsVisible() &&
            !std::isnan(detailsDesiredHeight_) &&
            !double_utils::are_close(detailsContent_.ActualHeight(), detailsDesiredHeight_) &&
            Slot() != InvalidIndex)
        {
            detailsDesiredHeight_ = detailsContent_.ActualHeight();
            detailsElement_.ContentHeight(detailsDesiredHeight_);
        }
    }

    void DataGridRow::OnRowDetailsChanged() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid)
        {
            internalOwningGrid->OnRowDetailsChanged();
        }
    }

    void DataGridRow::Recycle()
    {
        interactionInfo_ = {};
        isRecycled_ = true;
    }

    void DataGridRow::RemoveDetailsContentSizeChangedHandler()
    {
        if(detailsContentSizeChangedEvent_)
        {
            detailsContent_.SizeChanged(detailsContentSizeChangedEvent_);
            detailsContentSizeChangedEvent_ = {};
        }
    }

    void DataGridRow::UnloadDetailsTemplate(bool const recycle, bool const setDetailsVisibilityToCollapsed)
    {
        if (detailsElement_)
        {
            if (detailsContent_)
            {
                if (detailsLoaded_)
                {
                    InternalOwningGrid()->OnUnloadingRowDetails(*this, detailsContent_);
                }

                detailsContent_.DataContext(nullptr);
                if (!recycle)
                {
                    RemoveDetailsContentSizeChangedHandler();
                    detailsContent_ = nullptr;
                }
            }

            if (!recycle)
            {
                detailsElement_.Children().Clear();
            }

            detailsElement_.ContentHeight(0);
        }

        if (!recycle)
        {
            appliedDetailsTemplate_ = nullptr;
            SuspendedHandlers::SetValueNoCallback(*this, DetailsTemplateProperty(), nullptr);
        }

        detailsLoaded_ = false;
        appliedDetailsVisibility_ = std::nullopt;

        if (setDetailsVisibilityToCollapsed)
        {
            SuspendedHandlers::SetValueNoCallback(*this, DetailsVisibilityProperty(), box_value(Visibility::Collapsed));
        }
    }

    DataTemplate DataGridRow::ActualDetailsTemplate() const
    {
        assert(static_cast<bool>(owningGrid_));
        auto const currentDetailsTemplate = DetailsTemplate();

        return currentDetailsTemplate ? currentDetailsTemplate : InternalOwningGrid()->RowDetailsTemplate();
    }
}
