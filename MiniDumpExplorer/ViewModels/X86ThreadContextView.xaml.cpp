#include "pch.h"
#include "X86ThreadContextView.xaml.h"

#include "Helpers/UIHelper.h"

#if __has_include("X86ThreadContextView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X86ThreadContextView.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    X86ThreadContextView::X86ThreadContextView() = default;

    void X86ThreadContextView::InitializeComponent()
    {
        X86ThreadContextViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void X86ThreadContextView::SetupFlyoutMenus()
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
            dr0(),
            dr1(),
            dr2(),
            dr3(),
            dr6(),
            dr7()
        );
    }
}
