// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "symbol_engine.h"

#include <array>
#include <filesystem>
#include <format>
#include <ranges>

// ReSharper disable once CommentTypo
// Found in "$(VSINSTALLDIR)\DIA SDK\include"
#include <cvconst.h>

#include "cv_info_pdb70.h"
#include "exit_scope.h"
#include "guid_utils.h"
#include "hex_dump.h"
#include "locale_number_formatting.h"
#include "local_variable.h"
#include "module_match.h"
#include "pe_file.h"
#include "stream_hex_dump.h"
#include "stream_thread_context.h"
#include "symbol_info_buffer.h"
#include "symbol_type_info.h"
#include "sym_tag_enum.h"
#include "wide_runtime_error.h"
#include "windows_error.h"

#pragma comment(lib, "dbghelp.lib")

auto constexpr cba_start_op_maybe = 0xA0000000;

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::stream_hex_dump;

namespace
{
    size_t constexpr trace_max_function_name_length{1000};
    size_t constexpr max_buffer_size = sizeof(SYMBOL_INFOW) + (trace_max_function_name_length * sizeof(wchar_t));
    auto constexpr progress_xml_start = L"<Progress percent=\""sv;
    auto constexpr progress_xml_end = L"\"/>\n"sv;
    auto constexpr downloading_xml_start = L"<Activity name=\"Downloading file "sv;
    auto constexpr downloading_xml_end = L"\" details=\""sv;

    dlg_help_utils::dbg_help::i_stack_walk_callback* g_callback{nullptr};

#pragma pack(push, 1)
    struct symbol_load_w64
    {
        [[maybe_unused]] DWORD size_of_struct; // set to sizeof(symbol_load_w64)
        DWORD64 base_of_image; // base load address of module
        DWORD check_sum; // checksum from the pe header
        DWORD time_date_stamp; // date/time stamp from pe header
        DWORD unknown;
        WCHAR file_name[133]; // symbols file or image name
        uint16_t reparse; // load failure reparse
        uint64_t h_file; // file handle, if passed
        DWORD flags;
    };

    struct symbol_load
    {
        [[maybe_unused]] DWORD size_of_struct; // set to sizeof(symbol_load)
        DWORD64 base_of_image; // base load address of module
        DWORD check_sum; // checksum from the pe header
        DWORD time_date_stamp; // date/time stamp from pe header
        DWORD unknown1;
    };

    // ReSharper disable CppStaticAssertFailure
    static_assert(sizeof(symbol_load) == 24);
    static_assert(sizeof(symbol_load_w64) == 304);
    // ReSharper restore CppStaticAssertFailure
#pragma pack(pop)

    // ReSharper disable CppParameterMayBeConst
    BOOL CALLBACK read_process_memory_routine([[maybe_unused]] HANDLE h_process, DWORD64 lp_base_address, PVOID lp_buffer, DWORD n_size, PDWORD lp_number_of_bytes_read)
    {
        return g_callback->read_process_memory(lp_base_address, lp_buffer, n_size, lp_number_of_bytes_read);
    }

    // ReSharper restore CppParameterMayBeConst

    DWORD64 CALLBACK get_module_base_routine([[maybe_unused]] HANDLE h_process, DWORD64 const address)
    {
        return g_callback->get_module_base_routine(address);
    }

    // ReSharper disable CppParameterMayBeConst
    PVOID CALLBACK function_table_access_routine(HANDLE h_process, DWORD64 const address_base)
    {
        auto* result = g_callback->function_table_access(address_base);
        if (result != nullptr) return result;

        result = SymFunctionTableAccess64AccessRoutines(h_process, address_base, read_process_memory_routine, get_module_base_routine);
        return result;
    }

    // ReSharper restore CppParameterMayBeConst

    // ReSharper disable CppParameterMayBeConst
    DWORD64 CALLBACK translate_address([[maybe_unused]] _In_ HANDLE h_process, _In_ HANDLE h_thread, _In_ LPADDRESS64 lp_address)
    {
        return g_callback->translate_address(h_thread, lp_address);
    }

    // ReSharper restore CppParameterMayBeConst

    std::wstring_view action_code_to_string(ULONG const action_code)
    {
        switch (action_code)
        {
        case CBA_DEFERRED_SYMBOL_LOAD_START:
            return L"CBA_DEFERRED_SYMBOL_LOAD_START"sv;

        case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
            return L"CBA_DEFERRED_SYMBOL_LOAD_COMPLETE"sv;

        case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
            return L"CBA_DEFERRED_SYMBOL_LOAD_FAILURE"sv;

        case CBA_SYMBOLS_UNLOADED:
            return L"CBA_SYMBOLS_UNLOADED"sv;

        case CBA_DUPLICATE_SYMBOL:
            return L"CBA_DUPLICATE_SYMBOL"sv;

        case CBA_READ_MEMORY:
            return L"CBA_READ_MEMORY"sv;

        case CBA_DEFERRED_SYMBOL_LOAD_CANCEL:
            return L"CBA_DEFERRED_SYMBOL_LOAD_CANCEL"sv;

        case CBA_SET_OPTIONS:
            return L"CBA_SET_OPTIONS"sv;

        case CBA_EVENT:
            return L"CBA_EVENT"sv;

        case CBA_DEFERRED_SYMBOL_LOAD_PARTIAL:
            return L"CBA_DEFERRED_SYMBOL_LOAD_PARTIAL"sv;

        case CBA_DEBUG_INFO:
            return L"CBA_DEBUG_INFO"sv;

        case CBA_SRCSRV_INFO:
            return L"CBA_SRCSRV_INFO"sv;

        case CBA_SRCSRV_EVENT:
            return L"CBA_SRCSRV_EVENT"sv;

        case CBA_UPDATE_STATUS_BAR:
            return L"CBA_UPDATE_STATUS_BAR"sv;

        case CBA_ENGINE_PRESENT:
            return L"CBA_ENGINE_PRESENT"sv;

        case CBA_CHECK_ENGOPT_DISALLOW_NETWORK_PATHS:
            return L"CBA_CHECK_ENGOPT_DISALLOW_NETWORK_PATHS"sv;

        case CBA_CHECK_ARM_MACHINE_THUMB_TYPE_OVERRIDE:
            return L"CBA_CHECK_ARM_MACHINE_THUMB_TYPE_OVERRIDE"sv;

        case CBA_XML_LOG:
            return L"CBA_XML_LOG"sv;

        case cba_start_op_maybe:
            return L"cba_start_op_maybe"sv;

        default:
            break;
        }

        return L"unknown event type"sv;
    }

    std::wstring_view severity_to_string(DWORD const severity)
    {
        switch (severity)
        {
        case sevInfo:
            return L"info"sv;

        case sevProblem:
            return L"problem"sv;

        case sevAttn:
            return L"attn"sv;

        case sevFatal:
            return L"fatal"sv;

        case sevMax:
            return L"max"sv;

        default:
            break;
        }

        return L"unknown severity type"sv;
    }

