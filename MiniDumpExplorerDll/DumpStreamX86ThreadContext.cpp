#include "pch.h"
#include "DumpStreamX86ThreadContext.h"
#if __has_include("DumpStreamX86ThreadContext.g.cpp")
#include "DumpStreamX86ThreadContext.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamX86ThreadContext::DumpStreamX86ThreadContext()
    {
        InitializeComponent();
    }

    IDumpFileStreamX86ThreadContext DumpStreamX86ThreadContext::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamX86ThreadContext::ThreadContext(IDumpFileStreamX86ThreadContext const value)
    {
        thread_context_ = value;
    }

    bool DumpStreamX86ThreadContext::HasExtendedRegisters() const
    {
        return has_extended_registers_;
    }

    void DumpStreamX86ThreadContext::HasExtendedRegisters(bool const value)
    {
        has_extended_registers_ = value;
    }
}
