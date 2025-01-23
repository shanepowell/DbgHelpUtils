#include "pch.h"
#include "DataGridColumn.h"

#include <winrt/Windows.Graphics.Display.h>

#include "DataGrid.h"
#include "DataGridColumnCollection.h"
#include "DataGridColumnHeader.h"
#include "DataGridFillerColumn.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DataGridCell.h"
#include "DbgHelpUtils/exit_scope.h"
#include "Helpers/ControlHelpers.h"
#include "Utility/OptionalUtility.h"
#include "Utility/ValidationUtil.h"

#include <format>

// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnInternal.h"

#if __has_include("DataGridColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        constexpr bool DefaultCanUserReorder = true;
        constexpr bool DefaultCanUserResize = true;
        constexpr bool DefaultCanUserSort = true;
        constexpr bool DefaultIsReadOnly = false;
    }

    DataGridColumn::DataGridColumn() = default;

    double DataGridColumn::ActualWidth() const
    {
        if (!owningGrid_ || !width_ || std::isnan(width_.DisplayValue()))
        {
            return ActualMinWidth();
        }

        return width_.DisplayValue();
    }

    bool DataGridColumn::CanUserReorder() const
    {
        if (canUserReorderInternal_.has_value())
        {
            return canUserReorderInternal_.value();
        }
        if (owningGrid_)
        {
            return InternalOwningGrid()->CanUserReorderColumns();
        }
        return DefaultCanUserReorder;
    }

    void DataGridColumn::CanUserReorder(bool const value)
    {
        canUserReorderInternal_ = value;
    }

    bool DataGridColumn::CanUserResize() const
    {
        if (canUserResizeInternal_.has_value())
        {
            return canUserResizeInternal_.value();
        }
        if (owningGrid_)
        {
            return InternalOwningGrid()->CanUserResizeColumns();
        }
        return DefaultCanUserResize;
    }

    void DataGridColumn::CanUserResize(bool value)
    {
        canUserResizeInternal_ = value;
    }

    bool DataGridColumn::CanUserSort() const
    {
        if (canUserSortInternal_.has_value())
        {
            return canUserSortInternal_.value();
        }
        return DefaultCanUserSort;
    }

    void DataGridColumn::CanUserSort(bool value)
    {
        canUserSortInternal_ = value;
    }

    Style DataGridColumn::CellStyle() const
    {
        return cellStyle_;
    }

    void DataGridColumn::CellStyle(Style const& value)
    {
        if (cellStyle_ != value)
        {
            auto const previousStyle = cellStyle_;
            cellStyle_ = value;
            if (owningGrid_)
            {
                InternalOwningGrid()->OnColumnCellStyleChanged(*this, previousStyle);
            }
        }
    }

    Data::Binding DataGridColumn::ClipboardContentBinding() const
    {
        return clipboardContentBinding_;
    }

    void DataGridColumn::ClipboardContentBinding(Data::Binding const& value)
    {
        clipboardContentBinding_ = value;
    }

    int32_t DataGridColumn::DisplayIndex() const
    {
        if (owningGrid_ && InternalOwningGrid()->ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
        {
            return displayIndexWithFiller_ - 1;
        }
        return displayIndexWithFiller_;
    }

    void DataGridColumn::DisplayIndex(int32_t value)
    {
        if (value == std::numeric_limits<decltype(value)>::max())
        {
            throw DataGridError::ValueMustBeLessThan(L"value"sv, L"DisplayIndex"sv, std::numeric_limits<decltype(value)>::max());
        }

        if (owningGrid_)
        {
            auto const internalOwningGrid = InternalOwningGrid();
            if (internalOwningGrid->ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
            {
                value++;
            }

            if (displayIndexWithFiller_ != value)
            {
                if (value < 0 || value >= internalOwningGrid->ColumnsItemsInternal().size())
                {
                    throw DataGridError::ValueMustBeBetween(L"value"sv, L"DisplayIndex"sv, 0, true, internalOwningGrid->ColumnsItemsInternal().size(), false);
                }

                // Will throw an error if a visible frozen column is placed inside a non-frozen area or vice versa.
                internalOwningGrid->OnColumnDisplayIndexChanging(*this, value);
                displayIndexWithFiller_ = value;
                dlg_help_utils::scope_exit clearInDisplayIndexAdjustments{ [owningGrid = internalOwningGrid] { owningGrid->InDisplayIndexAdjustments(false); } };
                internalOwningGrid->InDisplayIndexAdjustments(true);
                internalOwningGrid->OnColumnDisplayIndexChanged(*this);
                internalOwningGrid->OnColumnDisplayIndexChanged_PostNotification();
            }
        }
        else
        {
            if (value < -1)
            {
                throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value"sv, L"DisplayIndex"sv, -1);
            }

            displayIndexWithFiller_ = value;
        }
    }

    Style DataGridColumn::DragIndicatorStyle() const
    {
        return dragIndicatorStyle_;
    }

    void DataGridColumn::DragIndicatorStyle(Style const& value)
    {
        dragIndicatorStyle_ = value;
    }

    Style DataGridColumn::HeaderStyle() const
    {
        return headerStyle_;
    }

    void DataGridColumn::HeaderStyle(Style const& value)
    {
        if (headerStyle_ != value)
        {
            auto const previousStyle = headerStyle_;
            headerStyle_ = value;
            if (headerCell_)
            {
                InternalHeaderCell()->EnsureStyle(previousStyle);
            }
        }
    }

    Windows::Foundation::IInspectable DataGridColumn::Header() const
    {
        return header_;
    }

    void DataGridColumn::Header(Windows::Foundation::IInspectable const& value)
    {
        if (header_ != value)
        {
            header_ = value;
            if (headerCell_)
            {
                headerCell_.Content(header_);
            }
        }
    }

    bool DataGridColumn::IsAutoGenerated() const
    {
        return isAutoGenerated_;
    }

    void DataGridColumn::IsAutoGenerated(bool const value)
    {
        isAutoGenerated_ = value;
    }

    bool DataGridColumn::IsFrozen() const
    {
        return isFrozen_;
    }

    void DataGridColumn::IsFrozen(bool const value)
    {
        isFrozen_ = value;
    }

    bool DataGridColumn::IsReadOnly() const
    {
        if(owningGrid_)
        {
            return isReadOnly_.value_or(DefaultIsReadOnly);
        }

        if(isReadOnly_.has_value())
        {
            return isReadOnly_.value() || InternalOwningGrid()->IsReadOnly();
        }

        return InternalOwningGrid()->GetColumnReadOnlyState(*this, DefaultIsReadOnly);;
    }

    void DataGridColumn::IsReadOnly(bool const value)

    {
        if(isReadOnly_ != value)
        {
            if(owningGrid_)
            {
                InternalOwningGrid()->OnColumnReadOnlyStateChanging(*this, value);
            }
            isReadOnly_ = value;
        }
    }

    double DataGridColumn::MaxWidth() const
    {
        if(maxWidth_.has_value())
        {
            return maxWidth_.value();
        }
        return std::numeric_limits<double>::infinity();
    }

    void DataGridColumn::MaxWidth(double const value)
    {
        if(value < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value"sv, L"MaxWidth"sv, 0);
        }

        if (value < ActualMinWidth())
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value"sv, L"MaxWidth"sv, L"MinWidth"sv);
        }

        if (!maxWidth_.has_value() || maxWidth_.value() != value)
        {
            double const oldValue = ActualMaxWidth();
            maxWidth_ = value;

            if (auto const internalOwningGrid = InternalOwningGrid(); internalOwningGrid)
            {
                internalOwningGrid->OnColumnMaxWidthChanged(*this, oldValue);
            }
        }
    }

    double DataGridColumn::MinWidth() const
    {
        if(minWidth_.has_value())
        {
            return minWidth_.value();
        }
        return 0;
    }

    void DataGridColumn::MinWidth(double value)
    {
        if (std::isnan(value))
        {
            throw DataGridError::ValueCannotBeSetToNAN(L"MinWidth"sv);
        }

        if (value < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value"sv, L"MinWidth"sv, 0);
        }

        if (std::isinf(value))
        {
            throw DataGridError::ValueCannotBeSetToInfinity(L"MinWidth"sv);
        }

        if (value > ActualMaxWidth())
        {
            throw DataGridError::ValueMustBeLessThanOrEqualTo(L"value"sv, L"MinWidth"sv, L"MaxWidth"sv);
        }

        if (!minWidth_.has_value() || minWidth_.value() != value)
        {
            double const oldValue = ActualMinWidth();
            minWidth_ = value;
            if (auto const internalOwningGrid = InternalOwningGrid(); internalOwningGrid)
            {
                internalOwningGrid->OnColumnMinWidthChanged(*this, oldValue);
            }
        }
    }

    Windows::Foundation::IReference<DataGridSortDirection> DataGridColumn::SortDirection() const
    {
        if(!sortDirection_.has_value())
        {
            return nullptr;
        }

        return sortDirection_;
    }

    void DataGridColumn::SortDirection(Windows::Foundation::IReference<DataGridSortDirection> const& value)
    {
        if(auto const optionalValue = Utility::to_optional(value);
            sortDirection_ != optionalValue)
        {
            sortDirection_ = optionalValue;
            
            if (headerCell_)
            {
                InternalHeaderCell()->ApplyState(true /*useTransitions*/);
            }
        }
    }

    Windows::Foundation::IInspectable DataGridColumn::Tag() const
    {
        return tag_;
    }

    void DataGridColumn::Tag(Windows::Foundation::IInspectable const& value)
    {
        tag_ = value;
    }

    Visibility DataGridColumn::Visibility() const
    {
        return visibility_;
    }

    void DataGridColumn::Visibility(Microsoft::UI::Xaml::Visibility const value)
    {
        if (visibility_ != value)
        {
            if (owningGrid_)
            {
                InternalOwningGrid()->OnColumnVisibleStateChanging(*this);
            }
            visibility_ = value;

            if (headerCell_)
            {
                headerCell_.Visibility(visibility_);
            }

            if (owningGrid_)
            {
                InternalOwningGrid()->OnColumnVisibleStateChanged(*this);
            }
        }
    }

    Windows::Foundation::IInspectable DataGridColumn::Width() const
    {
        return WidthLength();
    }

    void DataGridColumn::Width(Windows::Foundation::IInspectable const& value)
    {
        if(auto const valueString = value.try_as<hstring>();
            valueString)
        {
            WidthLength(MiniDumpExplorer::DataGridLength::ConvertFromString(valueString.value()));
            return;
        }

        if(auto const valueLength = value.try_as<MiniDumpExplorer::DataGridLength>();
            valueLength)
        {
            WidthLength(valueLength);
            return;
        }

        throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"dataGridRow unsupported width set type {}", static_cast<std::wstring_view>(get_class_name(value)))};
    }

    MiniDumpExplorer::DataGridLength DataGridColumn::WidthLength() const
    {
        if (width_)
        {
            return width_;
        }
        if (owningGrid_)
        {
            return InternalOwningGrid()->ColumnWidth();
        }

        return MiniDumpExplorer::DataGridLength::Auto();
    }

    void DataGridColumn::WidthLength(MiniDumpExplorer::DataGridLength const& value)
    {
        if (!width_ || width_ != value)
        {
            if (!settingWidthInternally_)
            {
                inheritsWidth_ = false;
            }

            if (owningGrid_)
            {
                auto const isDesignMode = Windows::ApplicationModel::DesignMode::DesignModeEnabled();
                if (auto const width = CoerceWidth(value);
                    width.IsStar() != WidthLength().IsStar() || isDesignMode)
                {
                    // If a column has changed either from or to a star value, we want to recalculate all
                    // star column widths.  They are recalculated during Measure based off what the value we set here.
                    SetWidthInternalNoCallback(width);
                    IsInitialDesiredWidthDetermined(false);
                    InternalOwningGrid()->OnColumnWidthChanged(*this);
                }
                else
                {
                    // If a column width's value is simply changing, we resize it (to the right only).
                    Resize(width.Value(), width.UnitType(), width.DesiredValue(), width.DisplayValue(), false);
                }
            }
            else
            {
                SetWidthInternalNoCallback(value);
            }
        }
    }

    bool DataGridColumn::ActualCanUserResize() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid(); !internalOwningGrid || internalOwningGrid->CanUserResizeColumns() == false || IsDataGridFillerColumn())
        {
            return false;
        }

        return canUserResizeInternal_.value_or(true);
    }

    double DataGridColumn::ActualMaxWidth() const
    {
        return maxWidth_.value_or(owningGrid_ ? owningGrid_.MaxColumnWidth() : std::numeric_limits<double>::infinity());
    }

    double DataGridColumn::ActualMinWidth() const
    {
        auto const minWidth = minWidth_.value_or(owningGrid_ ? owningGrid_.MinColumnWidth() : 0.0);
        if (WidthLength().IsStar())
        {
            return std::max<>(DataGrid::MinimumStarColumnWidth, minWidth);
        }

        return minWidth;
    }

    std::vector<std::wstring> const& DataGridColumn::BindingPaths()
    {
        if(!bindingPathsCreated_)
        {
            bindingPaths_ = CreateBindingPaths();
            bindingPathsCreated_ = true;
        }

        return bindingPaths_;
    }

    void DataGridColumn::DisplayIndexWithFiller(int32_t const value)
    {
        assert(value >= -1);
        assert(value < std::numeric_limits<int32_t>::max());
        displayIndexWithFiller_ = value;
    }

    MiniDumpExplorer::DataGridColumnHeader DataGridColumn::HeaderCell()
    {
        if (!headerCell_)
        {
            headerCell_ = CreateHeader();
        }

        return headerCell_;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumn::GetColumnContainingElement(FrameworkElement const& element)
    {
        // Walk up the tree to find the DataGridCell or DataGridColumnHeader that contains the element
        auto parent = element.try_as<DependencyObject>();
        while (parent)
        {
            if (auto const cell = parent.try_as<DataGridCell>(); cell)
            {
                return cell->OwningColumn();
            }

            if (auto const columnHeader = parent.try_as<DataGridColumnHeader>(); columnHeader)
            {
                return columnHeader->OwningColumn();
            }

            parent = Media::VisualTreeHelper::GetParent(parent);
        }

        return nullptr;
    }

    FrameworkElement DataGridColumn::GetCellContentFromRow(MiniDumpExplorer::DataGridRow const& dataGridRow) const
    {
        if (!dataGridRow)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"dataGridRow"s};
        }

        if (!owningGrid_)
        {
            throw DataGridError::NoOwningGrid(get_class_name(*this));
        }

        if (auto const internalDataGridRow = dataGridRow.try_as<DataGridRow>(); internalDataGridRow->OwningGrid() == owningGrid_)
        {
            assert(Index() != InvalidIndex);
            assert(Index() < InternalOwningGrid()->ColumnsItemsInternal().size());

            if (auto const dataGridCell = internalDataGridRow->Cells()[Index()]; dataGridCell)
            {
                return dataGridCell.Content().try_as<FrameworkElement>();
            }
        }

        return nullptr;
    }

    FrameworkElement DataGridColumn::GetCellContent(Windows::Foundation::IInspectable const& dataItem) const
    {
        if (!dataItem)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"dataItem"s};
        }

        if (!owningGrid_)
        {
            throw DataGridError::NoOwningGrid(get_class_name(*this));
        }

        assert(Index() != InvalidIndex);
        assert(Index() < InternalOwningGrid()->ColumnsItemsInternal().size());

        auto const dataGridRow = InternalOwningGrid()->GetRowFromItem(dataItem);
        if (!dataGridRow)
        {
            return {nullptr};
        }

        return GetCellContentFromRow(dataGridRow);
    }

    MiniDumpExplorer::DataGridLength DataGridColumn::CoerceWidth(MiniDumpExplorer::DataGridLength const& width) const
    {
        auto desiredValue = width.DesiredValue();
        if (std::isnan(desiredValue))
        {
            if (auto const internalOwnerGrid = InternalOwningGrid(); width.IsStar() && internalOwnerGrid)
            {
                auto totalStarValues = 0.0;
                auto totalStarDesiredValues = 0.0;
                auto totalNonStarDisplayWidths = 0.0;
                for (auto const& column : internalOwnerGrid->ColumnsInternal().GetDisplayedColumns([this](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->IsVisible() && c != *this && !std::isnan(c.WidthLength().DesiredValue()); }))
                {
                    if (column.WidthLength().IsStar())
                    {
                        totalStarValues += column.WidthLength().Value();
                        totalStarDesiredValues += column.WidthLength().DesiredValue();
                    }
                    else
                    {
                        totalNonStarDisplayWidths += column.ActualWidth();
                    }
                }

                if (totalStarValues == 0)
                {
                    // Compute the new star column's desired value based on the available space if there are no other visible star columns
                    desiredValue = std::max<>(ActualMinWidth(), internalOwnerGrid->CellsWidth() - totalNonStarDisplayWidths);
                }
                else
                {
                    // Otherwise, compute its desired value based on those of other visible star columns
                    desiredValue = totalStarDesiredValues * width.Value() / totalStarValues;
                }
            }
            else if (width.IsAbsolute())
            {
                desiredValue = width.Value();
            }
            else
            {
                desiredValue = ActualMinWidth();
            }
        }

        double displayValue = width.DisplayValue();
        if (std::isnan(displayValue))
        {
            displayValue = desiredValue;
        }

        displayValue = std::max<>(ActualMinWidth(), std::min(ActualMaxWidth(), displayValue));

        return MiniDumpExplorer::DataGridLength{width.Value(), width.UnitType(), desiredValue, displayValue};
    }

    void DataGridColumn::ComputeLayoutRoundedWidth(double const leftEdge)
    {
        if (owningGrid_ && owningGrid_.UseLayoutRounding())
        {
            double scale;
            if (ControlHelpers::IsXamlRootAvailable() && owningGrid_.XamlRoot())
            {
                scale = owningGrid_.XamlRoot().RasterizationScale();
            }
            else
            {
                scale = Windows::Graphics::Display::DisplayInformation::GetForCurrentView().RawPixelsPerViewPixel();
            }

            auto const roundedLeftEdge = std::floor((scale * leftEdge) + 0.5) / scale;
            auto const roundedRightEdge = std::floor((scale * (leftEdge + ActualWidth())) + 0.5) / scale;
            layoutRoundedWidth_ = roundedRightEdge - roundedLeftEdge;
        }
        else
        {
            layoutRoundedWidth_ = ActualWidth();
        }
    }

    MiniDumpExplorer::DataGridColumnHeader DataGridColumn::CreateHeader()
    {
        MiniDumpExplorer::DataGridColumnHeader result{};
        auto const internalResult = result.as<DataGridColumnHeader>();
        internalResult->OwningColumn(*this);
        result.Content(header_);
        internalResult->EnsureStyle(nullptr);

        return result;
    }

    void DataGridColumn::EnsureWidth()
    {
        SetWidthInternalNoCallback(CoerceWidth(WidthLength()));
    }

    FrameworkElement DataGridColumn::GenerateElementInternal(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem)
    {
        return GenerateElement(cell, dataItem);
    }

    Windows::Foundation::IInspectable DataGridColumn::GetCellValue(Windows::Foundation::IInspectable const& item, Data::Binding const& binding) const
    {
        assert(owningGrid_ != nullptr);

        Windows::Foundation::IInspectable content{nullptr};
        if (binding)
        {
            auto const internalOwnerGrid = InternalOwningGrid();
            auto const clipboardContentControl = internalOwnerGrid->ClipboardContentControl();
            clipboardContentControl.DataContext(item);
            clipboardContentControl.SetBinding(Controls::ContentControl::ContentProperty(), binding);
            content = clipboardContentControl.GetValue(Controls::ContentControl::ContentProperty());
        }

        return content;
    }

    std::vector<Utility::BindingInfo> DataGridColumn::GetInputBindings(FrameworkElement const& element, Windows::Foundation::IInspectable const& dataItem)
    {
        if (inputBindingsCreated_)
        {
            return inputBindings_;
        }

        return CreateBindings(element, dataItem, true /*twoWay*/);
    }

    Windows::Foundation::IInspectable DataGridColumn::PrepareCellForEditInternal(FrameworkElement const& editingElement, RoutedEventArgs const& editingEventArgs)
    {
        return PrepareCellForEdit(editingElement, editingEventArgs);
    }

    void DataGridColumn::RemoveEditingElement()
    {
        editingElement_ = nullptr;
        inputBindings_.clear();
        inputBindingsCreated_ = false;
        bindingPaths_.clear();
        bindingPathsCreated_ = false;
    }

    void DataGridColumn::Resize(double const value, DataGridLengthUnitType const unitType, double const desiredValue, double const displayValue, bool const userInitiated)
    {
        assert(owningGrid_ != nullptr);

        double newValue = value;
        double newDesiredValue = desiredValue;
        double newDisplayValue = std::max<>(ActualMinWidth(), std::min<>(ActualMaxWidth(), displayValue));
        DataGridLengthUnitType newUnitType = unitType;

        int starColumnsCount = 0;
        double totalDisplayWidth = 0.0;
        auto const internalOwnerGrid = InternalOwningGrid();
        for(auto column : internalOwnerGrid->ColumnsInternal().GetVisibleColumns())
        {
            column.as<DataGridColumn>()->EnsureWidth();
            totalDisplayWidth += column.ActualWidth();
            starColumnsCount += (column != *this && column.WidthLength().IsStar()) ? 1 : 0;
        }

        bool const hasInfiniteAvailableWidth = !internalOwnerGrid->RowsPresenterAvailableSize().has_value() || std::isinf(internalOwnerGrid->RowsPresenterAvailableSize().value().Width);

        // If we're using star sizing, we can only resize the column as much as the columns to the
        // right will allow (i.e. until they hit their max or min widths).
        if (!hasInfiniteAvailableWidth && (starColumnsCount > 0 || (unitType == DataGridLengthUnitType::Star && WidthLength().IsStar() && userInitiated)))
        {
            auto limitedDisplayValue = WidthLength().DisplayValue();
            auto const availableIncrease = std::max<>(0.0, internalOwnerGrid->CellsWidth() - totalDisplayWidth);
            
            if (auto desiredChange = newDisplayValue - WidthLength().DisplayValue();
                desiredChange > availableIncrease)
            {
                // The desired change is greater than the amount of available space,
                // so we need to decrease the widths of columns to the right to make room.
                desiredChange -= availableIncrease;
                double const actualChange = desiredChange + internalOwnerGrid->DecreaseColumnWidths(DisplayIndex() + 1, -desiredChange, userInitiated);
                limitedDisplayValue += availableIncrease + actualChange;
            }
            else if (desiredChange > 0)
            {
                // The desired change is positive but less than the amount of available space,
                // so there's no need to decrease the widths of columns to the right.
                limitedDisplayValue += desiredChange;
            }
            else
            {
                // The desired change is negative, so we need to increase the widths of columns to the right.
                limitedDisplayValue += desiredChange + internalOwnerGrid->IncreaseColumnWidths(DisplayIndex() + 1, -desiredChange, userInitiated);
            }

            if (ActualCanUserResize() || (WidthLength().IsStar() && !userInitiated))
            {
                newDisplayValue = limitedDisplayValue;
            }
        }

        if (userInitiated)
        {
            newDesiredValue = newDisplayValue;
            if (!WidthLength().IsStar())
            {
                inheritsWidth_ = false;
                newValue = newDisplayValue;
                newUnitType = DataGridLengthUnitType::Pixel;
            }
            else if (starColumnsCount > 0 && !hasInfiniteAvailableWidth)
            {
                // Recalculate star weight of this column based on the new desired value
                inheritsWidth_ = false;
                newValue = (WidthLength().Value() * newDisplayValue) / ActualWidth();
            }
        }

        auto const oldWidth = Width();
        SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength{std::min<>(std::numeric_limits<double>::max(), newValue), newUnitType, newDesiredValue, newDisplayValue});
        if (Width() != oldWidth)
        {
            internalOwnerGrid->OnColumnWidthChanged(*this);
        }
    }

    void DataGridColumn::SetWidthDesiredValue(double const desiredValue)
    {
        auto const width = WidthLength();
        SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength{width.Value(), width.UnitType(), desiredValue, width.DisplayValue()});
    }

    void DataGridColumn::SetWidthDisplayValue(double const displayValue)
    {
        auto const width = WidthLength();
        SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength{width.Value(), width.UnitType(), width.DesiredValue(), displayValue});
    }

    void DataGridColumn::SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength const& width)
    {
        width_ = width;
    }

    void DataGridColumn::SetWidthStarValue(double const value)
    {
        assert(WidthLength().IsStar());

        auto const width = WidthLength();
        inheritsWidth_ = false;
        SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength{value, width.UnitType(), width.DesiredValue(), width.DisplayValue()});
    }

    void DataGridColumn::CancelCellEdit([[maybe_unused]] FrameworkElement const& editingElement, [[maybe_unused]] Windows::Foundation::IInspectable const& uneditedValue)
    {
    }

    [[noreturn]] FrameworkElement DataGridColumn::GenerateEditingElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"GenerateEditingElement not implemented for {}", static_cast<std::wstring_view>(get_class_name(*this)))};
    }

    [[noreturn]] FrameworkElement DataGridColumn::GenerateElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"GenerateElement not implemented for {}", static_cast<std::wstring_view>(get_class_name(*this)))};
    }

    [[noreturn]] Windows::Foundation::IInspectable DataGridColumn::PrepareCellForEdit([[maybe_unused]] FrameworkElement const& editingElement, [[maybe_unused]] RoutedEventArgs const& editingEventArgs)
    {
        throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"PrepareCellForEdit not implemented for {}", static_cast<std::wstring_view>(get_class_name(*this)))};
    }

    void DataGridColumn::RefreshCellContent([[maybe_unused]] FrameworkElement const& element, [[maybe_unused]] Media::Brush const& computedRowForeground, [[maybe_unused]] std::wstring_view const& propertyName)
    {
    }

    void DataGridColumn::RefreshForeground([[maybe_unused]] FrameworkElement const& element, [[maybe_unused]] Media::Brush const& computedRowForeground)
    {
    }

    void DataGridColumn::NotifyPropertyChanged(std::wstring_view const& propertyName) const
    {
        if (owningGrid_)
        {
            InternalOwningGrid()->RefreshColumnElements(*this, propertyName);
        }
    }

    std::vector<std::wstring> DataGridColumn::CreateBindingPaths()
    {
        if (!inputBindingsCreated_ && owningGrid_)
        {
            if (auto const row = InternalOwningGrid()->EditingRow().try_as<DataGridRow>();
                row && row->Cells().Count() > Index())
            {
                // Finds the input bindings if they don't already exist
                GenerateEditingElementInternal(row->Cells()[Index()], row->DataContext());
            }
        }

        if(!inputBindingsCreated_)
        {
            return std::vector<std::wstring>{};
        }

        // Use the editing bindings if they've already been created
        std::vector<std::wstring> bindingPaths;

        assert(owningGrid_ != nullptr);

        // We're going to return the path of every active binding
        for (auto const& binding : inputBindings_)
        {
            if (binding.BindingExpression() && binding.BindingExpression().ParentBinding() && binding.BindingExpression().ParentBinding().Path())
            {
                bindingPaths.emplace_back(binding.BindingExpression().ParentBinding().Path().Path());
            }
        }

        return bindingPaths;
    }

    std::vector<Utility::BindingInfo> DataGridColumn::CreateBindings(FrameworkElement const& element, Windows::Foundation::IInspectable const& dataItem, bool const twoWay)
    {
        return ValidationUtil::GetBindingInfo<MiniDumpExplorer::DataGrid>(element, dataItem, twoWay, false /*useBlockList*/, true /*searchChildren*/);
    }

    FrameworkElement DataGridColumn::GenerateEditingElementInternal(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem)
    {
        if (!editingElement_)
        {
            editingElement_ = GenerateEditingElement(cell, dataItem);
        }

        if (!inputBindingsCreated_ && editingElement_)
        {
            inputBindings_ = CreateBindings(editingElement_, dataItem, true /*twoWay*/);
            inputBindingsCreated_ = true;

            // Setup all the active input bindings to support validation
            for(auto& bindingData : inputBindings_)
            {
                
                if (auto const bindingExpression = bindingData.BindingExpression();
                    bindingExpression)
                {
                    if(auto const parentBinding = bindingExpression.ParentBinding();
                        parentBinding &&
                        parentBinding.UpdateSourceTrigger() != Data::UpdateSourceTrigger::Explicit)
                    {
                        Data::Binding binding{};
                        binding.Converter(parentBinding.Converter());
                        binding.ConverterLanguage(parentBinding.ConverterLanguage());
                        binding.ConverterParameter(parentBinding.ConverterParameter());
                        binding.ElementName(parentBinding.ElementName());
                        binding.FallbackValue(parentBinding.FallbackValue());
                        binding.Mode(parentBinding.Mode());
                        binding.Path(PropertyPath{parentBinding.Path().Path()});
                        binding.RelativeSource(parentBinding.RelativeSource());
                        binding.Source(parentBinding.Source());
                        binding.TargetNullValue(parentBinding.TargetNullValue());
                        binding.UpdateSourceTrigger(Data::UpdateSourceTrigger::Explicit);
                        bindingData.Element().SetBinding(bindingData.BindingTarget(), binding);
                        bindingData.BindingExpression(bindingData.Element().GetBindingExpression(bindingData.BindingTarget()));
                    }
                }
            }
        }

        return editingElement_;
    }
}
