#pragma once

namespace Utility
{
    inline bool RectContains(winrt::Windows::Foundation::Rect const& rect, winrt::Windows::Foundation::Point const& point)
    {
        return point.X >= rect.X && point.X <= rect.X + rect.Width && point.Y >= rect.Y && point.Y <= rect.Y + rect.Height;
    }
}
