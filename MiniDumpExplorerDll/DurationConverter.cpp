#include "pch.h"
#include "DurationConverter.h"
#if __has_include("DurationConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DurationConverter.g.cpp"
#endif

#include "../DbgHelpUtils/time_units.h"

#include <sstream>

namespace winrt::MiniDumpExplorer::implementation
{
    DurationSourceType DurationConverter::Source() const
    {
        return source_;
    }

    void DurationConverter::Source(DurationSourceType const value)
    {
        source_ = value;
    }

    namespace
    {
        template<typename ... Args>
        hstring ExtractAndConvertToDurationString(Windows::Foundation::IInspectable const& value, DurationSourceType const source)
        {
            using namespace dlg_help_utils::time_units;
            std::wstringstream rv;
            auto found_t{false};

            ([&rv, &found_t, &value, &source]([[maybe_unused]]auto arg) mutable
            {
                if(found_t) return;
                using T = decltype(arg);
                auto temp = value.try_as<Windows::Foundation::IReference<T>>();
                if(!temp) return;
                switch(source)
                {
                case DurationSourceType::Milliseconds:
                    rv << std::chrono::milliseconds(static_cast<long long>(temp.Value()));
                    break;

                case DurationSourceType::Seconds:
                    rv << std::chrono::seconds(static_cast<long long>(temp.Value()));
                    break;

                default:
                    throw hresult_not_implemented();
                }
                found_t = true;
            }(Args{}), ...);
            return hstring{std::move(rv).str()};
        }
    }

    Windows::Foundation::IInspectable DurationConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        return box_value(ExtractAndConvertToDurationString<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(value, source_));
    }

    Windows::Foundation::IInspectable DurationConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
