#include "pch.h"
#include "HandleDataStreamEntry.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#if __has_include("HandleDataStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleDataStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    HandleDataStreamEntry::HandleDataStreamEntry() = default;

    void HandleDataStreamEntry::Set(uint32_t const index, dlg_help_utils::handle_data_stream const& handle_data, std::wstring_view const& type, uint32_t const total_handles)
    {
        index_ = index;
        handle_data_ = handle_data;
        if(type.empty())
        {
            Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            type_ = rm.MainResourceMap().GetValue(L"Resources/HandleDataStreamEntryUnNamedType").ValueAsString();
        }
        else
        {
            type_ = type;
        }
        totalHandles_ = total_handles;
    }
}
