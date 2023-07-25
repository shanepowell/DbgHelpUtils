#include "pch.h"
#include "StyleExtensions.h"

#include "App.xaml.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Utility/logger.h"

#if __has_include("StyleExtensions.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "StyleExtensions.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    StyleExtensions::StyleExtensions() = default;

    ResourceDictionary StyleExtensions::GetResources(Style const& obj)
    {
        return obj.GetValue(ResourcesProperty()).as<ResourceDictionary>();
    }

    void StyleExtensions::SetResources(Style const& obj, ResourceDictionary const& value)
    {
        obj.SetValue(ResourcesProperty(), value);
    }

    DependencyProperty StyleExtensions::ResourcesProperty()
    {
        static DependencyProperty s_ResourcesProperty = DependencyProperty::RegisterAttached(
            L"Resources",
            winrt::xaml_typename<ResourceDictionary>(),
            winrt::xaml_typename<MiniDumpExplorer::StyleExtensions>(),
            PropertyMetadata(nullptr, ResourcesChanged)
        );
        return s_ResourcesProperty;
    }

    void StyleExtensions::ResourcesChanged(DependencyObject const& sender, DependencyPropertyChangedEventArgs const& e)
    {
        try
        {
            auto const frameworkElement = sender.try_as<FrameworkElement>();
            if (!frameworkElement)
            {
                return;
            }

            auto const resources = frameworkElement.Resources();
            auto const mergedDictionaries = resources ? resources.MergedDictionaries() : nullptr;
            if (!mergedDictionaries)
            {
                return;
            }

            for (uint32_t index = 0; index < mergedDictionaries.Size(); ++index)
            {
                if (mergedDictionaries.GetAt(index).try_as<StyleExtensionResourceDictionary>())
                {
                    // Remove the existing resource dictionary
                    mergedDictionaries.RemoveAt(index);
                    break;
                }
            }

            if (const auto resource = e.NewValue().try_as<ResourceDictionary>(); resource)
            {
                const auto clonedResources = make<StyleExtensionResourceDictionary>();
                clonedResources.CopyFrom(resource);
                mergedDictionaries.Append(clonedResources);
            }

            if (frameworkElement.IsLoaded())
            {
                // Only force if the style was applied after the control was loaded
                ForceControlToReloadThemeResources(frameworkElement);
            }
        }
        catch(dlg_help_utils::exceptions::wide_runtime_error const& exception)
        {
            logger::Log().LogMessage(log_level::error, std::format(L"StyleExtensions::ResourcesChanged: {}", exception.message()));
        }
        catch(std::exception const& exception)
        {
            logger::Log().LogMessage(log_level::error, std::format("StyleExtensions::ResourcesChanged: {}", exception.what()));
        }
        catch(hresult_error const& exception)
        {
            logger::Log().LogMessage(log_level::error, std::format(L"StyleExtensions::ResourcesChanged: hresult_error: {0} / code: {1}", exception.message(), dlg_help_utils::stream_hex_dump::to_hex(exception.code().value)));
        }
        catch(...)
        {
            logger::Log().LogMessage(log_level::error, "StyleExtensions::ResourcesChanged: Unknown exception"sv);
        }
    }

    void StyleExtensions::ForceControlToReloadThemeResources(FrameworkElement const& frameworkElement)
    {
        // To force the refresh of all resource references.
        // Note: Doesn't work when in high-contrast.
        auto const currentRequestedTheme = frameworkElement.RequestedTheme();
        frameworkElement.RequestedTheme(currentRequestedTheme == ElementTheme::Dark
            ? ElementTheme::Light
            : ElementTheme::Dark);
        frameworkElement.RequestedTheme(currentRequestedTheme);
    }
}
