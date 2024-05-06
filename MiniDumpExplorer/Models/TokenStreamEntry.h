#pragma once

#include "TokenStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_token_info.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct TokenStreamEntry : TokenStreamEntryT<TokenStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<TokenStreamEntry>
    {
        TokenStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_token_info token);

        uint32_t Index() const { return index_; }
        uint32_t TokenId() const { return token_->TokenId; }
        uint64_t TokenHandle() const { return token_->TokenHandle; }
        uint32_t TokenDataSize() const { return static_cast<uint32_t>(token_.size()); }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_token_info token_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct TokenStreamEntry : TokenStreamEntryT<TokenStreamEntry, implementation::TokenStreamEntry>
    {
    };
}
