#pragma once

#include "CommentStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct CommentStreamPage : CommentStreamPageT<CommentStreamPage>, NotifyPropertyChangedBase<CommentStreamPage>, MiniDumpPageBase<CommentStreamPage>
    {
        CommentStreamPage();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint32_t Index() const { return index_; }
        hstring Comment() const { return comment_; }

    private:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        uint32_t index_{};
        hstring comment_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct CommentStreamPage : CommentStreamPageT<CommentStreamPage, implementation::CommentStreamPage>
    {
    };
}
