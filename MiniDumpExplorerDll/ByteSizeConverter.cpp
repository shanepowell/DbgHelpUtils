#include "pch.h"
#include "ByteSizeConverter.h"
#if __has_include("ByteSizeConverter.g.cpp")
#include "ByteSizeConverter.g.cpp"
#endif

#include "../DbgHelpUtils/size_units.h"

#include <sstream>

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        template<typename ... Args>
        hstring ExtractAndConvertToByteSizeString(Windows::Foundation::IInspectable const& value)
        {
            using namespace dlg_help_utils::size_units::base_10;
            std::wstringstream rv;
            auto found_t{false};

            ([&rv, &found_t, &value]([[maybe_unused]]auto arg) mutable
            {
                if(found_t) return;
                using T = decltype(arg);
                auto temp = value.try_as<Windows::Foundation::IReference<T>>();
                if(!temp) return;
                rv << bytes{static_cast<long long>(temp.Value())};
                found_t = true;
            }(Args{}), ...);
            return hstring{std::move(rv).str()};
        }
    }

    Windows::Foundation::IInspectable ByteSizeConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        return box_value(ExtractAndConvertToByteSizeString<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(value));
    }

    Windows::Foundation::IInspectable ByteSizeConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
