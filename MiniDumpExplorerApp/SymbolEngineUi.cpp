#include "Pch.h"
#include "SymbolEngineUi.h"

SymbolEngineUi::SymbolEngineUi()
{
}

void SymbolEngineUi::SetUi(winrt::MiniDumpExplorer::MainPage ui)
{
    ui_ = ui;
}

bool SymbolEngineUi::deferred_symbol_load_cancel([[maybe_unused]] std::wstring_view const& module_name)
{
    return ui_.DeferredSymbolLoadCancel(module_name);
}

void SymbolEngineUi::deferred_symbol_load_partial([[maybe_unused]] std::wstring_view const& module_name)
{
    ui_.DeferredSymbolLoadPartial(module_name);
}

void SymbolEngineUi::start_download([[maybe_unused]] std::wstring_view const& module_name)
{
    ui_.StartDownload(module_name);
}

void SymbolEngineUi::download_percent([[maybe_unused]] unsigned percent)
{
    ui_.DownloadPercent(percent);
}

void SymbolEngineUi::download_complete()
{
    ui_.DownloadComplete();
}

std::wostream& SymbolEngineUi::log_stream() const
{
    log_.clear();
    return log_;
}

bool SymbolEngineUi::symbol_load_debug() const
{
    return true;
}

bool SymbolEngineUi::symbol_load_debug_memory() const
{
    return true;
}
