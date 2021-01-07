#pragma once

namespace MiniDumpExplorerApp
{
    struct Wow64FloatingSaveArea : winrt::implements<Wow64FloatingSaveArea, winrt::MiniDumpExplorer::IWow64FloatingSaveArea>
    {
        Wow64FloatingSaveArea(WOW64_FLOATING_SAVE_AREA const& float_save);

        [[nodiscard]] uint32_t ControlWord() const;
        [[nodiscard]] uint32_t StatusWord() const;
        [[nodiscard]] uint32_t TagWord() const;
        [[nodiscard]] uint32_t ErrorOffset() const;
        [[nodiscard]] uint32_t ErrorSelector() const;
        [[nodiscard]] uint32_t DataOffset() const;
        [[nodiscard]] uint32_t DataSelector() const;
        [[nodiscard]] winrt::com_array<uint8_t> RegisterArea() const;
        [[nodiscard]] uint32_t Cr0NpxState() const;

        [[nodiscard]] winrt::hstring RegisterAreaHexDump() const;

    private:
        WOW64_FLOATING_SAVE_AREA const& float_save_;
        winrt::hstring register_area_hex_dump_;
    };
}
