#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "cv_info_pdb70.h"
#include "stream_module_name.h"

namespace dlg_help_utils
{
    class mini_dump;

    class stream_module : public stream_module_name
    {
    public:
        explicit stream_module(mini_dump const& dump, MINIDUMP_MODULE const& module);

        MINIDUMP_MODULE const* operator->() const { return &module_; }
        [[nodiscard]] void const* cv_record() const { return cv_record_; }
        [[nodiscard]] cv_info_pdb70 const& pdb_info() const { return pdb_info_; }
        [[nodiscard]] void const* misc_record() const { return misc_record_; }

    private:
        MINIDUMP_MODULE const& module_;
        void const* cv_record_{nullptr};
        cv_info_pdb70 pdb_info_{};
        void const* misc_record_{nullptr};
    };
}
