#include "pch.h"
#include "GlobalOptionItems.h"

#include "Helpers/GlobalOptions.h"
#include "Utility/logger.h"

#if __has_include("GlobalOptionItems.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "GlobalOptionItems.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace std::chrono_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    GlobalOptionItems::GlobalOptionItems()
    {
        GlobalOptions::Options().OnNumberDisplayFormatChanged([ptr = get_weak()](auto const)
            {
                if(auto const self = ptr.get())
                {
                    self->OnNumberDisplayFormatChanged();
                    return true;
                }

                return false;
            });

        GlobalOptions::Options().OnSizeNumberDisplayFormatChanged([ptr = get_weak()](auto const, auto const, auto const)
            {
                if(auto const self = ptr.get())
                {
                    self->OnSizeNumberDisplayFormatChanged();
                    return true;
                }

                return false;
            });
    }

    bool GlobalOptionItems::DisplayHexadecimalNumericFormat()
    {
        return GlobalOptions::Options().NumberDisplayFormat() == NumberDisplayFormat::Hexadecimal;
    }

    void GlobalOptionItems::DisplayHexadecimalNumericFormat(bool const value)
    {
        auto const numericFormat = value ? NumberDisplayFormat::Hexadecimal : NumberDisplayFormat::Decimal;
        GlobalOptions::Options().NumberDisplayFormat(numericFormat);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatAuto()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Auto;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatAuto([[maybe_unused]] bool const value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Auto);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatBytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Bytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatBytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Bytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Kilobytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Kilobytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Megabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Megabytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Gigabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Gigabytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Terabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Terabytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Petabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Petabytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatExabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Exabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatExabytes([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Exabytes);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatRaw()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormat::Raw;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatRaw([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormat::Raw);
    }

    bool GlobalOptionItems::SizeNumberDisplayPrintFormatFull()
    {
        return GlobalOptions::Options().SizeFormat() == dlg_help_utils::size_units::print::full;
    }

    void GlobalOptionItems::SizeNumberDisplayPrintFormatFull([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeFormat(dlg_help_utils::size_units::print::full);
    }

    bool GlobalOptionItems::SizeNumberDisplayPrintFormatCompact()
    {
        return GlobalOptions::Options().SizeFormat() == dlg_help_utils::size_units::print::compact;
    }

    void GlobalOptionItems::SizeNumberDisplayPrintFormatCompact([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeFormat(dlg_help_utils::size_units::print::compact);
    }

    bool GlobalOptionItems::SizeNumberDisplayBase16()
    {
        return GlobalOptions::Options().SizeBase() == SizeDisplayNumberBase::Base16;
    }

    void GlobalOptionItems::SizeNumberDisplayBase16([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeBase(SizeDisplayNumberBase::Base16);
    }

    bool GlobalOptionItems::SizeNumberDisplayBase10()
    {
        return GlobalOptions::Options().SizeBase() == SizeDisplayNumberBase::Base10;
    }

    void GlobalOptionItems::SizeNumberDisplayBase10([[maybe_unused]] bool value)
    {
        GlobalOptions::Options().SizeBase(SizeDisplayNumberBase::Base10);
    }

    event_token GlobalOptionItems::PropertyChanged(Data::PropertyChangedEventHandler const& value)
    {
        return propertyChanged_.add(value);
    }

    void GlobalOptionItems::PropertyChanged(event_token const& token)
    {
        propertyChanged_.remove(token);
    }

    void GlobalOptionItems::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged_(*this, Data::PropertyChangedEventArgs(propertyName));
    }

    void GlobalOptionItems::OnNumberDisplayFormatChanged()
    {
        RaisePropertyChanged(L"DisplayHexadecimalNumericFormat");
    }

    void GlobalOptionItems::OnSizeNumberDisplayFormatChanged()
    {
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatAuto");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatBytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatKilobytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatMegabytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatGigabytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatTerabytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatPetabytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatExabytes");
        RaisePropertyChanged(L"SizeNumberDisplayUnitFormatRaw");

        RaisePropertyChanged(L"SizeNumberDisplayPrintFormatFull");
        RaisePropertyChanged(L"SizeNumberDisplayPrintFormatCompact");

        RaisePropertyChanged(L"SizeNumberDisplayBase16");
        RaisePropertyChanged(L"SizeNumberDisplayBase10");
    }
}
