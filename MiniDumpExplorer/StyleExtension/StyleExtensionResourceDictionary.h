#pragma once

#include "StyleExtensionResourceDictionary.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct StyleExtensionResourceDictionary : StyleExtensionResourceDictionaryT<StyleExtensionResourceDictionary>
    {
        StyleExtensionResourceDictionary();

        void CopyFrom(Microsoft::UI::Xaml::ResourceDictionary const& resource) const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StyleExtensionResourceDictionary : StyleExtensionResourceDictionaryT<StyleExtensionResourceDictionary, implementation::StyleExtensionResourceDictionary>
    {
    };
}
