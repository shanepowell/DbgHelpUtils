#pragma once

#include "DbgHelpUtils/wide_runtime_error.h"
#include "Utility/logger.h"

namespace Utility
{
    template <typename TAction, typename TSection>
    winrt::Windows::Foundation::IAsyncAction run(std::string_view const& title, TAction action, TSection section)
    {
        try
        {
            co_await action();
        }
        catch (dlg_help_utils::exceptions::wide_runtime_error const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("{0} (wre) failed{1}: {2}\n", title, section(), dlg_help_utils::string_conversation::wstring_to_utf8(e.message())));
        }
        catch (std::runtime_error const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("{0} (re) failed{1}: {2}\n", title, section(), e.what()));
        }
        catch (std::exception const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("{0} (e) failed{1}: {2}\n", title, section(), e.what()));
        }
        catch (...)
        {
            logger::Log().LogMessage(log_level::error, std::format("{0} (u) failed{1}\n", title, section()));
        }
    }

    inline std::string for_stream_index(size_t const index)
    {
        return std::format(" for stream index: {0}", index);
    }
}
