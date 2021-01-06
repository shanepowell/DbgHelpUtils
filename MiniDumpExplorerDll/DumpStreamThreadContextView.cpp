#include "pch.h"
#include "DumpStreamThreadContextView.h"
#if __has_include("DumpStreamThreadContextView.g.cpp")
#include "DumpStreamThreadContextView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadContextView::DumpStreamThreadContextView()
    {
        InitializeComponent();
    }

    IDumpFileStreamThreadContext DumpStreamThreadContextView::ThreadContext() const
    {
        return thread_context_;
    }

    void DumpStreamThreadContextView::ThreadContext(IDumpFileStreamThreadContext value)
    {
        thread_context_ = value;
    }
}
