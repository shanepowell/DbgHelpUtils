#include "pch.h"
#include "RecentFileItem.h"

#include <filesystem>

#include <winrt/Windows.Storage.h>
#include <winrt/Microsoft.Ui.Xaml.Media.Imaging.h>

#include "Helpers/GlobalOptions.h"
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
        const auto& fileStorage = co_await Windows::Storage::StorageFile::GetFileFromPathAsync(file);
        const auto& fileThumbnail = co_await fileStorage.GetThumbnailAsync(thumbnailMode);

        Media::Imaging::BitmapImage bitmapImage;
        bitmapImage.SetSource(fileThumbnail);

        co_return bitmapImage;
    }
}

namespace winrt::MiniDumpExplorer::implementation
{
    RecentFileItem::RecentFileItem(hstring const& fullPath)
        : fullPath_{fullPath.c_str()}
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

        GlobalOptions::Options().OnSizeNumberDisplayFormatChanged([ptr = get_weak()](auto const, auto const, const auto)
            {
                if(auto const self = ptr.get())
                {
                    self->OnSizeNumberDisplayFormatChanged();
                    return true;
                }

                return false;
            });
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

    event_token RecentFileItem::PropertyChanged(Data::PropertyChangedEventHandler const& value)
    {
        return propertyChanged_.add(value);
    }

    void RecentFileItem::PropertyChanged(event_token const& token)
    {
        propertyChanged_.remove(token);
    }

    void RecentFileItem::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged_(*this, Data::PropertyChangedEventArgs{ propertyName });
    }

    void RecentFileItem::OnSizeNumberDisplayFormatChanged()
    {
        RaisePropertyChanged(L"Size");
    }
}
