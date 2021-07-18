#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <comdef.h>

#include <optional>
#include <string_view>
#include <experimental/generator>

#include "call_convention.h"
#include "data_kind.h"
#include "basic_type.h"
#include "sym_tag_enum.h"
#include "udt_kind_type.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info
    {
    public:
        symbol_type_info(DWORD64 module_base, ULONG type_index);

        [[nodiscard]] std::optional<enum sym_tag_enum> sym_tag() const;
        [[nodiscard]] std::optional<std::wstring> name() const;
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
        [[nodiscard]] std::optional<DWORD> class_parent_id() const;
        [[nodiscard]] std::optional<DWORD> nested() const;
        [[nodiscard]] DWORD sym_index() const { return type_index_; }
        [[nodiscard]] std::optional<DWORD> sym_index_raw() const;
        [[nodiscard]] std::optional<DWORD> lexical_parent() const;
        [[nodiscard]] std::optional<ULONG64> address() const;
        [[nodiscard]] std::optional<udt_kind_type> udt_kind() const;
        [[nodiscard]] std::optional<DWORD> virtual_base_offset() const;
        [[nodiscard]] std::optional<DWORD> virtual_base_displacement_table_index() const;
        [[nodiscard]] std::optional<bool> is_reference() const;
        [[nodiscard]] std::optional<bool> indirect_virtual_base_class() const;
        [[nodiscard]] std::optional<_variant_t> const_value() const;
        [[nodiscard]] std::optional<calling_convention> calling_convention() const;

        [[nodiscard]] std::experimental::generator<symbol_type_info> children() const;

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
        DWORD64 module_base_;
        ULONG type_index_;
    };
}
