#pragma once

#include "DataGridBoundColumn.g.h"

#include "DataGridColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;

    struct DataGridBoundColumn : DataGridBoundColumnT<DataGridBoundColumn, DataGridColumn>
    {
    protected:
        DataGridBoundColumn();

    public:
        Microsoft::UI::Xaml::Data::Binding Binding() const { return binding_; }
        void Binding(Microsoft::UI::Xaml::Data::Binding const& value);
        Microsoft::UI::Xaml::Style EditingElementStyle() const { return editingElementStyle_; }
        void EditingElementStyle(Microsoft::UI::Xaml::Style const& value);
        Microsoft::UI::Xaml::Style ElementStyle() const { return elementStyle_; }
        void ElementStyle(Microsoft::UI::Xaml::Style const& value);

        MiniDumpExplorer::DataGrid const& OwningGrid() const { return owningGrid_; }

        Microsoft::UI::Xaml::Data::Binding ClipboardContentBinding() const override;

        Microsoft::UI::Xaml::DependencyProperty BindingTarget() const { return bindingTarget_; }
        void BindingTarget(Microsoft::UI::Xaml::DependencyProperty const& value);

        void SetHeaderFromBinding();

    protected:
        std::vector<std::wstring> CreateBindingPaths() override;
        std::vector<Utility::BindingInfo> CreateBindings(Microsoft::UI::Xaml::FrameworkElement const& element, Windows::Foundation::IInspectable const& dataItem, bool twoWay) override;

    private:
        Microsoft::UI::Xaml::Data::Binding binding_{ nullptr };
        MiniDumpExplorer::DataGrid owningGrid_{ nullptr };
        Microsoft::UI::Xaml::Style editingElementStyle_{ nullptr };
        Microsoft::UI::Xaml::Style elementStyle_{ nullptr };
        Microsoft::UI::Xaml::DependencyProperty bindingTarget_{ nullptr };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridBoundColumn : DataGridBoundColumnT<DataGridBoundColumn, implementation::DataGridBoundColumn>
    {
    };
}
