#include "pch.h"
#include "DataGridTemplateColumn.h"

#include "DataGridError.h"

#if __has_include("DataGridTemplateColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridTemplateColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridTemplateColumn::DataGridTemplateColumn() = default;

    void DataGridTemplateColumn::CellEditingTemplate(DataTemplate const& value)
    {
        if (cellEditingTemplate_ != value)
        {
            RemoveEditingElement();
            cellEditingTemplate_ = value;
        }
    }

    void DataGridTemplateColumn::CellTemplate(DataTemplate const& value)
    {
        if (cellTemplate_ != value)
        {
            if (!cellTemplate_)
            {
                RemoveEditingElement();
            }

            cellTemplate_ = value;
        }
    }

    bool DataGridTemplateColumn::HasDistinctTemplates() const
    {
        return cellTemplate_ != cellEditingTemplate_;
    }

    void DataGridTemplateColumn::CancelCellEdit([[maybe_unused]] FrameworkElement const& editingElement, [[maybe_unused]] Windows::Foundation::IInspectable const& uneditedValue)
    {
        [[maybe_unused]] auto result = GenerateEditingElement(nullptr, nullptr);
    }

    FrameworkElement DataGridTemplateColumn::GenerateEditingElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        if (cellEditingTemplate_)
        {
            return cellEditingTemplate_.LoadContent().try_as<FrameworkElement>();
        }

        if (cellTemplate_)
        {
            return cellTemplate_.LoadContent().try_as<FrameworkElement>();
        }

        if (Windows::ApplicationModel::DesignMode::DesignModeEnabled())
        {
            return nullptr;
        }
        else
        {
            throw DataGridError::DataGridTemplateColumn::MissingTemplateForType(L"DataGridTemplateColumn");
        }
    }

    FrameworkElement DataGridTemplateColumn::GenerateElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        if (cellTemplate_)
        {
            return cellTemplate_.LoadContent().try_as<FrameworkElement>();
        }

        if (cellEditingTemplate_)
        {
            return cellEditingTemplate_.LoadContent().try_as<FrameworkElement>();
        }

        if (Windows::ApplicationModel::DesignMode::DesignModeEnabled())
        {
            return nullptr;
        }
        else
        {
            throw DataGridError::DataGridTemplateColumn::MissingTemplateForType(L"DataGridTemplateColumn");
        }
    }

    Windows::Foundation::IInspectable DataGridTemplateColumn::PrepareCellForEdit([[maybe_unused]] FrameworkElement const& editingElement, [[maybe_unused]] RoutedEventArgs const& editingEventArgs)
    {
        return nullptr;
    }
}
