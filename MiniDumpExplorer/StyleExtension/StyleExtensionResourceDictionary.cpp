#include "pch.h"
#include "StyleExtensionResourceDictionary.h"
#if __has_include("StyleExtensionResourceDictionary.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "StyleExtensionResourceDictionary.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    StyleExtensionResourceDictionary::StyleExtensionResourceDictionary() = default;

    void StyleExtensionResourceDictionary::CopyFrom(ResourceDictionary const& resource) const
    {
        for (auto const& [key, value] : resource)
        {
            [[maybe_unused]] auto result = Insert(key, value);
        }

        const auto themeDictionaries = ThemeDictionaries();
        for (auto const& [key, value] : resource.ThemeDictionaries())
        {
            [[maybe_unused]] auto result = themeDictionaries.Insert(key, value);
        }

        const auto mergedDictionaries = MergedDictionaries();
        for (auto const& value : resource.MergedDictionaries())
        {
            mergedDictionaries.Append(value);
        }
    }
}
