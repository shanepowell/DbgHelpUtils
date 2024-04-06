#pragma once

#include "MiniDumpExceptionParameter.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpExceptionParameter : MiniDumpExceptionParameterT<MiniDumpExceptionParameter>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpExceptionParameter>
    {
        MiniDumpExceptionParameter(uint32_t index, uint64_t parameter);

        uint32_t Index() const { return index_; }
        uint64_t Parameter() const { return parameter_; }

    private:
        uint32_t index_;
        uint64_t parameter_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpExceptionParameter : MiniDumpExceptionParameterT<MiniDumpExceptionParameter, implementation::MiniDumpExceptionParameter>
    {
    };
}
