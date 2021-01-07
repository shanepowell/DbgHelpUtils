#include "pch.h"
#include "Wow64FloatingSaveArea.h"

#include "../DbgHelpUtils/hex_dump.h"

namespace MiniDumpExplorerApp
{
    Wow64FloatingSaveArea::Wow64FloatingSaveArea(WOW64_FLOATING_SAVE_AREA const& float_save)
        : float_save_{float_save}
    {
        std::wstringstream ss;
        dlg_help_utils::hex_dump::hex_dump(ss, float_save_.RegisterArea, sizeof(float_save_.RegisterArea), 0);
        register_area_hex_dump_ = std::move(ss).str();
    }

    uint32_t Wow64FloatingSaveArea::ControlWord() const
    {
        return float_save_.ControlWord;
    }

    uint32_t Wow64FloatingSaveArea::StatusWord() const
    {
        return float_save_.StatusWord;
    }

    uint32_t Wow64FloatingSaveArea::TagWord() const
    {
        return float_save_.TagWord;
    }

    uint32_t Wow64FloatingSaveArea::ErrorOffset() const
    {
        return float_save_.ErrorOffset;
    }

    uint32_t Wow64FloatingSaveArea::ErrorSelector() const
    {
        return float_save_.ErrorSelector;
    }

    uint32_t Wow64FloatingSaveArea::DataOffset() const
    {
        return float_save_.DataOffset;
    }

    uint32_t Wow64FloatingSaveArea::DataSelector() const
    {
        return float_save_.DataSelector;
    }

    winrt::com_array<uint8_t> Wow64FloatingSaveArea::RegisterArea() const
    {
        return winrt::com_array<uint8_t>(float_save_.RegisterArea, float_save_.RegisterArea + WOW64_SIZE_OF_80387_REGISTERS);
    }

    uint32_t Wow64FloatingSaveArea::Cr0NpxState() const
    {
        return float_save_.Cr0NpxState;
    }

    winrt::hstring Wow64FloatingSaveArea::RegisterAreaHexDump() const
    {
        return register_area_hex_dump_;
    }
}
