#include "pch.h"
#include "NumberConverter.h"

#include "../DbgHelpUtils/locale_number_formatting.h"

#if __has_include("NumberConverter.g.cpp")
#include "NumberConverter.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        template<typename ... Args>
        hstring ExtractAndConvertToHex(Windows::Foundation::IInspectable const& value)
        {
            std::wstring rv;

            ([&rv, &value]([[maybe_unused]]auto arg) mutable
            {
                if(!rv.empty()) return;
                using T = decltype(arg);
                auto temp = value.try_as<Windows::Foundation::IReference<T>>();
                if(!temp) return;
                rv = locale_formatting::to_wstring(temp.Value());
            }(Args{}), ...);
            return hstring{rv};
        }
    }

    Windows::Foundation::IInspectable NumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        return box_value(ExtractAndConvertToHex<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(value));
    }

    Windows::Foundation::IInspectable NumberConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
