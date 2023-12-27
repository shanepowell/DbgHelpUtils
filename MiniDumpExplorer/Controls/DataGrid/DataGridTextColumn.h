#pragma once

#include "DataGridTextColumn.g.h"

#include "DataGridBoundColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCell;

    struct DataGridTextColumn : DataGridTextColumnT<DataGridTextColumn, DataGridBoundColumn>
    {
        DataGridTextColumn();

        static Microsoft::UI::Xaml::DependencyProperty FontFamilyProperty();

        Microsoft::UI::Xaml::Media::FontFamily FontFamily() const;
        void FontFamily(Microsoft::UI::Xaml::Media::FontFamily const& value) const;
        double FontSize() const;
        void FontSize(double value);
        Windows::UI::Text::FontStyle FontStyle() const;
        void FontStyle(Windows::UI::Text::FontStyle const& value);
        Windows::UI::Text::FontWeight FontWeight() const;
        void FontWeight(Windows::UI::Text::FontWeight const& value);
        Microsoft::UI::Xaml::Media::Brush Foreground() const;
        void Foreground(Microsoft::UI::Xaml::Media::Brush const& value);

        void RefreshForeground(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) override;

    protected:
        void CancelCellEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Windows::Foundation::IInspectable PrepareCellForEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs) override;
        void RefreshCellContent(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground, std::wstring_view const& propertyName) override;

    private:

        template<typename T>
        void RefreshControlForeground(T const& control, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) const
        {
            if (!foreground_)
            {
                if (computedRowForeground)
                {
                    control.Foreground(computedRowForeground);
                }
            }
            else
            {
                control.Foreground(foreground_);
            }
        }

        void SetTextFontSize(Microsoft::UI::Xaml::DependencyObject const& textElement, Microsoft::UI::Xaml::DependencyProperty const& fontSizeProperty) const;

        void OnFontFamilyPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;

    private:
        std::optional<double> fontSize_{};
        std::optional<Windows::UI::Text::FontStyle> fontStyle_{};
        std::optional<Windows::UI::Text::FontWeight> fontWeight_{};
        Microsoft::UI::Xaml::Media::Brush foreground_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridTextColumn : DataGridTextColumnT<DataGridTextColumn, implementation::DataGridTextColumn>
    {
    };
}
