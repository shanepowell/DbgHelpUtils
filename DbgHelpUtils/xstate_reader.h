#pragma once

#include <generator>

#include "windows_setup.h"
#include "stream_thread_context.h"

namespace dlg_help_utils
{

    class xstate_reader
    {
    public:
        xstate_reader(stream_thread_context::context_x64 const* context = nullptr);
        xstate_reader(WOW64_CONTEXT const* context);

        [[nodiscard]] bool is_supported() const;
        [[nodiscard]] bool is_in_init_state() const;

        struct ymm_register
        {
            uint32_t index;
            PM128A xmm;
            PM128A ymm;
        };
        [[nodiscard]] std::generator<ymm_register> ymm_registers() const;
        [[nodiscard]] ymm_register get_ymm_register(uint32_t index) const;

    private:
        void const* context_;
        bool supported_;
    };
}