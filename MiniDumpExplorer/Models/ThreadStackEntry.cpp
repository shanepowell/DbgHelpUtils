#include "pch.h"
#include "ThreadStackEntry.h"

#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/mini_dump_memory_walker_create_handle.h"
#include "DbgHelpUtils/mini_dump_memory_walker_release_handle.h"
#include "DbgHelpUtils/register_names.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/symbol_type_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/SymbolEngineHelper.h"
#include "Helpers/WindowHelper.h"
#include "Utility/mini_dump_walker_store.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <format>

#include "Utility/logger.h"

#if __has_include("ThreadStackEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadStackEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils::stream_stack_dump;
using namespace dlg_help_utils;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadStackEntry::ThreadStackEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
            },
            {
            }, 
            {
            },
            {
            })
    {
    }

    // ReSharper disable CppParameterMayBeConst
    void ThreadStackEntry::Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, stack_function_call_entry entry)  // NOLINT(performance-unnecessary-value-param)
    // ReSharper restore CppParameterMayBeConst
    {
        entry_ = std::move(entry);
        line_ = entry_.line;
        index_ = entry_.index;
        isInLine_ = entry_.symbol_info.in_line;
        isStackFrame_ = true;

        children_.Clear();

        if(!entry_.symbol_info.parameters.empty())
        {
            MiniDumpExplorer::ThreadStackEntry childEntry;
            childEntry.as<ThreadStackEntry>()->Set(walker_store, entry_, entry_.symbol_info.parameters, variable_type::parameters);
            children_.Append(childEntry);
        }

        if(!entry_.symbol_info.local_variables.empty())
        {
            MiniDumpExplorer::ThreadStackEntry childEntry;
            childEntry.as<ThreadStackEntry>()->Set(walker_store, entry_, entry_.symbol_info.local_variables, variable_type::local_variables);
            children_.Append(childEntry);
        }

        areChildrenLoaded_ = true;
    }

    void ThreadStackEntry::Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, stack_function_call_entry entry, std::vector<dbg_help::variable> const& variables, variable_type const variable_type)  // NOLINT(performance-unnecessary-value-param)
    {
        entry_ = std::move(entry);
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        line_ = rm.MainResourceMap().GetValue(GetVariableTypeTitle(variable_type)).ValueAsString();

        size_t index = 0;
        for (auto const& variable : variables)
        {
            MiniDumpExplorer::ThreadStackEntry childEntry;
            childEntry.as<ThreadStackEntry>()->Set(walker_store, index, entry_, variable, true);
            children_.Append(childEntry);
            ++index;
        }
    }

    // ReSharper disable CppParameterMayBeConst
    void ThreadStackEntry::Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, size_t const index, stack_function_call_entry entry, dbg_help::variable variable, bool const parameter)  // NOLINT(performance-unnecessary-value-param)
    // ReSharper restore CppParameterMayBeConst
    {
        size_t constexpr hex_length = sizeof(uint64_t) * 2;

        entry_ = std::move(entry);
        variable_ = std::move(variable);
        index_ = index;
        isParameter_ = parameter;
        isLocalVariable_ = !parameter;

        children_.Clear();
        areChildrenLoaded_ = true;

        auto const name = symbol_type_utils::get_symbol_type_friendly_name(variable_.symbol_info);
        if(variable_.registry_value.has_value() || variable_.frame_data.has_value())
        {
            std::wstringstream os;

            os << (variable_.registry_value 
                    ? (variable_.frame_data 
                        ? std::format(L" [{0}{1:+}]({2})", register_names::get_register_name(variable_.registry_value->register_type), variable_.frame_data->data_offset, stream_hex_dump::to_hex(variable_.frame_data->data_address, static_cast<std::streamsize>(hex_length)))
                        : std::format(L" [{}]", register_names::get_register_name(variable_.registry_value->register_type))
                      )
                    : std::format(L" ({})", stream_hex_dump::to_hex(variable_.frame_data->data_address, hex_length))
                  );

            if(variable_.frame_data)
            {
                // ReSharper disable once CppTooWideScopeInitStatement
                auto stream = walker_store->walker_.get_process_memory_stream(variable_.frame_data->data_address, variable_.frame_data->data_size);
                if(stream.eof())
                {
                    os << std::format(L"{}{}[{}]{}\n", resources::get_failed_to_find_address_prefix(), name, resources::get_failed_to_find_address_name_address_separator(), stream_hex_dump::to_hex_full(variable_.frame_data->data_address), resources::get_failed_to_find_address_postfix());
                }
                else
                {
                    areChildrenLoaded_ = false;

                    MiniDumpExplorer::ThreadStackEntry childEntry;
                    childEntry.as<ThreadStackEntry>()->SetDummy();
                    children_.Append(childEntry);

                    loadChildren_ = [this, walker_store, variable = variable_, stream = std::move(stream)]() mutable
                    {
                        auto const address = variable.frame_data->data_address;
                        LoadVariableSymbolAtChildren(std::move(walker_store), std::move(variable), address, std::move(stream));
                    };
                }
            }
            else if(variable_.registry_value)
            {
                areChildrenLoaded_ = false;

                MiniDumpExplorer::ThreadStackEntry childEntry;
                childEntry.as<ThreadStackEntry>()->SetDummy();
                children_.Append(childEntry);

                loadChildren_ = [this, walker_store, variable = variable_]
                {
                    LoadVariableSymbolAtChildren(std::move(walker_store), std::move(variable), 0, {&variable.registry_value->value, variable.registry_value->value_size});
                };
            }

            line_ = std::move(os).str();
        }
        else
        {
            line_ = std::format(L"{0} : {1}", name, resources::get_unknown_variable_type());
        }
    }

    // ReSharper disable CppParameterMayBeConst
    void ThreadStackEntry::Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, symbol_type_utils::dump_variable_symbol_data data)  // NOLINT(performance-unnecessary-value-param)
    // ReSharper restore CppParameterMayBeConst
    {
        line_ = data.line;

        if(data.sub_lines)
        {
            MiniDumpExplorer::ThreadStackEntry childEntry;
            childEntry.as<ThreadStackEntry>()->SetDummy();
            children_.Append(childEntry);

            loadChildren_ = [this, walker_store, data = std::move(data)]()
                {
                    LoadSubLinesChildren(std::move(walker_store), std::move(data));
                };
        }
        else
        {
            areChildrenLoaded_ = true;
        }
    }

    void ThreadStackEntry::LoadChildren()
    {
        if (auto expected = false;
            !areChildrenLoaded_.compare_exchange_strong(expected, true))
        {
            return;
        }

        if (loadChildren_)
        {
            loadChildren_();
            loadChildren_ = {};
        }
        RaisePropertyChanged(L"AreChildrenLoaded");
    }

    void ThreadStackEntry::SetDummy()
    {
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        loadingChildren_ = true;
        line_ = rm.MainResourceMap().GetValue(L"Resources/ThreadStackEntryLoading").ValueAsString();
    }

    // ReSharper disable CppParameterMayBeConst
    fire_and_forget ThreadStackEntry::LoadVariableSymbolAtChildren(std::shared_ptr<Utility::mini_dump_walker_store> walker_store, dbg_help::variable variable, uint64_t const variable_address, mini_dump_memory_stream variable_stream)
    // ReSharper restore CppParameterMayBeConst
    {
        auto anchor_self = get_strong();
        auto weak_self = get_weak();

        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        auto& symbolEngineHelper = SymbolEngineHelper::Instance();
        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());

        std::wstring const currentLine{line_};

        {
            logger::Log().LogMessage(log_level::debug, std::format(L"start LoadVariableSymbolAtChildren for line [{}]", currentLine));
            mini_dump_memory_walker_create_handle create_handle{walker_store->walker_};
            for (auto line : symbol_type_utils::variable_symbol_at(walker_store->walker_, variable.symbol_info, variable.symbol_info, variable_address, variable_stream))
            {
                // ReSharper disable once CppAssignedValueIsNeverUsed
                anchor_self = {};
                if(WindowHelper::IsExiting())
                {
                    co_return;
                }

                MiniDumpExplorer::ThreadStackEntry childEntry;
                childEntry.as<ThreadStackEntry>()->Set(walker_store, std::move(line));

                if(WindowHelper::IsExiting())
                {
                    co_return;
                }

                mini_dump_memory_walker_release_handle handle{walker_store->walker_};
                {
                    logger::Log().LogMessage(log_level::debug, std::format(L"LoadVariableSymbolAtChildren start UI update for line [{}]", currentLine));
                    co_await ui_thread;

                    anchor_self = weak_self.get();
                    if(anchor_self && !WindowHelper::IsExiting())
                    {
                        children_.Append(childEntry);
                    }
                    else
                    {
                        // it's been removed while loading the items
                        // need to switch QueueController thread to avoid symbol_engine callback fault
                        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                        co_return;
                    }

                    co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                    logger::Log().LogMessage(log_level::debug, std::format(L"LoadVariableSymbolAtChildren end UI update for line [{}]", currentLine));
                }
            }
        }
        logger::Log().LogMessage(log_level::debug, std::format(L"end LoadVariableSymbolAtChildren for line [{}]", currentLine));

        co_await ui_thread;

        if(WindowHelper::IsExiting())
        {
            co_return;
        }

        children_.RemoveAt(0);
    }

    // ReSharper disable CppParameterMayBeConst
    fire_and_forget ThreadStackEntry::LoadSubLinesChildren(std::shared_ptr<Utility::mini_dump_walker_store> walker_store, symbol_type_utils::dump_variable_symbol_data data)  // NOLINT(performance-unnecessary-value-param)
    // ReSharper restore CppParameterMayBeConst
    {
        auto anchor_self = get_strong();
        auto weak_self = get_weak();

        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        auto& symbolEngineHelper = SymbolEngineHelper::Instance();
        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());

        std::wstring const currentLine{line_};

        {
            logger::Log().LogMessage(log_level::debug, std::format(L"start LoadSubLinesChildren for line [{}]", currentLine));
            mini_dump_memory_walker_create_handle create_handle{ walker_store->walker_ };
            for (auto line : data.sub_lines())
            {
                // ReSharper disable once CppAssignedValueIsNeverUsed
                anchor_self = {};
                if (WindowHelper::IsExiting())
                {
                    co_return;
                }

                MiniDumpExplorer::ThreadStackEntry childEntry;
                childEntry.as<ThreadStackEntry>()->Set(walker_store, std::move(line));

                if (WindowHelper::IsExiting())
                {
                    co_return;
                }

                mini_dump_memory_walker_release_handle handle{ walker_store->walker_ };
                {
                    logger::Log().LogMessage(log_level::debug, std::format(L"LoadSubLinesChildren start UI update for line [{}]", currentLine));
                    co_await ui_thread;

                    anchor_self = weak_self.get();
                    if (anchor_self && !WindowHelper::IsExiting())
                    {
                        children_.Append(childEntry);
                    }
                    else
                    {
                        // it's been removed while loading the items
                        // need to switch QueueController thread to avoid symbol_engine callback fault
                        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                        co_return;
                    }

                    co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                    logger::Log().LogMessage(log_level::debug, std::format(L"LoadSubLinesChildren end UI update for line [{}]", currentLine));
                }
            }
        }
        logger::Log().LogMessage(log_level::debug, std::format(L"end LoadSubLinesChildren for line [{}]", currentLine));

        co_await ui_thread;

        if(WindowHelper::IsExiting())
        {
            co_return;
        }

        children_.RemoveAt(0);
    }

    param::hstring ThreadStackEntry::GetVariableTypeTitle(variable_type const type)
    {
        switch(type)
        {
        case variable_type::parameters:
            return L"Resources/ThreadStackEntryParameters";
        case variable_type::local_variables:
            return L"Resources/ThreadStackEntryLocalVariables";
        }

        throw exceptions::wide_runtime_error{std::format(L"variable_type type ({}) not supported", static_cast<int>(type))};
    }
}
