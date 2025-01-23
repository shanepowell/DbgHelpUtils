#include "pch.h"
#include "RecentFileItem.h"

#include <filesystem>

#include <winrt/Windows.Storage.h>
#include <winrt/Microsoft.Ui.Xaml.Media.Imaging.h>

#include "Utility/logger.h"

#if __has_include("RecentFileItem.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentFileItem.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace
{
    Windows::Foundation::IAsyncOperation<Media::ImageSource> GetIconFromFile(hstring const& file, Windows::Storage::FileProperties::ThumbnailMode const thumbnailMode = Windows::Storage::FileProperties::ThumbnailMode::DocumentsView)
    {
        const auto fileStorage = co_await Windows::Storage::StorageFile::GetFileFromPathAsync(file);
        const auto fileThumbnail = co_await fileStorage.GetThumbnailAsync(thumbnailMode);

        Media::Imaging::BitmapImage bitmapImage;
        bitmapImage.SetSource(fileThumbnail);

        co_return bitmapImage;
    }
}

namespace winrt::MiniDumpExplorer::implementation
{
    RecentFileItem::RecentFileItem(uint32_t const index, hstring const& fullPath)
        : GlobalOptionsNotifyPropertyChangedBase(
            {
            },
            {
                L"Size"
            },
            {
            },
            {
            },
            {
            },
            {
            })
        , fullPath_{fullPath.c_str()}
        , index_{index}
    {
        std::filesystem::path const path{fullPath.c_str()};

        location_ = path.parent_path().c_str();
        if(path.has_filename())
        {
            name_ = path.filename().c_str();
        }

        if(exists(path))
        {
            exists_ = true;
            size_ = file_size(path);
        }
    }

    Windows::Foundation::IAsyncAction RecentFileItem::LoadIconAsync()
    {
        try
        {
            icon_ = co_await GetIconFromFile(fullPath_.c_str());
            RaisePropertyChanged(L"Icon");
        }
        catch(...)
        {
            logger::Log().HandleUnknownException();
        }
    }
}
