// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "symbol_engine.h"

#include <array>
#include <filesystem>
#include <format>
#include <ranges>
#include <unordered_set>

// ReSharper disable once CommentTypo
// Found in "$(VSINSTALLDIR)\DIA SDK\include"
#include <cvconst.h>

#include "cv_info_pdb70.h"
#include "exit_scope.h"
#include "filesystem_utils.h"
#include "guid_utils.h"
#include "hex_dump.h"
#include "join.h"
#include "locale_number_formatting.h"
#include "m128a_utils.h"
#include "variable.h"
#include "module_match.h"
#include "overload.h"
#include "pe_file.h"
#include "stream_hex_dump.h"
#include "stream_thread_context.h"
#include "string_compare.h"
#include "symbol_info_buffer.h"
#include "symbol_type_info.h"
#include "symbol_type_info_cache.h"
#include "sym_tag_enum.h"
#include "wide_runtime_error.h"
#include "windows_error.h"
#include "xstate_reader.h"

#pragma comment(lib, "dbghelp.lib")

auto constexpr cba_start_op_maybe = 0xA0000000;

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::m128a_utils;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils::dbg_help;

namespace
{
    size_t constexpr trace_max_function_name_length{1000};
    size_t constexpr max_buffer_size = sizeof(SYMBOL_INFOW) + (trace_max_function_name_length * sizeof(wchar_t));
    auto constexpr progress_xml_start = L"<Progress percent=\""sv;
    auto constexpr progress_xml_end = L"\"/>\n"sv;
    auto constexpr downloading_xml_start = L"<Activity name=\"Downloading file "sv;
    auto constexpr downloading_xml_end = L"\" details=\""sv;

    i_stack_walk_callback* g_callback{nullptr};

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
    static_assert(sizeof(symbol_load) == 24); // NOLINT
    static_assert(sizeof(symbol_load_w64) == 304); // NOLINT
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