    std::wstring_view get_symbol_load_filename(ULONG64 const callback_data, std::wstring_view const download_module)
    {
        if (!download_module.empty())
        {
            return download_module;
        }

        if (callback_data == 0)
        {
            return {};
        }

        auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data);
        if (size_of_struct == sizeof(IMAGEHLP_DEFERRED_SYMBOL_LOADW64))
        {
            auto const* evt = reinterpret_cast<IMAGEHLP_DEFERRED_SYMBOL_LOADW64 const*>(callback_data);
            return evt->FileName;
        }
        if (size_of_struct == sizeof(symbol_load_w64))
        {
            auto const* evt = reinterpret_cast<symbol_load_w64 const*>(callback_data);
            return evt->file_name;
        }
        return {};
    }

    void dump_sizeof_struct_data(ULONG64 const callback_data,
                                 dlg_help_utils::dbg_help::i_symbol_load_callback const& callback)
    {
        if (callback.symbol_load_debug_memory() && callback_data != 0)
        {
            auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data);
            auto const* data = reinterpret_cast<uint8_t const*>(callback_data);
            dlg_help_utils::hex_dump::hex_dump(callback.log_stream(), data, size_of_struct, 5);
        }
    }

    BOOL CALLBACK sym_register_callback_proc64([[maybe_unused]] __in HANDLE h_process, __in ULONG const action_code,
                                               __in_opt ULONG64 const callback_data,
                                               __in_opt ULONG64 const user_context)
    {
        auto& symbol_engine = *reinterpret_cast<dlg_help_utils::dbg_help::i_symbol_callback*>(user_context);
        auto& callback = symbol_engine.callback();
        // ReSharper disable once CommentTypo
        // If SYMOPT_DEBUG is set, then the symbol handler will pass
        // verbose information on its attempt to load symbols.
        // This information be delivered as text strings.

        if (callback.symbol_load_debug())
        {
            callback.log_stream() << std::format(L"DlgHelp: action_code: {0} ({1} callback_data:({2})\n", action_code_to_string(action_code), to_hex(action_code), to_hex(callback_data));
        }

        if (callback_data == 0) return FALSE;

        switch (action_code)
        {
        case CBA_EVENT:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_EVENTW const*>(callback_data);
                callback.log_stream() << std::format(L"DlgHelp: {0}:[{1}]:({2})[{3}] {4}\n", action_code_to_string(action_code), severity_to_string(evt->severity), to_hex(evt->code), evt->desc, to_hex(evt->object));
            }
            return TRUE;

        case CBA_DEFERRED_SYMBOL_LOAD_START:
        case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
        case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                if (auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data); size_of_struct == sizeof(IMAGEHLP_DEFERRED_SYMBOL_LOADW64))
                {
                    auto const* evt = reinterpret_cast<IMAGEHLP_DEFERRED_SYMBOL_LOADW64 const*>(callback_data);
                    callback.log_stream() << 
                        std::format(L"DlgHelp: {0}[default]:BaseOfImage[{1}]:CheckSum[{2}]:TimeDateStamp[{3}]:FileName[{4}]:Reparse[{5}]:hFile[{6}]:Flags[{7}]\n"
                            , action_code_to_string(action_code)
                            , to_hex(evt->BaseOfImage)
                            , to_hex(evt->CheckSum)
                            , to_hex(evt->TimeDateStamp)
                            , evt->FileName
                            , evt->Reparse
                            , to_hex(evt->hFile)
                            , to_hex(evt->Flags));
                }
                else if (size_of_struct == sizeof(symbol_load_w64))
                {
                    auto const* evt = reinterpret_cast<symbol_load_w64 const*>(callback_data);
                    callback.log_stream() << 
                        std::format(L"DlgHelp: {0}[symbol_load_w64]:BaseOfImage[{1}]:CheckSum[{2}]:TimeDateStamp[{3}]:Unknown[{4}]:FileName[{5}]:Reparse[{6}]:hFile[{7}]:Flags[{8}]\n"
                            , action_code_to_string(action_code)
                            , to_hex(evt->base_of_image)
                            , to_hex(evt->check_sum)
                            , to_hex(evt->time_date_stamp)
                            , to_hex(evt->unknown)
                            , evt->file_name
                            , evt->reparse
                            , to_hex(evt->h_file)
                            , to_hex(evt->flags));
                }
                else
                {
                    callback.log_stream() << 
                        std::format(L"DlgHelp: {0}: unknown event data size: {1} != {2}|{3}\n"
                            , action_code_to_string(action_code)
                            , locale_formatting::to_wstring(size_of_struct)
                            , locale_formatting::to_wstring(sizeof(IMAGEHLP_DEFERRED_SYMBOL_LOADW64))
                            , locale_formatting::to_wstring(sizeof(symbol_load_w64)));
                }
            }
            dump_sizeof_struct_data(callback_data, callback);
            break;

        case CBA_DEFERRED_SYMBOL_LOAD_PARTIAL:
            {
                if (auto const filename = get_symbol_load_filename(callback_data, symbol_engine.downloading_module()); !filename.empty())
                {
                    callback.deferred_symbol_load_partial(filename);
                }

                dump_sizeof_struct_data(callback_data, callback);
            }
            break;

        case CBA_DEFERRED_SYMBOL_LOAD_CANCEL:
            {
                if (auto const filename = get_symbol_load_filename(callback_data, symbol_engine.downloading_module()); !filename.empty())
                {
                    if (callback.deferred_symbol_load_cancel(filename))
                    {
                        return TRUE;
                    }
                }

                dump_sizeof_struct_data(callback_data, callback);
            }
            break;

        case CBA_READ_MEMORY:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_READ_MEMORY const*>(callback_data);
                // ReSharper disable once StringLiteralTypo
                callback.log_stream() << std::format(L"DlgHelp: {0}:address[{1}]:bytes:[{2}]\n", action_code_to_string(action_code), to_hex(evt->addr), to_hex(evt->bytes));
            }

            if (g_callback != nullptr && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_READ_MEMORY const*>(callback_data);
                return g_callback->read_process_memory(evt->addr, evt->buf, evt->bytes, evt->bytesread,
                                                       !symbol_engine.loading_module());
            }
            break;

        case CBA_DUPLICATE_SYMBOL:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                if (auto const * evt = reinterpret_cast<IMAGEHLP_DUPLICATE_SYMBOL64 const*>(callback_data); evt->SizeOfStruct == sizeof(IMAGEHLP_DUPLICATE_SYMBOL64))
                {
                    // ReSharper disable once StringLiteralTypo
                    callback.log_stream() << std::format(L"DlgHelp: {0}:NumberOfDups[{1}]:SelectedSymbol[{2}]\n", action_code_to_string(action_code), locale_formatting::to_wstring(evt->NumberOfDups), locale_formatting::to_wstring(evt->SelectedSymbol));
                    callback.log_stream() << std::format(L"    Symbol: Address: [{}]\n", to_hex(evt->Symbol->Address));
                    callback.log_stream() << std::format(L"            Size: [{}]\n", locale_formatting::to_wstring(evt->Symbol->Size));
                    callback.log_stream() << std::format(L"            Flags: [{}]\n", to_hex(evt->Symbol->Flags));
                    callback.log_stream() << std::format(L"            Name: [{}]\n", dlg_help_utils::string_conversation::acp_to_wstring(evt->Symbol->Name));
                }
                else
                {
                    callback.log_stream() << std::format(L"DlgHelp: {0}: unknown event data size : {1} != {2}\n", action_code_to_string(action_code), locale_formatting::to_wstring(evt->SizeOfStruct), locale_formatting::to_wstring(sizeof(IMAGEHLP_DUPLICATE_SYMBOL64)));
                }
            }
            dump_sizeof_struct_data(callback_data, callback);
            break;

        case CBA_XML_LOG:
            if (callback_data != 0)
            {
                std::wstring_view const xml{reinterpret_cast<wchar_t const*>(callback_data)};

                if (callback.symbol_load_debug())
                {
                    callback.log_stream() << std::format(L"DlgHelp: {0}:{1}", action_code_to_string(action_code), xml);
                    if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                    {
                        callback.log_stream() << L'\n';
                    }
                }

                if (auto const download_file = symbol_engine.downloading_module(); !download_file.empty())
                {
                    if (xml.starts_with(progress_xml_start))
                    {
                        if (auto const end_pos = xml.find(progress_xml_end, progress_xml_start.size()); end_pos != std::wstring_view::npos)
                        {
                            auto const percent_str = std::wstring{
                                xml.substr(progress_xml_start.size(), end_pos - progress_xml_start.size())
                            };
                            auto const percent = std::stoul(percent_str);
                            callback.download_percent(percent);
                        }
                    }
                    else if (xml == L"</Activity>\n"sv)
                    {
                        symbol_engine.set_downloading_module({});
                        callback.download_complete();
                    }
                }
                else if (xml.starts_with(downloading_xml_start))
                {
                    if (auto const end_pos = xml.find(downloading_xml_end, downloading_xml_start.size()); end_pos != std::wstring_view::npos)
                    {
                        auto const module = xml.substr(downloading_xml_start.size(),
                                                       end_pos - downloading_xml_start.size());
                        symbol_engine.set_downloading_module(std::wstring{module});
                        callback.start_download(module);
                    }
                }
            }
            return TRUE;

        case CBA_SRCSRV_EVENT:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_EVENTW const*>(callback_data);
                callback.log_stream() << std::format(L"DlgHelp: {0}:[{1}]:({2})[{3}] {4}\n", action_code_to_string(action_code), severity_to_string(evt->severity), to_hex(evt->code), evt->desc, to_hex(evt->object));
            }
            return TRUE;

        case CBA_SRCSRV_INFO:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                std::wstring_view const xml{reinterpret_cast<wchar_t const*>(callback_data)};
                callback.log_stream() << std::format(L"DlgHelp: {0}:{1}", action_code_to_string(action_code), xml);
                if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                {
                    callback.log_stream() << L'\n';
                }
            }
            return TRUE;

        case cba_start_op_maybe:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                if (auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data); size_of_struct == sizeof(symbol_load))
                {
                    auto const* evt = reinterpret_cast<symbol_load const*>(callback_data);
                    callback.log_stream() << std::format(L"DlgHelp: {0}[symbol_load]:BaseOfImage[{1}]:CheckSum[{2}]:TimeDateStamp:[{3}]:Unknown:[{4}]\n", action_code_to_string(action_code), to_hex(evt->base_of_image), to_hex(evt->check_sum), to_hex(evt->time_date_stamp), to_hex(evt->unknown1));
                }
                else
                {
                    callback.log_stream() << std::format(L"DlgHelp: {0}: unknown event data size: {1} != {2}\n", action_code_to_string(action_code), locale_formatting::to_wstring(size_of_struct), locale_formatting::to_wstring(sizeof(symbol_load)));
                }
            }
            dump_sizeof_struct_data(callback_data, callback);
            break;

        default:
            if (callback_data != 0 && callback.symbol_load_debug_memory())
            {
                auto const* data = reinterpret_cast<uint8_t const*>(callback_data);
                dlg_help_utils::hex_dump::hex_dump(callback.log_stream(), data, 10, 5);
            }
            break;
        }

        // Return false to any ActionCode we don't handle
        // or we could generate some undesirable behavior.
        return FALSE;
    }

    // ReSharper disable CppParameterMayBeConst
    BOOL CALLBACK find_file_in_path_callback(_In_ PCWSTR filename, _In_ PVOID context)
    {
        auto const& symbol_engine = *static_cast<dlg_help_utils::dbg_help::i_symbol_callback const*>(context);
        if (symbol_engine.loading_module_check_sum() == 0)
        {
            return FALSE;
        }

        dlg_help_utils::pe_file module_file{filename};
        module_file.open_pe();

        if (module_file.is_valid())
        {
            if (module_file.is_x86_pe() && module_file.x86_pe_header().CheckSum == symbol_engine.
                loading_module_check_sum())
            {
                return FALSE;
            }

            if (module_file.is_x64_pe() && module_file.x64_pe_header().CheckSum == symbol_engine.
                loading_module_check_sum())
            {
                return FALSE;
            }
        }

        if (auto const & callback = symbol_engine.callback(); callback.symbol_load_debug())
        {
            callback.log_stream() << std::format(L"DlgHelp: FindFile: {} checksum check failure\n", filename);
        }

        return TRUE;
    }

    struct find_symbol_callback_context
    {
        find_symbol_callback_context(HANDLE process, std::vector<dlg_help_utils::dbg_help::symbol_type_info>& rv)
        : process{process}
        , rv{rv}
        {
        }

        HANDLE process;
        std::vector<dlg_help_utils::dbg_help::symbol_type_info>& rv;
    };

    BOOL CALLBACK find_symbol_callback(_In_ PSYMBOL_INFOW symbol_info, [[maybe_unused]] _In_ ULONG symbol_size, _In_opt_ PVOID user_context)
    {
        find_symbol_callback_context& symbols{*static_cast<find_symbol_callback_context*>(user_context)};
        symbols.rv.emplace_back(symbols.process, symbol_info->ModBase, symbol_info->Index);
        return TRUE;
    }

    struct local_variable_info
    {
        local_variable_info(HANDLE process, dlg_help_utils::dbg_help::thread_context_type type, uint64_t const frame_address_offset, void const* thread_context, std::vector<dlg_help_utils::dbg_help::local_variable> &locals, std::vector<dlg_help_utils::dbg_help::local_variable> &parameters)
        : process{process}
        , type{type}
        , frame_address_offset{frame_address_offset}
        , thread_context{thread_context}
        , locals{locals}
        , parameters{parameters}
        {
        }

        HANDLE process;
        dlg_help_utils::dbg_help::thread_context_type type;
        uint64_t frame_address_offset;
        void const* thread_context;
        std::vector<dlg_help_utils::dbg_help::local_variable> &locals;
        std::vector<dlg_help_utils::dbg_help::local_variable> &parameters;
    };

    template<typename T>
    std::pair<uint64_t,uint64_t> make_register_value(uint64_t const raw_value)
    {
        constexpr uint64_t size_masks[] =
        {
            0x0,                        // 0
            0xFF,                       // 1
            0xFFFF,                     // 2
            0xFFFFFF,                   // 3
            0xFFFFFFFF,                 // 4
            0xFFFFFFFFFF,               // 5
            0xFFFFFFFFFFFF,             // 6
            0xFFFFFFFFFFFFFF,           // 7
            0xFFFFFFFFFFFFFFFF,         // 8
        };
        return std::make_pair<uint64_t,uint64_t>(raw_value & size_masks[sizeof T], sizeof T);
    }

    std::pair<uint64_t,uint64_t> get_x86_register_value(CV_HREG_e register_type, dlg_help_utils::stream_thread_context::context_x86 const& context)
    {
        switch(register_type) // NOLINT
        {
        case CV_AMD64_AL:
            return make_register_value<uint8_t>(context.Eax);

        case CV_AMD64_BL:
            return make_register_value<uint8_t>(context.Ebx);

        case CV_AMD64_CL:
            return make_register_value<uint8_t>(context.Ecx);

        case CV_AMD64_DL:
            return make_register_value<uint8_t>(context.Edx);

        case CV_AMD64_AX:
            return make_register_value<uint16_t>(context.Eax);

        case CV_AMD64_BX:
            return make_register_value<uint16_t>(context.Ebx);

        case CV_AMD64_CX:
            return make_register_value<uint16_t>(context.Ecx);

        case CV_AMD64_DX:
            return make_register_value<uint16_t>(context.Edx);

        case CV_AMD64_SP:
            return make_register_value<uint16_t>(context.Esp);

        case CV_AMD64_BP:
            return make_register_value<uint16_t>(context.Ebp);

        case CV_AMD64_SI:
            return make_register_value<uint16_t>(context.Esi);

        case CV_AMD64_DI:
            return make_register_value<uint16_t>(context.Edi);

        case CV_AMD64_EAX:
            return make_register_value<uint32_t>(context.Eax);

        case CV_AMD64_EBX:
            return make_register_value<uint32_t>(context.Ebx);

        case CV_AMD64_ECX:
            return make_register_value<uint32_t>(context.Ecx);

        case CV_AMD64_EDX:
            return make_register_value<uint32_t>(context.Edx);

        case CV_AMD64_ESP:
            return make_register_value<uint32_t>(context.Esp);

        case CV_AMD64_EBP:
            return make_register_value<uint32_t>(context.Ebp);

        case CV_AMD64_ESI:
            return make_register_value<uint32_t>(context.Esi);

        case CV_AMD64_EDI:
            return make_register_value<uint32_t>(context.Edi);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in x86 context", static_cast<uint32_t>(register_type))};
        }
    }

    std::pair<uint64_t,uint64_t> get_wow64_register_value(CV_HREG_e register_type, WOW64_CONTEXT const& context)
    {
        switch(register_type) // NOLINT
        {
        case CV_AMD64_AL:
            return make_register_value<uint8_t>(context.Eax);

        case CV_AMD64_BL:
            return make_register_value<uint8_t>(context.Ebx);

        case CV_AMD64_CL:
            return make_register_value<uint8_t>(context.Ecx);

        case CV_AMD64_DL:
            return make_register_value<uint8_t>(context.Edx);

        case CV_AMD64_AX:
            return make_register_value<uint16_t>(context.Eax);

        case CV_AMD64_BX:
            return make_register_value<uint16_t>(context.Ebx);

        case CV_AMD64_CX:
            return make_register_value<uint16_t>(context.Ecx);

        case CV_AMD64_DX:
            return make_register_value<uint16_t>(context.Edx);

        case CV_AMD64_SP:
            return make_register_value<uint16_t>(context.Esp);

        case CV_AMD64_BP:
            return make_register_value<uint16_t>(context.Ebp);

        case CV_AMD64_SI:
            return make_register_value<uint16_t>(context.Esi);

        case CV_AMD64_DI:
            return make_register_value<uint16_t>(context.Edi);

        case CV_AMD64_EAX:
            return make_register_value<uint32_t>(context.Eax);

        case CV_AMD64_EBX:
            return make_register_value<uint32_t>(context.Ebx);

        case CV_AMD64_ECX:
            return make_register_value<uint32_t>(context.Ecx);

        case CV_AMD64_EDX:
            return make_register_value<uint32_t>(context.Edx);

        case CV_AMD64_ESP:
            return make_register_value<uint32_t>(context.Esp);

        case CV_AMD64_EBP:
            return make_register_value<uint32_t>(context.Ebp);

        case CV_AMD64_ESI:
            return make_register_value<uint32_t>(context.Esi);

        case CV_AMD64_EDI:
            return make_register_value<uint32_t>(context.Edi);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in WOW64 (x86) context", static_cast<uint32_t>(register_type))};
        }
    }

    std::pair<uint64_t,uint64_t> get_x64_register_value(CV_HREG_e register_type, dlg_help_utils::stream_thread_context::context_x64 const& context)
    {
        switch(register_type) // NOLINT
        {
        case CV_AMD64_AL:
            return make_register_value<uint8_t>(context.Rax);

        case CV_AMD64_BL:
            return make_register_value<uint8_t>(context.Rbx);

        case CV_AMD64_CL:
            return make_register_value<uint8_t>(context.Rcx);

        case CV_AMD64_DL:
            return make_register_value<uint8_t>(context.Rdx);

        case CV_AMD64_AX:
            return make_register_value<uint16_t>(context.Rax);

        case CV_AMD64_BX:
            return make_register_value<uint16_t>(context.Rbx);

        case CV_AMD64_CX:
            return make_register_value<uint16_t>(context.Rcx);

        case CV_AMD64_DX:
            return make_register_value<uint16_t>(context.Rdx);

        case CV_AMD64_SP:
            return make_register_value<uint16_t>(context.Rsp);

        case CV_AMD64_BP:
            return make_register_value<uint16_t>(context.Rbp);

        case CV_AMD64_SI:
            return make_register_value<uint16_t>(context.Rsi);

        case CV_AMD64_DI:
            return make_register_value<uint16_t>(context.Rdi);

        case CV_AMD64_EAX:
            return make_register_value<uint32_t>(context.Rax);

        case CV_AMD64_EBX:
            return make_register_value<uint32_t>(context.Rbx);

        case CV_AMD64_ECX:
            return make_register_value<uint32_t>(context.Rcx);

        case CV_AMD64_EDX:
            return make_register_value<uint32_t>(context.Rdx);

        case CV_AMD64_ESP:
            return make_register_value<uint32_t>(context.Rsp);

        case CV_AMD64_EBP:
            return make_register_value<uint32_t>(context.Rbp);

        case CV_AMD64_ESI:
            return make_register_value<uint32_t>(context.Rsi);

        case CV_AMD64_EDI:
            return make_register_value<uint32_t>(context.Rdi);

        case CV_AMD64_RAX:
            return make_register_value<uint64_t>(context.Rax);

        case CV_AMD64_RBX:
            return make_register_value<uint64_t>(context.Rbx);

        case CV_AMD64_RCX:
            return make_register_value<uint64_t>(context.Rcx);

        case CV_AMD64_RDX:
            return make_register_value<uint64_t>(context.Rdx);

        case CV_AMD64_RSP:
            return make_register_value<uint64_t>(context.Rsp);

        case CV_AMD64_RBP:
            return make_register_value<uint64_t>(context.Rbp);

        case CV_AMD64_RSI:
            return make_register_value<uint64_t>(context.Rsi);

        case CV_AMD64_RDI:
            return make_register_value<uint64_t>(context.Rdi);

        case CV_AMD64_R8B:
            return make_register_value<uint8_t>(context.R8);

        case CV_AMD64_R9B:
            return make_register_value<uint8_t>(context.R9);

        case CV_AMD64_R10B:
            return make_register_value<uint8_t>(context.R10);

        case CV_AMD64_R11B:
            return make_register_value<uint8_t>(context.R11);

        case CV_AMD64_R12B:
            return make_register_value<uint8_t>(context.R12);

        case CV_AMD64_R13B:
            return make_register_value<uint8_t>(context.R13);

        case CV_AMD64_R14B:
            return make_register_value<uint8_t>(context.R14);

        case CV_AMD64_R15B:
            return make_register_value<uint8_t>(context.R15);

        case CV_AMD64_R8W:
            return make_register_value<uint16_t>(context.R8);

        case CV_AMD64_R9W:
            return make_register_value<uint16_t>(context.R9);

        case CV_AMD64_R10W:
            return make_register_value<uint16_t>(context.R10);

        case CV_AMD64_R11W:
            return make_register_value<uint16_t>(context.R11);

        case CV_AMD64_R12W:
            return make_register_value<uint16_t>(context.R12);

        case CV_AMD64_R13W:
            return make_register_value<uint16_t>(context.R13);

        case CV_AMD64_R14W:
            return make_register_value<uint16_t>(context.R14);

        case CV_AMD64_R15W:
            return make_register_value<uint16_t>(context.R15);

        case CV_AMD64_R8D:
            return make_register_value<uint32_t>(context.R8);

        case CV_AMD64_R9D:
            return make_register_value<uint32_t>(context.R9);

        case CV_AMD64_R10D:
            return make_register_value<uint32_t>(context.R10);

        case CV_AMD64_R11D:
            return make_register_value<uint32_t>(context.R11);

        case CV_AMD64_R12D:
            return make_register_value<uint32_t>(context.R12);

        case CV_AMD64_R13D:
            return make_register_value<uint32_t>(context.R13);

        case CV_AMD64_R14D:
            return make_register_value<uint32_t>(context.R14);

        case CV_AMD64_R15D:
            return make_register_value<uint32_t>(context.R15);

        case CV_AMD64_R8:
            return make_register_value<uint64_t>(context.R8);

        case CV_AMD64_R9:
            return make_register_value<uint64_t>(context.R9);

        case CV_AMD64_R10:
            return make_register_value<uint64_t>(context.R10);

        case CV_AMD64_R11:
            return make_register_value<uint64_t>(context.R11);

        case CV_AMD64_R12:
            return make_register_value<uint64_t>(context.R12);

        case CV_AMD64_R13:
            return make_register_value<uint64_t>(context.R13);

        case CV_AMD64_R14:
            return make_register_value<uint64_t>(context.R14);

        case CV_AMD64_R15:
            return make_register_value<uint64_t>(context.R15);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in x64 context", static_cast<uint32_t>(register_type))};
        }
    }

    dlg_help_utils::dbg_help::registry_info get_registry_value_info(dlg_help_utils::dbg_help::thread_context_type const type, CV_HREG_e const registry, void const* thread_context)
    {
        uint64_t value{0};
        uint64_t value_size{0};

        switch(type)
        {
        case dlg_help_utils::dbg_help::thread_context_type::x86:
            std::tie(value, value_size) = get_x86_register_value(registry, *static_cast<dlg_help_utils::stream_thread_context::context_x86 const*>(thread_context));
            break;
        case dlg_help_utils::dbg_help::thread_context_type::wow64:
            std::tie(value, value_size) = get_wow64_register_value(registry, *static_cast<WOW64_CONTEXT const*>(thread_context));
            break;
        case dlg_help_utils::dbg_help::thread_context_type::x64:
            std::tie(value, value_size) = get_x64_register_value(registry, *static_cast<dlg_help_utils::stream_thread_context::context_x64 const*>(thread_context));
            break;
        }

        return {registry, value, value_size};
    }

    dlg_help_utils::dbg_help::frame_data_info get_frame_data_info(PSYMBOL_INFOW symbol_info, uint64_t const address)
    {
        return dlg_help_utils::dbg_help::frame_data_info{static_cast<int>(symbol_info->Address), address + symbol_info->Address, symbol_info->Size};
    }

    bool is_already_found(std::vector<dlg_help_utils::dbg_help::local_variable> const& variables, PSYMBOL_INFOW symbol_info)
    {
        return std::ranges::any_of(variables, [symbol_info](dlg_help_utils::dbg_help::local_variable const& variable)
        {
            if(variable.symbol_info.module_base() != symbol_info->ModBase || variable.symbol_info.sym_index() != symbol_info->Index)
            {
                return false;
            }

            if ((symbol_info->Flags & (SYMFLAG_REGREL | SYMFLAG_REGISTER)) != 0 && 
                (!variable.registry_value.has_value() || static_cast<CV_HREG_e>(symbol_info->Register) != variable.registry_value.value().register_type))
            {
                return false;
            }


            if ((symbol_info->Flags & (SYMFLAG_REGREL | SYMFLAG_FRAMEREL)) != 0 && 
                (!variable.frame_data.has_value() || variable.frame_data.value().data_offset != static_cast<int>(symbol_info->Address)))
            {
                return false;
            }

            return true;
        });
    }

    BOOL CALLBACK find_local_variable_callback(_In_ PSYMBOL_INFOW symbol_info, [[maybe_unused]] _In_ ULONG symbol_size, _In_opt_ PVOID user_context)
    {
        auto const is_local = (symbol_info->Flags & SYMFLAG_LOCAL) != 0;
        auto const is_parameter = (symbol_info->Flags & SYMFLAG_PARAMETER) != 0;

        if (!is_local)
        {
            // Ignore anything not a local variable
            return TRUE;
        }

        if (symbol_info->Flags & SYMFLAG_NULL)
        {
            // Ignore 'NULL' objects
            return TRUE;
        }

        local_variable_info const& info{*static_cast<local_variable_info*>(user_context)};

        // check for duplicates...
        if(is_already_found(is_parameter ? info.parameters : info.locals, symbol_info))
        {
            return TRUE;
        }

        auto& variable = is_parameter ? info.parameters.emplace_back(dlg_help_utils::dbg_help::symbol_type_info{info.process, symbol_info->ModBase, symbol_info->Index})
                                      : info.locals.emplace_back(dlg_help_utils::dbg_help::symbol_type_info{info.process, symbol_info->ModBase, symbol_info->Index});

        if (symbol_info->Flags & SYMFLAG_REGREL)
        {
            variable.registry_value = get_registry_value_info(info.type, static_cast<CV_HREG_e>(symbol_info->Register), info.thread_context);
            variable.frame_data = get_frame_data_info(symbol_info, variable.registry_value->value);
        }
        else if (symbol_info->Flags & SYMFLAG_REGISTER)
        {
            variable.registry_value = get_registry_value_info(info.type, static_cast<CV_HREG_e>(symbol_info->Register), info.thread_context);
        }
        else if (symbol_info->Flags & SYMFLAG_FRAMEREL)
        {
            variable.frame_data = get_frame_data_info(symbol_info, info.frame_address_offset);
        }

        return TRUE;
    }
    // ReSharper restore CppParameterMayBeConst
}

