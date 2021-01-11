#pragma once

namespace MiniDumpExplorerApp
{
    template<typename T>
    [[nodiscard]] static T GetResult(T stream)
    {
        if(stream)
        {
            return stream;
        }

        throw winrt::hresult_illegal_method_call();
    }

    [[nodiscard]] inline winrt::clock::time_point GetResult(SYSTEMTIME const& value)
    {
        FILETIME file_time;
        if(!SystemTimeToFileTime(&value, &file_time))
        {
            throw winrt::hresult_invalid_argument();
        }
        
        return winrt::clock::from_FILETIME(file_time);
    }

}
