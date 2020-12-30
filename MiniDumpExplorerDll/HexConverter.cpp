#include "pch.h"
#include <utility>

#include "HexConverter.h"

#include "../DbgHelpUtils/stream_hex_dump.h"

#if __has_include("HexConverter.g.cpp")
#include "HexConverter.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        template<typename ... Args>
        hstring ExtractAndConvertToHex(Windows::Foundation::IInspectable const& value)
        {
            std::wstringstream rv;
            auto found_t{false};

            ([&rv, &found_t, &value]([[maybe_unused]]auto arg) mutable
            {
                if(found_t) return;
                using T = decltype(arg);
                auto temp = value.try_as<Windows::Foundation::IReference<T>>();
                if(!temp) return;
                rv << dlg_help_utils::stream_hex_dump::to_hex<T>(temp.Value());
                found_t = true;
            }(Args{}), ...);
            return hstring{std::move(rv).str()};
        }
    }

    Windows::Foundation::IInspectable HexConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        return box_value(ExtractAndConvertToHex<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(value));
    }

    Windows::Foundation::IInspectable HexConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
