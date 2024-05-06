#pragma once

#include "AvrfBackTraceInformation.g.h"

#include "NotifyPropertyChangedBase.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <avrfsdk.h>

namespace winrt::MiniDumpExplorer::implementation
{
    struct AvrfBackTraceInformation : AvrfBackTraceInformationT<AvrfBackTraceInformation>, NotifyPropertyChangedBase<AvrfBackTraceInformation>
    {
        AvrfBackTraceInformation();

        uint32_t Depth() const { return backtrace_information_->Depth; }
        uint32_t Index() const { return backtrace_information_->Index; }
        Windows::Foundation::Collections::IObservableVector<uint64_t> ReturnAddresses() const { return returnAddresses_; }

        void Set(AVRF_BACKTRACE_INFORMATION const* backtrace_information);

    private:
        AVRF_BACKTRACE_INFORMATION const* backtrace_information_{};
        Windows::Foundation::Collections::IObservableVector<uint64_t> returnAddresses_{single_threaded_observable_vector<uint64_t>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct AvrfBackTraceInformation : AvrfBackTraceInformationT<AvrfBackTraceInformation, implementation::AvrfBackTraceInformation>
    {
    };
}
