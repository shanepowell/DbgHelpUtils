#pragma once
#include "process_heaps_statistic_view.h"
#include "size_units.h"

namespace dlg_help_utils::heap::statistic_views
{

    class statistic_view_options
    {
    public:
        statistic_view_options() = default;

        [[nodiscard]] size_units::base_16::bytes& range_size() { return range_size_; }
        [[nodiscard]] size_units::base_16::bytes const& range_size() const { return range_size_; }

        [[nodiscard]] process_heaps_statistic_view::sort_column_type& view_sort_column() { return view_sort_column_; }
        [[nodiscard]] process_heaps_statistic_view::sort_column_type const& view_sort_column() const { return view_sort_column_; }

        [[nodiscard]] process_heaps_statistic_view::sort_order_type& view_sort_order() { return view_sort_order_; }
        [[nodiscard]] process_heaps_statistic_view::sort_order_type const& view_sort_order() const { return view_sort_order_; }

    private:
        size_units::base_16::bytes range_size_{size_units::base_16::kilobytes{1}};
        process_heaps_statistic_view::sort_column_type view_sort_column_{process_heaps_statistic_view::sort_column_type::size};
        process_heaps_statistic_view::sort_order_type view_sort_order_{process_heaps_statistic_view::sort_order_type::ascending};
    };
}
