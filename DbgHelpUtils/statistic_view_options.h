#pragma once
#include <vector>

#include "size_units.h"

namespace dlg_help_utils::heap::statistic_views
{

    class statistic_view_options
    {
    public:
        statistic_view_options();

        [[nodiscard]] size_units::base_16::bytes& range_size() { return range_size_; }
        [[nodiscard]] size_units::base_16::bytes const& range_size() const { return range_size_; }

    private:
        size_units::base_16::bytes range_size_;
    };
}
