#pragma once
#include <functional>

#include "Controls/DataGrid/DataGridDataConnection.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Utility/SortObservableVector.h"
#include "Utility/logger.h"

namespace Utility
{
    enum class CompareResult
    {
        Less,
        Equal,
        Greater
    };

    extern wchar_t const* UnorderedTag;

    template<typename T>
    CompareResult SortCompare(T const& a, T const& b)
    {
        if constexpr (std::is_same_v<T, winrt::hstring> || std::is_same_v<T, std::wstring> || std::is_same_v<T, std::wstring_view>)
        {
            auto const result = dlg_help_utils::string_utils::icompare(a, b);
            if(result < 0)
            {
                return CompareResult::Less;
            }
            if (result > 0)
            {
                return CompareResult::Greater;
            }
            return CompareResult::Equal;
        }
        else
        {
            if(a < b)
            {
                return CompareResult::Less;
            }
            if (a > b)
            {
                return CompareResult::Greater;
            }
            return CompareResult::Equal;
        }
    }

    template<typename T, typename M>
    consteval auto MakeComparer(M T::* member)
    {
        return [member](T const& a, T const& b)
        {
            auto boundMember = std::mem_fn(member);
            return SortCompare(boundMember(a), boundMember(b));
        };
    }

    template<bool LessThanResult, bool GreaterThanResult, typename T, typename TCompare, typename TSortOrder>
    auto MakeStdSortComparer(TCompare columnComparer, TSortOrder equalSortOrder)
    {
        return [columnComparer = std::move(columnComparer), equalSortOrder = std::move(equalSortOrder)](T const& a, T const& b)
        {
            switch(columnComparer(a, b))
            {
            case CompareResult::Less:
                return LessThanResult;
            case CompareResult::Equal:
                return equalSortOrder.at(a) < equalSortOrder.at(b);
            case CompareResult::Greater:
                return GreaterThanResult;
            default: 
                throw dlg_help_utils::exceptions::wide_runtime_error{ std::format(L"Unsupported compare result") };
            }
        };
    }

    template<typename T>
    std::unordered_map<T, uint32_t> GetCurrentSortOrder(winrt::Windows::Foundation::Collections::IObservableVector<T> const& items)
    {
        std::unordered_map<T, uint32_t> sortOrder;
        uint32_t index{0};
        for(const auto& item : items)
        {
            sortOrder[item] = index++;
        }
        return sortOrder;
    }

    template<typename T>
    std::function<bool(T const& a, T const& b)> GetColumnComparer(winrt::Windows::Foundation::Collections::IObservableVector<T> const& items, std::unordered_map<std::wstring, std::function<CompareResult(T const&, T const&)>> const& columnSorters, winrt::Windows::Foundation::IReference<winrt::MiniDumpExplorer::DataGridSortDirection> const& sortDirection, winrt::hstring const& columnTag)
    {
        if(sortDirection)
        {
            switch(sortDirection.Value())
            {
            case winrt::MiniDumpExplorer::DataGridSortDirection::Ascending:
                return MakeStdSortComparer<false, true, T>(columnSorters.at(std::wstring{columnTag}), GetCurrentSortOrder(items));

            case winrt::MiniDumpExplorer::DataGridSortDirection::Descending:
                return MakeStdSortComparer<true, false, T>(columnSorters.at(std::wstring{columnTag}), GetCurrentSortOrder(items));

            default:
                throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Unsupported sort direction : {}", static_cast<int>(sortDirection.Value()))};
            }
        }

        return MakeStdSortComparer<true, false, T>(columnSorters.at(UnorderedTag), GetCurrentSortOrder(items));
    }


    template<typename T>
    void ColumnSort(winrt::Windows::Foundation::Collections::IObservableVector<T> const& items, std::unordered_map<std::wstring, std::function<CompareResult(T const&, T const&)>> const& columnSorters, winrt::MiniDumpExplorer::DataGrid const& dataGrid, winrt::MiniDumpExplorer::DataGridColumnEventArgs const& args)
    {
        try
        {
            auto sortDirection = args.Column().SortDirection();

            if(sortDirection)
            {
                switch(sortDirection.Value())
                {
                case winrt::MiniDumpExplorer::DataGridSortDirection::Ascending:
                    sortDirection = {};
                    break;
                case winrt::MiniDumpExplorer::DataGridSortDirection::Descending:
                    sortDirection = winrt::MiniDumpExplorer::DataGridSortDirection::Ascending;
                    break;
                }
            }
            else
            {
                sortDirection = winrt::MiniDumpExplorer::DataGridSortDirection::Descending;
            }

            SortObservableVector(items, GetColumnComparer(items, columnSorters, sortDirection, args.Column().Tag().as<winrt::hstring>()));

            args.Column().SortDirection(sortDirection);

            for(auto const& column : dataGrid.Columns())
            {
                if(column != args.Column())
                {
                    column.SortDirection({});
                }
            }
        }
        catch(...)
        {
            logger::Log().HandleUnknownException();
        }
    }

}
