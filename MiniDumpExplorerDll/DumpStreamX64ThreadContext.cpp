#include "pch.h"
#include "DumpStreamX64ThreadContext.h"
#if __has_include("DumpStreamX64ThreadContext.g.cpp")
#include "DumpStreamX64ThreadContext.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamX64ThreadContext::DumpStreamX64ThreadContext()
    {
        InitializeComponent();
    }

    IDumpFileStreamX64ThreadContext DumpStreamX64ThreadContext::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamX64ThreadContext::ThreadContext(IDumpFileStreamX64ThreadContext value)
    {
        thread_context_ = value;
    }
}
