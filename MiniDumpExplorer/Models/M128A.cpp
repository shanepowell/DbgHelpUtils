#include "pch.h"
#include "M128A.h"

#include "DbgHelpUtils/m128a_utils.h"

#if __has_include("M128A.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "M128A.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    M128A::M128A()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"UInt128",
                L"Int128",
                L"Int64",
                L"Int64_0",
                L"Int64_1",
                L"UInt64",
                L"UInt64_0",
                L"UInt64_1",
                L"Int32",
                L"Int32_0",
                L"Int32_1",
                L"Int32_2",
                L"Int32_3",
                L"UInt32",
                L"UInt32_0",
                L"UInt32_1",
                L"UInt32_2",
                L"UInt32_3",
                L"Int16",
                L"Int16_0",
                L"Int16_1",
                L"Int16_2",
                L"Int16_3",
                L"Int16_4",
                L"Int16_5",
                L"Int16_6",
                L"Int16_7",
                L"UInt16",
                L"UInt16_0",
                L"UInt16_1",
                L"UInt16_2",
                L"UInt16_3",
                L"UInt16_4",
                L"UInt16_5",
                L"UInt16_6",
                L"UInt16_7",
                L"Int8",
                L"Int8_0",
                L"Int8_1",
                L"Int8_2",
                L"Int8_3",
                L"Int8_4",
                L"Int8_5",
                L"Int8_6",
                L"Int8_7",
                L"Int8_8",
                L"Int8_9",
                L"Int8_10",
                L"Int8_11",
                L"Int8_12",
                L"Int8_13",
                L"Int8_14",
                L"Int8_15",
                L"UInt8",
                L"UInt8_0",
                L"UInt8_1",
                L"UInt8_2",
                L"UInt8_3",
                L"UInt8_4",
                L"UInt8_5",
                L"UInt8_6",
                L"UInt8_7",
                L"UInt8_8",
                L"UInt8_9",
                L"UInt8_10",
                L"UInt8_11",
                L"UInt8_12",
                L"UInt8_13",
                L"UInt8_14",
                L"UInt8_15"
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
                L"Float32",
                L"Float32_0",
                L"Float32_1",
                L"Float32_2",
                L"Float32_3",
                L"Float64",
                L"Float64_0",
                L"Float64_1"
            })
    {
        Set({});
    }

    void M128A::Set(_M128A const& m128a)
    {
        m128a_ = m128a;

        int8_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_int8s(m128a_))
        {
            int8_.Append(box_value(v));
        }

        uint8_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_uint8s(m128a_))
        {
            uint8_.Append(v);
        }

        int16_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_int16s(m128a_))
        {
            int16_.Append(v);
        }

        uint16_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_uint16s(m128a_))
        {
            uint16_.Append(v);
        }

        int32_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_int32s(m128a_))
        {
            int32_.Append(v);
        }

        uint32_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_uint32s(m128a_))
        {
            uint32_.Append(v);
        }

        int64_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_int64s(m128a_))
        {
            int64_.Append(v);
        }

        uint64_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_uint64s(m128a_))
        {
            uint64_.Append(v);
        }

        float32_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_floats(m128a_))
        {
            float32_.Append(v);
        }

        float64_.Clear();
        for (auto const v : dlg_help_utils::m128a_utils::to_doubles(m128a_))
        {
            float64_.Append(v);
        }
    }
}
