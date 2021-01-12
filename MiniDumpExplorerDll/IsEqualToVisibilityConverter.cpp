#include "pch.h"
#include "IsEqualToVisibilityConverter.h"
#if __has_include("IsEqualToVisibilityConverter.g.cpp")
#include "IsEqualToVisibilityConverter.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    bool IsEqualToVisibilityConverter::IsReversed() const
    {
        return reversed_;
    }

    void IsEqualToVisibilityConverter::IsReversed(bool const value)
    {
        reversed_ = value;
    }

    hstring IsEqualToVisibilityConverter::Value() const
    {
        return test_value_;
    }

    void IsEqualToVisibilityConverter::Value(hstring const value)
    {
        test_value_ = value;
    }

    namespace
    {
        template<typename ... Args>
        bool ExtractAndTestValue(Windows::Foundation::IInspectable const& value, hstring const test_value)
        {
            bool rv{false};
            auto found_t{false};

            ([&rv, &found_t, &value, &test_value]([[maybe_unused]]auto arg) mutable
            {
                if(found_t) return;
                using T = decltype(arg);
                auto temp = value.try_as<Windows::Foundation::IReference<T>>();
                if(!temp) return;
                std::wistringstream ss(std::wstring{test_value});
                T test_value_source;
                ss >> test_value_source;
                if(ss.bad() || ss.fail() || !ss.eof())
                {
                    throw hresult_invalid_argument();
                }
                rv = temp.Value() == test_value_source;
                found_t = true;
            }(Args{}), ...);
            return rv;
        }
    }

    Windows::Foundation::IInspectable IsEqualToVisibilityConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        auto val = ExtractAndTestValue<uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(value, test_value_);
        if(reversed_)
        {
            val = !val;
        }

        if(val)
        {
            return box_value(Windows::UI::Xaml::Visibility::Visible);
        }

        return box_value(Windows::UI::Xaml::Visibility::Collapsed);
    }

    Windows::Foundation::IInspectable IsEqualToVisibilityConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
