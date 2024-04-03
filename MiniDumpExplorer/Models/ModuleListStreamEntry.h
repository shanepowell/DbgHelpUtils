#pragma once

#include "ModuleListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_module.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct VsFixedFileInfo;
    struct CvInfoPdb70;
    struct MiniDumpLocationDescriptor;

    struct ModuleListStreamEntry : ModuleListStreamEntryT<ModuleListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ModuleListStreamEntry>
    {
        ModuleListStreamEntry();

        uint32_t Index() const { return index_; }
        hstring Name() const { return name_; }
        hstring Path() const { return path_; }
        uint64_t Base() const { return module_->BaseOfImage; }
        uint32_t CheckSum() const { return module_->CheckSum; }
        uint32_t SizeOfImage() const { return module_->SizeOfImage; }
        uint32_t TimeDateStamp() const { return module_->TimeDateStamp; }
        MiniDumpExplorer::VsFixedFileInfo FileInfo() const { return fileInfo_; }
        MiniDumpExplorer::MiniDumpLocationDescriptor CvRecord() const { return cvRecord_; }
        MiniDumpExplorer::CvInfoPdb70 PdbInfo() const { return pdbInfo_; }
        MiniDumpExplorer::MiniDumpLocationDescriptor MiscRecord() const { return miscRecord_; }
        uint64_t Reserved0() const { return module_->Reserved0; }
        uint64_t Reserved1() const { return module_->Reserved1; }

        void Set(uint32_t index, dlg_help_utils::stream_module const& module);

    private:
        uint32_t index_;
        dlg_help_utils::stream_module module_;
        hstring name_;
        hstring path_;
        MiniDumpExplorer::VsFixedFileInfo fileInfo_{};
        MiniDumpExplorer::MiniDumpLocationDescriptor cvRecord_{};
        MiniDumpExplorer::CvInfoPdb70 pdbInfo_{};
        MiniDumpExplorer::MiniDumpLocationDescriptor miscRecord_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ModuleListStreamEntry : ModuleListStreamEntryT<ModuleListStreamEntry, implementation::ModuleListStreamEntry>
    {
    };
}
