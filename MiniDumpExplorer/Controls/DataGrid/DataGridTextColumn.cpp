#include "pch.h"
#include "DataGridTextColumn.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Text.h>

#include "DataGridCell.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DbgHelpUtils/wide_runtime_error.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellInternal.h"

#if __has_include("DataGridTextColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridTextColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
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
        constexpr double LeftMargin = 12.0;
        constexpr double RightMargin = 12.0;
    }

    DataGridTextColumn::DataGridTextColumn()
    {
        BindingTarget(Controls::TextBox::TextProperty());
    }

    DependencyProperty DataGridTextColumn::FontFamilyProperty()
    {
        static DependencyProperty property = DependencyProperty::Register(
            L"FontFamily",
            xaml_typename<Media::FontFamily>(),
            xaml_typename<MiniDumpExplorer::DataGridTextColumn>(),
            PropertyMetadata{ nullptr, [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridTextColumn>()->OnFontFamilyPropertyChanged(e); } });

        return property;
    }

    Media::FontFamily DataGridTextColumn::FontFamily() const
    {
        return unbox_value<Media::FontFamily>(GetValue(FontFamilyProperty()));
    }

    void DataGridTextColumn::FontFamily(Media::FontFamily const& value) const
    {
        SetValue(FontFamilyProperty(), box_value(value));
    }

    double DataGridTextColumn::FontSize() const
    {
        return fontSize_.value_or(std::nan(""));
    }

    void DataGridTextColumn::FontSize(double value)
    {
        if(fontSize_ == value) return;
        fontSize_ = value;
        NotifyPropertyChanged(FontSizeName);
    }

    Windows::UI::Text::FontStyle DataGridTextColumn::FontStyle() const
    {
        return fontStyle_.value_or(Windows::UI::Text::FontStyle::Normal);
    }

    void DataGridTextColumn::FontStyle(Windows::UI::Text::FontStyle const& value)
    {
        if(fontStyle_ == value) return;
        fontStyle_ = value;
        NotifyPropertyChanged(FontStyleName);
    }

    Windows::UI::Text::FontWeight DataGridTextColumn::FontWeight() const
    {
        return fontWeight_.value_or(Windows::UI::Text::FontWeights::Normal());
    }

    void DataGridTextColumn::FontWeight(Windows::UI::Text::FontWeight const& value)
    {
        if(fontWeight_ == value) return;
        fontWeight_ = value;
        NotifyPropertyChanged(FontWeightName);
    }

    Media::Brush DataGridTextColumn::Foreground() const
    {
        return foreground_;
    }

    void DataGridTextColumn::Foreground(Media::Brush const& value)
    {
        if(foreground_ == value) return;
        foreground_ = value;
        NotifyPropertyChanged(ForegroundName);
    }

    void DataGridTextColumn::CancelCellEdit(FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue)
    {
        if (auto const textBox = editingElement.try_as<Controls::TextBox>();
            textBox)
        {
            textBox.Text(unbox_value<hstring>(uneditedValue));
        }
    }

    FrameworkElement DataGridTextColumn::GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        Controls::TextBox textBox{};
        textBox.VerticalAlignment(VerticalAlignment::Stretch);
        textBox.Background(Media::SolidColorBrush{Microsoft::UI::Colors::Transparent()});

        if (DependencyProperty::UnsetValue() != ReadLocalValue(FontFamilyProperty()))
        {
            textBox.FontFamily(FontFamily());
        }

        if (fontSize_.has_value())
        {
            textBox.FontSize(fontSize_.value());
        }

        if (fontStyle_.has_value())
        {
            textBox.FontStyle(fontStyle_.value());
        }

        if (fontWeight_.has_value())
        {
            textBox.FontWeight(fontWeight_.value());
        }

        auto const internalCell = cell.try_as<DataGridCell>();
        auto const internalRow = internalCell ? internalCell->InternalOwningRow() : nullptr;
        RefreshControlForeground(textBox, internalRow ? internalRow->ComputedForeground() : nullptr);

        if (auto const binding = Binding();
            binding)
        {
            textBox.SetBinding(BindingTarget(), binding);
        }

        return textBox;
    }

    FrameworkElement DataGridTextColumn::GenerateElement(MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        Controls::TextBlock textBlockElement{};
        textBlockElement.Margin(Thickness{LeftMargin, 0.0, RightMargin, 0.0});
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

        if (auto const binding = Binding();
            binding)
        {
            textBlockElement.SetBinding(textBlockElement.TextProperty(), binding);
        }

        return textBlockElement;
    }

    Windows::Foundation::IInspectable DataGridTextColumn::PrepareCellForEdit(FrameworkElement const& editingElement, RoutedEventArgs const& editingEventArgs)
    {
        if (auto const textBox = editingElement.try_as<Controls::TextBox>();
            textBox)
        {
            auto const uneditedText = textBox.Text();
            auto const length = uneditedText.size();
            
            if (auto const keyEventArgs = editingEventArgs.try_as<Input::KeyRoutedEventArgs>();
                keyEventArgs && keyEventArgs.Key() == Windows::System::VirtualKey::F2)
            {
                // Put caret at the end of the text
                textBox.Select(length, length);
            }
            else
            {
                // Select all text
                textBox.Select(0, length);
            }

            return box_value(uneditedText);
        }

        return box_value(hstring{});
    }

    void DataGridTextColumn::RefreshCellContent(FrameworkElement const& element, Media::Brush const& computedRowForeground, std::wstring_view const& propertyName)
    {
        if (!element)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"ArgumentNull: element"s};
        }

        auto const textBox = element.try_as<Controls::TextBox>();
        if (!textBox)
        {
            auto const textBlock = element.try_as<Controls::TextBlock>();
            if (!textBlock)
            {
                throw DataGridError::DataGrid::ValueIsNotAnInstanceOfEitherOr(L"element"sv, L"TextBox"sv, L"TextBlock"sv);
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
            textBox.FontFamily(FontFamily());
        }
        else if (propertyName == FontSizeName)
        {
            SetTextFontSize(textBox, Controls::Control::FontSizeProperty());
        }
        else if (propertyName == FontStyleName)
        {
            textBox.FontStyle(FontStyle());
        }
        else if (propertyName == FontWeightName)
        {
            textBox.FontWeight(FontWeight());
        }
        else if (propertyName == ForegroundName)
        {
            RefreshControlForeground(textBox, computedRowForeground);
        }
        else
        {
            if (auto const fontFamily = FontFamily(); fontFamily)
            {
                textBox.FontFamily(fontFamily);
            }

            SetTextFontSize(textBox, Controls::Control::FontSizeProperty());
            textBox.FontStyle(FontStyle());
            textBox.FontWeight(FontWeight());
            RefreshControlForeground(textBox, computedRowForeground);
        }
    }

    void DataGridTextColumn::RefreshForeground(FrameworkElement const& element, Media::Brush const& computedRowForeground)
    {
        if (auto const textBox = element.try_as<Controls::TextBox>(); textBox)
        {
            RefreshControlForeground(textBox, computedRowForeground);
        }
        else if (auto const textBlock = element.try_as<Controls::TextBlock>(); textBlock)
        {
            RefreshControlForeground(textBlock, computedRowForeground);
        }
    }

    void DataGridTextColumn::SetTextFontSize(DependencyObject const& textElement, DependencyProperty const& fontSizeProperty) const
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

    void DataGridTextColumn::OnFontFamilyPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        NotifyPropertyChanged(FontFamilyName);
    }
}
