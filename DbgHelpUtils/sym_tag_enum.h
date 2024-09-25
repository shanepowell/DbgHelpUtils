#pragma once
namespace dlg_help_utils::dbg_help
{
    // ReSharper disable CppInconsistentNaming
    // ReSharper disable IdentifierTypo

    enum class sym_tag_enum : uint32_t  // NOLINT(performance-enum-size)
    {
        Null,
        Exe,
        Compiland,
        CompilandDetails,
        CompilandEnv,
        Function,
        Block,
        Data,
        Annotation,
        Label,
        PublicSymbol,
        UDT,
        Enum,
        FunctionType,
        PointerType,
        ArrayType,
        BaseType,
        Typedef,
        BaseClass,
        Friend,
        FunctionArgType,
        FuncDebugStart,
        FuncDebugEnd,
        UsingNamespace,
        VTableShape,
        VTable,
        Custom,
        Thunk,
        CustomType,
        ManagedType,
        Dimension,
        CallSite,
        InlineSite,
        BaseInterface,
        VectorType,
        MatrixType,
        HLSLType,
        Caller,
        Callee,
        Export,
        HeapAllocationSite,
        CoffGroup,
        Inlinee,
        Max
    };

    // ReSharper restore IdentifierTypo
    // ReSharper restore CppInconsistentNaming
}
