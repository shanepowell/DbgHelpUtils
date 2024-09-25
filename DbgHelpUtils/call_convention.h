// ReSharper disable IdentifierTypo
#pragma once

namespace dlg_help_utils::dbg_help
{
    enum class calling_convention : uint8_t
    {
        near_c      = 0x00, // near right to left push, caller pops stack
        far_c       = 0x01, // far right to left push, caller pops stack
        near_pascal = 0x02, // near left to right push, callee pops stack
        far_pascal  = 0x03, // far left to right push, callee pops stack
        near_fast   = 0x04, // near left to right push with regs, callee pops stack
        far_fast    = 0x05, // far left to right push with regs, callee pops stack
        skipped     = 0x06, // skipped (unused) call index
        near_std    = 0x07, // near standard call
        far_std     = 0x08, // far standard call
        near_sys    = 0x09, // near sys call
        far_sys     = 0x0a, // far sys call
        thiscall    = 0x0b, // this call (this passed in register)
        mipscall    = 0x0c, // Mips call
        generic     = 0x0d, // Generic call sequence
        alphacall   = 0x0e, // Alpha call
        ppccall     = 0x0f, // PPC call
        shcall      = 0x10, // Hitachi SuperH call
        armcall     = 0x11, // ARM call
        am33call    = 0x12, // AM33 call
        tricall     = 0x13, // TriCore Call
        sh5_call     = 0x14, // Hitachi SuperH-5 call
        m32rcall    = 0x15, // M32R Call
        clrcall     = 0x16, // clr call
        inlinecall  = 0x17, // Marker for routines always inlined and thus lacking a convention
        near_vector = 0x18, // near left to right push with regs, callee pops stack
        swift       = 0x19, // Swift calling convention
        reserved    = 0x20  // first unused call enumeration
    };
}
