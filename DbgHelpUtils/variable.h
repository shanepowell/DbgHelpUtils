#pragma once

// ReSharper disable once CommentTypo
// Found in "$(VSINSTALLDIR)\DIA SDK\include"
#include <cvconst.h>

#include <variant>

#include "float80.h"
#include "int128.h"
#include "mini_dump_memory_stream.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::dbg_help
{
    using reg_value_t = std::variant<uint8_t, uint16_t, uint32_t, uint64_t, std::uint128_t, float, double, float80_t>;

    struct registry_info
    {
        CV_HREG_e register_type{};
        reg_value_t value{};
    };

    struct frame_data_info
    {
        int32_t data_offset{};
        uint64_t data_address{};
        uint32_t data_size{};
    };

    struct variable
    {
        symbol_type_info symbol_info;
        std::optional<registry_info> registry_value{};
        std::optional<frame_data_info> frame_data{};
    };

    inline mini_dump_memory_stream to_stream(reg_value_t const& value)
    {
        mini_dump_memory_stream stream;
        std::visit([&stream](auto&& arg) { stream = {&arg, sizeof(arg)}; }, value);
        return stream;
    }
}
