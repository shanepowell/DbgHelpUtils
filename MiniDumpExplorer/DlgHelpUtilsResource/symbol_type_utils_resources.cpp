#include "pch.h"
#include "DbgHelpUtils/symbol_type_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"
#include "Utility/GetResourceValue.h"


using namespace std::string_literals;
using namespace std::string_view_literals;

namespace dlg_help_utils::symbol_type_utils::resources
{
    using namespace dbg_help;

    namespace
    {
        std::unordered_map<sym_tag_enum, std::wstring> const& sym_tag_descriptions()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<sym_tag_enum, std::wstring> const sym_tag_descriptions =
            {
                { sym_tag_enum::Null, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Null").ValueAsString()}},
                { sym_tag_enum::Exe, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Exe").ValueAsString()}},
                { sym_tag_enum::Compiland, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Compiland").ValueAsString()}},
                { sym_tag_enum::CompilandDetails, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CompilandDetails").ValueAsString()}},
                { sym_tag_enum::CompilandEnv, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CompilandEnv").ValueAsString()}},
                { sym_tag_enum::Function, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Function").ValueAsString()}},
                { sym_tag_enum::Block, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Block").ValueAsString()}},
                { sym_tag_enum::Data, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Data").ValueAsString()}},
                { sym_tag_enum::Annotation, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Annotation").ValueAsString()}},
                { sym_tag_enum::Label, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Label").ValueAsString()}},
                { sym_tag_enum::PublicSymbol, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PublicSymbol").ValueAsString()}},
                { sym_tag_enum::UDT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/UDT").ValueAsString()}},
                { sym_tag_enum::Enum, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Enum").ValueAsString()}},
                { sym_tag_enum::FunctionType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FunctionType").ValueAsString()}},
                { sym_tag_enum::PointerType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PointerType").ValueAsString()}},
                { sym_tag_enum::ArrayType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/ArrayType").ValueAsString()}},
                { sym_tag_enum::BaseType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BaseType").ValueAsString()}},
                { sym_tag_enum::Typedef, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Typedef").ValueAsString()}},
                { sym_tag_enum::BaseClass, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BaseClass").ValueAsString()}},
                { sym_tag_enum::Friend, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Friend").ValueAsString()}},
                { sym_tag_enum::FunctionArgType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FunctionArgType").ValueAsString()}},
                { sym_tag_enum::FuncDebugStart, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FuncDebugStart").ValueAsString()}},
                { sym_tag_enum::FuncDebugEnd, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FuncDebugEnd").ValueAsString()}},
                { sym_tag_enum::UsingNamespace, std::wstring{rm.MainResourceMap().GetValue(L"Resources/UsingNamespace").ValueAsString()}},
                { sym_tag_enum::VTableShape, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VTableShape").ValueAsString()}},
                { sym_tag_enum::VTable, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VTable").ValueAsString()}},
                { sym_tag_enum::Custom, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Custom").ValueAsString()}},
                { sym_tag_enum::Thunk, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Thunk").ValueAsString()}},
                { sym_tag_enum::CustomType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CustomType").ValueAsString()}},
                { sym_tag_enum::ManagedType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/ManagedType").ValueAsString()}},
                { sym_tag_enum::Dimension, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Dimension").ValueAsString()}},
                { sym_tag_enum::CallSite, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CallSite").ValueAsString()}},
                { sym_tag_enum::InlineSite, std::wstring{rm.MainResourceMap().GetValue(L"Resources/InlineSite").ValueAsString()}},
                { sym_tag_enum::BaseInterface, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BaseInterface").ValueAsString()}},
                { sym_tag_enum::VectorType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VectorType").ValueAsString()}},
                { sym_tag_enum::MatrixType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MatrixType").ValueAsString()}},
                { sym_tag_enum::HLSLType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/HLSLType").ValueAsString()}},
                { sym_tag_enum::Caller, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Caller").ValueAsString()}},
                { sym_tag_enum::Callee, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Callee").ValueAsString()}},
                { sym_tag_enum::Export, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Export").ValueAsString()}},
                { sym_tag_enum::HeapAllocationSite, std::wstring{rm.MainResourceMap().GetValue(L"Resources/HeapAllocationSite").ValueAsString()}},
                { sym_tag_enum::CoffGroup, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CoffGroup").ValueAsString()}},
                { sym_tag_enum::Inlinee, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Inlinee").ValueAsString()}},
            };

            return sym_tag_descriptions;
        }

        std::unordered_map<basic_type, std::wstring> const& basic_type_descriptions()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<basic_type, std::wstring> const basic_type_descriptions =
            {
                { basic_type::NoType, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NoType").ValueAsString()}},
                { basic_type::Void, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Void").ValueAsString()}},
                { basic_type::Char, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Char").ValueAsString()}},
                { basic_type::WChar, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WChar").ValueAsString()}},
                { basic_type::Int, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Int").ValueAsString()}},
                { basic_type::UInt, std::wstring{rm.MainResourceMap().GetValue(L"Resources/UInt").ValueAsString()}},
                { basic_type::Float, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Float").ValueAsString()}},
                { basic_type::BCD, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BCD").ValueAsString()}},
                { basic_type::Bool, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Bool").ValueAsString()}},
                { basic_type::Long, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Long").ValueAsString()}},
                { basic_type::ULong, std::wstring{rm.MainResourceMap().GetValue(L"Resources/ULong").ValueAsString()}},
                { basic_type::Currency, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Currency").ValueAsString()}},
                { basic_type::Date, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Date").ValueAsString()}},
                { basic_type::Variant, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Variant").ValueAsString()}},
                { basic_type::Complex, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Complex").ValueAsString()}},
                { basic_type::Bit, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Bit").ValueAsString()}},
                { basic_type::BSTR, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BSTR").ValueAsString()}},
                { basic_type::HResult, std::wstring{rm.MainResourceMap().GetValue(L"Resources/HResult").ValueAsString()}},
                { basic_type::Char16, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Char16").ValueAsString()}},
                { basic_type::Char32, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Char32").ValueAsString()}},
                { basic_type::Char8, std::wstring{rm.MainResourceMap().GetValue(L"Resources/Char8").ValueAsString()}},
            };

            return basic_type_descriptions;
        }

        std::unordered_map<udt_kind_type, std::wstring> const& udt_kind_type_descriptions()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<udt_kind_type, std::wstring> const udt_kind_type_descriptions =
            {
                { udt_kind_type::struct_type, std::wstring{rm.MainResourceMap().GetValue(L"Resources/struct_type").ValueAsString()}},
                { udt_kind_type::class_type, std::wstring{rm.MainResourceMap().GetValue(L"Resources/class_type").ValueAsString()}},
                { udt_kind_type::union_type, std::wstring{rm.MainResourceMap().GetValue(L"Resources/union_type").ValueAsString()}},
                { udt_kind_type::interface_type, std::wstring{rm.MainResourceMap().GetValue(L"Resources/interface_type").ValueAsString()}},
            };

            return udt_kind_type_descriptions;
        }

        std::unordered_map<calling_convention, std::wstring> const& calling_convention_descriptions()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<calling_convention, std::wstring> const calling_convention_descriptions =
            {
                { calling_convention::near_c, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_c").ValueAsString()}},
                { calling_convention::far_c, std::wstring{rm.MainResourceMap().GetValue(L"Resources/far_c").ValueAsString()}},
                { calling_convention::near_pascal, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_pascal").ValueAsString()}},
                { calling_convention::far_pascal, std::wstring{rm.MainResourceMap().GetValue(L"Resources/far_pascal").ValueAsString()}},
                { calling_convention::near_fast, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_fast").ValueAsString()}},
                { calling_convention::far_fast, std::wstring{rm.MainResourceMap().GetValue(L"Resources/far_fast").ValueAsString()}},
                { calling_convention::skipped, std::wstring{rm.MainResourceMap().GetValue(L"Resources/skipped").ValueAsString()}},
                { calling_convention::near_std, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_std").ValueAsString()}},
                { calling_convention::far_std, std::wstring{rm.MainResourceMap().GetValue(L"Resources/far_std").ValueAsString()}},
                { calling_convention::near_sys, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_sys").ValueAsString()}},
                { calling_convention::far_sys, std::wstring{rm.MainResourceMap().GetValue(L"Resources/far_sys").ValueAsString()}},
                { calling_convention::thiscall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/thiscall").ValueAsString()}},
                { calling_convention::mipscall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/mipscall").ValueAsString()}},
                { calling_convention::generic, std::wstring{rm.MainResourceMap().GetValue(L"Resources/generic").ValueAsString()}},
                { calling_convention::alphacall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/alphacall").ValueAsString()}},
                { calling_convention::ppccall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/ppccall").ValueAsString()}},
                { calling_convention::shcall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/shcall").ValueAsString()}},
                { calling_convention::armcall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/armcall").ValueAsString()}},
                { calling_convention::am33call, std::wstring{rm.MainResourceMap().GetValue(L"Resources/am33call").ValueAsString()}},
                { calling_convention::tricall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/tricall").ValueAsString()}},
                { calling_convention::sh5_call, std::wstring{rm.MainResourceMap().GetValue(L"Resources/sh5_call").ValueAsString()}},
                { calling_convention::m32rcall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/m32rcall").ValueAsString()}},
                { calling_convention::clrcall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/clrcall").ValueAsString()}},
                { calling_convention::inlinecall, std::wstring{rm.MainResourceMap().GetValue(L"Resources/inlinecall").ValueAsString()}},
                { calling_convention::near_vector, std::wstring{rm.MainResourceMap().GetValue(L"Resources/near_vector").ValueAsString()}},
                { calling_convention::swift, std::wstring{rm.MainResourceMap().GetValue(L"Resources/swift").ValueAsString()}},
                { calling_convention::reserved, std::wstring{rm.MainResourceMap().GetValue(L"Resources/reserved").ValueAsString()}},
            };

            return calling_convention_descriptions;
        }

        std::unordered_map<data_kind, std::wstring> const& data_kind_descriptions()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<data_kind, std::wstring> const data_kind_descriptions =
            {
                { data_kind::data_is_local, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_local").ValueAsString()}},//L"local"s },
                { data_kind::data_is_unknown, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_unknown").ValueAsString()}},//L"unknown"s },
                { data_kind::data_is_static_local, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_static_local").ValueAsString()}},//L"static local"s },
                { data_kind::data_is_param, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_param").ValueAsString()}},//L"param"s },
                { data_kind::data_is_object_ptr, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_object_ptr").ValueAsString()}},//L"object ptr"s },
                { data_kind::data_is_file_static, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_file_static").ValueAsString()}},//L"file static"s },
                { data_kind::data_is_global, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_global").ValueAsString()}},//L"global"s },
                { data_kind::data_is_member, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_member").ValueAsString()}},//L"member"s },
                { data_kind::data_is_static_member, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_static_member").ValueAsString()}},//L"static member"s },
                { data_kind::data_is_constant, std::wstring{rm.MainResourceMap().GetValue(L"Resources/data_is_constant").ValueAsString()}},//L"constant"s },
            };

            return data_kind_descriptions;
        }
    }

    std::wstring sym_tag_to_string(sym_tag_enum const type)
    {
        return flags_string_utils::generate_enum_string(type, sym_tag_descriptions());
    }

    std::wstring basic_type_to_string(basic_type const type)
    {
        return flags_string_utils::generate_enum_string(type, basic_type_descriptions());
    }

    std::wstring udt_kind_to_string(udt_kind_type const type)
    {
        return flags_string_utils::generate_enum_string(type, udt_kind_type_descriptions());
    }

    std::wstring calling_convention_to_string(calling_convention const type)
    {
        return flags_string_utils::generate_enum_string(type, calling_convention_descriptions());
    }

    std::wstring data_kind_convention_to_string(data_kind const type)
    {
        return flags_string_utils::generate_enum_string(type, data_kind_descriptions());
    }

    std::wstring_view get_symbol_name_none()
    {
        static auto instance{Utility::GetResourceValue(L"Resources/SymbolNameNone")};
        return instance;
    }

    std::wstring get_unsupported_type()
    {
        return Utility::GetResourceValue(L"Resources/SymbolTypeUnsupported");
    }

    std::wstring get_on_type()
    {
        return Utility::GetResourceValue(L"Resources/SymbolOnType");
    }

    std::wstring get_variable_unknown()
    {
        return Utility::GetResourceValue(L"Resources/SymbolVariableUnknown");
    }

    std::wstring get_bcd_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolBcdValueUnsupported");
    }

    std::wstring get_currency_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolCurrencyValueUnsupported");
    }

    std::wstring get_date_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolDateValueUnsupported");
    }

    std::wstring get_variant_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolVariantValueUnsupported");
    }

    std::wstring get_complex_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolComplexValueUnsupported");
    }

    std::wstring get_bstr_value_unsupported()
    {
        return Utility::GetResourceValue(L"Resources/SymbolBSTRValueUnsupported");
    }

    std::wstring get_bits_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolBitsTitle");
    }

    std::wstring get_bit_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolBitTitle");
    }

    std::wstring get_enum_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolEnumTitle");
    }

    std::wstring get_reference_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolReferenceTitle");
    }

    std::wstring get_pointer_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolPointerTitle");
    }

    std::wstring get_int8_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt8Title");
    }

    std::wstring get_int16_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt16Title");
    }

    std::wstring get_int32_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt32Title");
    }

    std::wstring get_int64_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt64Title");
    }

    std::wstring get_int128_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt128Title");
    }

    std::wstring get_int256_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolInt256Title");
    }

    std::wstring get_uint8_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt8Title");
    }

    std::wstring get_uint16_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt16Title");
    }

    std::wstring get_uint32_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt32Title");
    }

    std::wstring get_uint64_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt64Title");
    }

    std::wstring get_uint128_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt128Title");
    }

    std::wstring get_uint256_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolUInt256Title");
    }

    std::wstring get_float32_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFloat32Title");
    }

    std::wstring get_float64_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFloat64Title");
    }

    std::wstring get_float128_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFloat128Title");
    }

    std::wstring get_float256_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFloat256Title");
    }

    std::wstring get_failed_to_find_string()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFailedToFindTitle");
    }

    std::wstring get_symbol_title()
    {
        return Utility::GetResourceValue(L"Resources/SymbolTitle");
    }

    std::wstring get_is_zero_length_string()
    {
        return Utility::GetResourceValue(L"Resources/SymbolIsZeroLength");
    }

    std::wstring get_memory_address_in_range_string()
    {
        return Utility::GetResourceValue(L"Resources/SymbolMemoryAddressInRange");
    }

    std::wstring get_fields_string()
    {
        return Utility::GetResourceValue(L"Resources/SymbolFields");
    }
}
