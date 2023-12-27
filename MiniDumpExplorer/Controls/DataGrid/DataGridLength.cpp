#include "pch.h"
#include "DataGridLength.h"

#include <limits>

#include "DataGridError.h"
#include "DbgHelpUtils/double_utils.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/string_utils.h"

#if __has_include("DataGridLength.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridLength.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        constexpr double DataGridLengthDefaultValue = 1.0;
        constexpr std::wstring_view _starSuffix = L"*"sv;

        double GetValueOrPixelValue(double const lengthValue, DataGridLengthUnitType const type)
        {
            return type == DataGridLengthUnitType::Pixel ? lengthValue : std::numeric_limits<double>::quiet_NaN();
        }
    }

    DataGridLength::DataGridLength(double const lengthValue)
        : DataGridLength(lengthValue, DataGridLengthUnitType::Pixel)
    {
    }

    DataGridLength::DataGridLength(double const lengthValue, DataGridLengthUnitType const type)
        : DataGridLength(lengthValue, type, GetValueOrPixelValue(lengthValue, type), GetValueOrPixelValue(lengthValue, type))
    {
    }

    DataGridLength::DataGridLength(double const lengthValue, DataGridLengthUnitType const type, double const desiredValue, double const displayValue)
    {
        if (std::isnan(lengthValue))
        {
            throw DataGridError::ValueCannotBeSetToNAN(L"value"sv);
        }

        if (std::isinf(lengthValue))
        {
            throw DataGridError::ValueCannotBeSetToInfinity(L"lengthValue"sv);
        }

        if (std::isinf(desiredValue))
        {
            throw DataGridError::ValueCannotBeSetToInfinity(L"desiredValue"sv);
        }

        if (std::isinf(displayValue))
        {
            throw DataGridError::ValueCannotBeSetToInfinity(L"displayValue"sv);
        }

        if (lengthValue < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"value"sv, L"value"sv, 0);
        }

        if (desiredValue < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"desiredValue"sv, L"desiredValue"sv, 0);
        }

        if (displayValue < 0)
        {
            throw DataGridError::ValueMustBeGreaterThanOrEqualTo(L"displayValue"sv, L"displayValue"sv, 0);
        }

        if (type != DataGridLengthUnitType::Auto &&
            type != DataGridLengthUnitType::SizeToCells &&
            type != DataGridLengthUnitType::SizeToHeader &&
            type != DataGridLengthUnitType::Star &&
            type != DataGridLengthUnitType::Pixel)
        {
            throw DataGridError::DataGridLength::InvalidUnitType(L"type"sv);
        }

        desiredValue_ = desiredValue;
        displayValue_ = displayValue;
        unitValue_ = type == DataGridLengthUnitType::Auto ? DataGridLengthDefaultValue : lengthValue;
        unitType_ = type;
    }

    MiniDumpExplorer::DataGridLength DataGridLength::Auto()
    {
        static const MiniDumpExplorer::DataGridLength autoValue{DataGridLengthDefaultValue, DataGridLengthUnitType::Auto};
        return autoValue;
    }

    MiniDumpExplorer::DataGridLength DataGridLength::SizeToCells()
    {
        static const MiniDumpExplorer::DataGridLength sizeToCellsValue{DataGridLengthDefaultValue, DataGridLengthUnitType::SizeToCells};
        return sizeToCellsValue;
    }

    MiniDumpExplorer::DataGridLength DataGridLength::SizeToHeader()
    {
        static const MiniDumpExplorer::DataGridLength sizeToHeaderValue{DataGridLengthDefaultValue, DataGridLengthUnitType::SizeToHeader};
        return sizeToHeaderValue;
    }

    MiniDumpExplorer::DataGridLength DataGridLength::ConvertFromString(hstring const& value)
    {
        if(value.empty())
        {
            throw DataGridError::CannotConvertFrom(L"(empty)"sv);
        }
        const std::wstring originalValue{value};
        auto const stringValue = dlg_help_utils::string_utils::trim(L' ', originalValue);

        if (stringValue.ends_with(_starSuffix))
        {
            auto const stringValueWithoutSuffix = stringValue.substr(0, stringValue.size() - _starSuffix.size());

            double starWeight;
            if (stringValueWithoutSuffix.empty())
            {
                starWeight = 1;
            }
            else
            {
                size_t index{0};
                starWeight = std::stod(std::wstring{stringValueWithoutSuffix}, &index);
            }

            return MiniDumpExplorer::DataGridLength{starWeight, DataGridLengthUnitType::Star};
        }

        if (dlg_help_utils::string_utils::iequals(L"auto"sv, stringValue))
        {
            return MiniDumpExplorer::DataGridLength::Auto();
        }
        if (dlg_help_utils::string_utils::iequals(L"sizetocells"sv, stringValue))
        {
            return MiniDumpExplorer::DataGridLength::SizeToCells();
        }
        if (dlg_help_utils::string_utils::iequals(L"sizetoheader"sv, stringValue))
        {
            return MiniDumpExplorer::DataGridLength::SizeToHeader();
        }

        // Conversion from numeric type
        size_t index{0};
        const double doubleValue = std::stod(std::wstring{stringValue}, &index);
        if (std::isnan(doubleValue))
        {
            return Auto();
        }
        return MiniDumpExplorer::DataGridLength{doubleValue};
    }

    hstring DataGridLength::ConvertToString(MiniDumpExplorer::DataGridLength const& value)
    {
        switch (value.UnitType())
        {
        case DataGridLengthUnitType::Auto:
            return hstring{L"Auto"sv};
        case DataGridLengthUnitType::SizeToCells:
            return hstring{L"SizeToCells"sv};
        case DataGridLengthUnitType::SizeToHeader:
            return hstring{L"SizeToHeader"sv};
        case DataGridLengthUnitType::Star:
            if(double_utils::are_close(1.0, value.Value()))
            {
                return hstring{_starSuffix};
            }
            return hstring{std::to_wstring(value.Value())} + hstring{_starSuffix};
        case DataGridLengthUnitType::Pixel:
            return hstring{std::to_wstring(value.Value())};
        default:
            throw DataGridError::DataGridLength::InvalidUnitType(L"unitType"sv);
        }
    }

    bool DataGridLength::operator==(DataGridLength const& value) const
    {
       return unitType_ == value.unitType_
        && unitValue_ == value.unitValue_
        && desiredValue_ == value.desiredValue_
        && displayValue_ == value.displayValue_
        ;
    }
}
