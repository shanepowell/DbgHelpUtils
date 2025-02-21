#pragma once

#include "mini_dump_memory_walker.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker_release_handle
    {
    public:
        mini_dump_memory_walker_release_handle(mini_dump_memory_walker& walker)
            : walker_{&walker}
        {
            walker_->release_handle();
        }

        mini_dump_memory_walker_release_handle(mini_dump_memory_walker_release_handle&&) = default;
        mini_dump_memory_walker_release_handle& operator=(mini_dump_memory_walker_release_handle&&) = default;

        mini_dump_memory_walker_release_handle(mini_dump_memory_walker_release_handle const&) = delete;
        mini_dump_memory_walker_release_handle& operator=(mini_dump_memory_walker_release_handle const&) = delete;

        ~mini_dump_memory_walker_release_handle()
        {
            walker_->create_handle();
        }

    private:
        mini_dump_memory_walker* walker_;
    };
}