#include "pch.h"
#include "MiniDumpException.h"

#include "DbgHelpUtils/exception_utils.h"

#if __has_include("MiniDumpException.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpException.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpException::MiniDumpException()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"ExceptionCode",
                L"ExceptionFlags",
                L"ExceptionRecord",
                L"ExceptionAddress",
                L"ExceptionInformation"
            },
            {
            },
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void MiniDumpException::Set(MINIDUMP_EXCEPTION const* exception)
    {
        exception_ = exception;
        exceptionCodeString_ = dlg_help_utils::exception_utils::exception_code_to_string(exception_->ExceptionCode);
        for (auto flag : dlg_help_utils::exception_utils::exception_flags_to_list(exception_->ExceptionFlags))
        {
            exceptionFlagsList_.Append(flag);
        }

        for (uint32_t parameter = 0; parameter < exception_->NumberParameters; ++parameter)
        {
            exceptionInformation_.Append(MiniDumpExplorer::MiniDumpExceptionParameter{parameter, exception_->ExceptionInformation[parameter]});
        }
    }
}
