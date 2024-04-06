#include "pch.h"
#include "ExceptionParametersDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/DataGridColumnSorter.h"

#if __has_include("ExceptionParametersDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ExceptionParametersDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::MiniDumpExceptionParameter const& a, MiniDumpExplorer::MiniDumpExceptionParameter const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::MiniDumpExceptionParameter::Index)},
                {L"Parameter", Utility::MakeComparer(&MiniDumpExplorer::MiniDumpExceptionParameter::Parameter)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::MiniDumpExceptionParameter::Index)}
            };

    }

    ExceptionParametersDataSource::ExceptionParametersDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ExceptionParametersDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ExceptionParametersDataSource::CanSort() const
    {
        return true;
    }

    bool ExceptionParametersDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ExceptionParametersDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ExceptionParametersDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ExceptionParametersDataSource::CanGroup() const
    {
        return false;
    }

    bool ExceptionParametersDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ExceptionParametersDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ExceptionParametersDataSource::HasDataType() const
    {
        return true;
    }

    hstring ExceptionParametersDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::MiniDumpExceptionParameter>().Name;
    }

    hstring ExceptionParametersDataSource::DataTypeName()
    {
        return L"MiniDumpExceptionParameter";
    }

    bool ExceptionParametersDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ExceptionParametersDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ExceptionParametersDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ExceptionParametersDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ExceptionParametersDataSource::ClearDataProperties()
    {
    }

    void ExceptionParametersDataSource::UpdateDataProperties()
    {
    }

    hstring ExceptionParametersDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ExceptionParametersDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    void ExceptionParametersDataSource::SetException([[maybe_unused]] MiniDumpExplorer::MiniDumpException const& exception) const
    {
        entries_.Clear();
        for (auto const& parameter : exception.ExceptionInformation())
        {
            entries_.Append(parameter);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ExceptionParametersDataSource::SetupDataProperties()
    {
    }
}
