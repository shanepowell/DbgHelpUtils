#pragma once

#include "RecentFileItem.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem>
    {
        RecentFileItem(hstring const& fullPath);

        hstring Name() const { return name_; }
        Microsoft::UI::Xaml::Media::ImageSource Icon() const { return icon_; }
        hstring Location() const { return location_; }
        uint64_t Size() const { return size_; }
        bool Exists() const { return exists_; }
        std::wstring const& FullPath() const { return fullPath_; }

        Windows::Foundation::IAsyncAction LoadIconAsync();
        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(event_token const& token);

    private:
        void RaisePropertyChanged(hstring const& propertyName);
        void OnSizeNumberDisplayFormatChanged();

    private:
        std::wstring fullPath_;
        hstring name_;
        hstring location_;
        Microsoft::UI::Xaml::Media::ImageSource icon_{nullptr};
        size_t size_{};
        bool exists_{false};
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem, implementation::RecentFileItem>
    {
    };
}
