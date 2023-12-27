#pragma once

namespace DataGridInternal
{
    struct DataGridInteractionInfo
    {
        uint32_t CapturedPointerId{0};
        bool IsPointerOver{false};
    };
}