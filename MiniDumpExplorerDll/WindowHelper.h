#pragma once

class WindowHelper
{
public:
    static HWND GetMainWindowHandle();
    static void SetupFileOpenPicker(winrt::Windows::Storage::Pickers::FileOpenPicker const& picker);
};
