#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <comdef.h>

#include <memory>
#include <optional>
#include <string>
#include <experimental/generator>

#include "basic_type.h"
#include "call_convention.h"
#include "data_kind.h"
#include "sym_tag_enum.h"
#include "udt_kind_type.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info_cache;
    struct symbol_type_and_field_offset;

    class symbol_type_info
    {
    private:
        struct key_compare_only{};
        friend class symbol_type_info_cache;

    public:
        symbol_type_info() = default;
        symbol_type_info(symbol_type_info_cache& cache, HANDLE process, DWORD64 module_base, ULONG type_index, std::wstring_view export_name = {});
        symbol_type_info(key_compare_only, HANDLE process, DWORD64 module_base, ULONG type_index);

        [[nodiscard]] std::optional<sym_tag_enum> sym_tag() const;
        [[nodiscard]] std::optional<std::wstring_view> name() const;
        [[nodiscard]] std::optional<ULONG64> length() const;
        [[nodiscard]] std::optional<symbol_type_info> type() const;
        [[nodiscard]] std::optional<symbol_type_info> type_id() const;
        [[nodiscard]] std::optional<basic_type> base_type() const;
        [[nodiscard]] std::optional<DWORD> array_index_typeid() const;
        [[nodiscard]] std::optional<data_kind> data_kind() const;
        [[nodiscard]] std::optional<DWORD> address_offset() const;
        [[nodiscard]] std::optional<DWORD> offset() const;
        [[nodiscard]] std::optional<DWORD> array_count() const;
        [[nodiscard]] std::optional<DWORD> children_count() const;
        [[nodiscard]] std::optional<DWORD> bit_position() const;
        [[nodiscard]] std::optional<bool> is_virtual_base_class() const;
        [[nodiscard]] std::optional<DWORD> virtual_base_pointer_offset() const;
        [[nodiscard]] std::optional<symbol_type_info> class_parent_id() const;
        [[nodiscard]] std::optional<DWORD> nested() const;
        [[nodiscard]] DWORD64 module_base() const { return module_base_; }
        [[nodiscard]] DWORD sym_index() const { return type_index_; }
        [[nodiscard]] std::optional<DWORD> sym_index_raw() const;
        [[nodiscard]] std::optional<symbol_type_info> lexical_parent() const;
        [[nodiscard]] std::optional<ULONG64> address() const;
        [[nodiscard]] std::optional<udt_kind_type> udt_kind() const;
        [[nodiscard]] std::optional<DWORD> virtual_base_offset() const;
        [[nodiscard]] std::optional<DWORD> virtual_base_displacement_table_index() const;
        [[nodiscard]] std::optional<bool> is_reference() const;
        [[nodiscard]] std::optional<bool> indirect_virtual_base_class() const;
        [[nodiscard]] VARIANT const_value() const;
        [[nodiscard]] std::optional<calling_convention> calling_convention() const;

        [[nodiscard]] std::experimental::generator<symbol_type_info> children() const;

        [[nodiscard]] std::optional<symbol_type_and_field_offset> find_field_in_type(std::wstring_view field_name) const;
        [[nodiscard]] symbol_type_and_field_offset get_field_in_type(std::wstring_view type_name, std::wstring_view field_name) const;

        [[nodiscard]] std::wstring to_address_string() const;
        static [[nodiscard]] std::optional<symbol_type_info> from_address_string(symbol_type_info_cache& cache, HANDLE process, std::wstring_view address);

        [[nodiscard]] std::wstring const& export_name() const;

        [[nodiscard]] bool operator==(symbol_type_info const& other) const
        {
            return process_ == other.process_ &&
                module_base_ == other.module_base_ &&
                type_index_ == other.type_index_;
        }

        struct HashFunction
        {
            size_t operator()(symbol_type_info const& type) const
            {
                return std::hash<HANDLE>()(type.process_) ^
                        std::hash<DWORD64>()(type.module_base_) ^
                        std::hash<ULONG>()(type.type_index_);
            }
        };

    private:
        enum class optional_type
        {
            optional,
            required
        };
        [[nodiscard]] std::optional<DWORD> get_dword_type(IMAGEHLP_SYMBOL_TYPE_INFO type, std::wstring_view const& function, optional_type optional) const;
        [[nodiscard]] std::optional<ULONG64> get_unlong64_type(IMAGEHLP_SYMBOL_TYPE_INFO type, std::wstring_view const& function, optional_type optional) const;
        [[nodiscard]] std::optional<bool> get_bool_type(IMAGEHLP_SYMBOL_TYPE_INFO type, std::wstring_view const& function, optional_type optional) const;
        static void throw_sym_get_type_info_error(std::wstring_view const& function, optional_type optional);

    private:
        class cache_type_info;
        friend struct HashFunction;

        HANDLE process_{};
        DWORD64 module_base_{};
        ULONG type_index_{};
        std::shared_ptr<cache_type_info> cache_info_{};
    };

    struct symbol_type_and_field_offset
    {
        symbol_type_info type;
        uint64_t field_offset{};
    };
}
