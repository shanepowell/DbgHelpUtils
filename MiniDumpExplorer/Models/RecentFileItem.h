#pragma once

#include "RecentFileItem.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem>, GlobalOptionsNotifyPropertyChangedBase<RecentFileItem>
    {
        RecentFileItem(uint32_t index, hstring const& fullPath);

        hstring Name() const { return name_; }
        Microsoft::UI::Xaml::Media::ImageSource Icon() const { return icon_; }
        hstring Location() const { return location_; }
        uint64_t Size() const { return size_; }
        uint32_t Index() const { return index_; }
        bool Exists() const { return exists_; }
        std::wstring const& FullPath() const { return fullPath_; }

        Windows::Foundation::IAsyncAction LoadIconAsync();

    private:
        std::wstring fullPath_;
        hstring name_;
        hstring location_;
        Microsoft::UI::Xaml::Media::ImageSource icon_{nullptr};
        size_t size_{};
        uint32_t index_{};
        bool exists_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem, implementation::RecentFileItem>
    {
    };
}
