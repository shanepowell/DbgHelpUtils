#include "common_symbol_utils.h"

#include "stream_utils.h"
#include "thread_ex_list_stream.h"
#include "thread_list_stream.h"

namespace dlg_help_utils::common_symbol_utils
{

    std::vector<uint64_t> get_teb_addresses(mini_dump const& mini_dump)
    {
        std::vector<uint64_t> teb_addresses;
        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadListStream);
            stream.has_value())
        {
            if (thread_list_stream const thread_list{ mini_dump, std::get<size_t>(stream.value()) }; 
                thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                for (auto const& thread : thread_list.list())
                {
                    if (thread->Teb != 0)
                    {
                        teb_addresses.emplace_back(thread->Teb);
                    }
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadExListStream);
            stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ mini_dump, std::get<size_t>(stream.value()) }; 
                thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                for (auto const& thread : thread_ex_list.list())
                {
                    if (thread->Teb != 0)
                    {
                        teb_addresses.emplace_back(thread->Teb);
                    }
                }
            }
        }

        return teb_addresses;
    }

    void gather_system_addresses(mini_dump const& mini_dump, std::set<uint64_t>& system_area_addresses)
    {
        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadListStream);
            stream.has_value())
        {
            if (thread_list_stream const thread_list{ mini_dump, std::get<size_t>(stream.value()) };
                thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                for (auto const& thread : thread_list.list())
                {
                    if (thread->Teb != 0)
                    {
                        system_area_addresses.insert(thread->Teb);
                        if (thread->Stack.Memory.Rva == 0)
                        {
                            system_area_addresses.insert(thread->Stack.StartOfMemoryRange);
                        }
                    }
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadExListStream);
            stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ mini_dump, std::get<size_t>(stream.value()) }; 
                thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                for (auto const& thread : thread_ex_list.list())
                {
                    if (thread->Teb != 0)
                    {
                        system_area_addresses.insert(thread->Teb);
                        if (thread->Stack.Memory.Rva == 0)
                        {
                            system_area_addresses.insert(thread->Stack.StartOfMemoryRange);
                        }
                    }
                }
            }
        }
    }
}
