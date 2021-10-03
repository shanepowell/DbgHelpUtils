#pragma once

#include "windows_setup.h"
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class mini_dump;

    class stream_thread_context
    {
    public:
        stream_thread_context() = default;
        explicit stream_thread_context(mini_dump const& dump, MINIDUMP_LOCATION_DESCRIPTOR const& location);

#ifdef _WIN64

#define SIZE_OF_80387_REGISTERS      80
#define MAXIMUM_SUPPORTED_EXTENSION     512
        typedef struct {
            DWORD   ControlWord;
            DWORD   StatusWord;
            DWORD   TagWord;
            DWORD   ErrorOffset;
            DWORD   ErrorSelector;
            DWORD   DataOffset;
            DWORD   DataSelector;
            BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
            DWORD   Spare0;
        } FLOATING_SAVE_AREA;

#pragma pack(push,4)

        //
        // Context Frame
        //
        //  This frame has a several purposes: 1) it is used as an argument to
        //  NtContinue, 2) is is used to constuct a call frame for APC delivery,
        //  and 3) it is used in the user level thread creation routines.
        //
        //  The layout of the record conforms to a standard call frame.
        //

        typedef struct _X86_CONTEXT {

            //
            // The flags values within this flag control the contents of
            // a CONTEXT record.
            //
            // If the context record is used as an input parameter, then
            // for each portion of the context record controlled by a flag
            // whose value is set, it is assumed that that portion of the
            // context record contains valid context. If the context record
            // is being used to modify a threads context, then only that
            // portion of the threads context will be modified.
            //
            // If the context record is used as an IN OUT parameter to capture
            // the context of a thread, then only those portions of the thread's
            // context corresponding to set flags will be returned.
            //
            // The context record is never used as an OUT only parameter.
            //

            DWORD ContextFlags;

            //
            // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
            // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
            // included in CONTEXT_FULL.
            //

            DWORD   Dr0;
            DWORD   Dr1;
            DWORD   Dr2;
            DWORD   Dr3;
            DWORD   Dr6;
            DWORD   Dr7;

            //
            // This section is specified/returned if the
            // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
            //

            FLOATING_SAVE_AREA FloatSave;

            //
            // This section is specified/returned if the
            // ContextFlags word contians the flag CONTEXT_SEGMENTS.
            //

            DWORD   SegGs;
            DWORD   SegFs;
            DWORD   SegEs;
            DWORD   SegDs;

            //
            // This section is specified/returned if the
            // ContextFlags word contians the flag CONTEXT_INTEGER.
            //

            DWORD   Edi;
            DWORD   Esi;
            DWORD   Ebx;
            DWORD   Edx;
            DWORD   Ecx;
            DWORD   Eax;

            //
            // This section is specified/returned if the
            // ContextFlags word contians the flag CONTEXT_CONTROL.
            //

            DWORD   Ebp;
            DWORD   Eip;
            DWORD   SegCs;              // MUST BE SANITIZED
            DWORD   EFlags;             // MUST BE SANITIZED
            DWORD   Esp;
            DWORD   SegSs;

            //
            // This section is specified/returned if the ContextFlags word
            // contains the flag CONTEXT_EXTENDED_REGISTERS.
            // The format and contexts are processor specific
            //

            BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

        } X86_CONTEXT;

#pragma pack(pop)

        using context_x86 = X86_CONTEXT;
        using context_x64 = CONTEXT;
#else

#pragma warning(push)
#pragma warning(disable : 4201)
        typedef struct DECLSPEC_ALIGN(16) {
            WORD   ControlWord;
            WORD   StatusWord;
            BYTE  TagWord;
            BYTE  Reserved1;
            WORD   ErrorOpcode;
            DWORD ErrorOffset;
            WORD   ErrorSelector;
            WORD   Reserved2;
            DWORD DataOffset;
            WORD   DataSelector;
            WORD   Reserved3;
            DWORD MxCsr;
            DWORD MxCsr_Mask;
            M128A FloatRegisters[8];

            M128A XmmRegisters[16];
            BYTE  Reserved4[96];

        } XSAVE_FORMAT;

        typedef XSAVE_FORMAT XMM_SAVE_AREA32;
        typedef struct DECLSPEC_ALIGN(16) {

            //
            // Register parameter home addresses.
            //
            // N.B. These fields are for convience - they could be used to extend the
            //      context record in the future.
            //

            DWORD64 P1Home;
            DWORD64 P2Home;
            DWORD64 P3Home;
            DWORD64 P4Home;
            DWORD64 P5Home;
            DWORD64 P6Home;

            //
            // Control flags.
            //

            DWORD ContextFlags;
            DWORD MxCsr;

            //
            // Segment Registers and processor flags.
            //

            WORD   SegCs;
            WORD   SegDs;
            WORD   SegEs;
            WORD   SegFs;
            WORD   SegGs;
            WORD   SegSs;
            DWORD EFlags;

            //
            // Debug registers
            //

            DWORD64 Dr0;
            DWORD64 Dr1;
            DWORD64 Dr2;
            DWORD64 Dr3;
            DWORD64 Dr6;
            DWORD64 Dr7;

            //
            // Integer registers.
            //

            DWORD64 Rax;
            DWORD64 Rcx;
            DWORD64 Rdx;
            DWORD64 Rbx;
            DWORD64 Rsp;
            DWORD64 Rbp;
            DWORD64 Rsi;
            DWORD64 Rdi;
            DWORD64 R8;
            DWORD64 R9;
            DWORD64 R10;
            DWORD64 R11;
            DWORD64 R12;
            DWORD64 R13;
            DWORD64 R14;
            DWORD64 R15;

            //
            // Program counter.
            //

            DWORD64 Rip;

            //
            // Floating point state.
            //

            union {
                XMM_SAVE_AREA32 FltSave;
                struct {
                    M128A Header[2];
                    M128A Legacy[8];
                    M128A Xmm0;
                    M128A Xmm1;
                    M128A Xmm2;
                    M128A Xmm3;
                    M128A Xmm4;
                    M128A Xmm5;
                    M128A Xmm6;
                    M128A Xmm7;
                    M128A Xmm8;
                    M128A Xmm9;
                    M128A Xmm10;
                    M128A Xmm11;
                    M128A Xmm12;
                    M128A Xmm13;
                    M128A Xmm14;
                    M128A Xmm15;
                } DUMMYSTRUCTNAME;
            } DUMMYUNIONNAME;

            //
            // Vector registers.
            //

            M128A VectorRegister[26];
            DWORD64 VectorControl;

            //
            // Special debug control registers.
            //

            DWORD64 DebugControl;
            DWORD64 LastBranchToRip;
            DWORD64 LastBranchFromRip;
            DWORD64 LastExceptionToRip;
            DWORD64 LastExceptionFromRip;
        } X64_CONTEXT;
#pragma warning(pop)

        using context_x86 = CONTEXT;
        using context_x64 = X64_CONTEXT;
#endif

        [[nodiscard]] void const* context() const { return context_; }
        [[nodiscard]] size_t size() const { return size_; }
        [[nodiscard]] bool x86_thread_context_available() const { return x86_thread_context_available_; }
        [[nodiscard]] context_x86 const& x86_thread_context() const { return *x86_thread_context_; }
        [[nodiscard]] bool wow64_thread_context_available() const { return wow64_thread_context_available_; }
        [[nodiscard]] WOW64_CONTEXT const& wow64_thread_context() const { return *wow64_thread_context_; }
        [[nodiscard]] bool x64_thread_context_available() const { return x64_thread_context_available_; }
        [[nodiscard]] context_x64 const& x64_thread_context() const { return *x64_thread_context_; }

        [[nodiscard]] bool x86_thread_context_has_extended_registers() const
        {
            return x86_thread_context_has_extended_registers_;
        }

        [[nodiscard]] bool wow64_thread_context_has_extended_registers() const
        {
            return wow64_thread_context_has_extended_registers_;
        }

    private:
        void const* context_{nullptr};
        size_t size_{};
        bool x86_thread_context_available_{false};
        context_x86 const* x86_thread_context_{nullptr};
        bool wow64_thread_context_available_{false};
        WOW64_CONTEXT const* wow64_thread_context_{nullptr};
        bool x64_thread_context_available_{false};
        context_x64 const* x64_thread_context_{nullptr};
        bool x86_thread_context_has_extended_registers_{false};
        bool wow64_thread_context_has_extended_registers_{false};
    };
}
