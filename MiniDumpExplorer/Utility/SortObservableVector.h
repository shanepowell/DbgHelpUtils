#pragma once

#include <algorithm>
#include <vector>

#include "WinrtEmptyValue.h"

namespace Utility
{
    template<typename T, typename Compare>
    void SortObservableVector(winrt::Windows::Foundation::Collections::IObservableVector<T> const& observable, Compare&& comp)  // NOLINT(cppcoreguidelines-missing-std-forward)
    {
        // Slurp into a vector
        std::vector<T> v(observable.Size(), winrt_empty_value<T>());
        observable.GetMany(0, v);

        // Sort the vector
        std::sort(begin(v), end(v), std::forward<Compare>(comp));

        // Shove the results back in
        observable.ReplaceAll(v);
    }
}
