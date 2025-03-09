#include "pch.h"
#include "Float80RegisterView.xaml.h"

#include "Helpers/UIHelper.h"

#if __has_include("Float80RegisterView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Float80RegisterView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    Float80RegisterView::Float80RegisterView()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Value"
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void Float80RegisterView::InitializeComponent()
    {
        Float80RegisterViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void Float80RegisterView::Value(MiniDumpExplorer::Float80Register const& value)
    {
        value_ = value;
        RaisePropertyChanged(L"Value");
    }

    void Float80RegisterView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            float80Value()
        );
    }
}
