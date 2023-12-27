#pragma once
#include "DbgHelpUtils/wide_runtime_error.h"

namespace DataGridError
{
    inline dlg_help_utils::exceptions::wide_runtime_error ValueCannotBeSetToNAN(std::wstring_view const& paramName)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{} cannot be set to double.NAN.", paramName)};
    }

    inline dlg_help_utils::exceptions::wide_runtime_error ValueCannotBeSetToInfinity(std::wstring_view const& paramName)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{} cannot be set to infinity.", paramName)};
    }

    template<typename T>
    dlg_help_utils::exceptions::wide_runtime_error ValueMustBeGreaterThanOrEqualTo(std::wstring_view const& paramName, std::wstring_view const& valueName, T const& object)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0} argument out of value: {1} must be greater than or equal to {2}.", paramName, valueName, object)};
    }

    inline dlg_help_utils::exceptions::wide_runtime_error CannotConvertFrom(std::wstring_view const& paramName)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"DataGridLengthConverter cannot convert from {0}.", paramName)};
    }

    template<typename TLow, typename THigh>
    dlg_help_utils::exceptions::wide_runtime_error ValueMustBeBetween(std::wstring_view const& paramName, std::wstring_view const& valueName, TLow const& lowValue, bool const lowInclusive, THigh const& highValue, bool const highInclusive)
    {
        if (lowInclusive && highInclusive)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be greater than or equal to {2} and less than or equal to {3}.", paramName, valueName, lowValue, highValue)};
        }

        if (lowInclusive && !highInclusive)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be greater than or equal to {2} and less than {3}.", paramName, valueName, lowValue, highValue)};
        }

        if (!lowInclusive && highInclusive)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be greater than {2} and less than or equal to {3}.", paramName, valueName, lowValue, highValue)};
        }

        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be greater than {2} and less than {3}.", paramName, valueName, lowValue, highValue)};
    }

    template<typename T>
    dlg_help_utils::exceptions::wide_runtime_error ValueMustBeLessThan(std::wstring_view const& paramName, std::wstring_view const& valueName, T const& value)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be less than {2}.", paramName, valueName, value)};
    }

    template<typename T>
    dlg_help_utils::exceptions::wide_runtime_error ValueMustBeLessThanOrEqualTo(std::wstring_view const& paramName, std::wstring_view const& valueName, T const& value)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}:{1} must be less than or equal to {2}.", paramName, valueName, value)};
    }

    inline dlg_help_utils::exceptions::wide_runtime_error NoOwningGrid(std::wstring_view const& type)
    {
        return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"There is no instance of DataGrid assigned to this {0}. Operation cannot be completed.", type)};
    }

    namespace DataGrid
    {
        inline dlg_help_utils::exceptions::wide_runtime_error NotifyCollectionChangedActionReplaceNotSupported()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"NotifyCollectionChangedAction::Replace not supported."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error CollectionChangeItemChangedNotSupported()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"CollectionChange::ItemChanged not supported."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error CannotChangeItemsWhenLoadingRows()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Items cannot be added, removed or reset while rows are loading or unloading."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ItemIsNotContainedInTheItemsSource(std::wstring_view const& paramName)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}: The item is not contained in the ItemsSource.", paramName)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error UnderlyingPropertyIsReadOnly(std::wstring_view const& paramName)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0} cannot be set because the underlying property is read only.", paramName)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ValueIsNotAnInstanceOf(std::wstring_view const& paramName, winrt::Windows::UI::Xaml::Interop::TypeName const& type)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0} The value is not an instance of {1}.", paramName, type.Name)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ValueIsNotAnInstanceOfEitherOr(std::wstring_view const& paramName, std::wstring_view const& type1, std::wstring_view const& type2)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0} The value is not an instance of {1} or {2}.", paramName, type1, type2)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error CannotChangeColumnCollectionWhileAdjustingDisplayIndexes()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Column collection cannot be changed while adjusting display indexes."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ValueCannotBeSetToNull(std::wstring_view const& paramName, std::wstring_view const& valueName)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0}: {1} cannot be set to a null value.", paramName, valueName)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ColumnNotInThisDataGrid()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Provided column does not belong to this DataGrid."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ColumnCannotBeCollapsed()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Column cannot be collapsed."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error NoCurrentRow()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"There is no current row. Operation cannot be completed."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ColumnCannotBeReassignedToDifferentDataGrid()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Column already belongs to a DataGrid instance and cannot be reassigned."};
        }
    }

    namespace DataGridComboBoxColumn
    {
        inline dlg_help_utils::exceptions::wide_runtime_error UnsetBinding(std::wstring_view const& header)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Binding for column {0} is null. Ensure that the binding path has been set correctly.", header)};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error UnknownBindingPath(winrt::Microsoft::UI::Xaml::Data::Binding const& binding, std::wstring_view const& type)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Binding path {0} could not be found in type {1}. Ensure that the binding path has been set correctly.", binding.Path().Path(), type)};
        }
    }

    namespace DataGridRow
    {
        inline dlg_help_utils::exceptions::wide_runtime_error InvalidRowIndexCannotCompleteOperation()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Invalid row index. Operation cannot be completed.")};
        }
    }

    namespace DataGridLength
    {
        inline dlg_help_utils::exceptions::wide_runtime_error InvalidUnitType(std::wstring_view const& paramName)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"{0} is not a valid DataGridLengthUnitType.", paramName)};
        }
    }

    namespace DataGridSelectedItemsCollection
    {
        inline dlg_help_utils::exceptions::wide_runtime_error CannotChangeSelectedItemsCollectionInSingleMode()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Can only change SelectedItems collection in Extended selection mode. Use SelectedItem property in Single selection mode."};
        }
    }

    namespace DataGridAutomationPeer
    {
        inline dlg_help_utils::exceptions::wide_runtime_error OperationCannotBePerformed()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"DataGridAutomationPeer: Cannot perform the operation."};
        }

        inline dlg_help_utils::exceptions::wide_runtime_error ElementNotEnabled()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"DataGridAutomationPeer: Element not enabled."};
        }
    }

    namespace DataGridColumnHeader
    {
        inline dlg_help_utils::exceptions::wide_runtime_error ContentDoesNotSupportUIElements()
        {
            return dlg_help_utils::exceptions::wide_runtime_error{L"Content does not support UIElement; use ContentTemplate instead."};
        }
    }

    namespace DataGridTemplateColumn
    {
        inline dlg_help_utils::exceptions::wide_runtime_error MissingTemplateForType(std::wstring_view const& type)
        {
            return dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Missing template. Cannot initialize {0}.", type)};
        }
    }
}
