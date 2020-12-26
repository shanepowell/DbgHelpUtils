﻿#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <cstdint>

namespace dlg_help_utils
{
    class stream_handle_object_information
    {
    public:
        explicit stream_handle_object_information(MINIDUMP_HANDLE_OBJECT_INFORMATION const& info);

        MINIDUMP_HANDLE_OBJECT_INFORMATION const* operator->() const { return &info_; }

        [[nodiscard]] void const* data() const
        {
            return reinterpret_cast<uint8_t const*>(&info_) + sizeof(MINIDUMP_HANDLE_OBJECT_INFORMATION);
        }

    private:
        MINIDUMP_HANDLE_OBJECT_INFORMATION const& info_;
    };
}
