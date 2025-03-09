#include "pch.h"
#include "Wow64ThreadContextView.xaml.h"

#include "Helpers/UIHelper.h"

#if __has_include("Wow64ThreadContextView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Wow64ThreadContextView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    Wow64ThreadContextView::Wow64ThreadContextView() = default;

    void Wow64ThreadContextView::InitializeComponent()
    {
        Wow64ThreadContextViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void Wow64ThreadContextView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            contextFlags(),
            cs(),
            ss(),
            eip(),
            esp(),
            ebp(),
            eFlags(),
            eax(),
            ax(),
            al(),
            ah(),
            ebx(),
            bx(),
            bl(),
            bh(),
            ecx(),
            cx(),
            cl(),
            ch(),
            edx(),
            dx(),
            dl(),
            dh(),
            edi(),
            di(),
            dil(),
            esi(),
            si(),
            sil(),
            controlWord(),
            statusWord(),
            tagWord(),
            errorOffset(),
            errorSelector(),
            dataOffset(),
            dataSelector(),
            cr0NpxState(),
            dr0(),
            dr1(),
            dr2(),
            dr3(),
            dr6(),
            dr7()
        );
    }
}
