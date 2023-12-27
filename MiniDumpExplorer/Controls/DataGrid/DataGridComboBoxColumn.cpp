#include "pch.h"
#include "DataGridComboBoxColumn.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Text.h>

#include "DataGrid.h"
#include "DataGridCell.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/TypeHelper.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellInternal.h"
#include "DataGridColumnInternal.h"
#include "Helpers/UIHelper.h"

#if __has_include("DataGridComboBoxColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridComboBoxColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        const std::wstring FontFamilyName = L"FontFamily"s;
        const std::wstring FontSizeName = L"FontSize"s;
        const std::wstring FontStyleName = L"FontStyle"s;
        const std::wstring FontWeightName = L"FontWeight"s;
        const std::wstring ForegroundName = L"Foreground"s;
        const std::wstring ItemsSourceName = L"ItemsSource"s;
        const std::wstring DisplayMemberPathName = L"DisplayMemberPath"s;
        constexpr double LeftMargin = 12.0;
        constexpr double RightMargin = 12.0;
    }

    DataGridComboBoxColumn::DataGridComboBoxColumn() = default;

    DependencyProperty DataGridComboBoxColumn::ItemsSourceProperty()
    {
        static DependencyProperty s_ItemsSourceProperty = DependencyProperty::Register(
            ItemsSourceName,
            xaml_typename<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>>(),
            xaml_typename<MiniDumpExplorer::DataGridComboBoxColumn>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridComboBoxColumn>()->OnItemsSourcePropertyChanged(e); } });

        return s_ItemsSourceProperty;
    }

    DependencyProperty DataGridComboBoxColumn::DisplayMemberPathProperty()
    {
        static DependencyProperty s_DisplayMemberPathProperty = DependencyProperty::Register(
            DisplayMemberPathName,
            xaml_typename<hstring>(),
            xaml_typename<MiniDumpExplorer::DataGridComboBoxColumn>(),
            PropertyMetadata{nullptr});

        return s_DisplayMemberPathProperty;
    }

    DependencyProperty DataGridComboBoxColumn::FontFamilyProperty()
    {
        static DependencyProperty s_FontFamilyProperty = DependencyProperty::Register(
            FontFamilyName,
            xaml_typename<Media::FontFamily>(),
            xaml_typename<MiniDumpExplorer::DataGridComboBoxColumn>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridComboBoxColumn>()->OnFontFamilyPropertyChanged(e); } });

        return s_FontFamilyProperty;
    }

    Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> DataGridComboBoxColumn::ItemsSource() const
    {
        return GetValue(ItemsSourceProperty()).as<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>>();
    }

    void DataGridComboBoxColumn::ItemsSource(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value) const
    {
        SetValue(ItemsSourceProperty(), value);
    }

    hstring DataGridComboBoxColumn::DisplayMemberPath() const
    {
        return unbox_value<hstring>(GetValue(DisplayMemberPathProperty()));
    }

    void DataGridComboBoxColumn::DisplayMemberPath(hstring const& value) const
    {
        SetValue(DisplayMemberPathProperty(), box_value(value));
    }

    Media::FontFamily DataGridComboBoxColumn::FontFamily() const
    {
        return unbox_value<Media::FontFamily>(GetValue(FontFamilyProperty()));
    }

    void DataGridComboBoxColumn::FontFamily(Media::FontFamily const& value) const
    {
        SetValue(FontFamilyProperty(), box_value(value));
    }

    double DataGridComboBoxColumn::FontSize() const
    {
        return fontSize_.value_or(std::nan(""));
    }

    void DataGridComboBoxColumn::FontSize(double const value)
    {
        if(fontSize_ == value) return;
        fontSize_ = value;
        NotifyPropertyChanged(FontSizeName);
    }

    Windows::UI::Text::FontStyle DataGridComboBoxColumn::FontStyle() const
    {
        return fontStyle_.value_or(Windows::UI::Text::FontStyle::Normal);
    }

    void DataGridComboBoxColumn::FontStyle(Windows::UI::Text::FontStyle const& value)
    {
        if(fontStyle_ == value) return;
        fontStyle_ = value;
        NotifyPropertyChanged(FontStyleName);
    }

    Windows::UI::Text::FontWeight DataGridComboBoxColumn::FontWeight() const
    {
        return fontWeight_.value_or(Windows::UI::Text::FontWeights::Normal());
    }

    void DataGridComboBoxColumn::FontWeight(Windows::UI::Text::FontWeight const& value)
    {
        if(fontWeight_ == value) return;
        fontWeight_ = value;
        NotifyPropertyChanged(FontWeightName);
    }

    Media::Brush DataGridComboBoxColumn::Foreground() const
    {
        return foreground_;
    }

    void DataGridComboBoxColumn::Foreground(Media::Brush const& value)
    {
        if(foreground_ == value) return;
        foreground_ = value;
        NotifyPropertyChanged(ForegroundName);
    }

    void DataGridComboBoxColumn::CancelCellEdit(FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue)
    {
        auto const comboBox = editingElement.try_as<Controls::ComboBox>();

        if (!comboBox)
        {
            return;
        }

        if (auto const dependencyObject = uneditedValue.try_as<DependencyObject>();
            dependencyObject)
        {
            auto const bindingPropertyName = TypeHelper::GetBindingPropertyName(Binding());
            auto const value = TypeHelper::GetNestedDependencyPropertyValue(uneditedValue, bindingPropertyName);

            if (!value)
            {
                comboBox.SelectedItem(uneditedValue);
            }
            else
            {
                auto const items = ItemsSource();
                if(auto const selection = std::ranges::find_if(items, 
                        [&value, &bindingPropertyName](auto const& item)
                        {
                            return TypeHelper::GetNestedDependencyPropertyValue(item, bindingPropertyName) == value;
                        });
                    selection != items.end())
                {
                    comboBox.SelectedItem(*selection);
                }
            }
        }
        else
        {
            comboBox.SelectedItem(nullptr);
        }
    }

    FrameworkElement DataGridComboBoxColumn::GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem)
    {
        EnsureColumnBinding(dataItem);

        auto comboBox = Controls::ComboBox{};
        comboBox.Margin({});
        comboBox.HorizontalAlignment(HorizontalAlignment::Stretch);
        comboBox.VerticalAlignment(VerticalAlignment::Center);

        if (dataItem)
        {
            auto value = TypeHelper::GetNestedDependencyPropertyValue(dataItem, Binding().Path().Path());

            auto const items = ItemsSource();
            if(!DisplayMemberPath().empty())
            {
                auto const bindingPropertyName = TypeHelper::GetBindingPropertyName(Binding());
                if(auto const selection = std::ranges::find_if(items, 
                        [&value, &bindingPropertyName](auto const& item)
                        {
                            return TypeHelper::GetNestedDependencyPropertyValue(item, bindingPropertyName) == value;
                        });
                    selection != items.end())
                {
                    comboBox.SelectedItem(*selection);
                }
                else
                {
                    comboBox.SelectedItem(nullptr);
                }
            }
            else
            {
                if(auto const selection = std::ranges::find_if(items, 
                        [&value](auto const& item)
                        {
                            return item == value;
                        });
                    selection != items.end())
                {
                    comboBox.SelectedItem(*selection);
                }
                else
                {
                    comboBox.SelectedItem(nullptr);
                }
            }
        }

        auto itemsSourceBinding = Data::Binding{};
        itemsSourceBinding.Source(*this);
        itemsSourceBinding.Path(PropertyPath{ ItemsSourceName });

        auto displayMemberPathBinding = Data::Binding{};
        displayMemberPathBinding.Source(*this);
        displayMemberPathBinding.Path(PropertyPath{ DisplayMemberPathName });

        comboBox.SetBinding(Controls::ItemsControl::ItemsSourceProperty(), std::move(itemsSourceBinding));
        comboBox.SetBinding(Controls::ItemsControl::DisplayMemberPathProperty(), std::move(displayMemberPathBinding));

        if (DependencyProperty::UnsetValue() != ReadLocalValue(FontFamilyProperty()))
        {
            comboBox.FontFamily(FontFamily());
        }

        if (fontSize_.has_value())
        {
            comboBox.FontSize(fontSize_.value());
        }

        if (fontStyle_.has_value())
        {
            comboBox.FontStyle(fontStyle_.value());
        }

        if (fontWeight_.has_value())
        {
            comboBox.FontWeight(fontWeight_.value());
        }

        auto const internalCell = cell.try_as<DataGridCell>();
        auto const internalRow = internalCell ? internalCell->InternalOwningRow() : nullptr;
        RefreshControlForeground(comboBox, internalRow ? internalRow->ComputedForeground() : nullptr);

        comboBox.SelectionChanged({ [dataItem = dataItem, weak_ptr = get_weak()]([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Controls::SelectionChangedEventArgs const& e) mutable
            {
                if (auto const instance = weak_ptr.get())
                {
                    if (auto const item = e.AddedItems().Size() > 0 ? e.AddedItems().GetAt(0) : nullptr;
                        item)
                    {
                        if(!instance->DisplayMemberPath().empty())
                        {
                            TypeHelper::SetNestedDependencyPropertyValue(dataItem, TypeHelper::GetNestedDependencyPropertyValue(item, TypeHelper::GetBindingPropertyName(instance->Binding())), instance->Binding().Path().Path());
                        }
                        else
                        {
                            TypeHelper::SetNestedDependencyPropertyValue(dataItem, item, instance->Binding().Path().Path());
                        }
                    }
                }
            } });

        return comboBox;
    }

    FrameworkElement DataGridComboBoxColumn::GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem)
    {
        EnsureColumnBinding(dataItem);

        auto textBlockElement = Controls::TextBlock{};
        textBlockElement.Margin({ LeftMargin, 0.0, RightMargin, 0.0 });
        textBlockElement.VerticalAlignment(VerticalAlignment::Center);

        if (DependencyProperty::UnsetValue() != ReadLocalValue(FontFamilyProperty()))
        {
            textBlockElement.FontFamily(FontFamily());
        }

        if (fontSize_.has_value())
        {
            textBlockElement.FontSize(fontSize_.value());
        }

        if (fontStyle_.has_value())
        {
            textBlockElement.FontStyle(fontStyle_.value());
        }

        if (fontWeight_.has_value())
        {
            textBlockElement.FontWeight(fontWeight_.value());
        }

        auto const internalCell = cell.try_as<DataGridCell>();
        auto const internalRow = internalCell ? internalCell->InternalOwningRow() : nullptr;
        RefreshControlForeground(textBlockElement, internalRow ? internalRow->ComputedForeground() : nullptr);

        if (Binding() && EnsureOwningGrid())
        {
            if (DisplayMemberPath().empty())
            {
                textBlockElement.SetBinding(Controls::TextBlock::TextProperty(), Binding());
            }
            else
            {
                textBlockElement.Text(GetDisplayValue(dataItem));

                HookDataItemPropertyChanged(dataItem);
            }
        }

        return textBlockElement;
    }

    Windows::Foundation::IInspectable DataGridComboBoxColumn::PrepareCellForEdit(FrameworkElement const& editingElement, [[maybe_unused]] RoutedEventArgs const& editingEventArgs)
    {
        if (auto const comboBox = editingElement.try_as<Controls::ComboBox>(); comboBox)
        {
            comboBox.IsDropDownOpen(true);
            return comboBox.SelectedItem();
        }

        return nullptr;
    }

    void DataGridComboBoxColumn::RefreshCellContent(FrameworkElement const& element, Media::Brush const& computedRowForeground, std::wstring_view const& propertyName)
    {
        if (!element)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"ArgumentNull: element"s};
        }

        auto const comboBox = element.try_as<Controls::ComboBox>();
        if (!comboBox)
        {
            auto const textBlock = element.try_as<Controls::TextBlock>();
            if (!textBlock)
            {
                throw DataGridError::DataGrid::ValueIsNotAnInstanceOfEitherOr(L"element"sv, L"ComboBox"sv, L"TextBlock"sv);
            }

            if (propertyName == FontFamilyName)
            {
                textBlock.FontFamily(FontFamily());
            }
            else if (propertyName == FontSizeName)
            {
                SetTextFontSize(textBlock, Controls::TextBlock::FontSizeProperty());
            }
            else if (propertyName == FontStyleName)
            {
                textBlock.FontStyle(FontStyle());
            }
            else if (propertyName == FontWeightName)
            {
                textBlock.FontWeight(FontWeight());
            }
            else if (propertyName == ForegroundName)
            {
                RefreshControlForeground(textBlock, computedRowForeground);
            }
            else if (propertyName == ItemsSourceName)
            {
                InternalOwningGrid()->OnColumnBindingChanged(*this);
            }
            else
            {
                if (auto const fontFamily = FontFamily(); fontFamily)
                {
                    textBlock.FontFamily(fontFamily);
                }

                SetTextFontSize(textBlock, Controls::TextBlock::FontSizeProperty());
                textBlock.FontStyle(FontStyle());
                textBlock.FontWeight(FontWeight());
                RefreshControlForeground(textBlock, computedRowForeground);
            }

            return;
        }

        if (propertyName == FontFamilyName)
        {
            comboBox.FontFamily(FontFamily());
        }
        else if (propertyName == FontSizeName)
        {
            SetTextFontSize(comboBox, Controls::Control::FontSizeProperty());
        }
        else if (propertyName == FontStyleName)
        {
            comboBox.FontStyle(FontStyle());
        }
        else if (propertyName == FontWeightName)
        {
            comboBox.FontWeight(FontWeight());
        }
        else if (propertyName == ForegroundName)
        {
            RefreshControlForeground(comboBox, computedRowForeground);
        }
        else
        {
            if (auto const fontFamily = FontFamily(); fontFamily)
            {
                comboBox.FontFamily(fontFamily);
            }

            SetTextFontSize(comboBox, Controls::Control::FontSizeProperty());
            comboBox.FontStyle(FontStyle());
            comboBox.FontWeight(FontWeight());
            RefreshControlForeground(comboBox, computedRowForeground);
        }
    }

    void DataGridComboBoxColumn::RefreshForeground(FrameworkElement const& element, Media::Brush const& computedRowForeground)
    {
        if (auto const comboBox = element.try_as<Controls::ComboBox>(); comboBox)
        {
            RefreshControlForeground(comboBox, computedRowForeground);
        }
        else if (auto const textBlock = element.try_as<Controls::TextBlock>(); textBlock)
        {
            RefreshControlForeground(textBlock, computedRowForeground);
        }
    }

    void DataGridComboBoxColumn::OnItemsSourcePropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        NotifyPropertyChanged(ItemsSourceName);
    }

    void DataGridComboBoxColumn::OnFontFamilyPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        NotifyPropertyChanged(FontFamilyName);
    }

    void DataGridComboBoxColumn::SetTextFontSize(DependencyObject const& textElement, DependencyProperty const& fontSizeProperty) const
    {
        if (const double newFontSize = FontSize();
            std::isnan(newFontSize))
        {
            textElement.ClearValue(fontSizeProperty);
        }
        else
        {
            textElement.SetValue(fontSizeProperty, box_value(newFontSize));
        }
    }

    void DataGridComboBoxColumn::UnhookOwningGrid()
    {
        if(!owningGrid_)
        {
            return;
        }

        if(owningGridCollectionChangedEvent_)
        {
            owningGrid_.Columns().VectorChanged(owningGridCollectionChangedEvent_);
            owningGridCollectionChangedEvent_ = {};
        }

        if(ownerGridLoadingRow_)
        {
            owningGrid_.LoadingRow(ownerGridLoadingRow_);
            ownerGridLoadingRow_ = {};
        }

        if(ownerGridUnloadingRow_)
        {
            owningGrid_.UnloadingRow(ownerGridUnloadingRow_);
            ownerGridUnloadingRow_ = {};
        }

        if(ownerGridCellEditEnded_)
        {
            owningGrid_.CellEditEnded(ownerGridCellEditEnded_);
            ownerGridCellEditEnded_ = {};
        }

        owningGrid_ = nullptr;
    }

    bool DataGridComboBoxColumn::EnsureOwningGrid()
    {
        if (auto owningGrid = OwningGrid(); owningGrid)
        {
            if (owningGrid != owningGrid_)
            {
                UnhookOwningGrid();

                owningGrid_ = std::move(owningGrid);
                owningGridCollectionChangedEvent_ = owningGrid_.Columns().VectorChanged([weakPtr = get_weak()](Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& e)
                    {
                        if(auto const instance = weakPtr.get();
                            instance)
                        {
                            instance->Columns_CollectionChanged(sender, e);
                        }
                    });
                ownerGridLoadingRow_ = owningGrid_.LoadingRow([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
                    {
                        if(auto const instance = weakPtr.get();
                            instance)
                        {
                            instance->OwningGrid_LoadingRow(sender, e);
                        }
                    });
                ownerGridUnloadingRow_ = owningGrid_.UnloadingRow([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
                    {
                        if(auto const instance = weakPtr.get();
                            instance)
                        {
                            instance->OwningGrid_UnloadingRow(sender, e);
                        }
                    });
                ownerGridCellEditEnded_ = owningGrid_.CellEditEnded([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEditEndedEventArgs const& e)
                    {
                        if(auto const instance = weakPtr.get();
                            instance)
                        {
                            instance->OwningGrid_CellEditEnded(sender, e);
                        }
                    });
            }

            return true;
        }

        return false;
    }

    void DataGridComboBoxColumn::OwningGrid_LoadingRow([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
    {
        auto const row = e.Row();
        HookDataItemPropertyChanged(row.DataContext());
        SetDisplayMemberPathValue(*row.as<DataGridRow>());
    }

    void DataGridComboBoxColumn::OwningGrid_UnloadingRow([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e)
    {
        UnhookDataItemPropertyChanged(e.Row().DataContext());
    }

    void DataGridComboBoxColumn::OwningGrid_CellEditEnded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEditEndedEventArgs const& e) const
    {
        SetDisplayMemberPathValue(e.Row());
    }

    void DataGridComboBoxColumn::Columns_CollectionChanged([[maybe_unused]] Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& sender, [[maybe_unused]] Windows::Foundation::Collections::IVectorChangedEventArgs const& e)
    {
        if (!OwningGrid() && owningGrid_)
        {
            for(auto [dataItem, event] : notifyingDataItems_)
            {
                dataItem.as<Data::INotifyPropertyChanged>().PropertyChanged(event);
            }
            notifyingDataItems_.clear();

            UnhookOwningGrid();
        }
    }

    void DataGridComboBoxColumn::SetDisplayMemberPathValue(MiniDumpExplorer::DataGridRow const& row) const
    {
        if (OwningGrid() && !DisplayMemberPath().empty())
        {
            if (auto const textBlock = GetCellContent(row).try_as<Controls::TextBlock>();
                textBlock)
            {
                textBlock.Text(GetDisplayValue(row.DataContext()));
            }
        }
    }

    hstring DataGridComboBoxColumn::GetDisplayValue(Windows::Foundation::IInspectable const& dataItem) const
    {
        if (auto const binding = Binding(); binding && binding.Path() && dataItem)
        {
            auto value = TypeHelper::GetNestedDependencyPropertyValue(dataItem, binding.Path().Path());

            auto const bindingPropertyName = TypeHelper::GetBindingPropertyName(binding);
            auto const items = ItemsSource();
            if(auto const selection = std::ranges::find_if(items, 
                    [&value, &bindingPropertyName](auto const& item)
                    {
                        return TypeHelper::GetNestedDependencyPropertyValue(item, bindingPropertyName) == value;
                    });
                selection != items.end())
            {
                return UIHelper::ObjectToString(*selection);
            }
        }

        return {};
    }

    void DataGridComboBoxColumn::EnsureColumnBinding(Windows::Foundation::IInspectable const& dataItem) const
    {
        auto const binding = Binding();
        auto const bindingPath = binding ? binding.Path() : PropertyPath{nullptr};
        if (!binding || !bindingPath)
        {
            if (auto const header = Header().try_as<hstring>(); header.has_value() && !header.value().empty())
            {
                throw DataGridError::DataGridComboBoxColumn::UnsetBinding(header.value());
            }

            throw DataGridError::DataGridComboBoxColumn::UnsetBinding(get_class_name(*this));
        }

        if (auto const property = dataItem && binding && bindingPath && !bindingPath.Path().empty() ?  TypeHelper::GetNestedDependencyPropertyValue(dataItem, bindingPath.Path()) : Windows::Foundation::IInspectable{nullptr};
            !property && dataItem)
        {
            throw DataGridError::DataGridComboBoxColumn::UnknownBindingPath(binding, get_class_name(dataItem));
        }
    }

    void DataGridComboBoxColumn::HookDataItemPropertyChanged(Windows::Foundation::IInspectable const& dataItem)
    {
        if (Binding().Mode() == Data::BindingMode::OneTime)
        {
            return;
        }

        auto const notifyingDataItem = dataItem.try_as<Data::INotifyPropertyChanged>();

        if (!notifyingDataItem)
        {
            return;
        }

        if (!notifyingDataItems_.contains(dataItem))
        {
            auto event = notifyingDataItem.PropertyChanged([weakPtr = get_weak()](Windows::Foundation::IInspectable const& sender, Data::PropertyChangedEventArgs const& e)
            {
                if(auto const instance = weakPtr.get(); instance)
                {
                    instance->DataItem_PropertyChanged(sender, e);
                }
            });
            notifyingDataItems_.insert(std::make_pair(dataItem, event));
        }
    }

    void DataGridComboBoxColumn::UnhookDataItemPropertyChanged(Windows::Foundation::IInspectable const& dataItem)
    {
        auto const notifyingDataItem = dataItem.try_as<Data::INotifyPropertyChanged>();

        if (!notifyingDataItem)
        {
            return;
        }

        if (auto const it = notifyingDataItems_.find(dataItem); it != notifyingDataItems_.end())
        {
            notifyingDataItem.PropertyChanged(it->second);
            notifyingDataItems_.erase(it);
        }
    }

    void DataGridComboBoxColumn::DataItem_PropertyChanged(Windows::Foundation::IInspectable const& dataItem, Data::PropertyChangedEventArgs const& e) const
    {
        auto const binding = Binding();
        auto const bindingPath = binding ? binding.Path() : PropertyPath{nullptr};
        if (auto const owningGrid = InternalOwningGrid();
            owningGrid && bindingPath && bindingPath.Path() == e.PropertyName())
        {
            if (auto const dataGridRow = owningGrid->GetRowFromItem(dataItem);
                dataGridRow)
            {
                if(auto const textBlockElement = GetCellContent(dataGridRow).try_as<Controls::TextBlock>(); textBlockElement)
                {
                    textBlockElement.Text(GetDisplayValue(dataItem));
                }
            }
        }
    }
}
