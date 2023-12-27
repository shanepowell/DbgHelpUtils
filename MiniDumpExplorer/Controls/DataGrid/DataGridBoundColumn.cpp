#include "pch.h"
#include "DataGridBoundColumn.h"

#include "DataGrid.h"
#include "DataGridDataConnection.h"
#include "Utility/ValidationUtil.h"

#include "DataGridColumnInternal.h"

#if __has_include("DataGridBoundColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridBoundColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridBoundColumn::DataGridBoundColumn() = default;

    void DataGridBoundColumn::Binding(Data::Binding const& value)
    {
        if(binding_ != value)
        {
            if (OwningGrid() && !OwningGrid().CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/))
            {
                // Edited value couldn't be committed, so we force a CancelEdit
                InternalOwningGrid()->CancelEdit(DataGridEditingUnit::Row, false /*raiseEvents*/);
            }

            binding_ = value;

            if (binding_)
            {
                // Force the TwoWay binding mode if there is a Path present.  TwoWay binding requires a Path.
                if (binding_.Path() && !binding_.Path().Path().empty())
                {
                    binding_.Mode(Data::BindingMode::TwoWay);
                }

                if (!binding_.Converter())
                {
                    binding_.Converter(DataGridValueConverter{});
                }

                binding_.UpdateSourceTrigger(Data::UpdateSourceTrigger::Explicit);

                // Apply the new Binding to existing rows in the DataGrid
                if (OwningGrid())
                {
                    // TODO: We want to clear the Bindings if Binding is set to null
                    // but there's no way to do that right now.  Revisit this if UWP
                    // implements the equivalent of BindingOperations.ClearBinding.
                    InternalOwningGrid()->OnColumnBindingChanged(*this);
                }
            }

            RemoveEditingElement();
        }
    }

    void DataGridBoundColumn::EditingElementStyle(Style const& value)
    {
        if (editingElementStyle_ != value)
        {
            editingElementStyle_ = value;

            // We choose not to update the elements already editing in the Grid here.
            // They will get the EditingElementStyle next time they go into edit mode.
        }
    }

    void DataGridBoundColumn::ElementStyle(Style const& value)
    {
        if (elementStyle_ != value)
        {
            elementStyle_ = value;
            if (auto const internalOwnerGrid = InternalOwningGrid(); internalOwnerGrid)
            {
                internalOwnerGrid->OnColumnElementStyleChanged(*this);
            }
        }
    }

    Data::Binding DataGridBoundColumn::ClipboardContentBinding() const
    {
        if(auto binding = base_type::ClipboardContentBinding();
            binding)
        {
            return binding;
        }

        return Binding();
    }

    void DataGridBoundColumn::BindingTarget(DependencyProperty const& value)
    {
        if (bindingTarget_ != value)
        {
            bindingTarget_ = value;
        }
    }

    void DataGridBoundColumn::SetHeaderFromBinding()
    {
        if (auto const internalOwnerGrid = InternalOwningGrid();
            internalOwnerGrid && !Header() && binding_ && binding_.Path())
        {
            if (auto const header = internalOwnerGrid->DataConnection().GetPropertyDisplayName(Binding().Path().Path());
                !header.empty())
            {
                Header(box_value(header));
            }
        }
    }

    std::vector<std::wstring> DataGridBoundColumn::CreateBindingPaths()
    {
        if (binding_ && binding_.Path())
        {
            return { std::wstring{binding_.Path().Path()} };
        }

        return base_type::CreateBindingPaths();
    }

    std::vector<Utility::BindingInfo> DataGridBoundColumn::CreateBindings(FrameworkElement const& element, Windows::Foundation::IInspectable const& dataItem, bool const twoWay)
    {
        Utility::BindingInfo bindingData{};
        if (twoWay && bindingTarget_)
        {
            bindingData.BindingExpression(element.GetBindingExpression(bindingTarget_));
            if (bindingData.BindingExpression())
            {
                bindingData.BindingTarget(bindingTarget_);
                bindingData.Element(element);
                return { bindingData };
            }
        }

        for (auto const& bindingTarget : ValidationUtil::GetDependencyProperties(element, false))
        {
            bindingData.BindingExpression(element.GetBindingExpression(bindingTarget));
            if (bindingData.BindingExpression()
                && bindingData.BindingExpression().ParentBinding() == binding_)
            {
                BindingTarget(bindingTarget);
                bindingData.BindingTarget(bindingTarget);
                bindingData.Element(element);
                return { bindingData };
            }
        }

        return base_type::CreateBindings(element, dataItem, twoWay);
    }
}
