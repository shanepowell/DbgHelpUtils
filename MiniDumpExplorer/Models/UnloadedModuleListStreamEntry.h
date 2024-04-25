#pragma once

#include "UnloadedModuleListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_unloaded_module.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct UnloadedModuleListStreamEntry : UnloadedModuleListStreamEntryT<UnloadedModuleListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<UnloadedModuleListStreamEntry>
    {
        UnloadedModuleListStreamEntry();

        uint32_t Index() const { return index_; }
        hstring Name() const { return name_; }
        hstring Path() const { return path_; }
        uint64_t BaseOfImage() const { return module_->BaseOfImage; }
        uint32_t CheckSum() const { return module_->CheckSum; }
        uint32_t SizeOfImage() const { return module_->SizeOfImage; }

        // not a TimeStamp any longer, really a BuildFileHash
        // see https://devblogs.microsoft.com/oldnewthing/20180103-00/?p=97705
        uint32_t BuildFileHash() const { return module_->TimeDateStamp; }

        void Set(uint32_t index, dlg_help_utils::stream_unloaded_module const& module);

    private:
        uint32_t index_{};
        dlg_help_utils::stream_unloaded_module module_;
        hstring name_;
        hstring path_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct UnloadedModuleListStreamEntry : UnloadedModuleListStreamEntryT<UnloadedModuleListStreamEntry, implementation::UnloadedModuleListStreamEntry>
    {
    };
}
