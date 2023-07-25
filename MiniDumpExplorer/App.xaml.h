// ReSharper disable CppUnusedIncludeDirective
#pragma once

#include "App.xaml.g.h"
#include "Triggers/ControlSizeTrigger.h"
#include "Triggers/IsEqualStateTrigger.h"
#include "Triggers/IsNullOrEmptyStateTrigger.h"
#include "StyleExtension/StyleExtensions.h"
#include "StyleExtension/StyleExtensionResourceDictionary.h"
#include "Controls/SettingsCard/SettingsCardAutomationPeer.h"
#include "Controls/SettingsCard/SettingsCard.h"
#include "Controls/SettingsExpander/SettingsExpanderAutomationPeer.h"
#include "Controls/SettingsExpander/SettingsExpanderItemStyleSelector.h"
#include "Controls/SettingsExpander/SettingsExpander.h"
#include "Converters/HexNumberConverter.h"
#include "Models/RecentFileItem.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
    
    private:
        Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
