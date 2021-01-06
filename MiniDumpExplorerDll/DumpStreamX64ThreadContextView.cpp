#include "pch.h"
#include "DumpStreamX64ThreadContextView.h"
#if __has_include("DumpStreamX64ThreadContextView.g.cpp")
#include "DumpStreamX64ThreadContextView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamX64ThreadContextView::DumpStreamX64ThreadContextView()
    {
        InitializeComponent();
    }

    IDumpFileStreamX64ThreadContext DumpStreamX64ThreadContextView::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamX64ThreadContextView::ThreadContext(IDumpFileStreamX64ThreadContext value)
    {
        thread_context_ = value;
    }
}
