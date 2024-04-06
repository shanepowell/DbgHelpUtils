#pragma once

#include "MiniDumpException.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpExceptionParameter;

    struct MiniDumpException : MiniDumpExceptionT<MiniDumpException>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpException>
    {
        MiniDumpException();

        uint32_t ExceptionCode() const { return exception_->ExceptionCode; }
        hstring ExceptionCodeString() const { return exceptionCodeString_; }
        uint32_t ExceptionFlags() const { return exception_->ExceptionFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> ExceptionFlagsList() const { return exceptionFlagsList_; }
        uint64_t ExceptionRecord() const { return exception_->ExceptionRecord; }
        uint64_t ExceptionAddress() const { return exception_->ExceptionAddress; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::MiniDumpExceptionParameter> ExceptionInformation() const { return exceptionInformation_; }

        void Set(MINIDUMP_EXCEPTION const* exception);

    private:
        MINIDUMP_EXCEPTION const* exception_{nullptr};
        hstring exceptionCodeString_{};
        Windows::Foundation::Collections::IObservableVector<hstring> exceptionFlagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::MiniDumpExceptionParameter> exceptionInformation_{single_threaded_observable_vector<MiniDumpExplorer::MiniDumpExceptionParameter>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpException : MiniDumpExceptionT<MiniDumpException, implementation::MiniDumpException>
    {
    };
}
