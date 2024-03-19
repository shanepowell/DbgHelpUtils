#pragma once

#include "GlobalOptionItems.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct GlobalOptionItems : GlobalOptionItemsT<GlobalOptionItems>
    {
        GlobalOptionItems();

        static bool DisplayHexadecimalNumericFormat();
        static void DisplayHexadecimalNumericFormat(bool value);

        static bool SizeNumberDisplayUnitFormatAuto();
        static void SizeNumberDisplayUnitFormatAuto(bool value);
        static bool SizeNumberDisplayUnitFormatBytes();
        static void SizeNumberDisplayUnitFormatBytes(bool value);
        static bool SizeNumberDisplayUnitFormatKilobytes();
        static void SizeNumberDisplayUnitFormatKilobytes(bool value);
        static bool SizeNumberDisplayUnitFormatMegabytes();
        static void SizeNumberDisplayUnitFormatMegabytes(bool value);
        static bool SizeNumberDisplayUnitFormatGigabytes();
        static void SizeNumberDisplayUnitFormatGigabytes(bool value);
        static bool SizeNumberDisplayUnitFormatTerabytes();
        static void SizeNumberDisplayUnitFormatTerabytes(bool value);
        static bool SizeNumberDisplayUnitFormatPetabytes();
        static void SizeNumberDisplayUnitFormatPetabytes(bool value);
        static bool SizeNumberDisplayUnitFormatExabytes();
        static void SizeNumberDisplayUnitFormatExabytes(bool value);
        static bool SizeNumberDisplayUnitFormatRaw();
        static void SizeNumberDisplayUnitFormatRaw(bool value);

        static bool SizeNumberDisplayPrintFormatFull();
        static void SizeNumberDisplayPrintFormatFull(bool value);
        static bool SizeNumberDisplayPrintFormatCompact();
        static void SizeNumberDisplayPrintFormatCompact(bool value);

        static bool SizeNumberDisplayBase16();
        static void SizeNumberDisplayBase16(bool value);
        static bool SizeNumberDisplayBase10();
        static void SizeNumberDisplayBase10(bool value);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(event_token const& token);

    private:
        void RaisePropertyChanged(hstring const& propertyName);
        void OnNumberDisplayFormatChanged();
        void OnSizeNumberDisplayFormatChanged();

    private:
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct GlobalOptionItems : GlobalOptionItemsT<GlobalOptionItems, implementation::GlobalOptionItems>
    {
    };
}