namespace dlg_help_utils::dbg_help
{
    symbol_engine::symbol_engine(i_symbol_load_callback& callback)
        : callback_{&callback}
          , symbol_(static_cast<SYMBOL_INFOW*>(malloc(max_buffer_size)))
    {
        memset(symbol_.get(), 0, max_buffer_size);
        symbol_->MaxNameLen = trace_max_function_name_length;
        symbol_->SizeOfStruct = sizeof(SYMBOL_INFOW);
        line_.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);

        if (!SymInitializeW(process_, nullptr, FALSE))
        {
            windows_error::throw_windows_api_error(L"SymInitialize"sv);
        }

        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | (callback.symbol_load_debug() ? SYMOPT_DEBUG : 0));
        SymRegisterCallbackW64(process_, sym_register_callback_proc64, reinterpret_cast<ULONG64>(static_cast<i_symbol_callback*>(this)));
    }

    symbol_engine::~symbol_engine()
    {
        SymCleanup(process_);
    }

    void symbol_engine::clear_modules()
    {
        for (const auto& [handle, base, size, module_image_path, name] : modules_ | std::views::values)
        {
            if (handle != 0 && !SymUnloadModule64(process_, handle))
            {
                windows_error::throw_windows_api_error(L"SymUnloadModule64"sv, to_hex(handle));
            }
        }

        modules_.clear();
        clear_cached_type_info();
    }

    void symbol_engine::load_module(std::wstring module_name, DWORD64 const module_base, DWORD const module_size,
                                    DWORD const module_time_stamp, DWORD const module_check_sum, void const* cv_record,
                                    DWORD const cv_record_size, void const* misc_record, DWORD const misc_record_size,
                                    [[maybe_unused]] VS_FIXEDFILEINFO const& version_info)
    {
        std::filesystem::path const path{module_name};
        auto const module = path.filename().wstring();

        cv_info_pdb70 const pdb{cv_record, cv_record_size};
        DWORD64 handle{0};

        if (pdb.is_valid())
        {
            auto const debug_module_info_size = sizeof(IMAGE_DEBUG_DIRECTORY) + cv_record_size;
            auto const debug_module_info = std::make_unique<uint8_t[]>(debug_module_info_size);
            auto* info = reinterpret_cast<IMAGE_DEBUG_DIRECTORY*>(debug_module_info.get());

            info->TimeDateStamp = module_time_stamp;
            info->Characteristics = 0;
            info->MajorVersion = HIWORD(version_info.dwProductVersionMS);
            info->MinorVersion = LOWORD(version_info.dwProductVersionMS);
            info->Type = IMAGE_DEBUG_TYPE_CODEVIEW;
            info->AddressOfRawData = 0;
            info->PointerToRawData = sizeof(IMAGE_DEBUG_DIRECTORY);

            memcpy(debug_module_info.get() + info->PointerToRawData, cv_record, cv_record_size);

            MODLOAD_DATA module_load_info;
            module_load_info.ssize = sizeof(module_load_info);
            module_load_info.ssig = DBHHEADER_DEBUGDIRS;
            module_load_info.data = debug_module_info.get();
            module_load_info.size = static_cast<DWORD>(debug_module_info_size);
            module_load_info.flags = 0;

            if (callback().symbol_load_debug())
            {
                callback().log_stream() << L" loading: DBHHEADER_DEBUGDIRS\n";
                callback().log_stream() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) << L'\n';
                callback().log_stream() << L" loading: PdbAge:" << pdb.get_age() << L'\n';
                callback().log_stream() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                callback().log_stream() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                callback().log_stream() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info);
        }

        if (pdb.is_valid() && handle == 0)
        {
            MODLOAD_PDBGUID_PDBAGE data;
            data.PdbGuid = pdb.get_signature();
            data.PdbAge = pdb.get_age();
            MODLOAD_DATA module_load_info;
            module_load_info.ssize = sizeof(module_load_info);
            module_load_info.ssig = DBHHEADER_PDBGUID;
            module_load_info.data = &data;
            module_load_info.size = sizeof(data);
            module_load_info.flags = 0;

            if (callback().symbol_load_debug())
            {
                callback().log_stream() << L" loading: DBHHEADER_PDBGUID\n";
                callback().log_stream() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) << L'\n';
                callback().log_stream() << L" loading: PdbAge:" << pdb.get_age() << L'\n';
                callback().log_stream() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                callback().log_stream() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                callback().log_stream() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info);
        }

        if (handle == 0)
        {
            auto const debug_module_info_size = sizeof(MODLOAD_CVMISC) + cv_record_size + misc_record_size;
            auto const debug_module_info = std::make_unique<uint8_t[]>(debug_module_info_size);
            auto* cv_info = reinterpret_cast<MODLOAD_CVMISC*>(debug_module_info.get());
            cv_info->dtImage = module_time_stamp;
            cv_info->cImage = module_size;
            cv_info->cCV = cv_record_size;
            if (cv_record_size > 0)
            {
                cv_info->oCV = sizeof(MODLOAD_CVMISC);
                memcpy(debug_module_info.get() + cv_info->oCV, cv_record, cv_record_size);
                cv_info->cMisc = 0;
                cv_info->oMisc = 0;
            }
            else if (misc_record_size > 0)
            {
                cv_info->oCV = 0;
                cv_info->cMisc = misc_record_size;
                cv_info->oMisc = sizeof(MODLOAD_CVMISC);
                memcpy(debug_module_info.get() + cv_info->oMisc, misc_record, misc_record_size);
            }
            else
            {
                cv_info->cMisc = 0;
                cv_info->oMisc = 0;
                cv_info->oCV = 0;
            }

            MODLOAD_DATA module_load_info;
            module_load_info.ssize = sizeof(module_load_info);
            module_load_info.ssig = DBHHEADER_CVMISC;
            module_load_info.data = debug_module_info.get();
            module_load_info.size = static_cast<DWORD>(debug_module_info_size);
            module_load_info.flags = 0;

            if (callback().symbol_load_debug())
            {
                callback().log_stream() << L" loading: DBHHEADER_CVMISC\n";
                callback().log_stream() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                callback().log_stream() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                callback().log_stream() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info);
        }

        if (handle == 0)
        {
            if (callback().symbol_load_debug())
            {
                callback().log_stream() << L" loading: default options\n";
                callback().log_stream() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                callback().log_stream() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                callback().log_stream() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, nullptr);
        }

        std::wstring module_image_path;
        if (handle != 0)
        {
            module_image_path = load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        stream_module_name name{module_name};
        modules_.insert(std::make_pair(std::move(module_name), module_info{handle, module_base, module_size, std::move(module_image_path), std::move(name)}));
    }

    void symbol_engine::load_module(std::wstring module_name, DWORD64 const module_base, DWORD const module_size,
                                    DWORD const module_time_stamp, DWORD const module_check_sum)
    {
        std::filesystem::path const path{module_name};
        auto const module = path.filename().wstring();

        auto const handle = load_module(module, module_base, module_size, nullptr);
        std::wstring module_image_path;

        if (handle != 0)
        {
            module_image_path = load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        stream_module_name name {module_name};
        modules_.insert(std::make_pair(std::move(module_name), module_info{handle, module_base, module_size, std::move(module_image_path), std::move(name)}));
    }

    void symbol_engine::unload_module(std::wstring const& module_name)
    {
        if (auto const it = modules_.find(module_name); it != modules_.end())
        {
            if (it->second.handle != 0 && !SymUnloadModule64(process_, it->second.handle))
            {
                windows_error::throw_windows_api_error(L"SymUnloadModule64"sv, to_hex(it->second.handle));
            }
            modules_.erase(it);
        }
    }

    bool symbol_engine::is_module_loaded(std::wstring const& module_name) const
    {
        return modules_.contains(module_name);
    }

    bool symbol_engine::is_module_symbols_available(std::wstring const& module_name) const
    {
        auto const it = modules_.find(module_name);
        if (it == modules_.end())
        {
            return false;
        }

        return it->second.handle != 0;
    }

    IMAGEHLP_MODULEW64 symbol_engine::get_module_information(std::wstring const& module_name) const
    {
        auto const it = modules_.find(module_name);
        if (it == modules_.end())
        {
            throw exceptions::wide_runtime_error{std::format(L"Module [{}] is not loaded", module_name)};
        }
        return get_module_information(it->second.base);
    }

    IMAGEHLP_MODULEW64 symbol_engine::get_module_information(DWORD64 const module_base) const
    {
        IMAGEHLP_MODULEW64 module{};
        module.SizeOfStruct = sizeof(module);
        if (!SymGetModuleInfoW64(process_, module_base, &module))
        {
            windows_error::throw_windows_api_error(L"SymUnloadModule64"sv, to_hex(module_base));
        }

        return module;
    }

    std::wstring const& symbol_engine::get_module_image_path(std::wstring const& module_name) const
    {
        auto const it = modules_.find(module_name);
        if (it == modules_.end())
        {
            throw exceptions::wide_runtime_error{std::format(L"Module [{}] is not loaded", module_name)};
        }
        return it->second.module_image_path;
    }

    std::optional<symbol_address_info> symbol_engine::address_to_info(DWORD64 const address)
    {
        auto const it = find_module_name(address);
        if (it == modules_.end())
        {
            return std::nullopt;
        }

        symbol_address_info info{};
        info.found = true;
        info.address = address;
        info.stack = 0;
        info.module_name = it->first;
        info.module_displacement = address - it->second.base;

        if (it->second.handle == 0)
        {
            return std::move(info);
        }

        if (!SymFromAddrW(process_, address, &info.symbol_displacement, symbol_.get()))
        {
            return std::move(info);
        }

        info.symbol_name = symbol_->Name;

        if (SymGetLineFromAddrW64(process_, address, &info.line_displacement, &line_))
        {
            info.line_number = line_.LineNumber;
            info.file_name = line_.FileName;
        }

        return std::move(info);
    }

    std::optional<symbol_address_info> symbol_engine::address_to_info(thread_context_type const type, STACKFRAME_EX const& frame, void const* thread_context)
    {
        auto const address = frame.AddrPC.Offset;
        auto const it = find_module_name(address);
        if (it == modules_.end())
        {
            return std::nullopt;
        }

        symbol_address_info info{};
        info.found = true;
        info.address = address;
        info.stack = frame.AddrStack.Offset;
        info.module_name = it->first;
        info.module_displacement = address - it->second.base;

        if (it->second.handle == 0)
        {
            return std::move(info);
        }

        IMAGEHLP_STACK_FRAME image_hlp_frame = {};
        image_hlp_frame.InstructionOffset = address;
        image_hlp_frame.ReturnOffset = frame.AddrReturn.Offset;
        image_hlp_frame.FrameOffset = frame.AddrFrame.Offset;
        image_hlp_frame.StackOffset = frame.AddrStack.Offset;
        image_hlp_frame.BackingStoreOffset = frame.AddrBStore.Offset;
        image_hlp_frame.FuncTableEntry = reinterpret_cast<ULONG64>(frame.FuncTableEntry);
        image_hlp_frame.Virtual = frame.Virtual;

        SymSetContext(process_, &image_hlp_frame, nullptr);
        if(auto const ec = GetLastError(); ec == ERROR_SUCCESS)
        {
            local_variables_walk(info.local_variables, info.parameters, type, frame.AddrFrame.Offset, thread_context);
        }
        else if(ec != ERROR_NOT_SUPPORTED)
        {
            windows_error::throw_windows_api_error(L"SymSetContext"sv, ec);
        }

        if (frame.StackFrameSize >= sizeof(STACKFRAME_EX) && frame.InlineFrameContext != INLINE_FRAME_CONTEXT_IGNORE && frame.InlineFrameContext != INLINE_FRAME_CONTEXT_INIT)
        {
            if (!SymFromInlineContextW(process_, address, frame.InlineFrameContext, &info.symbol_displacement, symbol_.get()))
            {
                return std::move(info);
            }
            info.frame_content_found = true;
            info.symbol_name = symbol_->Name;
            info.in_line = static_cast<sym_tag_enum>(symbol_->Tag) == sym_tag_enum::Inlinee;

            if (SymGetLineFromInlineContextW(process_, address, frame.InlineFrameContext, it->second.base, &info.line_displacement, &line_))
            {
                info.line_number = line_.LineNumber;
                info.file_name = line_.FileName;
            }

            if (SymSetScopeFromInlineContext(process_, address, frame.InlineFrameContext))
            {
                local_variables_walk(info.local_variables, info.parameters, type, frame.AddrFrame.Offset, thread_context, {}, symbol_walk_options::inline_variables);
            }
        }
        else
        {
            if (!SymFromAddrW(process_, address, &info.symbol_displacement, symbol_.get()))
            {
                return std::move(info);
            }
            info.symbol_name = symbol_->Name;

            if (SymGetLineFromAddrW64(process_, frame.AddrPC.Offset, &info.line_displacement, &line_))
            {
                info.line_number = line_.LineNumber;
                info.file_name = line_.FileName;
            }
        }

        return std::move(info);
    }

    std::optional<symbol_type_info> symbol_engine::get_type_info(std::wstring const& type_name)
    {
        auto [module_name, specific_type_name] = parse_type_info(type_name);
        return get_type_info(module_name, specific_type_name);
    }

    std::optional<symbol_type_info> symbol_engine::get_type_info(std::wstring const& module_name, std::wstring const& type_name)
    {
        if(type_name.empty())
        {
            return std::nullopt;
        }

        if(module_name.empty())
        {
            // search all currently loaded symbols....
            for (const auto& module : modules_ | std::views::values)
            {
                if(auto rv = load_type_info(module.base, type_name); rv.has_value())
                {
                    return rv;
                }
            }

            return std::nullopt;
        }

        auto it = modules_.find(module_name);
        if(it == modules_.end())
        {
            it = std::ranges::find_if(modules_, [&module_name](std::pair<std::wstring, module_info> const& entry){ return module_match::module_name_match(entry.second.name, module_name); });
        }

        if(it == modules_.end())
        {
            return std::nullopt;
        }

        return load_type_info(it->second.base, type_name);
    }

    std::vector<symbol_type_info> symbol_engine::module_types(std::wstring const& module_name)
    {
        std::vector<symbol_type_info> types;
        if(module_name.empty())
        {
            return types;
        }

        auto it = modules_.find(module_name);
        if(it == modules_.end())
        {
            it = std::ranges::find_if(modules_, [&module_name](std::pair<std::wstring, module_info> const& entry){ return module_match::module_name_match(entry.second.name, module_name); });
        }

        if(it == modules_.end())
        {
            return types;
        }

        find_symbol_callback_context context{process_, types};
        if(!SymEnumTypesW(process_, it->second.base, find_symbol_callback, &context))
        {
            windows_error::throw_windows_api_error(L"SymEnumTypesW"sv, to_hex(it->second.base));
        }

        return types;
    }

    std::optional<symbol_type_info> symbol_engine::get_symbol_info(std::wstring const& symbol_name) const
    {
        if(symbol_name.empty())
        {
            return std::nullopt;
        }

        auto const info = symbol_info_buffer::make();
        if(!SymFromNameW(process_, symbol_name.c_str(), &info->info))
        {
            return std::nullopt;
        }

        return symbol_type_info{process_, info->info.ModBase, info->info.Index};
    }

    std::vector<symbol_type_info> symbol_engine::symbol_walk(std::wstring const& find_mask, symbol_walk_options const option) const
    {
        std::vector<symbol_type_info> symbols;
        find_symbol_callback_context context{process_, symbols};
        if(!SymEnumSymbolsExW(process_, 0, find_mask.empty() ? L"*!*" : find_mask.c_str(), find_symbol_callback, &context, setup_enum_symbol_options(option)))
        {
            windows_error::throw_windows_api_error(L"SymEnumSymbolsExW"sv, find_mask);
        }

        return symbols;
    }

    void symbol_engine::local_variables_walk(std::vector<local_variable>& locals, std::vector<local_variable>& parameters, thread_context_type const type, uint64_t const frame_address_offset, void const* thread_context, std::wstring const& find_mask, symbol_walk_options const option) const
    {
        local_variable_info info{process_, type, frame_address_offset, thread_context, locals, parameters};
        if(!SymEnumSymbolsExW(process_, 0, find_mask.empty() ? L"*" : find_mask.c_str(), find_local_variable_callback, &info, setup_enum_symbol_options(option)))
        {
            if(auto const ec = GetLastError(); ec != ERROR_INVALID_PARAMETER && ec != ERROR_NOT_SUPPORTED)
            {
                windows_error::throw_windows_api_error(L"SymEnumSymbolsExW"sv, find_mask, ec);
            }
        }
    }

    std::experimental::generator<symbol_address_info> symbol_engine::stack_walk(stream_thread_context const& thread_context) const
    {
        thread_context_type type;
        DWORD machine_type;
        STACKFRAME_EX frame{};
        frame.StackFrameSize = sizeof(STACKFRAME_EX);

        if (thread_context.x86_thread_context_available())
        {
            machine_type = IMAGE_FILE_MACHINE_I386;
            type = thread_context_type::x86;
            auto const& context = thread_context.x86_thread_context();
            frame.AddrPC.Offset = context.Eip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = context.Ebp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = context.Esp;
            frame.AddrStack.Mode = AddrModeFlat;
        }
        else if (thread_context.wow64_thread_context_available())
        {
            machine_type = IMAGE_FILE_MACHINE_I386;
            type = thread_context_type::wow64;
            auto const& context = thread_context.wow64_thread_context();
            frame.AddrPC.Offset = context.Eip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = context.Ebp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = context.Esp;
            frame.AddrStack.Mode = AddrModeFlat;
        }
        else if (thread_context.x64_thread_context_available())
        {
            machine_type = IMAGE_FILE_MACHINE_AMD64;
            type = thread_context_type::x64;
            auto const& context = thread_context.x64_thread_context();
            frame.AddrPC.Offset = context.Rip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = context.Rsp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = context.Rsp;
            frame.AddrStack.Mode = AddrModeFlat;
        }
        else
        {
            co_return;
        }

        // force the loading of the IP module
        [[maybe_unused]] auto const base_address = g_callback->get_module_base_routine(frame.AddrPC.Offset);

        // StackWalkEx modifies the thread context passed in so always take a copy for it to work with
        auto const thread_context_copy = std::make_unique<uint8_t[]>(thread_context.size());
        memcpy(thread_context_copy.get(), thread_context.context(), thread_context.size());

        auto flags = SYM_STKWALK_DEFAULT | SYM_STKWALK_FORCE_FRAMEPTR;

        while (true)
        {
            if (!StackWalkEx(machine_type, process_, thread_, &frame, thread_context_copy.get(),
                             &read_process_memory_routine, &function_table_access_routine, &get_module_base_routine,
                             &translate_address, flags))
            {
                break;
            }

            auto const pc = frame.AddrPC.Offset;
            auto info = g_callback->find_symbol_info(type, frame, thread_context_copy.get());
            if (!info)
            {
                symbol_address_info rv{};
                rv.address = pc;
                rv.stack = frame.AddrStack.Offset;
                co_yield rv;
            }
            else
            {
                co_yield info.value();
            }

            flags = info && info.value().symbol_name.empty()
                        ? SYM_STKWALK_DEFAULT | SYM_STKWALK_FORCE_FRAMEPTR
                        : SYM_STKWALK_DEFAULT;
        }
    }

    std::map<std::wstring, symbol_engine::module_info>::const_iterator symbol_engine::find_module_name(
        DWORD64 const address)
    {
        for (auto it = modules_.begin(); it != modules_.end(); ++it)
        {
            if (address >= it->second.base && address < it->second.base + it->second.size)
            {
                return it;
            }
        }

        return modules_.end();
    }

    void symbol_engine::dump_loaded_module_information(DWORD64 const handle, DWORD64 const module_base,
                                                       std::wstring const& module_image_path) const
    {
        if (handle != 0 && callback().symbol_load_debug())
        {
            // ReSharper disable once CppUseStructuredBinding
            const auto info = get_module_information(module_base);
            callback().log_stream() << L"Loaded:\n";
            callback().log_stream() << L" SymType:" << info.SymType << L'\n';
            callback().log_stream() << L" ModuleName:" << info.ModuleName << L'\n';
            callback().log_stream() << L" ImageName:" << info.ImageName << L'\n';
            callback().log_stream() << L" ModuleImagePath:" << module_image_path << L'\n';
            callback().log_stream() << L" LoadedImageName:" << info.LoadedImageName << L'\n';
            callback().log_stream() << L" LoadedPdbName:" << info.LoadedPdbName << L'\n';
            callback().log_stream() << L" CheckSum:" << to_hex(info.CheckSum) << L'\n';
            callback().log_stream() << L" ImageSize:" << to_hex(info.ImageSize) << L'\n';
            callback().log_stream() << L" MachineType:" << to_hex(info.MachineType) << L'\n';
            callback().log_stream() << L" PdbSig70:" << guid_utils::to_string(info.PdbSig70) << L'\n';
            callback().log_stream() << L" PdbAge:" << info.PdbAge << L'\n';
            callback().log_stream() << L" PdbUnmatched:" << info.PdbUnmatched << L'\n';
            callback().log_stream() << L" DbgUnmatched:" << info.DbgUnmatched << L'\n';
        }
    }

    std::wstring symbol_engine::load_module_image_path(DWORD const module_size, DWORD module_time_stamp,
                                                       DWORD const module_check_sum, std::wstring const& module_name,
                                                       DWORD64 const handle)
    {
        if (handle != 0)
        {
            loading_module_check_sum_ = module_check_sum;
            std::array<wchar_t, MAX_PATH> buffer{};
            auto const result = SymFindFileInPathW(process_, nullptr, module_name.c_str(), &module_time_stamp,
                                                   module_size, 0, SSRVOPT_DWORDPTR, buffer.data(),
                                                   find_file_in_path_callback, static_cast<i_symbol_callback*>(this));
            loading_module_check_sum_ = 0;
            if (result)
            {
                return buffer.data();
            }

            if (callback().symbol_load_debug())
            {
                auto const ec = GetLastError();
                callback().log_stream() << L"Failed to find module image path for [" << module_name <<
                    L"] - SymFindFileInPathW: " << ec << L" - " << windows_error::get_windows_error_string(ec) << L'\n';
            }
        }

        return {};
    }

    DWORD64 symbol_engine::load_module(std::wstring const& module_name, DWORD64 const module_base, DWORD const module_size,
                                       MODLOAD_DATA* module_load_info)
    {
        loading_module_ = true;
        auto loading_module_handle = make_scope_exit([this]()
        {
            loading_module_ = false;
        });

        return SymLoadModuleExW(process_, nullptr, module_name.c_str(), nullptr, module_base, module_size,
                                module_load_info, 0);
    }

    std::optional<std::optional<symbol_type_info>> symbol_engine::get_cached_type_info(std::wstring const& type_name)
    {
        if(auto const it = cache_type_info_.find(type_name); it != cache_type_info_.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

    void symbol_engine::set_cached_type_info(std::wstring const& type_name, std::optional<symbol_type_info> const& type_info)
    {
        cache_type_info_.insert(std::make_pair(type_name, type_info));
    }

    void symbol_engine::clear_cached_type_info()
    {
        cache_type_info_.clear();
    }

    std::optional<symbol_type_info> symbol_engine::load_type_info(DWORD64 const module_base, std::wstring const& type_name)
    {
        if(auto const rv = get_cached_type_info(type_name); rv.has_value())
        {
            return rv.value();
        }

        auto const info = symbol_info_buffer::make();
        if(!SymGetTypeFromNameW(process_, module_base, type_name.c_str(), &info->info))
        {
            set_cached_type_info(type_name, std::nullopt);
            return std::nullopt;
        }

        auto rv = symbol_type_info{process_, module_base, info->info.TypeIndex};
        set_cached_type_info(type_name, rv);
        return rv;
    }

    std::tuple<std::wstring, std::wstring> symbol_engine::parse_type_info(std::wstring const& type_name)
    {
        auto const pos = type_name.find_first_of(L'!');
        if(pos == std::wstring::npos)
        {
            return std::make_tuple(std::wstring{}, type_name);
        }

        return std::make_tuple(type_name.substr(0, pos), type_name.substr(pos+1));
    }

    callback_handle symbol_engine::set_callback(i_stack_walk_callback& callback)
    {
        g_callback = &callback;
        return callback_handle{[]() { g_callback = nullptr; }};
    }

    DWORD symbol_engine::setup_enum_symbol_options(symbol_walk_options const option)
    {
        DWORD options = 0;

        switch(option)
        {
        case symbol_walk_options::default_symbols:
            options |= SYMENUM_OPTIONS_DEFAULT;
            break;

        case symbol_walk_options::inline_variables:
            options |= SYMENUM_OPTIONS_INLINE;
            break;
        }

        return options;
    }

    HANDLE symbol_engine::create_fake_id()
    {
        static std::atomic_ptrdiff_t next_fake_id{1};
        return reinterpret_cast<HANDLE>(next_fake_id++);
    }
}
