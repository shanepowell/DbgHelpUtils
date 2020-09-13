#include "mini_dump_stack_walk.h"

#include <string>
#include <filesystem>

#include "function_table_stream.h"
#include "hex_dump.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "range_units.h"
#include "stream_hex_dump.h"
#include "symbol_engine.h"
#include "unloaded_module_list_stream.h"

namespace dlg_help_utils::stream_stack_dump
{
    mini_dump_stack_walk::mini_dump_stack_walk(DWORD64 const stack_start_range, void const* stack,
                                               size_t const stack_size, memory_list_stream const& memory_list,
                                               memory64_list_stream const& memory64_list,
                                               function_table_stream const& function_table,
                                               module_list_stream const& module_list,
                                               unloaded_module_list_stream const& unloaded_module_list,
                                               pe_file_memory_mapping& pe_file_memory_mappings,
                                               dbg_help::symbol_engine& symbol_engine)
        : stack_start_range_{stack_start_range}
          , stack_{static_cast<uint8_t const*>(stack)}
          , stack_size_{stack_size}
          , memory_list_{memory_list}
          , memory64_list_{memory64_list}
          , function_table_{function_table}
          , module_list_{module_list}
          , unloaded_module_list_{unloaded_module_list}
          , pe_file_memory_mappings_{pe_file_memory_mappings}
          , symbol_engine_{symbol_engine}
          , callback_{symbol_engine.callback()}
    {
    }

    // ReSharper disable CppParameterMayBeConst
    bool mini_dump_stack_walk::read_process_memory(DWORD64 const base_address, PVOID buffer, DWORD const size,
                                                   LPDWORD number_of_bytes_read, bool const enable_module_loading)
    {
        if (!range_utils::validate_range(base_address, size))
        {
            if (callback_.symbol_load_debug())
            {
                callback_.log_stream() << L"read_process_memory " << stream_hex_dump::to_hex(base_address) << L" - " <<
                    size << L" - invalid address range\n";
            }
            return false;
        }

        auto const result = do_read_process_memory(base_address, buffer, size, number_of_bytes_read,
                                                   enable_module_loading);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << L"read_process_memory " << stream_hex_dump::to_hex(base_address) << L" - " << size
                << L" : " << std::boolalpha << result;
            if (result)
            {
                callback_.log_stream() << L" : bytes read: " << *number_of_bytes_read;
                if (*number_of_bytes_read > size)
                {
                    callback_.log_stream() << L" - error invalid result";
                }
                if (*number_of_bytes_read < size)
                {
                    callback_.log_stream() << L" - short read";
                }
            }
            callback_.log_stream() << L'\n';

            if (result)
            {
                if (callback_.symbol_load_debug_memory())
                {
                    hex_dump::hex_dump(callback_.log_stream(), buffer, *number_of_bytes_read, 5, true, 16,
                                       base_address);
                }
            }
        }

