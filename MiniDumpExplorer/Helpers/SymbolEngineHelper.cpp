#include "pch.h"
#include "SymbolEngineHelper.h"

#include "GlobalOptions.h"
#include "Utility/logger.h"

using namespace winrt;

SymbolEngineHelper::SymbolEngineHelper(Key)
    : dbg_help_queue_{Windows::System::DispatcherQueueController::CreateOnDedicatedThread()}
    , symbol_engine_{*this}
{
}

bool SymbolEngineHelper::deferred_symbol_load_cancel([[maybe_unused]] std::wstring_view const& module_name)
{
    return false;
}

void SymbolEngineHelper::deferred_symbol_load_partial([[maybe_unused]] std::wstring_view const& module_name)
{
}

void SymbolEngineHelper::start_download([[maybe_unused]] std::wstring_view const& module_name)
{
}

void SymbolEngineHelper::download_percent([[maybe_unused]] unsigned percent)
{
}

void SymbolEngineHelper::download_complete()
{
}

dlg_help_utils::dbg_help::log_handle SymbolEngineHelper::log_stream() const
{
    return {log_, [this] { log_complete(); }};
}

bool SymbolEngineHelper::symbol_load_debug() const
{
    return GlobalOptions::Options().SymbolLoadDebug();
}

bool SymbolEngineHelper::symbol_load_debug_memory() const
{
    return GlobalOptions::Options().SymbolLoadDebugMemory();
}

void SymbolEngineHelper::log_complete() const
{
    logger::Log().LogMessage(log_level::debug, std::move(log_).str());
    log_.clear();
}

SymbolEngineHelper& SymbolEngineHelper::Instance()
{
    static auto const instance{std::make_unique<SymbolEngineHelper>(Key{})};
    return *instance;
}
