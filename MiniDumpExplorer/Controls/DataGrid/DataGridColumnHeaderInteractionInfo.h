#pragma once

namespace DataGridInternal
{
    class DataGridColumnHeaderInteractionInfo
    {
    public:
        enum class drag_mode
        {
            None = 0,
            PointerPressed = 1,
            Drag = 2,
            Resize = 3,
            Reorder = 4
        };

        winrt::Microsoft::UI::Xaml::Input::Pointer CapturedPointer{nullptr};
        winrt::MiniDumpExplorer::DataGridColumn DragColumn{nullptr};
        drag_mode DragMode{drag_mode::None};
        uint32_t DragPointerId{};
        std::optional<winrt::Windows::Foundation::Point> DragStart{};
        double FrozenColumnsWidth{};
        bool HasUserInteraction() const { return DragMode != drag_mode::None; }
        std::optional<winrt::Windows::Foundation::Point> LastPointerPositionHeaders{};
        winrt::Microsoft::UI::Input::InputCursor OriginalCursor{nullptr};
        double OriginalHorizontalOffset{};
        double OriginalWidth{};
        std::optional<winrt::Windows::Foundation::Point> PressedPointerPositionHeaders{};
        uint32_t ResizePointerId{};
    };
}