        return result;
    }

    PVOID mini_dump_stack_walk::function_table_access(DWORD64 base_address)
    {
        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << L"function_table_access " << stream_hex_dump::to_hex(base_address) << '\n';
        }
        return get_mini_dump_function_table(base_address);
    }

    DWORD64 mini_dump_stack_walk::get_module_base_routine(DWORD64 address)
    {
        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << L"get_module_base_routine " << stream_hex_dump::to_hex(address) << '\n';
        }
        auto const base_address = get_loaded_module_base_routine(address);
        if (base_address != 0) return base_address;
        return get_unloaded_module_base_routine(address);
    }

    DWORD64 mini_dump_stack_walk::translate_address([[maybe_unused]] HANDLE h_thread, LPADDRESS64 lp_address)
    {
        if (lp_address == nullptr) return 0;
        return lp_address->Offset;
    }

    bool mini_dump_stack_walk::do_read_process_memory(DWORD64 const base_address, PVOID buffer, DWORD const size,
                                                      LPDWORD number_of_bytes_read,
                                                      bool const enable_module_loading) const
    {
        if (read_stack_memory(base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        if (memory_list_.found())
        {
            if (read_memory_list_memory(base_address, buffer, size, number_of_bytes_read))
            {
                return true;
            }
        }

        if (memory64_list_.found())
        {
            if (read_memory64_list_memory(base_address, buffer, size, number_of_bytes_read))
            {
                return true;
            }
        }

        if (read_memory_from_pe_file(base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        if (enable_module_loading && load_pe_file(base_address) && read_memory_from_pe_file(
            base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        return false;
    }

    bool mini_dump_stack_walk::read_stack_memory(DWORD64 const base_address, PVOID buffer, DWORD size,
                                                 LPDWORD number_of_bytes_read) const
    {
        size_t length = size;
        if (!range_utils::range_union(stack_start_range_, stack_size_, base_address, length)) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);

        memcpy(buffer, stack_ + (base_address - stack_start_range_), *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                       LPDWORD number_of_bytes_read) const
    {
        size_t length = size;
        auto const* memory = memory_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory64_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                         LPDWORD number_of_bytes_read) const
    {
        size_t length = size;
        auto const* memory = memory64_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory_from_pe_file(DWORD64 base_address, PVOID buffer, DWORD size,
                                                        LPDWORD number_of_bytes_read) const
    {
        size_t length = size;
        auto const* memory = pe_file_memory_mappings_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::load_pe_file(DWORD64 base_address) const
    {
        if (load_loaded_module(base_address))
        {
            return true;
        }

        return load_unloaded_module(base_address);
    }

    bool mini_dump_stack_walk::load_loaded_module(DWORD64 base_address) const
    {
        auto const module = module_list_.find_module(base_address);
        if (!module)
        {
            return false;
        }

        std::wstring const name{module->name()};
        if (!symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, (*module)->BaseOfImage, (*module)->SizeOfImage, (*module)->TimeDateStamp,
                                       (*module)->CheckSum, module->cv_record(), (*module)->CvRecord.DataSize,
                                       module->misc_record(), (*module)->MiscRecord.DataSize, (*module)->VersionInfo);
        }

        auto const& image_path = symbol_engine_.get_module_image_path(name);
        if (image_path.empty() || !std::filesystem::exists(image_path) || !std::filesystem::is_regular_file(image_path))
        {
            return false;
        }

        pe_file_memory_mappings_.load_pe_file(image_path, (*module)->BaseOfImage);
        return true;
    }

    bool mini_dump_stack_walk::load_unloaded_module(DWORD64 base_address) const
    {
        auto const module = unloaded_module_list_.find_module(base_address);
        if (!module)
        {
            return false;
        }

        std::wstring const name{module->name()};
        if (!symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, (*module)->BaseOfImage, (*module)->SizeOfImage, (*module)->TimeDateStamp,
                                       (*module)->CheckSum);
        }

        auto const& image_path = symbol_engine_.get_module_image_path(name);
        if (image_path.empty() && std::filesystem::exists(image_path) && std::filesystem::is_regular_file(image_path))
        {
            pe_file_memory_mappings_.load_pe_file(image_path, (*module)->BaseOfImage);
            return true;
        }

        return false;
    }

    PVOID mini_dump_stack_walk::get_mini_dump_function_table(DWORD64 base_address) const
    {
        if (!function_table_.found() || !function_table_.is_valid()) return nullptr;

        for (auto const& table : function_table_.list())
        {
            if (table->BaseAddress == base_address || base_address >= table->MinimumAddress && base_address <= table->
                MaximumAddress)
            {
                return const_cast<PVOID>(table.raw_function_table());
            }
        }
        return nullptr;
    }

    DWORD64 mini_dump_stack_walk::get_loaded_module_base_routine(DWORD64 address) const
    {
        if (!module_list_.found()) return 0;

        auto const module = module_list_.find_module(address);
        if (!module) return 0;

        std::wstring const module_name{module->name()};
        if (!symbol_engine_.is_module_loaded(module_name))
        {
            symbol_engine_.load_module(module_name, (*module)->BaseOfImage, (*module)->SizeOfImage,
                                       (*module)->TimeDateStamp, (*module)->CheckSum, module->cv_record(),
                                       (*module)->CvRecord.DataSize, module->misc_record(),
                                       (*module)->MiscRecord.DataSize, (*module)->VersionInfo);
        }

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << L"get_module_base_routine " << stream_hex_dump::to_hex(address) << " is in " <<
                module_name << " : " << stream_hex_dump::to_hex((*module)->BaseOfImage) << '\n';
        }
        return (*module)->BaseOfImage;
    }

    DWORD64 mini_dump_stack_walk::get_unloaded_module_base_routine(DWORD64 address) const
    {
        if (!unloaded_module_list_.found()) return 0;

        auto const module = unloaded_module_list_.find_module(address);
        if (!module) return 0;

        std::wstring const module_name{module->name()};
        if (!symbol_engine_.is_module_loaded(module_name))
        {
            symbol_engine_.load_module(module_name, (*module)->BaseOfImage, (*module)->SizeOfImage,
                                       (*module)->TimeDateStamp, (*module)->CheckSum);
        }

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << L"get_module_base_routine " << stream_hex_dump::to_hex(address) << " is in " <<
                module_name << " : " << stream_hex_dump::to_hex((*module)->BaseOfImage) << '\n';
        }
        return (*module)->BaseOfImage;
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_stack_walk::find_symbol_info(
        STACKFRAME_EX const& frame) const
    {
        auto const it_module = module_list_.find_module(frame.AddrPC.Offset);
        if (!it_module) return find_unloaded_module_symbol_info(frame.AddrPC.Offset, unloaded_module_list_,
                                                                symbol_engine_);

        auto const& module{it_module.value()};
        std::wstring const module_name{module.name()};

        if (!symbol_engine_.is_module_loaded(module_name))
        {
            symbol_engine_.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                       module->CheckSum, module.cv_record(), module->CvRecord.DataSize,
                                       module.misc_record(), module->MiscRecord.DataSize, module->VersionInfo);
        }

        return symbol_engine_.address_to_info(frame);
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_stack_walk::find_symbol_info(
        uint64_t address, module_list_stream const& module_list,
        unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine)
    {
        auto const it_module = module_list.find_module(address);
        if (!it_module) return find_unloaded_module_symbol_info(address, unloaded_module_list, symbol_engine);

        auto const& module{it_module.value()};
        std::wstring const module_name{module.name()};

        if (!symbol_engine.is_module_loaded(module_name))
        {
            symbol_engine.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                      module->CheckSum, module.cv_record(), module->CvRecord.DataSize,
                                      module.misc_record(), module->MiscRecord.DataSize, module->VersionInfo);
        }

        return symbol_engine.address_to_info(address);
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_stack_walk::find_unloaded_module_symbol_info(
        uint64_t address, unloaded_module_list_stream const& unloaded_module_list,
        dbg_help::symbol_engine& symbol_engine)
    {
        if (!unloaded_module_list.found()) return std::nullopt;

        auto const it_module = unloaded_module_list.find_module(address);
        if (!it_module) return std::nullopt;

        auto const& module{it_module.value()};
        std::wstring const module_name{it_module->name()};

        if (!symbol_engine.is_module_loaded(module_name))
        {
            symbol_engine.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                      module->CheckSum);
        }

        return symbol_engine.address_to_info(address);
    }

    // ReSharper restore CppParameterMayBeConst
}
