#include "pch.h"
#include "StackTraceView.h"
#if __has_include("StackTraceView.g.cpp")
#include "StackTraceView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    StackTraceView::StackTraceView()
    {
        InitializeComponent();
    }

    IStackTrace StackTraceView::StackTrace() const
    {
        return stack_trace_;
    }

    void StackTraceView::StackTrace(IStackTrace const value)
    {
        stack_trace_ = value;
    }
}
