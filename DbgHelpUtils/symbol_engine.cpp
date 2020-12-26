#include "symbol_engine.h"

#include <array>
#include <charconv>
#include <filesystem>
#include <sstream>

#include "cv_info_pdb70.h"
#include "guid_utils.h"
#include "pe_file.h"
#include "stream_hex_dump.h"
#include "stream_thread_context.h"
#include "wide_runtime_error.h"
#include "windows_error.h"
#include "exit_scope.h"
#include "hex_dump.h"

#pragma comment(lib, "dbghelp.lib")

auto const cba_start_op_maybe = 0xA0000000;

using namespace std::string_view_literals;
using namespace dlg_help_utils::stream_hex_dump;

// ReSharper disable CppInconsistentNaming
// ReSharper disable IdentifierTypo
enum SymTagEnum
{
    SymTagNull = 0x00,
    SymTagExe = 0x01,
    SymTagCompiland = 0x02,
    SymTagCompilandDetails = 0x03,
    SymTagCompilandEnv = 0x04,
    SymTagFunction = 0x05,
    SymTagBlock = 0x06,
    SymTagData = 0x07,
    SymTagAnnotation = 0x08,
    SymTagLabel = 0x09,
    SymTagPublicSymbol = 0x0a,
    SymTagUDT = 0x0b,
    SymTagEnum = 0x0c,
    SymTagFunctionType = 0x0d,
    SymTagPointerType = 0x0e,
    SymTagArrayType = 0x0f,
    SymTagBaseType = 0x10,
    SymTagTypedef = 0x11,
    SymTagBaseClass = 0x12,
    SymTagFriend = 0x13,
    SymTagFunctionArgType = 0x14,
    SymTagFuncDebugStart = 0x15,
    SymTagFuncDebugEnd = 0x16,
    SymTagUsingNamespace = 0x17,
    SymTagVTableShape = 0x18,
    SymTagVTable = 0x19,
    SymTagCustom = 0x1a,
    SymTagThunk = 0x1b,
    SymTagCustomType = 0x1c,
    SymTagManagedType = 0x1d,
    SymTagDimension = 0x1e,
    SymTagUnknown1 = 0x1f,
    SymTagInlineFunction = 0x20
};

// ReSharper restore IdentifierTypo
// ReSharper restore CppInconsistentNaming

