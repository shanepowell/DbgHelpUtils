#include "pch.h"
#include "ThreadContext.h"

#include "Models/X64ThreadContext.h"
#include "Models/X86ThreadContext.h"
#include "Models/Wow64ThreadContext.h"

#if __has_include("ThreadContext.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadContext.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadContext::ThreadContext() = default;

    void ThreadContext::Set(dlg_help_utils::stream_thread_context const& thread_context)
    {
        if (thread_context.x64_thread_context_available())
        {
            x64ThreadContext_ = MiniDumpExplorer::X64ThreadContext{};
            x64ThreadContext_.as<implementation::X64ThreadContext>()->Set(thread_context.x64_thread_context());
        }

        if (thread_context.wow64_thread_context_available())
        {
            wow64ThreadContext_ = MiniDumpExplorer::Wow64ThreadContext{};
            wow64ThreadContext_.as<implementation::Wow64ThreadContext>()->Set(thread_context.wow64_thread_context());
        }

        if (thread_context.x86_thread_context_available())
        {
            x86ThreadContext_ = MiniDumpExplorer::X86ThreadContext{};
            x86ThreadContext_.as<implementation::X86ThreadContext>()->Set(thread_context.x86_thread_context());
        }
    }
}
