#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include <string>

namespace dlg_help_utils::handles
{
    template <typename T, typename D>
    std::unique_ptr<T, D> make_handle(T* handle, D deleter)
    {
        return std::unique_ptr<T, D>{handle, deleter};
    }

    // ReSharper disable once CppParameterMayBeConst
    inline auto make_windows_handle(HANDLE handle)
    {
        return make_handle(handle == INVALID_HANDLE_VALUE ? nullptr : handle, CloseHandle);
    }

    inline auto make_empty_windows_handle()
    {
        return make_windows_handle(nullptr);
    }

    using windows_handle = decltype(make_windows_handle(HANDLE{}));

    // ReSharper disable once CppParameterMayBeConst
    inline auto make_windows_library_handle(HMODULE handle)
    {
        return make_handle(handle, FreeLibrary);
    }

    using windows_library_handle = decltype(make_windows_library_handle(HMODULE{}));

    inline auto load_library(std::wstring const& dll_source_path)
    {
        return make_windows_library_handle(dll_source_path.empty() ? nullptr : LoadLibraryW(dll_source_path.c_str()));
    }

    // ReSharper disable once CppParameterMayBeConst
    inline auto make_local_memory_handle(LPVOID ptr)
    {
        return make_handle(ptr, LocalFree);
    }

    using local_memory_handle = decltype(make_local_memory_handle(LPVOID{}));

    // ReSharper disable once CppParameterMayBeConst
    inline auto make_map_view_handle(LPVOID ptr)
    {
        return make_handle(ptr, UnmapViewOfFile);
    }

    using map_view_handle = decltype(make_map_view_handle(LPVOID{}));

    inline auto make_empty_map_view_handle()
    {
        return make_map_view_handle(nullptr);
    }
}