namespace
{
    HANDLE fake_process{reinterpret_cast<HANDLE>(1)};
    HANDLE fake_thread{reinterpret_cast<HANDLE>(2)};
    size_t const trace_max_function_name_length{1000};
    size_t const max_buffer_size = sizeof(SYMBOL_INFOW) + (trace_max_function_name_length * sizeof(wchar_t));
    auto const progress_xml_start = L"<Progress percent=\""sv;
    auto const progress_xml_end = L"\"/>\n"sv;
    auto const downloading_xml_start = L"<Activity name=\"Downloading file "sv;
    auto const downloading_xml_end = L"\" details=\""sv;

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
        HANDLE h_file; // file handle, if passed
        DWORD flags;
    };

    struct symbol_load
    {
        [[maybe_unused]] DWORD size_of_struct; // set to sizeof(symbol_load_w64)
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
    BOOL CALLBACK read_process_memory_routine([[maybe_unused]] HANDLE h_process, DWORD64 lp_base_address,
                                              PVOID lp_buffer, DWORD n_size, PDWORD lp_number_of_bytes_read)
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

        result = SymFunctionTableAccess64AccessRoutines(h_process, address_base, read_process_memory_routine,
                                                        get_module_base_routine);
        return result;
    }

    // ReSharper restore CppParameterMayBeConst

    // ReSharper disable CppParameterMayBeConst
    DWORD64 CALLBACK translate_address([[maybe_unused]] _In_ HANDLE h_process, _In_ HANDLE h_thread,
                                       _In_ LPADDRESS64 lp_address)
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
                                 dlg_help_utils::dbg_help::i_symbol_load_callback& callback)
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
            callback.log_stream() << L"DlgHelp: action_code: " << action_code_to_string(action_code) << L" (" <<
                to_hex(action_code) << L") callback_data:(" << to_hex(callback_data) << L"]\n";
        }

        if (callback_data == 0) return FALSE;

        switch (action_code)
        {
        case CBA_EVENT:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const* evt = reinterpret_cast<IMAGEHLP_CBA_EVENTW const*>(callback_data);
                callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":[" <<
                    severity_to_string(evt->severity) << L"]:(" << to_hex(evt->code) << L")[" << evt->desc << L"] " <<
                    evt->object << L'\n';
            }
            return TRUE;

        case CBA_DEFERRED_SYMBOL_LOAD_START:
        case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
        case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data);
                if (size_of_struct == sizeof(IMAGEHLP_DEFERRED_SYMBOL_LOADW64))
                {
                    auto const* evt = reinterpret_cast<IMAGEHLP_DEFERRED_SYMBOL_LOADW64 const*>(callback_data);
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":BaseOfImage[" <<
                        to_hex(evt->BaseOfImage) << L"]:CheckSum[" << to_hex(evt->CheckSum) << L"]:TimeDateStamp:[" <<
                        to_hex(evt->TimeDateStamp) << L"]:FileName[" << evt->FileName << L"]:Reparse[" << std::boolalpha
                        << evt->Reparse << L"]:hFile[" << to_hex(evt->hFile) << L"]:Flags[" << to_hex(evt->Flags) <<
                        L"]\n";
                }
                else if (size_of_struct == sizeof(symbol_load_w64))
                {
                    auto const* evt = reinterpret_cast<symbol_load_w64 const*>(callback_data);
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L"[c]:BaseOfImage["
                        << to_hex(evt->base_of_image) << L"]:CheckSum[" << to_hex(evt->check_sum) <<
                        L"]:TimeDateStamp:[" << to_hex(evt->time_date_stamp) << L"]:Unknown:[" << to_hex(evt->unknown)
                        << L"]:FileName[" << evt->file_name << L"]:Reparse[" << evt->reparse << L"]:hFile[" <<
                        to_hex(evt->h_file) << L"]:Flags[" << to_hex(evt->flags) << L"]\n";
                }
                else
                {
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) <<
                        L": unknown event data size: " << size_of_struct << L" != " << sizeof(
                            IMAGEHLP_DEFERRED_SYMBOL_LOADW64) << L'|' << sizeof(symbol_load_w64) << L'\n';
                }
            }
            dump_sizeof_struct_data(callback_data, callback);
            break;

        case CBA_DEFERRED_SYMBOL_LOAD_PARTIAL:
            {
                auto const filename = get_symbol_load_filename(callback_data, symbol_engine.downloading_module());
                if (!filename.empty())
                {
                    callback.deferred_symbol_load_partial(filename);
                }

                dump_sizeof_struct_data(callback_data, callback);
            }
            break;

        case CBA_DEFERRED_SYMBOL_LOAD_CANCEL:
            {
                auto const filename = get_symbol_load_filename(callback_data, symbol_engine.downloading_module());
                if (!filename.empty())
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
                callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":addr[" <<
                    to_hex(evt->addr) << L"]:bytes:[" << to_hex(evt->bytes) << L"]\n";
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
                auto const* evt = reinterpret_cast<IMAGEHLP_DUPLICATE_SYMBOL64 const*>(callback_data);
                if (evt->SizeOfStruct == sizeof(IMAGEHLP_DUPLICATE_SYMBOL64))
                {
                    // ReSharper disable once StringLiteralTypo
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":NumberOfDups[" <<
                        evt->NumberOfDups << L"]:SelectedSymbol[" << evt->SelectedSymbol << L"]\n";
                    callback.log_stream() << L"    Symbol: Address: [" << to_hex(evt->Symbol->Address) << L"]\n";
                    callback.log_stream() << L"            Size: [" << evt->Symbol->Size << L"]\n";
                    callback.log_stream() << L"            Flags: [" << to_hex(evt->Symbol->Flags) << L"]\n";
                    callback.log_stream() << L"            Name: [" << evt->Symbol->Name << L"]\n";
                }
                else
                {
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) <<
                        L": unknown event data size : " << evt->SizeOfStruct << L" != " << sizeof(
                            IMAGEHLP_DUPLICATE_SYMBOL64) << L'\n';
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
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":" << xml;
                    if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                    {
                        callback.log_stream() << L'\n';
                    }
                }

                auto const download_file = symbol_engine.downloading_module();
                if (!download_file.empty())
                {
                    if (xml.starts_with(progress_xml_start))
                    {
                        auto const end_pos = xml.find(progress_xml_end, progress_xml_start.size());
                        if (end_pos != std::wstring_view::npos)
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
                    auto const end_pos = xml.find(downloading_xml_end, downloading_xml_start.size());
                    if (end_pos != std::wstring_view::npos)
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
                callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":[" <<
                    severity_to_string(evt->severity) << L"]:(" << to_hex(evt->code) << L")[" << evt->desc << L"] " <<
                    evt->object << L'\n';
            }
            return TRUE;

        case CBA_SRCSRV_INFO:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                std::wstring_view const xml{reinterpret_cast<wchar_t const*>(callback_data)};
                callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L":" << xml;
                if (xml.find_first_of(L'\n') == std::wstring_view::npos)
                {
                    callback.log_stream() << L'\n';
                }
            }
            return TRUE;

        case cba_start_op_maybe:
            if (callback.symbol_load_debug() && callback_data != 0)
            {
                auto const size_of_struct = *reinterpret_cast<DWORD const*>(callback_data);
                if (size_of_struct == sizeof(symbol_load))
                {
                    auto const* evt = reinterpret_cast<symbol_load const*>(callback_data);
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) << L"[c]:BaseOfImage["
                        << to_hex(evt->base_of_image) << L"]:CheckSum[" << to_hex(evt->check_sum) <<
                        L"]:TimeDateStamp:[" << to_hex(evt->time_date_stamp) << L"]:Unknown:[" << to_hex(evt->unknown1)
                        << L"]\n";
                }
                else
                {
                    callback.log_stream() << L"DlgHelp: " << action_code_to_string(action_code) <<
                        L": unknown event data size: " << size_of_struct << L" != " << sizeof(symbol_load) << L'\n';
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

        auto& callback = symbol_engine.callback();
        if (callback.symbol_load_debug())
        {
            callback.log_stream() << L"DlgHelp: FindFile: " << filename << L" checksum check failure\n";
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

        if (!SymInitializeW(fake_process, nullptr, FALSE))
        {
            auto const ec = GetLastError();
            throw exceptions::wide_runtime_error{
                (std::wostringstream{} << L"SymInitialize failed. Error: " << ec << L" - " <<
                    windows_error::get_windows_error_string(ec)).str()
            };
        }

        SymSetOptions(SYMOPT_LOAD_LINES | (callback.symbol_load_debug() ? SYMOPT_DEBUG : 0));
        SymRegisterCallbackW64(fake_process, sym_register_callback_proc64,
                               reinterpret_cast<ULONG64>(static_cast<i_symbol_callback*>(this)));
    }

    symbol_engine::~symbol_engine()
    {
        SymCleanup(fake_process);
    }

    void symbol_engine::clear_modules()
    {
        for (auto const& [name, info] : modules_)
        {
            if (info.handle != 0 && !SymUnloadModule64(fake_process, info.handle))
            {
                auto const ec = GetLastError();
                throw exceptions::wide_runtime_error{
                    (std::wostringstream{} << L"SymUnloadModule64 failed. Error: " << ec << L" - " <<
                        windows_error::get_windows_error_string(ec)).str()
                };
            }
        }

        modules_.clear();
    }

    void symbol_engine::load_module(std::wstring module_name, DWORD64 const module_base, DWORD const module_size,
                                    DWORD module_time_stamp, DWORD const module_check_sum, void const* cv_record,
                                    DWORD const cv_record_size, void const* misc_record, DWORD const misc_record_size,
                                    [[maybe_unused]] VS_FIXEDFILEINFO const& version_info)
    {
        std::filesystem::path path{module_name};
        auto module = path.filename().wstring();

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
                callback().log_stream() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) <<
                    L'\n';
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
                callback().log_stream() << L" loading: PdbSig70:" << guid_utils::to_string(pdb.get_signature()) <<
                    L'\n';
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
            module_image_path =
                load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        modules_.insert(std::make_pair(std::move(module_name),
                                       module_info{handle, module_base, module_size, std::move(module_image_path)}));
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
            module_image_path =
                load_module_image_path(module_size, module_time_stamp, module_check_sum, module, handle);
            dump_loaded_module_information(handle, module_base, module_image_path);
        }

        modules_.insert(std::make_pair(std::move(module_name),
                                       module_info{handle, module_base, module_size, std::move(module_image_path)}));
    }

    void symbol_engine::unload_module(std::wstring const& module_name)
    {
        auto const it = modules_.find(module_name);
        if (it != modules_.end())
        {
            if (it->second.handle != 0 && !SymUnloadModule64(fake_process, it->second.handle))
            {
                auto const ec = GetLastError();
                throw exceptions::wide_runtime_error{
                    (std::wostringstream{} << L"SymUnloadModule64 failed. Error: " << ec << L" - " <<
                        windows_error::get_windows_error_string(ec)).str()
                };
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
            throw exceptions::wide_runtime_error{
                (std::wostringstream{} << L"Module [" << module_name << L"] is not loaded").str()
            };
        }
        return get_module_information(it->second.base);
    }

    IMAGEHLP_MODULEW64 symbol_engine::get_module_information(DWORD64 const module_base) const
    {
        IMAGEHLP_MODULEW64 module{};
        module.SizeOfStruct = sizeof(module);
        if (!SymGetModuleInfoW64(fake_process, module_base, &module))
        {
            auto const ec = GetLastError();
            throw exceptions::wide_runtime_error{
                (std::wostringstream{} << L"SymUnloadModule64 failed. Error: " << ec << L" - " <<
                    windows_error::get_windows_error_string(ec)).str()
            };
        }

        return module;
    }

    std::wstring const& symbol_engine::get_module_image_path(std::wstring const& module_name) const
    {
        auto const it = modules_.find(module_name);
        if (it == modules_.end())
        {
            throw exceptions::wide_runtime_error{
                (std::wostringstream{} << L"Module [" << module_name << L"] is not loaded").str()
            };
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

        if (!SymFromAddrW(fake_process, address, &info.symbol_displacement, symbol_.get()))
        {
            return std::move(info);
        }

        info.symbol_name = symbol_->Name;

        if (SymGetLineFromAddrW64(fake_process, address, &info.line_displacement, &line_))
        {
            info.line_number = line_.LineNumber;
            info.file_name = line_.FileName;
        }

        return std::move(info);
    }

    std::optional<symbol_address_info> symbol_engine::address_to_info(STACKFRAME_EX const& frame)
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

        if (frame.StackFrameSize >= sizeof(STACKFRAME_EX) && frame.InlineFrameContext != INLINE_FRAME_CONTEXT_IGNORE &&
            frame.InlineFrameContext != INLINE_FRAME_CONTEXT_INIT)
        {
            if (!SymFromInlineContextW(fake_process, address, frame.InlineFrameContext, &info.symbol_displacement,
                                       symbol_.get()))
            {
                return std::move(info);
            }
            info.frame_content_found = true;
            info.symbol_name = symbol_->Name;
            info.in_line = symbol_->Tag == SymTagInlineFunction;

            if (SymGetLineFromInlineContextW(fake_process, address, frame.InlineFrameContext, it->second.base,
                                             &info.line_displacement, &line_))
            {
                info.line_number = line_.LineNumber;
                info.file_name = line_.FileName;
            }
        }
        else
        {
            if (!SymFromAddrW(fake_process, address, &info.symbol_displacement, symbol_.get()))
            {
                return std::move(info);
            }
            info.symbol_name = symbol_->Name;

            if (SymGetLineFromAddrW64(fake_process, frame.AddrPC.Offset, &info.line_displacement, &line_))
            {
                info.line_number = line_.LineNumber;
                info.file_name = line_.FileName;
            }
        }

        return std::move(info);
    }

    std::experimental::generator<symbol_address_info> symbol_engine::stack_walk(
        stream_thread_context const& thread_context, i_stack_walk_callback& callback)
    {
        g_callback = &callback;

        DWORD machine_type;
        STACKFRAME_EX frame{};
        frame.StackFrameSize = sizeof(STACKFRAME_EX);

        if (thread_context.x86_thread_context_available())
        {
            machine_type = IMAGE_FILE_MACHINE_I386;
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
        [[maybe_unused]] auto const base_address = callback.get_module_base_routine(frame.AddrPC.Offset);

        // StackWalkEx modifies the thread context passed in so always take a copy for it to work with
        auto const thread_context_copy = std::make_unique<uint8_t[]>(thread_context.size());
        memcpy(thread_context_copy.get(), thread_context.context(), thread_context.size());

        auto flags = SYM_STKWALK_DEFAULT | SYM_STKWALK_FORCE_FRAMEPTR;

        while (true)
        {
            if (!StackWalkEx(machine_type, fake_process, fake_thread, &frame, thread_context_copy.get(),
                             &read_process_memory_routine, &function_table_access_routine, &get_module_base_routine,
                             &translate_address, flags))
            {
                break;
            }

            auto info = callback.find_symbol_info(frame);
            if (!info)
            {
                symbol_address_info rv{};
                rv.address = frame.AddrPC.Offset;
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

        g_callback = nullptr;
    }

    std::map<std::wstring, symbol_engine::module_info>::const_iterator symbol_engine::find_module_name(
        DWORD64 const address)
    {
        for (auto it = modules_.begin(); it != modules_.end(); ++it)
        {
            if (address >= it->second.base && address < it->second.base + it->second.size)
            {
                return std::move(it);
            }
        }

        return modules_.end();
    }

    void symbol_engine::dump_loaded_module_information(DWORD64 const handle, DWORD64 const module_base,
                                                       std::wstring const& module_image_path) const
    {
        if (handle != 0 && callback().symbol_load_debug())
        {
            auto info = get_module_information(module_base);
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
            auto const result = SymFindFileInPathW(fake_process, nullptr, module_name.c_str(), &module_time_stamp,
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

        return SymLoadModuleExW(fake_process, nullptr, module_name.c_str(), nullptr, module_base, module_size,
                                module_load_info, 0);
    }
}
