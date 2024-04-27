#include "pch.h"
#include "FileCrc32.h"

#include "DbgHelpUtils/mini_dump.h"
#include "Helpers/WindowHelper.h"

#if __has_include("FileCrc32.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "FileCrc32.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    FileCrc32::FileCrc32()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Crc32"
            },
            {
            }, 
            {
            },
            {
            })
    {
    }

    void FileCrc32::Set(std::shared_ptr<dlg_help_utils::mini_dump> const& dump_file)
    {
        is_calculating_ = true;

        RaisePropertyChanged(L"IsCalculating");
        RaisePropertyChanged(L"Crc32");

        CalculateCrc32(dump_file);
    }

    fire_and_forget FileCrc32::CalculateCrc32(std::shared_ptr<dlg_help_utils::mini_dump> const dump_file)
    {
        apartment_context ui_thread;

        auto weak_self = get_weak();
        co_await resume_background();

        if(WindowHelper::IsExiting())
        {
            co_return;
        }

        auto const crc32 = dump_file->data_crc32();

        co_await ui_thread;

        if(auto const self = weak_self.get();
            self && !WindowHelper::IsExiting())
        {
            crc32_ = crc32;
            is_calculating_ = false;

            RaisePropertyChanged(L"IsCalculating");
            RaisePropertyChanged(L"Crc32");
        }
    }
}
