#include "pch.h"
#include "KeyboardHelper.h"

#include <winrt/Microsoft.UI.Input.h>

namespace
{
    [[nodiscard]] bool IsKeyDown(winrt::Windows::System::VirtualKey const key)
    {
        auto const key_flags = static_cast<uint32_t>(winrt::Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(key));
        auto constexpr down_flag = static_cast<uint32_t>(winrt::Windows::UI::Core::CoreVirtualKeyStates::Down);
        return (key_flags & down_flag) == down_flag;
    }
}


KeyboardHelper::MetaKeyState1 KeyboardHelper::GetMetaKeyState1()
{
    return
    {
        .ctrl= IsKeyDown(winrt::Windows::System::VirtualKey::Control),
        .shift= IsKeyDown(winrt::Windows::System::VirtualKey::Shift)
    };
}

KeyboardHelper::MetaKeyState2 KeyboardHelper::GetMetaKeyState2()
{
    return
    {
        .ctrl= IsKeyDown(winrt::Windows::System::VirtualKey::Control),
        .shift= IsKeyDown(winrt::Windows::System::VirtualKey::Shift),
        .alt= IsKeyDown(winrt::Windows::System::VirtualKey::Menu)
    };
}
