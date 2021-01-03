#include "pch.h"
#include "DumpStreamThreadContext.h"
#if __has_include("DumpStreamThreadContext.g.cpp")
#include "DumpStreamThreadContext.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadContext::DumpStreamThreadContext()
    {
        InitializeComponent();
    }

    IDumpFileStreamThreadContext DumpStreamThreadContext::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamThreadContext::ThreadContext(IDumpFileStreamThreadContext value)
    {
        thread_context_ = value;
    }
}
