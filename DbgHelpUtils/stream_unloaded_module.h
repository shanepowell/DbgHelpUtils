#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "stream_module_name.h"

namespace dlg_help_utils
{
    class mini_dump;

    class stream_unloaded_module : public stream_module_name
    {
    public:
        explicit stream_unloaded_module(mini_dump const& dump, MINIDUMP_UNLOADED_MODULE const& module);

        MINIDUMP_UNLOADED_MODULE const* operator->() const { return &module_; }

    private:
        MINIDUMP_UNLOADED_MODULE const& module_;
    };
}
