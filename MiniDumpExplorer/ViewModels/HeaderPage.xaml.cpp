#include "pch.h"
#include "HeaderPage.xaml.h"
#include "MiniDumpPage.xaml.h"

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_type.h"
#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "Helpers/SymbolEngineHelper.h"
#include "Helpers/UIHelper.h"
#include "Models/RecentFileItem.h"
#include "Models/DumpFileTimeStamp.h"
#include "Utility/logger.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#if __has_include("HeaderPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HeaderPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    HeaderPage::HeaderPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Version",
                L"InternalVersion",
                L"NumberOfStreams",
                L"StreamDirectoryRva",
                L"CheckSum",
                L"Flags"
            },
            {
            }, 
            {
                L"TimeDateStamp"
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void HeaderPage::InitializeComponent()
    {
        HeaderPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    hstring HeaderPage::DumpType() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return hstring{ dlg_help_utils::system_info_utils::mini_dump_type_to_string(mini_dump_->type()) };
    }

    hstring HeaderPage::Signature() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return hstring{ mini_dump_->version_string() };
    }

    bool HeaderPage::IsValid() const
    {
        if(!mini_dump_)
        {
            return false;
        }
        return mini_dump_->is_valid();
    }

    bool HeaderPage::IsUserDump() const
    {
        if(!mini_dump_)
        {
            return false;
        }
        return mini_dump_->type() == dlg_help_utils::dump_file_type::user_mode_dump;
    }

    bool HeaderPage::IsKernelDump() const
    {
        if(!mini_dump_)
        {
            return false;
        }
        return mini_dump_->type() == dlg_help_utils::dump_file_type::kernel_mode_dump_x86 || mini_dump_->type() == dlg_help_utils::dump_file_type::kernel_mode_dump_x64;
    }

    bool HeaderPage::IsUnknownDump() const
    {
        return !IsUserDump() && !IsKernelDump();
    }

    uint16_t HeaderPage::Version() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return LOWORD(mini_dump_->header()->Version);
    }

    uint16_t HeaderPage::InternalVersion() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return HIWORD(mini_dump_->header()->Version);
    }

    uint32_t HeaderPage::NumberOfStreams() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->NumberOfStreams;
    }

    uint32_t HeaderPage::StreamDirectoryRva() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->StreamDirectoryRva;
    }

    uint32_t HeaderPage::CheckSum() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->CheckSum;
    }

    uint64_t HeaderPage::Flags() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->Flags;
    }

    void HeaderPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(headerVersion(),
            headerInternalVersion(),
            headerStreamDirectoryRva(),
            headerCheckSum(),
            headerDumpFileCrc32(),
            headerFlags());

        UIHelper::CreateStandardTimeStampMenu(timeDateStamp());

        UIHelper::CreateStandardSizeNumberMenu(headerFileSize());
    }

    void HeaderPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();

        SetupMinidumpHeader(miniDumpPage->MiniDumpInstance(), miniDumpPage->Helper(), miniDumpPage->File().Path(), miniDumpPage->DumpFileCrc32());
        RaisePropertyChanged(L"DumpType");
        RaisePropertyChanged(L"Signature");
        RaisePropertyChanged(L"IsValid");
        RaisePropertyChanged(L"IsUserDump");
        RaisePropertyChanged(L"IsKernelDump");
        RaisePropertyChanged(L"IsUnknownDump");
        RaisePropertyChanged(L"Version");
        RaisePropertyChanged(L"InternalVersion");
        RaisePropertyChanged(L"NumberOfStreams");
        RaisePropertyChanged(L"StreamDirectoryRva");
        RaisePropertyChanged(L"CheckSum");
        RaisePropertyChanged(L"TimeDateStamp");
        RaisePropertyChanged(L"Flags");
        RaisePropertyChanged(L"FlagsList");
        RaisePropertyChanged(L"FileItem");
        RaisePropertyChanged(L"DumpFileCrc32");
        RaisePropertyChanged(L"ProcessTarget");
        RaisePropertyChanged(L"IsProcessWow64");
        RaisePropertyChanged(L"IsProcessX64");
        RaisePropertyChanged(L"IsProcessX86");
        RaisePropertyChanged(L"IsProcessUnknown");
        RaisePropertyChanged(L"UserStackDbEnabledString");
        RaisePropertyChanged(L"UserStackDbEnabled");
    }

    void HeaderPage::SetupMinidumpHeader(std::shared_ptr<dlg_help_utils::mini_dump> const& miniDump, SymbolEngineHelper& symbolEngineHelper, hstring const& path, MiniDumpExplorer::FileCrc32 fileCrc32)  // NOLINT(performance-unnecessary-value-param)
    {
        mini_dump_ = miniDump;
        fileCrc32_ = std::move(fileCrc32);
        for (auto const& type : dlg_help_utils::mini_dump_type::to_strings(static_cast<MINIDUMP_TYPE>(mini_dump_->header()->Flags)))
        {
            flagsList_.Append(type);
        }

        fileItem_ = MiniDumpExplorer::RecentFileItem{0, path};
        timeDateStamp_.as<DumpFileTimeStamp>()->Set(mini_dump_->header()->TimeDateStamp, dlg_help_utils::misc_info_stream::get_dump_file_timezone_info(*mini_dump_));

        if (IsUserDump() && IsValid())
        {
            Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            dlg_help_utils::process::process_environment_block const peb{*mini_dump_, symbolEngineHelper.cache(), symbolEngineHelper.symbol_engine()};

            if (peb.is_wow64_target())
            {
                processTarget_ = rm.MainResourceMap().GetValue(L"Resources/ProcessTargetWow64").ValueAsString();
                isProcessWow64_ = true;
            }
            else if (peb.is_x64_target())
            {
                processTarget_ = rm.MainResourceMap().GetValue(L"Resources/ProcessTargetX64").ValueAsString();
                isProcessX64_ = true;
            }
            else if (peb.is_x86_target())
            {
                processTarget_ = rm.MainResourceMap().GetValue(L"Resources/ProcessTargetX86").ValueAsString();
                isProcessX86_ = true;
            }
            else
            {
                processTarget_ = rm.MainResourceMap().GetValue(L"Resources/ProcessTargetUnknown").ValueAsString();
                isProcessUnknown_ = true;
            }

            userStackDbEnabled_ = peb.user_stack_db_enabled();
            userStackDbEnabledString_ = rm.MainResourceMap().GetValue(userStackDbEnabled_ ? L"Resources/Enabled" : L"Resources/Disabled").ValueAsString();
        }

        // ReSharper disable once CppExpressionWithoutSideEffects
        LoadFileItemIcon();
    }

    fire_and_forget HeaderPage::LoadFileItemIcon() const
    {
        try
        {
            co_await fileItem_.as<RecentFileItem>()->LoadIconAsync();
        }
        catch(...)
        {
            logger::Log().HandleUnknownException();
        }
    }
}
