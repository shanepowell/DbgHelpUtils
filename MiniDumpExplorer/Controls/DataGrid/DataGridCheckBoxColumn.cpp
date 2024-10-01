#include "pch.h"
#include "DataGridCheckBoxColumn.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include "DataGridCell.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DataGrid.h"
#include "DataGridDisplayData.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Utility/RectContains.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellInternal.h"
#include "DataGridColumnInternal.h"

#if __has_include("DataGridCheckBoxColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCheckBoxColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        constexpr auto IsThreeStatePropertyName = L"IsThreeState"sv;
        constexpr double DefaultLeftMargin = 12.0;
    }

    DataGridCheckBoxColumn::DataGridCheckBoxColumn()
    {
        BindingTarget(Controls::Primitives::ToggleButton::IsCheckedProperty());
    }

    void DataGridCheckBoxColumn::IsThreeState(bool const value)
    {
        if(isThreeState_ == value)
        {
            return;
        }

        isThreeState_ = value;
        NotifyPropertyChanged(IsThreeStatePropertyName);
    }

    void DataGridCheckBoxColumn::CancelCellEdit(FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue)
    {
        if (auto const editingCheckBox = editingElement.as<Controls::CheckBox>();
            editingCheckBox)
        {
            editingCheckBox.IsChecked(uneditedValue ? Windows::Foundation::IReference{unbox_value<bool>(uneditedValue)} : Windows::Foundation::IReference<bool>{});
        }
    }

    FrameworkElement DataGridCheckBoxColumn::GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        auto checkBox = Controls::CheckBox{};
        auto const row = cell ? cell.as<DataGridCell>()->OwningRow() : nullptr;
        auto const internalRow = row ? row.as<DataGridRow>() : nullptr;
        ConfigureCheckBox(checkBox, internalRow ? internalRow->ComputedForeground() : nullptr);
        return checkBox;
    }

    FrameworkElement DataGridCheckBoxColumn::GenerateElement(MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        auto const internalCell = cell.try_as<DataGridCell>();
        auto isEnabled{false};
        auto checkBoxElement = Controls::CheckBox{};
        if (EnsureOwningGrid())
        {
            if (internalCell->RowIndex() != -1 && internalCell->ColumnIndex() != -1 &&
                internalCell->OwningRow() &&
                internalCell->InternalOwningRow()->Slot() == InternalOwningGrid()->CurrentSlot() &&
                internalCell->ColumnIndex() == InternalOwningGrid()->CurrentColumnIndex())
            {
                isEnabled = true;
                if (currentCheckBox_)
                {
                    currentCheckBox_.IsEnabled(false);
                }

                currentCheckBox_ = checkBoxElement;
            }
        }

        checkBoxElement.IsEnabled(isEnabled);
        checkBoxElement.IsHitTestVisible(false);
        checkBoxElement.IsTabStop(false);
        ConfigureCheckBox(checkBoxElement, (internalCell && internalCell->OwningRow()) ? internalCell->InternalOwningRow()->ComputedForeground() : nullptr);
        return checkBoxElement;
    }

    Windows::Foundation::IInspectable DataGridCheckBoxColumn::PrepareCellForEdit(FrameworkElement const& editingElement, RoutedEventArgs const& editingEventArgs)
    {
        auto const editingCheckBox = editingElement.try_as<Controls::CheckBox>();
        if (!editingCheckBox)
        {
            return box_value(false);
        }

        auto const uneditedValue = editingCheckBox.IsChecked();

        auto const pointerEventArgs = editingEventArgs.try_as<Input::PointerRoutedEventArgs>();
        auto editValue = false;
        if (pointerEventArgs)
        {
            // Editing was triggered by a mouse click
            auto const position = pointerEventArgs.GetCurrentPoint(editingCheckBox).Position();
            auto const rect = Windows::Foundation::Rect(0, 0, static_cast<float>(editingCheckBox.ActualWidth()), static_cast<float>(editingCheckBox.ActualHeight()));
            editValue = Utility::RectContains(rect, position);
        }
        else if (beganEditWithKeyboard_)
        {
            // Editing began by a user pressing spacebar
            editValue = true;
            beganEditWithKeyboard_ = false;
        }

        if (editValue)
        {
            // User clicked the checkbox itself or pressed space, let's toggle the IsChecked value
            if (editingCheckBox.IsThreeState())
            {
                if(auto const state = editingCheckBox.IsChecked();
                    !state)
                {
                    editingCheckBox.IsChecked(false);
                }
                else
                {
                    switch (state.Value())
                    {
                    case false:
                        editingCheckBox.IsChecked(true);
                        break;
                    case true:
                        editingCheckBox.IsChecked(nullptr);
                        break;
                    }
                }
            }
            else
            {
                editingCheckBox.IsChecked(!editingCheckBox.IsChecked().Value());
            }
        }

        return uneditedValue;
    }

    void DataGridCheckBoxColumn::RefreshCellContent(FrameworkElement const& element, Media::Brush const& computedRowForeground, std::wstring_view const& propertyName)
    {
        if (!element)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"ArgumentNull: element"s};
        }

        auto const checkBox = element.try_as<Controls::CheckBox>();
        if (!checkBox)
        {
            throw DataGridError::DataGrid::ValueIsNotAnInstanceOf(L"element"sv, xaml_typename<Controls::CheckBox>());
        }

        if (propertyName == IsThreeStatePropertyName)
        {
            checkBox.IsThreeState(IsThreeState());
        }
        else
        {
            checkBox.IsThreeState(IsThreeState());
            checkBox.Foreground(computedRowForeground);
        }
    }

    void DataGridCheckBoxColumn::RefreshForeground(FrameworkElement const& element, Media::Brush const& computedRowForeground)
    {
        if (auto const checkBox = element.try_as<Controls::CheckBox>();
            checkBox)
        {
            checkBox.Foreground(computedRowForeground);
        }
    }

    void DataGridCheckBoxColumn::Columns_CollectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Windows::Foundation::Collections::IVectorChangedEventArgs const& e)
    {
        if (!OwningGrid() && owningGrid_)
        {
            if(columnsCollectionChangedToken_)
            {
                owningGrid_.Columns().VectorChanged(columnsCollectionChangedToken_);
                columnsCollectionChangedToken_ = {};
            }
            if(currentCellChangedToken_)
            {
                owningGrid_.CurrentCellChanged(currentCellChangedToken_);
                currentCellChangedToken_ = {};
            }
            if(keyDownToken_)
            {
                owningGrid_.KeyDown(keyDownToken_);
                keyDownToken_ = {};
            }
            if(loadingRowToken_)
            {
                owningGrid_.LoadingRow(loadingRowToken_);
                loadingRowToken_ = {};
            }
            owningGrid_ = nullptr;
        }
    }

    void DataGridCheckBoxColumn::ConfigureCheckBox(Controls::CheckBox const& checkBox, Media::Brush const& computedRowForeground) const
    {
        checkBox.Margin(Thickness{.Left= DefaultLeftMargin, .Top= 0.0, .Right= 0.0, .Bottom= 0.0});
        checkBox.HorizontalAlignment(HorizontalAlignment::Left);
        checkBox.VerticalAlignment(VerticalAlignment::Center);
        checkBox.IsThreeState(IsThreeState());
        checkBox.UseSystemFocusVisuals(false);
        checkBox.Foreground(computedRowForeground);
        if (Binding())
        {
            checkBox.SetBinding(BindingTarget(), Binding());
        }
    }

    bool DataGridCheckBoxColumn::EnsureOwningGrid()
    {
        if (!OwningGrid())
        {
            return false;
        }

        if (OwningGrid() != owningGrid_)
        {
            owningGrid_ = OwningGrid();
            columnsCollectionChangedToken_ = owningGrid_.Columns().VectorChanged([weak_ptr = get_weak()](Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& e)
            {
                if (auto const instance = weak_ptr.get())
                {
                    instance->Columns_CollectionChanged(sender, e);
                }
            });
            currentCellChangedToken_ = owningGrid_.CurrentCellChanged([weak_ptr = get_weak()](Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e)
            {
                if (auto const instance = weak_ptr.get())
                {
                    instance->OwningGrid_CurrentCellChanged(sender, e);
                }
            });
            keyDownToken_ = owningGrid_.KeyDown([weak_ptr = get_weak()](Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e)
            {
                if (auto const instance = weak_ptr.get())
                {
                    instance->OwningGrid_KeyDown(sender, e);
                }
            });
            loadingRowToken_ = owningGrid_.LoadingRow([weak_ptr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
            {
                if (auto const instance = weak_ptr.get())
                {
                    instance->OwningGrid_LoadingRow(sender, e);
                }
            });
        }

        return true;
    }

    void DataGridCheckBoxColumn::OwningGrid_CurrentCellChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (currentCheckBox_)
        {
            currentCheckBox_.IsEnabled(false);
        }

        if (auto const internalOwningGrid = InternalOwningGrid(); 
            internalOwningGrid && internalOwningGrid->CurrentColumn() == *this &&
            internalOwningGrid->IsSlotVisible(internalOwningGrid->CurrentSlot()))
        {
            
            if (auto const row = internalOwningGrid->DisplayData().GetDisplayedElement(internalOwningGrid->CurrentSlot()).as<MiniDumpExplorer::DataGridRow>();
                row)
            {
                auto const checkBox = GetCellContent(row).try_as<Controls::CheckBox>();
                if (checkBox)
                {
                    checkBox.IsEnabled(true);
                }

                currentCheckBox_ = checkBox;
            }
        }
    }

    void DataGridCheckBoxColumn::OwningGrid_KeyDown([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e)
    {
        if (auto const internalOwningGrid = InternalOwningGrid(); 
            e.Key() == Windows::System::VirtualKey::Space &&
            internalOwningGrid &&
            internalOwningGrid->CurrentColumn() == *this)
        {
            if (auto const row = internalOwningGrid->DisplayData().GetDisplayedElement(internalOwningGrid->CurrentSlot()).try_as<MiniDumpExplorer::DataGridRow>();
                row)
            {
                if (auto const checkBox = GetCellContent(row).try_as<Controls::CheckBox>();
                    checkBox == currentCheckBox_)
                {
                    beganEditWithKeyboard_ = true;
                    internalOwningGrid->BeginEdit();
                    return;
                }
            }
        }

        beganEditWithKeyboard_ = false;
    }

    void DataGridCheckBoxColumn::OwningGrid_LoadingRow([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
    {
        auto const internalOwningGrid = InternalOwningGrid(); 
        if (!internalOwningGrid)
        {
            return;
        }

        auto const checkBox = GetCellContent(e.Row()).try_as<Controls::CheckBox>();
        if (!checkBox)
        {
            return;
        }

        if (internalOwningGrid->CurrentColumnIndex() == Index() && internalOwningGrid->CurrentSlot() == e.Row().as<DataGridRow>()->Slot())
        {
            if (currentCheckBox_)
            {
                currentCheckBox_.IsEnabled(false);
            }

            checkBox.IsEnabled(true);
            currentCheckBox_ = checkBox;
        }
        else
        {
            checkBox.IsEnabled(false);
        }
    }
}
