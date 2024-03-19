#include "pch.h"
#include "HeaderPage.xaml.h"
#include "MiniDumpPage.xaml.h"

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_type.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "Helpers/GlobalOptions.h"
#include "Helpers/UIHelper.h"
#include "Models/RecentFileItem.h"
#include "Utility/logger.h"

#if __has_include("HeaderPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HeaderPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    HeaderPage::HeaderPage()
    {
        InitializeComponent();
        SetupFlyoutMenus();
        SetupGlobalOptionHooks();
    }

    void HeaderPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        if(auto const miniDumpPage = e.Parameter().as<MiniDumpPage>();
            miniDumpPage->MiniDumpOpened())
        {
            SetupMinidumpHeader(miniDumpPage->MiniDump(), miniDumpPage->File().Path());
        }
        else
        {
            miniDumpLoadedEvent_ = miniDumpPage->MiniDumpLoaded([weakThis{ get_weak() }]([[maybe_unused]] auto const& sender, auto const& args)
            {
                if (auto strongThis{ weakThis.get() }; strongThis)
                {
                    strongThis->MiniDumpLoaded(args);
                }
            });
        }
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

    uint32_t HeaderPage::TimeDateStamp() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->TimeDateStamp;
    }

    uint64_t HeaderPage::Flags() const
    {
        if(!mini_dump_)
        {
            return {};
        }
        return mini_dump_->header()->Flags;
    }

    event_token HeaderPage::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged_.add(handler);
    }

    void HeaderPage::PropertyChanged(event_token const& token) noexcept
    {
        propertyChanged_.remove(token);
    }

    void HeaderPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(headerVersion());
        UIHelper::CreateStandardHexNumberMenu(headerInternalVersion());
        UIHelper::CreateStandardHexNumberMenu(headerStreamDirectoryRva());
        UIHelper::CreateStandardHexNumberMenu(headerCheckSum());
        UIHelper::CreateStandardHexNumberMenu(headerDumpFileCrc32());
        UIHelper::CreateStandardHexNumberMenu(headerFlags());

        UIHelper::CreateStandardSizeNumberMenu(headerFileSize());
    }

    void HeaderPage::SetupGlobalOptionHooks()
    {
        GlobalOptions::Options().OnNumberDisplayFormatChanged([ptr = get_weak()](auto const)
            {
                if(auto const self = ptr.get())
                {
                    self->OnNumberDisplayFormatChanged();
                    return true;
                }

                return false;
            });
    }

    void HeaderPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPage const& miniDumpPage)
    {
        auto const internalMiniDumpPage = miniDumpPage.as<MiniDumpPage>();

        SetupMinidumpHeader(internalMiniDumpPage->MiniDump(), internalMiniDumpPage->File().Path());
        internalMiniDumpPage->MiniDumpLoaded(miniDumpLoadedEvent_);
        miniDumpLoadedEvent_ = {};
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
        RaisePropertyChanged(L"DumpFileCrc32");
        RaisePropertyChanged(L"TimeDateStamp");
        RaisePropertyChanged(L"Flags");
        RaisePropertyChanged(L"FlagsList");
        RaisePropertyChanged(L"FileItem");
    }

    void HeaderPage::SetupMinidumpHeader(dlg_help_utils::mini_dump const& miniDump, hstring const& path)
    {
        mini_dump_ = &miniDump;
        data_crc32_ = mini_dump_->data_crc32();
        for (auto const& type : dlg_help_utils::mini_dump_type::to_strings(static_cast<MINIDUMP_TYPE>(mini_dump_->header()->Flags)))
        {
            flagsList_.Append(type);
        }

        fileItem_ = MiniDumpExplorer::RecentFileItem{path};

        // ReSharper disable once CppExpressionWithoutSideEffects
        LoadFileItemIcon();
    }

    void HeaderPage::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged_(*this, Data::PropertyChangedEventArgs{ propertyName });
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

    void HeaderPage::OnNumberDisplayFormatChanged()
    {
        RaisePropertyChanged(L"Version");
        RaisePropertyChanged(L"InternalVersion");
        RaisePropertyChanged(L"NumberOfStreams");
        RaisePropertyChanged(L"StreamDirectoryRva");
        RaisePropertyChanged(L"CheckSum");
        RaisePropertyChanged(L"DumpFileCrc32");
        RaisePropertyChanged(L"Flags");
    }
}