    void dump_sizeof_struct_data(ULONG64 const callback_data
        , i_symbol_load_callback const& callback)
    {
        if (callback.symbol_load_debug_memory() && callback_data != 0)
        {
            auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data);
            auto const* data = reinterpret_cast<uint8_t const*>(callback_data);
            dlg_help_utils::hex_dump::hex_dump(callback.log_stream().log(), data, size_of_struct, 5);
        }
    }

    BOOL CALLBACK sym_register_callback_proc64([[maybe_unused]] __in HANDLE h_process
        , __in ULONG const action_code
        , __in_opt ULONG64 const callback_data
        , __in_opt ULONG64 const user_context)
    {
        auto& symbol_engine = *reinterpret_cast<i_symbol_callback*>(user_context);
        auto& callback = symbol_engine.load_callback();
        // ReSharper disable once CommentTypo
        // If SYMOPT_DEBUG is set, then the symbol handler will pass
        // verbose information on its attempt to load symbols.
        // This information be delivered as text strings.

        if (callback.symbol_load_debug())
        {
            callback.log_stream().log() << std::format(L"DlgHelp: action_code: {0} ({1} callback_data:({2})\n", action_code_to_string(action_code), to_hex(action_code), to_hex(callback_data));
        }

        if (callback_data == 0) return FALSE;

        switch (action_code)
        {
        case CBA_EVENT:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_EVENTW const*>(callback_data);
                callback.log_stream().log() << std::format(L"DlgHelp: {0}:[{1}]:({2})[{3}] {4}\n", action_code_to_string(action_code), severity_to_string(evt->severity), to_hex(evt->code), evt->desc, to_hex(evt->object));
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
                    callback.log_stream().log() << 
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
                    callback.log_stream().log() << 
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
                    callback.log_stream().log() << 
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
                callback.log_stream().log() << std::format(L"DlgHelp: {0}:address[{1}]:bytes:[{2}]\n", action_code_to_string(action_code), to_hex(evt->addr), to_hex(evt->bytes));
            }

            if (g_callback != nullptr && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_READ_MEMORY const*>(callback_data);
                return g_callback->read_process_memory(evt->addr
                    , evt->buf
                    , evt->bytes
                    , evt->bytesread
                    , dlg_help_utils::enable_module_loading_t{!symbol_engine.loading_module()});
            }
            break;

        case CBA_DUPLICATE_SYMBOL:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                if (auto const * evt = reinterpret_cast<IMAGEHLP_DUPLICATE_SYMBOL64 const*>(callback_data); evt->SizeOfStruct == sizeof(IMAGEHLP_DUPLICATE_SYMBOL64))
                {
                    // ReSharper disable once StringLiteralTypo
                    auto handle = callback.log_stream();
                    handle.log() << std::format(L"DlgHelp: {0}:NumberOfDups[{1}]:SelectedSymbol[{2}]\n", action_code_to_string(action_code), locale_formatting::to_wstring(evt->NumberOfDups), locale_formatting::to_wstring(evt->SelectedSymbol));
                    handle.log() << std::format(L"    Symbol: Address: [{}]\n", to_hex(evt->Symbol->Address));
                    handle.log() << std::format(L"            Size: [{}]\n", locale_formatting::to_wstring(evt->Symbol->Size));
                    handle.log() << std::format(L"            Flags: [{}]\n", to_hex(evt->Symbol->Flags));
                    handle.log() << std::format(L"            Name: [{}]\n", dlg_help_utils::string_conversation::acp_to_wstring(evt->Symbol->Name));
                }
                else
                {
                    callback.log_stream().log() << std::format(L"DlgHelp: {0}: unknown event data size : {1} != {2}\n", action_code_to_string(action_code), locale_formatting::to_wstring(evt->SizeOfStruct), locale_formatting::to_wstring(sizeof(IMAGEHLP_DUPLICATE_SYMBOL64)));
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
                    auto handle = callback.log_stream();
                    handle.log() << std::format(L"DlgHelp: {0}:{1}", action_code_to_string(action_code), xml);
                    if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                    {
                        handle.log() << L'\n';
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
                        auto const module = xml.substr(downloading_xml_start.size(), end_pos - downloading_xml_start.size());
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
                callback.log_stream().log() << std::format(L"DlgHelp: {0}:[{1}]:({2})[{3}] {4}\n", action_code_to_string(action_code), severity_to_string(evt->severity), to_hex(evt->code), evt->desc, to_hex(evt->object));
            }
            return TRUE;

        case CBA_SRCSRV_INFO:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                std::wstring_view const xml{reinterpret_cast<wchar_t const*>(callback_data)};
                auto handle = callback.log_stream();
                handle.log() << std::format(L"DlgHelp: {0}:{1}", action_code_to_string(action_code), xml);
                if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                {
                    handle.log() << L'\n';
                }
            }
            return TRUE;

        case cba_start_op_maybe:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                if (auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data); size_of_struct == sizeof(symbol_load))
                {
                    auto const* evt = reinterpret_cast<symbol_load const*>(callback_data);
                    callback.log_stream().log() << std::format(L"DlgHelp: {0}[symbol_load]:BaseOfImage[{1}]:CheckSum[{2}]:TimeDateStamp:[{3}]:Unknown:[{4}]\n", action_code_to_string(action_code), to_hex(evt->base_of_image), to_hex(evt->check_sum), to_hex(evt->time_date_stamp), to_hex(evt->unknown1));
                }
                else
                {
                    callback.log_stream().log() << std::format(L"DlgHelp: {0}: unknown event data size: {1} != {2}\n", action_code_to_string(action_code), locale_formatting::to_wstring(size_of_struct), locale_formatting::to_wstring(sizeof(symbol_load)));
                }
            }
            dump_sizeof_struct_data(callback_data, callback);
            break;

        default:
            if (callback_data != 0 && callback.symbol_load_debug_memory())
            {
                auto const* data = reinterpret_cast<uint8_t const*>(callback_data);
                dlg_help_utils::hex_dump::hex_dump(callback.log_stream().log(), data, 10, 5);
            }
            break;
        }

        // Return false to any ActionCode we don't handle,
        // or we could generate some undesirable behavior.
        return FALSE;
    }

    // ReSharper disable CppParameterMayBeConst
    BOOL CALLBACK find_file_in_path_callback(_In_ PCWSTR filename, _In_ PVOID context)
    {
        auto const& symbol_engine = *static_cast<i_symbol_callback const*>(context);
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

        if (auto const & callback = symbol_engine.load_callback(); callback.symbol_load_debug())
        {
            callback.log_stream().log() << std::format(L"DlgHelp: FindFile: {} checksum check failure\n", filename);
        }

        return TRUE;
    }

    struct find_symbol_callback_context
    {
        find_symbol_callback_context(HANDLE process, std::vector<symbol_type_info>& rv, symbol_type_info_cache& symbol_cache, std::wstring_view find_mask)
        : process{process}
        , rv{rv}
        , symbol_cache{symbol_cache}
        , find_mask{find_mask}
        {
        }

        HANDLE process;
        std::vector<symbol_type_info>& rv;
        symbol_type_info_cache& symbol_cache;
        std::unordered_set<ULONG> tag_index_found;
        std::wstring_view find_mask;
    };

    BOOL CALLBACK find_symbol_callback(_In_ PSYMBOL_INFOW symbol_info, [[maybe_unused]] _In_ ULONG symbol_size, _In_opt_ PVOID user_context)
    {
        if(find_symbol_callback_context& symbols{*static_cast<find_symbol_callback_context*>(user_context)};
            !symbols.tag_index_found.contains(symbol_info->Index))
        {
            symbols.tag_index_found.insert(symbol_info->Index);
            std::wstring_view const export_name{symbol_info->Name, symbol_info->NameLen - 1};
            if(auto type = symbols.symbol_cache.get_or_create_symbol_type_info(symbols.process, symbol_info->ModBase, symbol_info->Index, export_name);
                symbols.find_mask.empty() ||
                (type.name().has_value() && dlg_help_utils::filesystem_utils::wildcard_match(type.name().value(), symbols.find_mask)) ||
                dlg_help_utils::filesystem_utils::wildcard_match(export_name, symbols.find_mask))
            {
                symbols.rv.emplace_back(std::move(type));
            }
        }
        return TRUE;
    }

    struct local_variable_info
    {
        local_variable_info(HANDLE process
            , thread_context_type type
            , uint64_t const frame_address_offset
            , void const* thread_context
            , std::vector<variable> &locals
            , std::vector<variable> &parameters
            , symbol_type_info_cache& symbol_cache
            , std::optional<std::exception>& failure)
        : process{process}
        , type{type}
        , frame_address_offset{frame_address_offset}
        , thread_context{thread_context}
        , locals{locals}
        , parameters{parameters}
        , symbol_cache{symbol_cache}
        , failure{failure}
        {
        }

        HANDLE process;
        thread_context_type type;
        uint64_t frame_address_offset;
        void const* thread_context;
        std::vector<variable> &locals;
        std::vector<variable> &parameters;
        symbol_type_info_cache& symbol_cache;
        std::optional<std::exception>& failure;
    };

    template<typename T>
    reg_value_t make_register_value(uint64_t const raw_value)
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
        return static_cast<T>(raw_value & size_masks[sizeof(T)]);
    }

    reg_value_t get_x86_register_value(CV_HREG_e register_type, dlg_help_utils::stream_thread_context::context_x86 const& context, uint64_t const address_offset)
    {
        auto const* float_registers = reinterpret_cast<dlg_help_utils::float80_t const*>(context.FloatSave.RegisterArea);

        switch(register_type) // NOLINT
        {
        case CV_REG_AL:
            return make_register_value<uint8_t>(context.Eax);

        case CV_REG_BL:
            return make_register_value<uint8_t>(context.Ebx);

        case CV_REG_CL:
            return make_register_value<uint8_t>(context.Ecx);

        case CV_REG_DL:
            return make_register_value<uint8_t>(context.Edx);

        case CV_REG_AX:
            return make_register_value<uint16_t>(context.Eax);

        case CV_REG_BX:
            return make_register_value<uint16_t>(context.Ebx);

        case CV_REG_CX:
            return make_register_value<uint16_t>(context.Ecx);

        case CV_REG_DX:
            return make_register_value<uint16_t>(context.Edx);

        case CV_REG_SP:
            return make_register_value<uint16_t>(context.Esp);

        case CV_REG_BP:
            return make_register_value<uint16_t>(context.Ebp);

        case CV_REG_SI:
            return make_register_value<uint16_t>(context.Esi);

        case CV_REG_DI:
            return make_register_value<uint16_t>(context.Edi);

        case CV_REG_EAX:
            return make_register_value<uint32_t>(context.Eax);

        case CV_REG_EBX:
            return make_register_value<uint32_t>(context.Ebx);

        case CV_REG_ECX:
            return make_register_value<uint32_t>(context.Ecx);

        case CV_REG_EDX:
            return make_register_value<uint32_t>(context.Edx);

        case CV_REG_ESP:
            return make_register_value<uint32_t>(context.Esp);

        case CV_REG_EBP:
            return make_register_value<uint32_t>(context.Ebp);

        case CV_REG_ESI:
            return make_register_value<uint32_t>(context.Esi);

        case CV_REG_EDI:
            return make_register_value<uint32_t>(context.Edi);

        // Debug registers
        case CV_REG_DR0:
            return make_register_value<uint64_t>(context.Dr0);

        case CV_REG_DR1:
            return make_register_value<uint64_t>(context.Dr1);

        case CV_REG_DR2:
            return make_register_value<uint64_t>(context.Dr2);

        case CV_REG_DR3:
            return make_register_value<uint64_t>(context.Dr3);

        case CV_REG_DR6:
            return make_register_value<uint64_t>(context.Dr6);

        case CV_REG_DR7:
            return make_register_value<uint64_t>(context.Dr7);


        case CV_REG_ST0:
            return float_registers[0];

        case CV_REG_ST1:
            return float_registers[1];

        case CV_REG_ST2:
            return float_registers[2];

        case CV_REG_ST3:
            return float_registers[3];

        case CV_REG_ST4:
            return float_registers[4];

        case CV_REG_ST5:
            return float_registers[5];

        case CV_REG_ST6:
            return float_registers[6];

        case CV_REG_ST7:
            return float_registers[7];

        case CV_REG_CTRL:
            return static_cast<uint32_t>(context.FloatSave.ControlWord);

        case CV_REG_STAT:
            return static_cast<uint32_t>(context.FloatSave.StatusWord);

        case CV_REG_TAG:
            return static_cast<uint32_t>(context.FloatSave.TagWord);

        case CV_REG_FPIP:
            return static_cast<uint32_t>(context.FloatSave.ErrorOffset);

        case CV_REG_FPCS:
            return static_cast<uint32_t>(context.FloatSave.ErrorSelector);

        case CV_REG_FPDO:
            return static_cast<uint32_t>(context.FloatSave.DataOffset);

        case CV_REG_FPDS:
            return static_cast<uint32_t>(context.FloatSave.DataSelector);

        case CV_REG_MM0:
            return float_registers[0].low;

        case CV_REG_MM1:
            return float_registers[1].low;

        case CV_REG_MM2:
            return float_registers[2].low;

        case CV_REG_MM3:
            return float_registers[3].low;

        case CV_REG_MM4:
            return float_registers[4].low;

        case CV_REG_MM5:
            return float_registers[5].low;

        case CV_REG_MM6:
            return float_registers[6].low;

        case CV_REG_MM7:
            return float_registers[7].low;

        case CV_ALLREG_VFRAME:
            return make_register_value<uint32_t>(address_offset);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in x86 context", static_cast<uint32_t>(register_type))};
        }
    }

    reg_value_t get_wow64_register_value(CV_HREG_e register_type, WOW64_CONTEXT const& context, uint64_t const address_offset)
    {
        auto const* float_registers = reinterpret_cast<dlg_help_utils::float80_t const*>(context.FloatSave.RegisterArea);
        dlg_help_utils::xstate_reader xstate_reader{ &context };

        switch(register_type) // NOLINT
        {
        case CV_REG_AL:
            return make_register_value<uint8_t>(context.Eax);

        case CV_REG_BL:
            return make_register_value<uint8_t>(context.Ebx);

        case CV_REG_CL:
            return make_register_value<uint8_t>(context.Ecx);

        case CV_REG_DL:
            return make_register_value<uint8_t>(context.Edx);

        case CV_REG_AX:
            return make_register_value<uint16_t>(context.Eax);

        case CV_REG_BX:
            return make_register_value<uint16_t>(context.Ebx);

        case CV_REG_CX:
            return make_register_value<uint16_t>(context.Ecx);

        case CV_REG_DX:
            return make_register_value<uint16_t>(context.Edx);

        case CV_REG_SP:
            return make_register_value<uint16_t>(context.Esp);

        case CV_REG_BP:
            return make_register_value<uint16_t>(context.Ebp);

        case CV_REG_SI:
            return make_register_value<uint16_t>(context.Esi);

        case CV_REG_DI:
            return make_register_value<uint16_t>(context.Edi);

        case CV_REG_EAX:
            return make_register_value<uint32_t>(context.Eax);

        case CV_REG_EBX:
            return make_register_value<uint32_t>(context.Ebx);

        case CV_REG_ECX:
            return make_register_value<uint32_t>(context.Ecx);

        case CV_REG_EDX:
            return make_register_value<uint32_t>(context.Edx);

        case CV_REG_ESP:
            return make_register_value<uint32_t>(context.Esp);

        case CV_REG_EBP:
            return make_register_value<uint32_t>(context.Ebp);

        case CV_REG_ESI:
            return make_register_value<uint32_t>(context.Esi);

        case CV_REG_EDI:
            return make_register_value<uint32_t>(context.Edi);

        // Debug registers
        case CV_REG_DR0:
            return make_register_value<uint64_t>(context.Dr0);

        case CV_REG_DR1:
            return make_register_value<uint64_t>(context.Dr1);

        case CV_REG_DR2:
            return make_register_value<uint64_t>(context.Dr2);

        case CV_REG_DR3:
            return make_register_value<uint64_t>(context.Dr3);

        case CV_REG_DR6:
            return make_register_value<uint64_t>(context.Dr6);

        case CV_REG_DR7:
            return make_register_value<uint64_t>(context.Dr7);


        case CV_REG_ST0:
            return float_registers[0];

        case CV_REG_ST1:
            return float_registers[1];

        case CV_REG_ST2:
            return float_registers[2];

        case CV_REG_ST3:
            return float_registers[3];

        case CV_REG_ST4:
            return float_registers[4];

        case CV_REG_ST5:
            return float_registers[5];

        case CV_REG_ST6:
            return float_registers[6];

        case CV_REG_ST7:
            return float_registers[7];

        case CV_REG_CTRL:
            return static_cast<uint32_t>(context.FloatSave.ControlWord);

        case CV_REG_STAT:
            return static_cast<uint32_t>(context.FloatSave.StatusWord);

        case CV_REG_TAG:
            return static_cast<uint32_t>(context.FloatSave.TagWord);

        case CV_REG_FPIP:
            return static_cast<uint32_t>(context.FloatSave.ErrorOffset);

        case CV_REG_FPCS:
            return static_cast<uint32_t>(context.FloatSave.ErrorSelector);

        case CV_REG_FPDO:
            return static_cast<uint32_t>(context.FloatSave.DataOffset);

        case CV_REG_FPDS:
            return static_cast<uint32_t>(context.FloatSave.DataSelector);

        case CV_REG_MM0:
            return float_registers[0].low;

        case CV_REG_MM1:
            return float_registers[1].low;

        case CV_REG_MM2:
            return float_registers[2].low;

        case CV_REG_MM3:
            return float_registers[3].low;

        case CV_REG_MM4:
            return float_registers[4].low;

        case CV_REG_MM5:
            return float_registers[5].low;

        case CV_REG_MM6:
            return float_registers[6].low;

        case CV_REG_MM7:
            return float_registers[7].low;

            // KATMAI registers
        case CV_REG_XMM0:
            return to_uint128(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_XMM1:
            return to_uint128(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_XMM2:
            return to_uint128(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_XMM3:
            return to_uint128(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_XMM4:
            return to_uint128(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_XMM5:
            return to_uint128(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_XMM6:
            return to_uint128(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_XMM7:
            return to_uint128(*xstate_reader.get_ymm_register(7).xmm);

        // AVX registers 256 bits
        case CV_REG_YMM0:
            return to_uint128(*xstate_reader.get_ymm_register(0).xmm);

        case CV_REG_YMM1:
            return to_uint128(*xstate_reader.get_ymm_register(1).xmm);

        case CV_REG_YMM2:
            return to_uint128(*xstate_reader.get_ymm_register(2).xmm);

        case CV_REG_YMM3:
            return to_uint128(*xstate_reader.get_ymm_register(3).xmm);

        case CV_REG_YMM4:
            return to_uint128(*xstate_reader.get_ymm_register(4).xmm);

        case CV_REG_YMM5:
            return to_uint128(*xstate_reader.get_ymm_register(5).xmm);

        case CV_REG_YMM6:
            return to_uint128(*xstate_reader.get_ymm_register(6).xmm);

        case CV_REG_YMM7:
            return to_uint128(*xstate_reader.get_ymm_register(7).xmm);


        // AVX registers upper 128 bits
        case CV_REG_YMM0H:
            return to_uint128(*xstate_reader.get_ymm_register(0).ymm);
            
        case CV_REG_YMM1H:
            return to_uint128(*xstate_reader.get_ymm_register(1).ymm);

        case CV_REG_YMM2H:
            return to_uint128(*xstate_reader.get_ymm_register(2).ymm);

        case CV_REG_YMM3H:
            return to_uint128(*xstate_reader.get_ymm_register(3).ymm);

        case CV_REG_YMM4H:
            return to_uint128(*xstate_reader.get_ymm_register(4).ymm);

        case CV_REG_YMM5H:
            return to_uint128(*xstate_reader.get_ymm_register(5).ymm);
            
        case CV_REG_YMM6H:
            return to_uint128(*xstate_reader.get_ymm_register(6).ymm);

        case CV_REG_YMM7H:
            return to_uint128(*xstate_reader.get_ymm_register(7).ymm);

        // AVX integer registers
        case CV_REG_YMM0I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(0).ymm);

        case CV_REG_YMM0I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(0).ymm);

        case CV_REG_YMM0I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(0).ymm);

        case CV_REG_YMM0I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(0).ymm);

        case CV_REG_YMM1I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(1).ymm);

        case CV_REG_YMM1I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(1).ymm);

        case CV_REG_YMM1I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(1).ymm);

        case CV_REG_YMM1I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(1).ymm);

        case CV_REG_YMM2I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(2).ymm);

        case CV_REG_YMM2I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(2).ymm);

        case CV_REG_YMM2I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(2).ymm);

        case CV_REG_YMM2I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(2).ymm);

        case CV_REG_YMM3I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(3).ymm);

        case CV_REG_YMM3I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(3).ymm);

        case CV_REG_YMM3I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(3).ymm);

        case CV_REG_YMM3I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(3).ymm);

        case CV_REG_YMM4I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(4).ymm);

        case CV_REG_YMM4I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(4).ymm);

        case CV_REG_YMM4I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(4).ymm);

        case CV_REG_YMM4I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(4).ymm);

        case CV_REG_YMM5I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(5).ymm);

        case CV_REG_YMM5I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(5).ymm);

        case CV_REG_YMM5I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(5).ymm);

        case CV_REG_YMM5I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(5).ymm);

        case CV_REG_YMM6I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(6).ymm);

        case CV_REG_YMM6I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(6).ymm);

        case CV_REG_YMM6I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(6).ymm);

        case CV_REG_YMM6I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(6).ymm);

        case CV_REG_YMM7I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(7).ymm);

        case CV_REG_YMM7I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(7).ymm);

        case CV_REG_YMM7I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(7).ymm);

        case CV_REG_YMM7I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(7).ymm);

        // AVX floating-point single precise registers
        case CV_REG_YMM0F0:
            return to_float32_0(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0F1:
            return to_float32_1(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0F2:
            return to_float32_2(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0F3:
            return to_float32_3(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0F4:
            return to_float32_0(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM0F5:
            return to_float32_1(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM0F6:
            return to_float32_2(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM0F7:
            return to_float32_3(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM1F0:
            return to_float32_0(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1F1:
            return to_float32_1(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1F2:
            return to_float32_2(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1F3:
            return to_float32_3(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1F4:
            return to_float32_0(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM1F5:
            return to_float32_1(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM1F6:
            return to_float32_2(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM1F7:
            return to_float32_3(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM2F0:
            return to_float32_0(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2F1:
            return to_float32_1(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2F2:
            return to_float32_2(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2F3:
            return to_float32_3(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2F4:
            return to_float32_0(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM2F5:
            return to_float32_1(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM2F6:
            return to_float32_2(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM2F7:
            return to_float32_3(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM3F0:
            return to_float32_0(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3F1:
            return to_float32_1(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3F2:
            return to_float32_2(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3F3:
            return to_float32_3(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3F4:
            return to_float32_0(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM3F5:
            return to_float32_1(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM3F6:
            return to_float32_2(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM3F7:
            return to_float32_3(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM4F0:
            return to_float32_0(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4F1:
            return to_float32_1(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4F2:
            return to_float32_2(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4F3:
            return to_float32_3(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4F4:
            return to_float32_0(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM4F5:
            return to_float32_1(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM4F6:
            return to_float32_2(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM4F7:
            return to_float32_3(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM5F0:
            return to_float32_0(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5F1:
            return to_float32_1(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5F2:
            return to_float32_2(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5F3:
            return to_float32_3(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5F4:
            return to_float32_0(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM5F5:
            return to_float32_1(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM5F6:
            return to_float32_2(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM5F7:
            return to_float32_3(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM6F0:
            return to_float32_0(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6F1:
            return to_float32_1(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6F2:
            return to_float32_2(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6F3:
            return to_float32_3(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6F4:
            return to_float32_0(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM6F5:
            return to_float32_1(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM6F6:
            return to_float32_2(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM6F7:
            return to_float32_3(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM7F0:
            return to_float32_0(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7F1:
            return to_float32_1(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7F2:
            return to_float32_2(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7F3:
            return to_float32_3(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7F4:
            return to_float32_0(*xstate_reader.get_ymm_register(7).xmm);
        case CV_REG_YMM7F5:
            return to_float32_1(*xstate_reader.get_ymm_register(7).xmm);
        case CV_REG_YMM7F6:
            return to_float32_2(*xstate_reader.get_ymm_register(7).xmm);
        case CV_REG_YMM7F7:
            return to_float32_3(*xstate_reader.get_ymm_register(7).xmm);

        // AVX floating-point double precise registers
        case CV_REG_YMM0D0:
            return to_float64_0(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0D1:
            return to_float64_1(*xstate_reader.get_ymm_register(0).ymm);
        case CV_REG_YMM0D2:
            return to_float64_0(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM0D3:
            return to_float64_1(*xstate_reader.get_ymm_register(0).xmm);
        case CV_REG_YMM1D0:
            return to_float64_0(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1D1:
            return to_float64_1(*xstate_reader.get_ymm_register(1).ymm);
        case CV_REG_YMM1D2:
            return to_float64_0(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM1D3:
            return to_float64_1(*xstate_reader.get_ymm_register(1).xmm);
        case CV_REG_YMM2D0:
            return to_float64_0(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2D1:
            return to_float64_1(*xstate_reader.get_ymm_register(2).ymm);
        case CV_REG_YMM2D2:
            return to_float64_0(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM2D3:
            return to_float64_1(*xstate_reader.get_ymm_register(2).xmm);
        case CV_REG_YMM3D0:
            return to_float64_0(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3D1:
            return to_float64_1(*xstate_reader.get_ymm_register(3).ymm);
        case CV_REG_YMM3D2:
            return to_float64_0(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM3D3:
            return to_float64_1(*xstate_reader.get_ymm_register(3).xmm);
        case CV_REG_YMM4D0:
            return to_float64_0(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4D1:
            return to_float64_1(*xstate_reader.get_ymm_register(4).ymm);
        case CV_REG_YMM4D2:
            return to_float64_0(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM4D3:
            return to_float64_1(*xstate_reader.get_ymm_register(4).xmm);
        case CV_REG_YMM5D0:
            return to_float64_0(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5D1:
            return to_float64_1(*xstate_reader.get_ymm_register(5).ymm);
        case CV_REG_YMM5D2:
            return to_float64_0(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM5D3:
            return to_float64_1(*xstate_reader.get_ymm_register(5).xmm);
        case CV_REG_YMM6D0:
            return to_float64_0(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6D1:
            return to_float64_1(*xstate_reader.get_ymm_register(6).ymm);
        case CV_REG_YMM6D2:
            return to_float64_0(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM6D3:
            return to_float64_1(*xstate_reader.get_ymm_register(6).xmm);
        case CV_REG_YMM7D0:
            return to_float64_0(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7D1:
            return to_float64_1(*xstate_reader.get_ymm_register(7).ymm);
        case CV_REG_YMM7D2:
            return to_float64_0(*xstate_reader.get_ymm_register(7).xmm);
        case CV_REG_YMM7D3:
            return to_float64_1(*xstate_reader.get_ymm_register(7).xmm);

        case CV_ALLREG_VFRAME:
            return make_register_value<uint32_t>(address_offset);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in WOW64 (x86) context", static_cast<uint32_t>(register_type))};
        }
    }

    reg_value_t get_x64_register_value(CV_HREG_e register_type, dlg_help_utils::stream_thread_context::context_x64 const& context, uint64_t const address_offset)
    {
        dlg_help_utils::xstate_reader xstate_reader{ &context };
        
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

        case CV_AMD64_SIL:
            return make_register_value<uint8_t>(context.Rsi);

        case CV_AMD64_DIL:
            return make_register_value<uint8_t>(context.Rdi);

        case CV_AMD64_BPL:
            return make_register_value<uint8_t>(context.Rbp);

        case CV_AMD64_SPL:
            return make_register_value<uint8_t>(context.Rsp);

        // Debug registers
        case CV_AMD64_DR0:
            return make_register_value<uint64_t>(context.Dr0);

        case CV_AMD64_DR1:
            return make_register_value<uint64_t>(context.Dr1);

        case CV_AMD64_DR2:
            return make_register_value<uint64_t>(context.Dr2);

        case CV_AMD64_DR3:
            return make_register_value<uint64_t>(context.Dr3);

        case CV_AMD64_DR6:
            return make_register_value<uint64_t>(context.Dr6);

        case CV_AMD64_DR7:
            return make_register_value<uint64_t>(context.Dr7);


        case CV_AMD64_ST0:
            return to_float80(context.Legacy[0]);

        case CV_AMD64_ST1:
            return to_float80(context.Legacy[1]);

        case CV_AMD64_ST2:
            return to_float80(context.Legacy[2]);

        case CV_AMD64_ST3:
            return to_float80(context.Legacy[3]);

        case CV_AMD64_ST4:
            return to_float80(context.Legacy[4]);

        case CV_AMD64_ST5:
            return to_float80(context.Legacy[5]);

        case CV_AMD64_ST6:
            return to_float80(context.Legacy[6]);

        case CV_AMD64_ST7:
            return to_float80(context.Legacy[7]);

        case CV_AMD64_CTRL:
            return context.FltSave.ControlWord;

        case CV_AMD64_STAT:
            return context.FltSave.StatusWord;

        case CV_AMD64_TAG:
            return context.FltSave.TagWord;

        case CV_AMD64_FPIP:
            return static_cast<uint32_t>(context.FltSave.ErrorOffset);

        case CV_AMD64_FPCS:
            return context.FltSave.ErrorSelector;

        case CV_AMD64_FPDO:
            return static_cast<uint32_t>(context.FltSave.DataOffset);

        case CV_AMD64_FPDS:
            return context.FltSave.DataSelector;

        case CV_AMD64_MM0:
            return to_uint64_low(context.Legacy[0]);

        case CV_AMD64_MM1:
            return to_uint64_low(context.Legacy[1]);

        case CV_AMD64_MM2:
            return to_uint64_low(context.Legacy[2]);

        case CV_AMD64_MM3:
            return to_uint64_low(context.Legacy[3]);

        case CV_AMD64_MM4:
            return to_uint64_low(context.Legacy[4]);

        case CV_AMD64_MM5:
            return to_uint64_low(context.Legacy[5]);

        case CV_AMD64_MM6:
            return to_uint64_low(context.Legacy[6]);

        case CV_AMD64_MM7:
            return to_uint64_low(context.Legacy[7]);

            // KATMAI registers
        case CV_AMD64_XMM0:
            return to_uint128(context.Xmm0);
        case CV_AMD64_XMM1:
            return to_uint128(context.Xmm1);
        case CV_AMD64_XMM2:
            return to_uint128(context.Xmm2);
        case CV_AMD64_XMM3:
            return to_uint128(context.Xmm3);
        case CV_AMD64_XMM4:
            return to_uint128(context.Xmm4);
        case CV_AMD64_XMM5:
            return to_uint128(context.Xmm5);
        case CV_AMD64_XMM6:
            return to_uint128(context.Xmm6);
        case CV_AMD64_XMM7:
            return to_uint128(context.Xmm7);

            // KATMAI sub-registers
        case CV_AMD64_XMM0_0:
            return to_uint32_0(context.Xmm0);
        case CV_AMD64_XMM0_1:
            return to_uint32_1(context.Xmm0);
        case CV_AMD64_XMM0_2:
            return to_uint32_2(context.Xmm0);
        case CV_AMD64_XMM0_3:
            return to_uint32_3(context.Xmm0);
        case CV_AMD64_XMM1_0:
            return to_uint32_0(context.Xmm1);
        case CV_AMD64_XMM1_1:
            return to_uint32_1(context.Xmm1);
        case CV_AMD64_XMM1_2:
            return to_uint32_2(context.Xmm1);
        case CV_AMD64_XMM1_3:
            return to_uint32_3(context.Xmm1);
        case CV_AMD64_XMM2_0:
            return to_uint32_0(context.Xmm2);
        case CV_AMD64_XMM2_1:
            return to_uint32_1(context.Xmm2);
        case CV_AMD64_XMM2_2:
            return to_uint32_2(context.Xmm2);
        case CV_AMD64_XMM2_3:
            return to_uint32_3(context.Xmm2);
        case CV_AMD64_XMM3_0:
            return to_uint32_0(context.Xmm3);
        case CV_AMD64_XMM3_1:
            return to_uint32_1(context.Xmm3);
        case CV_AMD64_XMM3_2:
            return to_uint32_2(context.Xmm3);
        case CV_AMD64_XMM3_3:
            return to_uint32_3(context.Xmm3);
        case CV_AMD64_XMM4_0:
            return to_uint32_0(context.Xmm4);
        case CV_AMD64_XMM4_1:
            return to_uint32_1(context.Xmm4);
        case CV_AMD64_XMM4_2:
            return to_uint32_2(context.Xmm4);
        case CV_AMD64_XMM4_3:
            return to_uint32_3(context.Xmm4);
        case CV_AMD64_XMM5_0:
            return to_uint32_0(context.Xmm5);
        case CV_AMD64_XMM5_1:
            return to_uint32_1(context.Xmm5);
        case CV_AMD64_XMM5_2:
            return to_uint32_2(context.Xmm5);
        case CV_AMD64_XMM5_3:
            return to_uint32_3(context.Xmm5);
        case CV_AMD64_XMM6_0:
            return to_uint32_0(context.Xmm6);
        case CV_AMD64_XMM6_1:
            return to_uint32_1(context.Xmm6);
        case CV_AMD64_XMM6_2:
            return to_uint32_2(context.Xmm6);
        case CV_AMD64_XMM6_3:
            return to_uint32_3(context.Xmm6);
        case CV_AMD64_XMM7_0:
            return to_uint32_0(context.Xmm7);
        case CV_AMD64_XMM7_1:
            return to_uint32_1(context.Xmm7);
        case CV_AMD64_XMM7_2:
            return to_uint32_2(context.Xmm7);
        case CV_AMD64_XMM7_3:
            return to_uint32_3(context.Xmm7);

        // AVX registers 256 bits
        case CV_AMD64_YMM0:
            return to_uint128(*xstate_reader.get_ymm_register(0).xmm);

        case CV_AMD64_YMM1:
            return to_uint128(*xstate_reader.get_ymm_register(1).xmm);

        case CV_AMD64_YMM2:
            return to_uint128(*xstate_reader.get_ymm_register(2).xmm);

        case CV_AMD64_YMM3:
            return to_uint128(*xstate_reader.get_ymm_register(3).xmm);

        case CV_AMD64_YMM4:
            return to_uint128(*xstate_reader.get_ymm_register(4).xmm);

        case CV_AMD64_YMM5:
            return to_uint128(*xstate_reader.get_ymm_register(5).xmm);

        case CV_AMD64_YMM6:
            return to_uint128(*xstate_reader.get_ymm_register(6).xmm);

        case CV_AMD64_YMM7:
            return to_uint128(*xstate_reader.get_ymm_register(7).xmm);

        case CV_AMD64_YMM8:
            return to_uint128(*xstate_reader.get_ymm_register(8).xmm);

        case CV_AMD64_YMM9:
            return to_uint128(*xstate_reader.get_ymm_register(9).xmm);

        case CV_AMD64_YMM10:
            return to_uint128(*xstate_reader.get_ymm_register(10).xmm);

        case CV_AMD64_YMM11:
            return to_uint128(*xstate_reader.get_ymm_register(11).xmm);

        case CV_AMD64_YMM12:
            return to_uint128(*xstate_reader.get_ymm_register(12).xmm);

        case CV_AMD64_YMM13:
            return to_uint128(*xstate_reader.get_ymm_register(13).xmm);

        case CV_AMD64_YMM14:
            return to_uint128(*xstate_reader.get_ymm_register(14).xmm);

        case CV_AMD64_YMM15:
            return to_uint128(*xstate_reader.get_ymm_register(15).xmm);


        // AVX registers upper 128 bits
        case CV_AMD64_YMM0H:
            return to_uint128(*xstate_reader.get_ymm_register(0).ymm);
            
        case CV_AMD64_YMM1H:
            return to_uint128(*xstate_reader.get_ymm_register(1).ymm);

        case CV_AMD64_YMM2H:
            return to_uint128(*xstate_reader.get_ymm_register(2).ymm);

        case CV_AMD64_YMM3H:
            return to_uint128(*xstate_reader.get_ymm_register(3).ymm);

        case CV_AMD64_YMM4H:
            return to_uint128(*xstate_reader.get_ymm_register(4).ymm);

        case CV_AMD64_YMM5H:
            return to_uint128(*xstate_reader.get_ymm_register(5).ymm);
            
        case CV_AMD64_YMM6H:
            return to_uint128(*xstate_reader.get_ymm_register(6).ymm);

        case CV_AMD64_YMM7H:
            return to_uint128(*xstate_reader.get_ymm_register(7).ymm);

        case CV_AMD64_YMM8H:
            return to_uint128(*xstate_reader.get_ymm_register(8).ymm);

        case CV_AMD64_YMM9H:
            return to_uint128(*xstate_reader.get_ymm_register(9).ymm);

        case CV_AMD64_YMM10H:
            return to_uint128(*xstate_reader.get_ymm_register(10).ymm);

        case CV_AMD64_YMM11H:
            return to_uint128(*xstate_reader.get_ymm_register(11).ymm);

        case CV_AMD64_YMM12H:
            return to_uint128(*xstate_reader.get_ymm_register(12).ymm);

        case CV_AMD64_YMM13H:
            return to_uint128(*xstate_reader.get_ymm_register(13).ymm);

        case CV_AMD64_YMM14H:
            return to_uint128(*xstate_reader.get_ymm_register(14).ymm);

        case CV_AMD64_YMM15H:
            return to_uint128(*xstate_reader.get_ymm_register(15).ymm);

        // ReSharper disable once CommentTypo
        //Lower/upper 8 bytes of XMM registers.  Unlike CV_AMD64_XMM<regnum><H/L>, these
        //values represent the bit patterns of the registers as 64-bit integers, not
        //the representation of these registers as a double.
        case CV_AMD64_XMM0IL:
            return to_uint64_low(context.FltSave.XmmRegisters[0]);

        case CV_AMD64_XMM1IL:
            return to_uint64_low(context.FltSave.XmmRegisters[1]);

        case CV_AMD64_XMM2IL:
            return to_uint64_low(context.FltSave.XmmRegisters[2]);

        case CV_AMD64_XMM3IL:
            return to_uint64_low(context.FltSave.XmmRegisters[3]);

        case CV_AMD64_XMM4IL:
            return to_uint64_low(context.FltSave.XmmRegisters[4]);

        case CV_AMD64_XMM5IL:
            return to_uint64_low(context.FltSave.XmmRegisters[5]);

        case CV_AMD64_XMM6IL:
            return to_uint64_low(context.FltSave.XmmRegisters[6]);

        case CV_AMD64_XMM7IL:
            return to_uint64_low(context.FltSave.XmmRegisters[7]);

        case CV_AMD64_XMM8IL:
            return to_uint64_low(context.FltSave.XmmRegisters[8]);

        case CV_AMD64_XMM9IL:
            return to_uint64_low(context.FltSave.XmmRegisters[9]);

        case CV_AMD64_XMM10IL:
            return to_uint64_low(context.FltSave.XmmRegisters[10]);

        case CV_AMD64_XMM11IL:
            return to_uint64_low(context.FltSave.XmmRegisters[11]);

        case CV_AMD64_XMM12IL:
            return to_uint64_low(context.FltSave.XmmRegisters[12]);

        case CV_AMD64_XMM13IL:
            return to_uint64_low(context.FltSave.XmmRegisters[13]);

        case CV_AMD64_XMM14IL:
            return to_uint64_low(context.FltSave.XmmRegisters[14]);

        case CV_AMD64_XMM15IL:
            return to_uint64_low(context.FltSave.XmmRegisters[15]);


        case CV_AMD64_XMM0IH:
            return to_uint64_high(context.FltSave.XmmRegisters[0]);

        case CV_AMD64_XMM1IH:
            return to_uint64_high(context.FltSave.XmmRegisters[1]);

        case CV_AMD64_XMM2IH:
            return to_uint64_high(context.FltSave.XmmRegisters[2]);

        case CV_AMD64_XMM3IH:
            return to_uint64_high(context.FltSave.XmmRegisters[3]);

        case CV_AMD64_XMM4IH:
            return to_uint64_high(context.FltSave.XmmRegisters[4]);

        case CV_AMD64_XMM5IH:
            return to_uint64_high(context.FltSave.XmmRegisters[5]);

        case CV_AMD64_XMM6IH:
            return to_uint64_high(context.FltSave.XmmRegisters[6]);

        case CV_AMD64_XMM7IH:
            return to_uint64_high(context.FltSave.XmmRegisters[7]);

        case CV_AMD64_XMM8IH:
            return to_uint64_high(context.FltSave.XmmRegisters[8]);

        case CV_AMD64_XMM9IH:
            return to_uint64_high(context.FltSave.XmmRegisters[9]);

        case CV_AMD64_XMM10IH:
            return to_uint64_high(context.FltSave.XmmRegisters[10]);

        case CV_AMD64_XMM11IH:
            return to_uint64_high(context.FltSave.XmmRegisters[11]);

        case CV_AMD64_XMM12IH:
            return to_uint64_high(context.FltSave.XmmRegisters[12]);

        case CV_AMD64_XMM13IH:
            return to_uint64_high(context.FltSave.XmmRegisters[13]);

        case CV_AMD64_XMM14IH:
            return to_uint64_high(context.FltSave.XmmRegisters[14]);

        case CV_AMD64_XMM15IH:
            return to_uint64_high(context.FltSave.XmmRegisters[15]);

        // AVX integer registers
        case CV_AMD64_YMM0I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(0).ymm);

        case CV_AMD64_YMM0I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(0).ymm);

        case CV_AMD64_YMM0I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(0).ymm);

        case CV_AMD64_YMM0I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(0).ymm);

        case CV_AMD64_YMM1I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(1).ymm);

        case CV_AMD64_YMM1I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(1).ymm);

        case CV_AMD64_YMM1I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(1).ymm);

        case CV_AMD64_YMM1I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(1).ymm);

        case CV_AMD64_YMM2I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(2).ymm);

        case CV_AMD64_YMM2I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(2).ymm);

        case CV_AMD64_YMM2I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(2).ymm);

        case CV_AMD64_YMM2I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(2).ymm);

        case CV_AMD64_YMM3I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(3).ymm);

        case CV_AMD64_YMM3I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(3).ymm);

        case CV_AMD64_YMM3I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(3).ymm);

        case CV_AMD64_YMM3I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(3).ymm);

        case CV_AMD64_YMM4I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(4).ymm);

        case CV_AMD64_YMM4I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(4).ymm);

        case CV_AMD64_YMM4I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(4).ymm);

        case CV_AMD64_YMM4I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(4).ymm);

        case CV_AMD64_YMM5I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(5).ymm);

        case CV_AMD64_YMM5I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(5).ymm);

        case CV_AMD64_YMM5I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(5).ymm);

        case CV_AMD64_YMM5I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(5).ymm);

        case CV_AMD64_YMM6I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(6).ymm);

        case CV_AMD64_YMM6I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(6).ymm);

        case CV_AMD64_YMM6I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(6).ymm);

        case CV_AMD64_YMM6I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(6).ymm);

        case CV_AMD64_YMM7I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(7).ymm);

        case CV_AMD64_YMM7I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(7).ymm);

        case CV_AMD64_YMM7I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(7).ymm);

        case CV_AMD64_YMM7I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(7).ymm);

        case CV_AMD64_YMM8I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(8).ymm);

        case CV_AMD64_YMM8I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(8).ymm);

        case CV_AMD64_YMM8I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(8).ymm);

        case CV_AMD64_YMM8I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(8).ymm);

        case CV_AMD64_YMM9I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(9).ymm);

        case CV_AMD64_YMM9I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(9).ymm);

        case CV_AMD64_YMM9I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(9).ymm);

        case CV_AMD64_YMM9I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(9).ymm);

        case CV_AMD64_YMM10I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(10).ymm);

        case CV_AMD64_YMM10I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(10).ymm);

        case CV_AMD64_YMM10I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(10).ymm);

        case CV_AMD64_YMM10I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(10).ymm);

        case CV_AMD64_YMM11I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(11).ymm);

        case CV_AMD64_YMM11I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(11).ymm);

        case CV_AMD64_YMM11I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(11).ymm);

        case CV_AMD64_YMM11I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(11).ymm);

        case CV_AMD64_YMM12I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(12).ymm);

        case CV_AMD64_YMM12I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(12).ymm);

        case CV_AMD64_YMM12I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(12).ymm);

        case CV_AMD64_YMM12I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(12).ymm);

        case CV_AMD64_YMM13I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(13).ymm);

        case CV_AMD64_YMM13I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(13).ymm);

        case CV_AMD64_YMM13I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(13).ymm);

        case CV_AMD64_YMM13I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(13).ymm);

        case CV_AMD64_YMM14I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(14).ymm);

        case CV_AMD64_YMM14I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(14).ymm);

        case CV_AMD64_YMM14I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(14).ymm);

        case CV_AMD64_YMM14I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(14).ymm);

        case CV_AMD64_YMM15I0:
            return to_uint32_0(*xstate_reader.get_ymm_register(15).ymm);

        case CV_AMD64_YMM15I1:
            return to_uint32_1(*xstate_reader.get_ymm_register(15).ymm);

        case CV_AMD64_YMM15I2:
            return to_uint32_2(*xstate_reader.get_ymm_register(15).ymm);

        case CV_AMD64_YMM15I3:
            return to_uint32_3(*xstate_reader.get_ymm_register(15).ymm);

        // AVX floating-point single precise registers
        case CV_AMD64_YMM0F0:
            return to_float32_0(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0F1:
            return to_float32_1(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0F2:
            return to_float32_2(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0F3:
            return to_float32_3(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0F4:
            return to_float32_0(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM0F5:
            return to_float32_1(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM0F6:
            return to_float32_2(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM0F7:
            return to_float32_3(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM1F0:
            return to_float32_0(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1F1:
            return to_float32_1(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1F2:
            return to_float32_2(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1F3:
            return to_float32_3(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1F4:
            return to_float32_0(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM1F5:
            return to_float32_1(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM1F6:
            return to_float32_2(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM1F7:
            return to_float32_3(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM2F0:
            return to_float32_0(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2F1:
            return to_float32_1(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2F2:
            return to_float32_2(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2F3:
            return to_float32_3(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2F4:
            return to_float32_0(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM2F5:
            return to_float32_1(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM2F6:
            return to_float32_2(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM2F7:
            return to_float32_3(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM3F0:
            return to_float32_0(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3F1:
            return to_float32_1(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3F2:
            return to_float32_2(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3F3:
            return to_float32_3(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3F4:
            return to_float32_0(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM3F5:
            return to_float32_1(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM3F6:
            return to_float32_2(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM3F7:
            return to_float32_3(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM4F0:
            return to_float32_0(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4F1:
            return to_float32_1(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4F2:
            return to_float32_2(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4F3:
            return to_float32_3(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4F4:
            return to_float32_0(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM4F5:
            return to_float32_1(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM4F6:
            return to_float32_2(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM4F7:
            return to_float32_3(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM5F0:
            return to_float32_0(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5F1:
            return to_float32_1(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5F2:
            return to_float32_2(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5F3:
            return to_float32_3(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5F4:
            return to_float32_0(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM5F5:
            return to_float32_1(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM5F6:
            return to_float32_2(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM5F7:
            return to_float32_3(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM6F0:
            return to_float32_0(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6F1:
            return to_float32_1(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6F2:
            return to_float32_2(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6F3:
            return to_float32_3(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6F4:
            return to_float32_0(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM6F5:
            return to_float32_1(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM6F6:
            return to_float32_2(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM6F7:
            return to_float32_3(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM7F0:
            return to_float32_0(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7F1:
            return to_float32_1(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7F2:
            return to_float32_2(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7F3:
            return to_float32_3(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7F4:
            return to_float32_0(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM7F5:
            return to_float32_1(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM7F6:
            return to_float32_2(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM7F7:
            return to_float32_3(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM8F0:
            return to_float32_0(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8F1:
            return to_float32_1(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8F2:
            return to_float32_2(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8F3:
            return to_float32_3(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8F4:
            return to_float32_0(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM8F5:
            return to_float32_1(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM8F6:
            return to_float32_2(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM8F7:
            return to_float32_3(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM9F0:
            return to_float32_0(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9F1:
            return to_float32_1(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9F2:
            return to_float32_2(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9F3:
            return to_float32_3(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9F4:
            return to_float32_0(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM9F5:
            return to_float32_1(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM9F6:
            return to_float32_2(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM9F7:
            return to_float32_3(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM10F0:
            return to_float32_0(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10F1:
            return to_float32_1(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10F2:
            return to_float32_2(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10F3:
            return to_float32_3(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10F4:
            return to_float32_0(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM10F5:
            return to_float32_1(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM10F6:
            return to_float32_2(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM10F7:
            return to_float32_3(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM11F0:
            return to_float32_0(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11F1:
            return to_float32_1(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11F2:
            return to_float32_2(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11F3:
            return to_float32_3(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11F4:
            return to_float32_0(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM11F5:
            return to_float32_1(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM11F6:
            return to_float32_2(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM11F7:
            return to_float32_3(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM12F0:
            return to_float32_0(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12F1:
            return to_float32_1(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12F2:
            return to_float32_2(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12F3:
            return to_float32_3(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12F4:
            return to_float32_0(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM12F5:
            return to_float32_1(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM12F6:
            return to_float32_2(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM12F7:
            return to_float32_3(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM13F0:
            return to_float32_0(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13F1:
            return to_float32_1(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13F2:
            return to_float32_2(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13F3:
            return to_float32_3(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13F4:
            return to_float32_0(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM13F5:
            return to_float32_1(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM13F6:
            return to_float32_2(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM13F7:
            return to_float32_3(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM14F0:
            return to_float32_0(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14F1:
            return to_float32_1(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14F2:
            return to_float32_2(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14F3:
            return to_float32_3(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14F4:
            return to_float32_0(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM14F5:
            return to_float32_1(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM14F6:
            return to_float32_2(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM14F7:
            return to_float32_3(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM15F0:
            return to_float32_0(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15F1:
            return to_float32_1(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15F2:
            return to_float32_2(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15F3:
            return to_float32_3(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15F4:
            return to_float32_0(*xstate_reader.get_ymm_register(15).xmm);
        case CV_AMD64_YMM15F5:
            return to_float32_1(*xstate_reader.get_ymm_register(15).xmm);
        case CV_AMD64_YMM15F6:
            return to_float32_2(*xstate_reader.get_ymm_register(15).xmm);
        case CV_AMD64_YMM15F7:
            return to_float32_3(*xstate_reader.get_ymm_register(15).xmm);

        // AVX floating-point double precise registers
        case CV_AMD64_YMM0D0:
            return to_float64_0(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0D1:
            return to_float64_1(*xstate_reader.get_ymm_register(0).ymm);
        case CV_AMD64_YMM0D2:
            return to_float64_0(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM0D3:
            return to_float64_1(*xstate_reader.get_ymm_register(0).xmm);
        case CV_AMD64_YMM1D0:
            return to_float64_0(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1D1:
            return to_float64_1(*xstate_reader.get_ymm_register(1).ymm);
        case CV_AMD64_YMM1D2:
            return to_float64_0(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM1D3:
            return to_float64_1(*xstate_reader.get_ymm_register(1).xmm);
        case CV_AMD64_YMM2D0:
            return to_float64_0(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2D1:
            return to_float64_1(*xstate_reader.get_ymm_register(2).ymm);
        case CV_AMD64_YMM2D2:
            return to_float64_0(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM2D3:
            return to_float64_1(*xstate_reader.get_ymm_register(2).xmm);
        case CV_AMD64_YMM3D0:
            return to_float64_0(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3D1:
            return to_float64_1(*xstate_reader.get_ymm_register(3).ymm);
        case CV_AMD64_YMM3D2:
            return to_float64_0(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM3D3:
            return to_float64_1(*xstate_reader.get_ymm_register(3).xmm);
        case CV_AMD64_YMM4D0:
            return to_float64_0(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4D1:
            return to_float64_1(*xstate_reader.get_ymm_register(4).ymm);
        case CV_AMD64_YMM4D2:
            return to_float64_0(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM4D3:
            return to_float64_1(*xstate_reader.get_ymm_register(4).xmm);
        case CV_AMD64_YMM5D0:
            return to_float64_0(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5D1:
            return to_float64_1(*xstate_reader.get_ymm_register(5).ymm);
        case CV_AMD64_YMM5D2:
            return to_float64_0(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM5D3:
            return to_float64_1(*xstate_reader.get_ymm_register(5).xmm);
        case CV_AMD64_YMM6D0:
            return to_float64_0(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6D1:
            return to_float64_1(*xstate_reader.get_ymm_register(6).ymm);
        case CV_AMD64_YMM6D2:
            return to_float64_0(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM6D3:
            return to_float64_1(*xstate_reader.get_ymm_register(6).xmm);
        case CV_AMD64_YMM7D0:
            return to_float64_0(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7D1:
            return to_float64_1(*xstate_reader.get_ymm_register(7).ymm);
        case CV_AMD64_YMM7D2:
            return to_float64_0(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM7D3:
            return to_float64_1(*xstate_reader.get_ymm_register(7).xmm);
        case CV_AMD64_YMM8D0:
            return to_float64_0(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8D1:
            return to_float64_1(*xstate_reader.get_ymm_register(8).ymm);
        case CV_AMD64_YMM8D2:
            return to_float64_0(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM8D3:
            return to_float64_1(*xstate_reader.get_ymm_register(8).xmm);
        case CV_AMD64_YMM9D0:
            return to_float64_0(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9D1:
            return to_float64_1(*xstate_reader.get_ymm_register(9).ymm);
        case CV_AMD64_YMM9D2:
            return to_float64_0(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM9D3:
            return to_float64_1(*xstate_reader.get_ymm_register(9).xmm);
        case CV_AMD64_YMM10D0:
            return to_float64_0(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10D1:
            return to_float64_1(*xstate_reader.get_ymm_register(10).ymm);
        case CV_AMD64_YMM10D2:
            return to_float64_0(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM10D3:
            return to_float64_1(*xstate_reader.get_ymm_register(10).xmm);
        case CV_AMD64_YMM11D0:
            return to_float64_0(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11D1:
            return to_float64_1(*xstate_reader.get_ymm_register(11).ymm);
        case CV_AMD64_YMM11D2:
            return to_float64_0(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM11D3:
            return to_float64_1(*xstate_reader.get_ymm_register(11).xmm);
        case CV_AMD64_YMM12D0:
            return to_float64_0(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12D1:
            return to_float64_1(*xstate_reader.get_ymm_register(12).ymm);
        case CV_AMD64_YMM12D2:
            return to_float64_0(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM12D3:
            return to_float64_1(*xstate_reader.get_ymm_register(12).xmm);
        case CV_AMD64_YMM13D0:
            return to_float64_0(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13D1:
            return to_float64_1(*xstate_reader.get_ymm_register(13).ymm);
        case CV_AMD64_YMM13D2:
            return to_float64_0(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM13D3:
            return to_float64_1(*xstate_reader.get_ymm_register(13).xmm);
        case CV_AMD64_YMM14D0:
            return to_float64_0(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14D1:
            return to_float64_1(*xstate_reader.get_ymm_register(14).ymm);
        case CV_AMD64_YMM14D2:
            return to_float64_0(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM14D3:
            return to_float64_1(*xstate_reader.get_ymm_register(14).xmm);
        case CV_AMD64_YMM15D0:
            return to_float64_0(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15D1:
            return to_float64_1(*xstate_reader.get_ymm_register(15).ymm);
        case CV_AMD64_YMM15D2:
            return to_float64_0(*xstate_reader.get_ymm_register(15).xmm);
        case CV_AMD64_YMM15D3:
            return to_float64_1(*xstate_reader.get_ymm_register(15).xmm);

        case CV_ALLREG_VFRAME:
            return make_register_value<uint64_t>(address_offset);

        default:
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Register [{}] unknown or unsupported in x64 context", static_cast<uint32_t>(register_type))};
        }
    }

    registry_info get_registry_value_info(thread_context_type const type, CV_HREG_e const registry, void const* thread_context, uint64_t const address_offset)
    {
        reg_value_t value;

        switch(type)
        {
        case thread_context_type::x86:
            value = get_x86_register_value(registry, *static_cast<dlg_help_utils::stream_thread_context::context_x86 const*>(thread_context), address_offset);
            break;
        case thread_context_type::wow64:
            value = get_wow64_register_value(registry, *static_cast<WOW64_CONTEXT const*>(thread_context), address_offset);
            break;
        case thread_context_type::x64:
            value = get_x64_register_value(registry, *static_cast<dlg_help_utils::stream_thread_context::context_x64 const*>(thread_context), address_offset);
            break;
        }

        return {.register_type= registry, .value= value};
    }

    uint64_t to_address_offset(reg_value_t const& value)
    {
        return std::visit(dlg_help_utils::overload{
                                // []<Integral T>(T arg) -> uint64_t { return arg; },
                                [](uint64_t arg) -> uint64_t { return arg; },
                                [](uint32_t arg) -> uint64_t { return arg; },
                                [](uint16_t arg) -> uint64_t { return arg; },
                                [](uint8_t arg) -> uint64_t { return arg; },
                                []<typename T>(T&&) -> uint64_t 
                                {
                                    throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"invalid address offset type [{}]", dlg_help_utils::string_conversation::utf8_to_wstring(typeid(T).name()))};
                                }, 
                          },
                          value);
    }

    frame_data_info get_frame_data_info(PSYMBOL_INFOW symbol_info, uint64_t const address_offset)
    {
        return frame_data_info{.data_offset= static_cast<int>(symbol_info->Address), .data_address= address_offset + symbol_info->Address, .data_size= symbol_info->Size};
    }

    bool is_already_found(std::vector<variable> const& variables, PSYMBOL_INFOW symbol_info)
    {
        return std::ranges::any_of(variables, [symbol_info](variable const& variable)
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

        try
        {
            // check for duplicates...
            if(is_already_found(is_parameter ? info.parameters : info.locals, symbol_info))
            {
                return TRUE;
            }

            auto& variable = is_parameter ? info.parameters.emplace_back(info.symbol_cache.get_or_create_symbol_type_info(info.process, symbol_info->ModBase, symbol_info->Index))
                                          : info.locals.emplace_back(info.symbol_cache.get_or_create_symbol_type_info(info.process, symbol_info->ModBase, symbol_info->Index));

            if (symbol_info->Flags & SYMFLAG_REGREL)
            {
                variable.registry_value = get_registry_value_info(info.type, static_cast<CV_HREG_e>(symbol_info->Register), info.thread_context, info.frame_address_offset);
                variable.frame_data = get_frame_data_info(symbol_info, to_address_offset(variable.registry_value->value));
            }
            else if (symbol_info->Flags & SYMFLAG_REGISTER)
            {
                variable.registry_value = get_registry_value_info(info.type, static_cast<CV_HREG_e>(symbol_info->Register), info.thread_context, info.frame_address_offset);
            }
            else if (symbol_info->Flags & SYMFLAG_FRAMEREL)
            {
                variable.frame_data = get_frame_data_info(symbol_info, info.frame_address_offset);
            }

            return TRUE;
        }
        catch(std::exception const& e)
        {
            info.failure = e;
            throw;
        }
    }
    // ReSharper restore CppParameterMayBeConst
}

namespace dlg_help_utils::dbg_help
{
    symbol_engine::symbol_engine(i_symbol_load_callback& callback)
        : load_callback_{&callback}
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

        std::array<wchar_t, 8096> buffer;
        if (!SymGetSearchPathW(process_, buffer.data(), static_cast<DWORD>(buffer.size() - 1)))
        {
            windows_error::throw_windows_api_error(L"SymGetSearchPath"sv);
        }
        buffer[buffer.size() - 1] = NULL;
        original_symbol_path_ = buffer.data();
        if(!original_symbol_path_.empty() && *original_symbol_path_.rbegin() != L';')
        {
            original_symbol_path_ += L';';
        }
    }

    symbol_engine::~symbol_engine()
    {
        SymCleanup(process_);
    }

    void symbol_engine::add_symbol_path(std::filesystem::path const& path)
    {
        if(auto [_, inserted] = extra_symbol_paths_.insert(path.wstring());
            inserted)
        {
            auto symbol_path = original_symbol_path_ + wjoin(extra_symbol_paths_ | std::views::reverse, L";"sv);
            if (!SymSetSearchPathW(process_, symbol_path.c_str()))
            {
                windows_error::throw_windows_api_error(L"SymSetSearchPath"sv);
            }
        }
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

    void symbol_engine::load_module(std::wstring module_name
        , DWORD64 const module_base
        , DWORD const module_size
        , DWORD const module_time_stamp
        , DWORD const module_check_sum
        , void const* cv_record
        , DWORD const cv_record_size
        , void const* misc_record
        , DWORD const misc_record_size
        , [[maybe_unused]] VS_FIXEDFILEINFO const& version_info
        , throw_on_error_t const throw_on_error)
    {
        std::filesystem::path const path{module_name};
        auto const module = path.filename().wstring();

        cv_info_pdb70 const pdb{cv_record, cv_record_size};
        DWORD64 handle{0};

        if (pdb.is_valid())
        {
            auto const debug_module_info_size = sizeof(IMAGE_DEBUG_DIRECTORY) + cv_record_size;

            // Most recent DbgHelp.dll now requires sizeof(IMAGE_DEBUG_DIRECTORY) + cv_record_size to be 8bytes-aligned
            constexpr auto mandatoryAlignment = 8;
            auto const debug_module_info_size_aligned = (debug_module_info_size + mandatoryAlignment) & (~static_cast<uint64_t>(mandatoryAlignment - 1));

            auto const debug_module_info = std::make_unique<uint8_t[]>(static_cast<size_t>(debug_module_info_size_aligned));
            auto* info = reinterpret_cast<IMAGE_DEBUG_DIRECTORY*>(debug_module_info.get());

            info->TimeDateStamp = module_time_stamp;
            info->Characteristics = 0;
            info->MajorVersion = HIWORD(version_info.dwProductVersionMS);
            info->MinorVersion = LOWORD(version_info.dwProductVersionMS);
            info->Type = IMAGE_DEBUG_TYPE_CODEVIEW;
            info->AddressOfRawData = 0;
            info->PointerToRawData = sizeof(IMAGE_DEBUG_DIRECTORY);
            info->SizeOfData = cv_record_size;

            memcpy(debug_module_info.get() + info->PointerToRawData, cv_record, cv_record_size);

            MODLOAD_DATA module_load_info;
            module_load_info.ssize = sizeof(module_load_info);
            module_load_info.ssig = DBHHEADER_DEBUGDIRS;
            module_load_info.data = debug_module_info.get();
            module_load_info.size = static_cast<DWORD>(debug_module_info_size_aligned);
            module_load_info.flags = 0;

            if (load_callback().symbol_load_debug())
            {
                auto log_handle = load_callback().log_stream();
                log_handle.log() << L" loading: DBHHEADER_DEBUGDIRS\n";
                log_handle.log() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) << L'\n';
                log_handle.log() << L" loading: PdbAge:" << pdb.get_age() << L'\n';
                log_handle.log() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                log_handle.log() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                log_handle.log() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info, throw_on_error);
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

            if (load_callback().symbol_load_debug())
            {
                auto log_handle = load_callback().log_stream();
                log_handle.log() << L" loading: DBHHEADER_PDBGUID\n";
                log_handle.log() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) << L'\n';
                log_handle.log() << L" loading: PdbAge:" << pdb.get_age() << L'\n';
                log_handle.log() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                log_handle.log() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                log_handle.log() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info, throw_on_error);
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

            if (load_callback().symbol_load_debug())
            {
                auto log_handle = load_callback().log_stream();
                log_handle.log() << L" loading: DBHHEADER_CVMISC\n";
                log_handle.log() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                log_handle.log() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                log_handle.log() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, &module_load_info, throw_on_error);
        }

        if (handle == 0)
        {
            if (load_callback().symbol_load_debug())
            {
                auto log_handle = load_callback().log_stream();
                log_handle.log() << L" loading: default options\n";
                log_handle.log() << L" loading: module_size:" << to_hex(module_size) << L'\n';
                log_handle.log() << L" loading: check_sum:" << to_hex(module_check_sum) << L'\n';
                log_handle.log() << L" loading: module_time_stamp:" << to_hex(module_time_stamp) << L'\n';
            }

            handle = load_module(module, module_base, module_size, nullptr, throw_on_error);
        }

        std::wstring module_image_path;
        if (handle != 0)
        {
            module_image_path = load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        stream_module_name name{module_name};
        modules_.insert(std::make_pair(std::move(module_name), module_info{.handle= handle, .base= module_base, .size= module_size, .module_image_path= std::move(module_image_path), .name= std::move(name)}));
    }

    void symbol_engine::load_module(std::wstring module_name
        , DWORD64 const module_base
        , DWORD const module_size
        , DWORD const module_time_stamp
        , DWORD const module_check_sum
        , throw_on_error_t const throw_on_error)
    {
        std::filesystem::path const path{module_name};
        auto const module = path.filename().wstring();

        auto const handle = load_module(module, module_base, module_size, nullptr, throw_on_error);
        std::wstring module_image_path;

        if (handle != 0)
        {
            module_image_path = load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        stream_module_name name {module_name};
        modules_.insert(std::make_pair(std::move(module_name), module_info{.handle= handle, .base= module_base, .size= module_size, .module_image_path= std::move(module_image_path), .name= std::move(name)}));
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

    std::wstring symbol_engine::find_module_name(std::wstring_view const& base_module_name) const
    {
        auto const it = std::ranges::find_if(modules_, [&base_module_name](auto const& value) { return string_utils::i_ends_with(value.first, base_module_name); });
        if(it == modules_.end())
        {
            return {};
        }

        return it->first;
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

    generator<std::wstring> symbol_engine::loaded_modules() const
    {
        for (const auto& module_name : modules_ | std::views::keys)
        {
            co_yield module_name;
        }
    }

    std::optional<symbol_type_info> symbol_engine::get_type_info(std::wstring const& type_name, throw_on_error_t const throw_on_error)
    {
        auto [module_name, specific_type_name] = parse_type_info(type_name);
        return get_type_info(module_name, specific_type_name, throw_on_error);
    }

    std::optional<symbol_type_info> symbol_engine::get_type_info(std::wstring const& module_name, std::wstring const& type_name, throw_on_error_t const throw_on_error)
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
                if(auto rv = load_type_info(module.base, type_name, throw_on_error); rv.has_value())
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

        return load_type_info(it->second.base, type_name, throw_on_error);
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

        find_symbol_callback_context context{process_, types, symbol_cache_, L""};
        if(!SymEnumTypesW(process_, it->second.base, find_symbol_callback, &context))
        {
            windows_error::throw_windows_api_error(L"SymEnumTypesW"sv, to_hex(it->second.base));
        }

        return types;
    }

    std::optional<symbol_type_info> symbol_engine::get_symbol_info(std::wstring const& symbol_name, throw_on_error_t const throw_on_error)
    {
        if(symbol_name.empty())
        {
            return std::nullopt;
        }

        auto const info = symbol_info_buffer::make();
        if(!SymFromNameW(process_, symbol_name.c_str(), &info->info))
        {
            if(throw_on_error)
            {
                windows_error::throw_windows_api_error(L"SymFromNameW"sv, symbol_name);
            }
            return std::nullopt;
        }
            
        return symbol_cache_.get_or_create_symbol_type_info(process_, info->info.ModBase, info->info.Index);
    }

    std::vector<symbol_type_info> symbol_engine::symbol_walk(std::wstring const& find_mask, std::optional<ULONG64> const module_base, symbol_walk_options const option)
    {
        std::vector<symbol_type_info> symbols;
        find_symbol_callback_context context{process_, symbols, symbol_cache_, find_mask};
        if(!SymEnumSymbolsExW(process_, module_base.value_or(0), L"*!*", find_symbol_callback, &context, setup_enum_symbol_options(option)))
        {
            windows_error::throw_windows_api_error(L"SymEnumSymbolsExW"sv, find_mask);
        }

        return symbols;
    }

    void symbol_engine::local_variables_walk(std::vector<variable>& locals
        , std::vector<variable>& parameters
        , thread_context_type const type
        , uint64_t const frame_address_offset
        , void const* thread_context
        , std::wstring const& find_mask
        , symbol_walk_options const option)
    {
        std::optional<std::exception> failure;
        local_variable_info info{process_, type, frame_address_offset, thread_context, locals, parameters, symbol_cache_, failure};
        if(!SymEnumSymbolsExW(process_, 0, find_mask.empty() ? L"*" : find_mask.c_str(), find_local_variable_callback, &info, setup_enum_symbol_options(option)))
        {
            if(auto const ec = GetLastError(); ec != ERROR_INVALID_PARAMETER && ec != ERROR_NOT_SUPPORTED)
            {
                if(failure.has_value())
                {
                    throw exceptions::wide_runtime_error{L"SymEnumSymbolsExW", failure.value()};
                }

                windows_error::throw_windows_api_error(L"SymEnumSymbolsExW"sv, find_mask, ec);
            }
        }
    }

    generator<symbol_address_info> symbol_engine::stack_walk(stream_thread_context const& thread_context) const
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
        std::ignore = g_callback->get_module_base_routine(frame.AddrPC.Offset);

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

    void symbol_engine::dump_loaded_module_information(DWORD64 const handle
        , DWORD64 const module_base
        , std::wstring const& module_image_path) const
    {
        if (handle != 0 && load_callback().symbol_load_debug())
        {
            auto log_handle = load_callback().log_stream();
            // ReSharper disable once CppUseStructuredBinding
            const auto info = get_module_information(module_base);
            log_handle.log() << L"Loaded:\n";
            log_handle.log() << L" SymType:" << info.SymType << L'\n';
            log_handle.log() << L" ModuleName:" << info.ModuleName << L'\n';
            log_handle.log() << L" ImageName:" << info.ImageName << L'\n';
            log_handle.log() << L" ModuleImagePath:" << module_image_path << L'\n';
            log_handle.log() << L" LoadedImageName:" << info.LoadedImageName << L'\n';
            log_handle.log() << L" LoadedPdbName:" << info.LoadedPdbName << L'\n';
            log_handle.log() << L" CheckSum:" << to_hex(info.CheckSum) << L'\n';
            log_handle.log() << L" ImageSize:" << to_hex(info.ImageSize) << L'\n';
            log_handle.log() << L" MachineType:" << to_hex(info.MachineType) << L'\n';
            log_handle.log() << L" PdbSig70:" << guid_utils::to_string(info.PdbSig70) << L'\n';
            log_handle.log() << L" PdbAge:" << info.PdbAge << L'\n';
            log_handle.log() << L" PdbUnmatched:" << info.PdbUnmatched << L'\n';
            log_handle.log() << L" DbgUnmatched:" << info.DbgUnmatched << L'\n';
        }
    }

    std::wstring symbol_engine::load_module_image_path(DWORD const module_size
        , DWORD module_time_stamp
        , DWORD const module_check_sum
        , std::wstring const& module_name
        , DWORD64 const handle)
    {
        if (handle != 0)
        {
            loading_module_check_sum_ = module_check_sum;
            std::array<wchar_t, MAX_PATH> buffer{};
            auto const result = SymFindFileInPathW(process_, nullptr, module_name.c_str(), &module_time_stamp,
                                                   module_size, 0, SSRVOPT_DWORDPTR, buffer.data(),
                                                   find_file_in_path_callback, this);
            loading_module_check_sum_ = 0;
            if (result)
            {
                return buffer.data();
            }

            if (load_callback().symbol_load_debug())
            {
                auto const ec = GetLastError();
                load_callback().log_stream().log() << std::format(L"Failed to find module image path for [{0}] - SymFindFileInPathW: {1} - {2}\n", module_name, ec, windows_error::get_windows_error_string(ec));
            }
        }

        return {};
    }

    DWORD64 symbol_engine::load_module(std::wstring const& module_name
        , DWORD64 const module_base
        , DWORD const module_size
        , MODLOAD_DATA* module_load_info
        , throw_on_error_t const throw_on_error)
    {
        loading_module_ = true;
        auto loading_module_handle = make_scope_exit([this]()
        {
            loading_module_ = false;
        });

        auto rv = SymLoadModuleExW(process_, nullptr, module_name.c_str(), nullptr, module_base, module_size, module_load_info, 0);

        if(rv == 0)
        {
            if(auto const ec = GetLastError(); ec == ERROR_SUCCESS)
            {
                rv = module_base;
            }
            else if(throw_on_error)
            {
                windows_error::throw_windows_api_error(L"SymLoadModuleExW"sv, module_name, ec);
            }
        }

        return rv;
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

    std::optional<symbol_type_info> symbol_engine::load_type_info(DWORD64 const module_base, std::wstring const& type_name, throw_on_error_t const throw_on_error)
    {
        if(auto const rv = get_cached_type_info(type_name); rv.has_value())
        {
            return rv.value();
        }

        auto const info = symbol_info_buffer::make();
        if(!SymGetTypeFromNameW(process_, module_base, type_name.c_str(), &info->info))
        {
            if(throw_on_error)
            {
                windows_error::throw_windows_api_error(L"SymGetTypeFromNameW"sv, type_name);
            }

            set_cached_type_info(type_name, std::nullopt);
            return std::nullopt;
        }

        auto rv = symbol_cache_.get_or_create_symbol_type_info(process_, module_base, info->info.TypeIndex);
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

    callback_handle symbol_engine::set_walk_callback(i_stack_walk_callback& callback)
    {
        if(g_callback != nullptr)
        {
            throw exceptions::wide_runtime_error{ L"Only one i_stack_walk_callback can be set at a time" };
        }

        g_callback = &callback;
        return callback_handle{[]()
        {
            if (g_callback == nullptr)
            {
                throw exceptions::wide_runtime_error{ L"i_stack_walk_callback already cleared" };
            }
            g_callback = nullptr;
        }};
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
