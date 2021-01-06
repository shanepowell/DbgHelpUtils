#include "pch.h"
#include "DumpStreamX86ThreadContextView.h"
#if __has_include("DumpStreamX86ThreadContextView.g.cpp")
#include "DumpStreamX86ThreadContextView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamX86ThreadContextView::DumpStreamX86ThreadContextView()
    {
        InitializeComponent();
    }

    IDumpFileStreamX86ThreadContext DumpStreamX86ThreadContextView::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamX86ThreadContextView::ThreadContext(IDumpFileStreamX86ThreadContext const value)
    {
        thread_context_ = value;
    }

    bool DumpStreamX86ThreadContextView::HasExtendedRegisters() const
    {
        return has_extended_registers_;
    }

    void DumpStreamX86ThreadContextView::HasExtendedRegisters(bool const value)
    {
        has_extended_registers_ = value;
    }
}
