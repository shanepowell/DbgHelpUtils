#include "pch.h"
#include "TokenStreamEntry.h"

#if __has_include("TokenStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "TokenStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    TokenStreamEntry::TokenStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"TokenId",
                L"TokenHandle",
            },
            {
                L"TokenDataSize"
            }, 
            {
            },
            {
            })
    {
    }

    void TokenStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_token_info token)
    {
        index_ = index;
        token_ = std::move(token);
    }
}
