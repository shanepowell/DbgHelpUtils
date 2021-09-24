#include "mini_dump_stack_walk.h"

#include <filesystem>
#include <format>
#include <string>

#include "function_table_stream.h"
#include "hex_dump.h"
#include "locale_number_formatting.h"
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
        : stack_start_range_{ stack_start_range }
        , stack_{ static_cast<uint8_t const*>(stack) }
        , stack_size_{ stack_size }
        , memory_list_{ memory_list }
        , memory64_list_{ memory64_list }
        , function_table_{ function_table }
        , module_list_{ module_list }
        , unloaded_module_list_{ unloaded_module_list }
        , pe_file_memory_mappings_{ pe_file_memory_mappings }
        , symbol_engine_{ symbol_engine }
        , callback_{ symbol_engine.callback() }
        , handle_{ dbg_help::symbol_engine::set_callback(*this) }
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
                callback_.log_stream() << std::format(L"read_process_memory {0} - {1} : invalid address range\n", stream_hex_dump::to_hex(base_address), locale_formatting::to_wstring(size));
            }
            return false;
        }

        auto const result = do_read_process_memory(base_address, buffer, size, number_of_bytes_read,
                                                   enable_module_loading);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << std::format(L"read_process_memory {0} - {1} : {2}", stream_hex_dump::to_hex(base_address), locale_formatting::to_wstring(size), result);
            if (result)
            {
                callback_.log_stream() << std::format(L" : bytes read: {}", locale_formatting::to_wstring(*number_of_bytes_read));
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
            callback_.log_stream() << std::format(L"function_table_access {}\n", stream_hex_dump::to_hex(base_address));
        }
        return get_mini_dump_function_table(base_address);
    }

    DWORD64 mini_dump_stack_walk::get_module_base_routine(DWORD64 address)
    {
        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << std::format(L"get_module_base_routine {}\n", stream_hex_dump::to_hex(address));
        }
        if (auto const base_address = get_loaded_module_base_routine(address); base_address != 0) return base_address;
        return get_unloaded_module_base_routine(address);
    }

    DWORD64 mini_dump_stack_walk::translate_address([[maybe_unused]] HANDLE h_thread, LPADDRESS64 lp_address)
    {
        if (lp_address == nullptr) return 0;
        return lp_address->Offset;
    }

    mini_dump_memory_stream mini_dump_stack_walk::get_process_memory_stream(DWORD64 base_address, DWORD64 size, bool enable_module_loading) const
    {
        return mini_dump_memory_stream{[this](uint64_t base_address, uint64_t& size, bool enable_module_loading) { return get_process_memory_range(base_address, size, enable_module_loading); }, base_address, size, enable_module_loading};
    }

    void const* mini_dump_stack_walk::get_process_memory_range(DWORD64 base_address, DWORD64& size, bool enable_module_loading) const
    {
        auto max_size = size;
        auto const* memory = get_stack_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory64_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory_from_pe_file(base_address, max_size, false);
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            max_size = size;
            memory = get_memory_from_pe_file(base_address, max_size, false);
            if (memory != nullptr)
            {
                size = max_size;
                return memory;
            }
        }

        return nullptr;
    }

    DWORD64 mini_dump_stack_walk::find_memory_range(DWORD64 base_address, DWORD64 element_size, DWORD64 max_elements, bool enable_module_loading) const
    {
        DWORD64 max_size = element_size * max_elements;
        auto const* memory = get_stack_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory64_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory_from_pe_file(base_address, max_size, false);
        if (memory != nullptr)
        {
            return max_size;
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            memory = get_memory_from_pe_file(base_address, max_size, false);
            if (memory != nullptr)
            {
                return max_size;
            }
        }

        return 0;
    }

    DWORD64 mini_dump_stack_walk::find_memory_range_if(DWORD64 base_address, DWORD64 element_size, DWORD64 max_elements, std::function<bool(void const*)> const& pred, bool const enable_module_loading) const
    {
        DWORD64 max_size = element_size * max_elements;
        auto const* memory = get_stack_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory64_list_memory(base_address, max_size, false);
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory_from_pe_file(base_address, max_size, false);
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            memory = get_memory_from_pe_file(base_address, max_size, false);
            if (memory != nullptr)
            {
                return find_max_element_size(memory, element_size, max_size, pred);
            }
        }

        return 0;
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
        if(stack_size_ == 0) return false;
        uint64_t length = size;
        if (!range_utils::range_union(stack_start_range_, stack_size_, base_address, length)) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);

        memcpy(buffer, stack_ + (base_address - stack_start_range_), *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                       LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = memory_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory64_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                         LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = memory64_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_stack_walk::read_memory_from_pe_file(DWORD64 base_address, PVOID buffer, DWORD size,
                                                        LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = pe_file_memory_mappings_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    void const* mini_dump_stack_walk::get_stack_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const
    {
        if(stack_size_ == 0) return nullptr;
        if (DWORD64 length = size; range_utils::range_union(stack_start_range_, stack_size_, base_address, length) && (!limit_size || length == size))
        {
            if(!limit_size)
            {
                size = length;
            }
            return stack_ + (base_address - stack_start_range_);
        }

        return nullptr;
    }

    void const* mini_dump_stack_walk::get_memory_list_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const
    {
        if (memory_list_.found())
        {
            DWORD64 length = size;
            if (auto const* memory = memory_list_.find_any_address_range(base_address, length); memory != nullptr && (!limit_size || length == size))
            {
                if(!limit_size)
                {
                    size = length;
                }
                return memory;
            }
        }

        return nullptr;
    }

    void const* mini_dump_stack_walk::get_memory64_list_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const
    {
        if (memory64_list_.found())
        {
            DWORD64 length = size;
            if (auto const* memory = memory64_list_.find_any_address_range(base_address, length); memory != nullptr && (!limit_size || length == size))
            {
                if(!limit_size)
                {
                    size = length;
                }
                return memory;
            }
        }

        return nullptr;
    }

    void const* mini_dump_stack_walk::get_memory_from_pe_file(DWORD64 base_address, DWORD64& size, bool limit_size) const
    {
        DWORD64 length = size;
        auto const* memory = pe_file_memory_mappings_.find_any_address_range(base_address, length);
        if (memory == nullptr || (limit_size && length != size)) return nullptr;
        if(!limit_size)
        {
            size = length;
        }
        return memory;
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

        load_module(*module);

        std::wstring const name{module->name()};
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

        load_module(*module);

        std::wstring const name{module->name()};
        if (auto const & image_path = symbol_engine_.get_module_image_path(name); image_path.empty() && std::filesystem::exists(image_path) && std::filesystem::is_regular_file(image_path))
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

        load_module(*module);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << std::format(L"get_module_base_routine {0} is in {1} : {2}\n", stream_hex_dump::to_hex(address), module->name(), stream_hex_dump::to_hex((*module)->BaseOfImage));
        }
        return (*module)->BaseOfImage;
    }

    DWORD64 mini_dump_stack_walk::get_unloaded_module_base_routine(DWORD64 address) const
    {
        if (!unloaded_module_list_.found()) return 0;

        auto const module = unloaded_module_list_.find_module(address);
        if (!module) return 0;

        load_module(*module);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream() << std::format(L"get_module_base_routine {0} is in {1} : {2}\n", stream_hex_dump::to_hex(address), module->name(), stream_hex_dump::to_hex((*module)->BaseOfImage));
        }
        return (*module)->BaseOfImage;
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_stack_walk::find_symbol_info(
        STACKFRAME_EX const& frame) const
    {
        auto const it_module = module_list_.find_module(frame.AddrPC.Offset);
        if (!it_module) return find_unloaded_module_symbol_info(frame.AddrPC.Offset, unloaded_module_list_,
                                                                symbol_engine_);

        load_module(it_module.value());
        return symbol_engine_.address_to_info(frame);
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_stack_walk::find_symbol_info(
        uint64_t address, module_list_stream const& module_list,
        unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine)
    {
        auto const it_module = module_list.find_module(address);
        if (!it_module) return find_unloaded_module_symbol_info(address, unloaded_module_list, symbol_engine);

        auto const& module{it_module.value()};

        if (std::wstring const module_name{module.name()}; !symbol_engine.is_module_loaded(module_name))
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

        if (std::wstring const module_name{it_module->name()}; !symbol_engine.is_module_loaded(module_name))
        {
            symbol_engine.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                      module->CheckSum);
        }

        return symbol_engine.address_to_info(address);
    }

    DWORD64 mini_dump_stack_walk::find_max_element_size(void const* memory, DWORD64 element_size, DWORD64 max_size, std::function<bool(void const*)> const& pred)
    {
        auto const element_max = max_size / element_size;
        DWORD64 index = 0;
        for(; index < element_max; ++ index, memory = static_cast<char const*>(memory) + element_size)
        {
            if(pred(memory))
            {
                break;
            }
        }

        return index;
    }

    // ReSharper restore CppParameterMayBeConst

    std::optional<dbg_help::symbol_type_info> mini_dump_stack_walk::get_type_info(std::wstring const& type_name) const
    {
        auto [module_name, specific_type_name] = dbg_help::symbol_engine::parse_type_info(type_name);

        if(type_name.empty())
        {
            return std::nullopt;
        }

        if(module_name.empty())
        {
            // load all loaded modules...
            for (auto const& module : module_list_.list())
            {
                load_module(module);
            }
        }
        else
        {
            load_module(module_name);
        }

        return symbol_engine_.get_type_info(module_name, specific_type_name);
    }

    std::optional<dbg_help::symbol_type_info> mini_dump_stack_walk::get_symbol_info(std::wstring const& symbol_name) const
    {
        auto [module_name, specific_type_name] = dbg_help::symbol_engine::parse_type_info(symbol_name);

        if(specific_type_name.empty())
        {
            return std::nullopt;
        }

        if(module_name.empty())
        {
            // load all loaded modules...
            for (auto const& module : module_list_.list())
            {
                load_module(module);
            }
        }
        else
        {
            load_module(module_name);
        }

        return dbg_help::symbol_engine::get_symbol_info(symbol_name);
    }

    std::experimental::generator<dbg_help::symbol_type_info> mini_dump_stack_walk::module_types(std::wstring const& module_name) const
    {
        load_module(module_name);
        return symbol_engine_.module_types(module_name);
    }

    void mini_dump_stack_walk::load_module(std::wstring const& module_name) const
    {
        if(auto const & module = module_list_.find_module(module_name); module.has_value())
        {
            load_module(module.value());
        }
        else
        {
            if(auto const & unloaded_module = unloaded_module_list_.find_module(module_name); unloaded_module.has_value())
            {
                load_module(unloaded_module.value());
            }
        }
    }

    void mini_dump_stack_walk::load_module(stream_module const& module) const
    {
        if (std::wstring const name{module.name()}; !symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                       module->CheckSum, module.cv_record(), module->CvRecord.DataSize,
                                       module.misc_record(), module->MiscRecord.DataSize, module->VersionInfo);
        }
    }

    void mini_dump_stack_walk::load_module(stream_unloaded_module const& module) const
    {
        if (std::wstring const name{module.name()}; !symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                       module->CheckSum);
        }
    }

}
