#include "pch.h"
#include "XStateDataEnabledFeaturesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Models/XStateDataEnabledFeature.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("XStateDataEnabledFeaturesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "XStateDataEnabledFeaturesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::XStateDataEnabledFeature const& a, MiniDumpExplorer::XStateDataEnabledFeature const& b)>> const ColumnSorters
            {
                {L"FeatureString", Utility::MakeComparer(&MiniDumpExplorer::XStateDataEnabledFeature::FeatureString)},
                {L"Offset", Utility::MakeComparer(&MiniDumpExplorer::XStateDataEnabledFeature::Offset)},
                {L"Size", Utility::MakeComparer(&MiniDumpExplorer::XStateDataEnabledFeature::Size)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::XStateDataEnabledFeature::Feature)}
            };

    }

    XStateDataEnabledFeaturesDataSource::XStateDataEnabledFeaturesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }


    // ReSharper disable CppMemberFunctionMayBeStatic

    bool XStateDataEnabledFeaturesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool XStateDataEnabledFeaturesDataSource::CanSort() const
    {
        return true;
    }

    bool XStateDataEnabledFeaturesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool XStateDataEnabledFeaturesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool XStateDataEnabledFeaturesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool XStateDataEnabledFeaturesDataSource::CanGroup() const
    {
        return false;
    }

    bool XStateDataEnabledFeaturesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool XStateDataEnabledFeaturesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool XStateDataEnabledFeaturesDataSource::HasDataType() const
    {
        return true;
    }

    hstring XStateDataEnabledFeaturesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::XStateDataEnabledFeature>().Name;
    }

    hstring XStateDataEnabledFeaturesDataSource::DataTypeName()
    {
        return L"XStateDataEnabledFeature";
    }

    bool XStateDataEnabledFeaturesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView XStateDataEnabledFeaturesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> XStateDataEnabledFeaturesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool XStateDataEnabledFeaturesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void XStateDataEnabledFeaturesDataSource::ClearDataProperties()
    {
    }

    void XStateDataEnabledFeaturesDataSource::UpdateDataProperties()
    {
    }

    hstring XStateDataEnabledFeaturesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void XStateDataEnabledFeaturesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    void XStateDataEnabledFeaturesDataSource::SetXStateData(XSTATE_CONFIG_FEATURE_MSC_INFO const& xStateData) const
    {
        entries_.Clear();

        for(uint32_t feature = 0; feature< MAXIMUM_XSTATE_FEATURES; ++feature)
        {
            if (auto const mask = 1ui64 << feature; (xStateData.EnabledFeatures & mask) == mask)
            {
                MiniDumpExplorer::XStateDataEnabledFeature data{};
                data.as<XStateDataEnabledFeature>()->Set(feature, xStateData.Features[feature]);
                entries_.Append(data);
            }
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void XStateDataEnabledFeaturesDataSource::SetupDataProperties()
    {
    }
}
