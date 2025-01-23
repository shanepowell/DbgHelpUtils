#include "pch.h"
#include "M128AView.xaml.h"

#include "Helpers/GlobalOptions.h"
#include "Helpers/UIHelper.h"

#if __has_include("M128AView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "M128AView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    M128AView::M128AView()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Value"
            },
            {
            }, 
            {
            },
            {
            },
            {
                L"IsInt128View",
                L"IsUInt128View",
                L"IsInt64View",
                L"IsUInt64View",
                L"IsInt32View",
                L"IsUInt32View",
                L"IsInt16View",
                L"IsUInt16View",
                L"IsInt8View",
                L"IsUInt8View",
                L"IsFloat32View",
                L"IsFloat64View"
            },
            {
            })
    {
    }

    void M128AView::InitializeComponent()
    {
        M128AViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    bool M128AView::IsInt128View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Int128;
    }

    bool M128AView::IsUInt128View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt128;
    }

    bool M128AView::IsInt64View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Int64;
    }

    bool M128AView::IsUInt64View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt64;
    }

    bool M128AView::IsInt32View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Int32;
    }

    bool M128AView::IsUInt32View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt32;
    }

    bool M128AView::IsInt16View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Int16;
    }

    bool M128AView::IsUInt16View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt16;
    }

    bool M128AView::IsInt8View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Int8;
    }

    bool M128AView::IsUInt8View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt8;
    }

    bool M128AView::IsFloat32View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Float32;
    }

    bool M128AView::IsFloat64View()
    {
        return GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::Float64;
    }

    void M128AView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardM128AHexNumberMenuMenu(
            int128(),
            uint128(),
            int64_0(),
            int64_1(),
            uint64_0(),
            uint64_1(),
            int32_0(),
            int32_1(),
            int32_2(),
            int32_3(),
            uint32_0(),
            uint32_1(),
            uint32_2(),
            uint32_3(),
            int16_0(),
            int16_1(),
            int16_2(),
            int16_3(),
            int16_4(),
            int16_5(),
            int16_6(),
            int16_7(),
            uint16_0(),
            uint16_1(),
            uint16_2(),
            uint16_3(),
            uint16_4(),
            uint16_5(),
            uint16_6(),
            uint16_7(),
            int8_0(),
            int8_1(),
            int8_2(),
            int8_3(),
            int8_4(),
            int8_5(),
            int8_6(),
            int8_7(),
            int8_8(),
            int8_9(),
            int8_10(),
            int8_11(),
            int8_12(),
            int8_13(),
            int8_14(),
            int8_15(),
            uint8_0(),
            uint8_1(),
            uint8_2(),
            uint8_3(),
            uint8_4(),
            uint8_5(),
            uint8_6(),
            uint8_7(),
            uint8_8(),
            uint8_9(),
            uint8_10(),
            uint8_11(),
            uint8_12(),
            uint8_13(),
            uint8_14(),
            uint8_15()
        );

        UIHelper::CreateStandardM128AFloatingPointMenu(
            float32_0(),
            float32_1(),
            float32_2(),
            float32_3(),
            float64_0(),
            float64_1()
        );
    }
}
