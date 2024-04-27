#pragma once

#include "FileCrc32.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct FileCrc32 : FileCrc32T<FileCrc32>, GlobalOptionsNotifyPropertyChangedBase<FileCrc32>
    {
        FileCrc32();

        uint32_t Crc32() const { return crc32_; }
        bool IsCalculating() const { return is_calculating_; }

        void Set(std::shared_ptr<dlg_help_utils::mini_dump> const& dump_file);

        fire_and_forget CalculateCrc32(std::shared_ptr<dlg_help_utils::mini_dump> dump_file);

    private:
        uint32_t crc32_{};
        bool is_calculating_{true};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct FileCrc32 : FileCrc32T<FileCrc32, implementation::FileCrc32>
    {
    };
}
