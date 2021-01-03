#include "pch.h"
#include "MiniDumpException.h"

#include "HStringHelper.h"
#include "../DbgHelpUtils/exception_stream.h"
#include "../DbgHelpUtils/exception_utils.h"

namespace MiniDumpExplorerApp
{
    MiniDumpException::MiniDumpException(dlg_help_utils::exception_stream const& exception)
        : exception_{exception}
    {
        if(!exception_.found())
        {
            return;
        }
        for(size_t index = 0; index < NumberParameters(); ++index)
        {
            parameters_.Append(to_hex_string(exception_.exception().ExceptionRecord.ExceptionInformation[index]));
        }
    }

    uint32_t MiniDumpException::ExceptionCode() const
    {
        return exception_.exception().ExceptionRecord.ExceptionCode;
    }

    winrt::hstring MiniDumpException::ExceptionCodeString() const
    {
        return winrt::hstring{dlg_help_utils::exception_utils::exception_code_to_string(ExceptionCode())};
    }

    uint32_t MiniDumpException::ExceptionFlags() const
    {
        return exception_.exception().ExceptionRecord.ExceptionFlags;
    }

    winrt::hstring MiniDumpException::ExceptionFlagsString() const
    {
        return winrt::hstring{dlg_help_utils::exception_utils::exception_flags_to_string(ExceptionFlags())};
    }

    uint64_t MiniDumpException::ExceptionRecord() const
    {
        return exception_.exception().ExceptionRecord.ExceptionRecord;
    }

    uint64_t MiniDumpException::ExceptionAddress() const
    {
        return exception_.exception().ExceptionRecord.ExceptionAddress;
    }

    uint32_t MiniDumpException::NumberParameters() const
    {
        return exception_.exception().ExceptionRecord.NumberParameters;
    }

    winrt::com_array<uint64_t> MiniDumpException::ExceptionInformation() const
    {
        return winrt::com_array<uint64_t>(exception_.exception().ExceptionRecord.ExceptionInformation, exception_.exception().ExceptionRecord.ExceptionInformation + exception_.exception().ExceptionRecord.NumberParameters);
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> MiniDumpException::Parameters() const
    {
        return parameters_;
    }
}
