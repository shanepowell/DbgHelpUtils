#pragma once

#include "DataGridLength.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridLength : DataGridLengthT<DataGridLength>
    {
        DataGridLength(double lengthValue);
        DataGridLength(double lengthValue, DataGridLengthUnitType type);
        DataGridLength(double lengthValue, DataGridLengthUnitType type, double desiredValue, double displayValue);

        static MiniDumpExplorer::DataGridLength Auto();
        static MiniDumpExplorer::DataGridLength SizeToCells();
        static MiniDumpExplorer::DataGridLength SizeToHeader();
        
        double DesiredValue() const { return desiredValue_; }
        double DisplayValue() const { return displayValue_; }
        bool IsAbsolute() const { return unitType_ == DataGridLengthUnitType::Pixel; }
        bool IsAuto() const { return unitType_ == DataGridLengthUnitType::Auto; }
        bool IsSizeToCells() const { return unitType_ == DataGridLengthUnitType::SizeToCells; }
        bool IsSizeToHeader() const { return unitType_ == DataGridLengthUnitType::SizeToHeader; }
        bool IsStar() const { return unitType_ == DataGridLengthUnitType::Star; }
        DataGridLengthUnitType UnitType() const { return unitType_; }
        double Value() const { return unitValue_; }

        static MiniDumpExplorer::DataGridLength ConvertFromString(hstring const& value);
        static hstring ConvertToString(MiniDumpExplorer::DataGridLength const& value);

        bool operator==(DataGridLength const& value) const;

    private:
        double unitValue_;
        double desiredValue_;
        double displayValue_;
        DataGridLengthUnitType unitType_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridLength : DataGridLengthT<DataGridLength, implementation::DataGridLength>
    {
    };
}
