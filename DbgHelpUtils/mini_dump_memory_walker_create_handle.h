#pragma once

#include "mini_dump_memory_walker.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker_create_handle
    {
    public:
        mini_dump_memory_walker_create_handle(mini_dump_memory_walker& walker)
            : walker_{&walker}
        {
            walker_->create_handle();
        }

        mini_dump_memory_walker_create_handle(mini_dump_memory_walker_create_handle&&) = default;
        mini_dump_memory_walker_create_handle& operator=(mini_dump_memory_walker_create_handle&&) = default;

        mini_dump_memory_walker_create_handle(mini_dump_memory_walker_create_handle const&) = delete;
        mini_dump_memory_walker_create_handle& operator=(mini_dump_memory_walker_create_handle const&) = delete;

        ~mini_dump_memory_walker_create_handle()
        {
            walker_->release_handle();
        }

    private:
        mini_dump_memory_walker* walker_;
    };
}