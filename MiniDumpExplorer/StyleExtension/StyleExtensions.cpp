#include "pch.h"
#include "StyleExtensions.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "StyleExtensionResourceDictionary.h"
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
        static DependencyProperty s_ResourcesProperty = DependencyProperty::Register(
            L"Resources",
            xaml_typename<ResourceDictionary>(),
            xaml_typename<MiniDumpExplorer::StyleExtensions>(),
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
        catch(...)
        {
            logger::Log().HandleUnknownException();
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
