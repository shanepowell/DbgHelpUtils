#include "pch.h"
#include "X64ThreadContextView.xaml.h"

#include "Helpers/UIHelper.h"

#if __has_include("X64ThreadContextView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "X64ThreadContextView.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    X64ThreadContextView::X64ThreadContextView() = default;

    void X64ThreadContextView::InitializeComponent()
    {
        X64ThreadContextViewT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void X64ThreadContextView::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            contextFlags(),
            cs(),
            ss(),
            rip(),
            rsp(),
            rbp(),
            eFlags(),
            rax(),
            eax(),
            ax(),
            al(),
            ah(),
            rbx(),
            ebx(),
            bx(),
            bl(),
            bh(),
            rcx(),
            ecx(),
            cx(),
            cl(),
            ch(),
            rdx(),
            edx(),
            dx(),
            dl(),
            dh(),
            rdi(),
            edi(),
            di(),
            dil(),
            rsi(),
            esi(),
            si(),
            sil(),
            r8(),
            r8d(),
            r8w(),
            r8b(),
            r9(),
            r9d(),
            r9w(),
            r9b(),
            r10(),
            r10d(),
            r10w(),
            r10b(),
            r11(),
            r11d(),
            r11w(),
            r11b(),
            r12(),
            r12d(),
            r12w(),
            r12b(),
            r13(),
            r13d(),
            r13w(),
            r13b(),
            r14(),
            r14d(),
            r14w(),
            r14b(),
            r15(),
            r15d(),
            r15w(),
            r15b(),
            controlWord(),
            statusWord(),
            tagWord(),
            errorOpcode(),
            errorOffset(),
            errorSelector(),
            dataOffset(),
            dataSelector(),
            mxCsr(),
            dr0(),
            dr1(),
            dr2(),
            dr3(),
            dr6(),
            dr7(),
            debugControl(),
            lastBranchToRip(),
            lastBranchFromRip(),
            lastExceptionToRip(),
            lastExceptionFromRip()
        );
    }
}
