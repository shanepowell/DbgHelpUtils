#pragma once
#include "generator.h"
#include <numeric>
#include <vector>

#include "range.h"

namespace Utility
{
    template <typename T>
    class IndexToValueTable
    {
    public:
        static constexpr int32_t InvalidIndex = int32_t{-1};

        IndexToValueTable() = default;

        [[nodiscard]] int32_t IndexCount() const
        {
            return std::accumulate(values_.begin(), values_.end(), 0, [](int32_t sum, const Range<T>& range) { return sum + range.Count(); });
        }

        [[nodiscard]] bool IsEmpty() const
        {
            return values_.empty();
        }

        [[nodiscard]] auto RangeCount() const
        {
            return values_.size();
        }

        void AddValue(int32_t const index, T value)
        {
            AddValues(index, 1, std::move(value));
        }

        void AddValues(int32_t const startIndex, int32_t const count, T value)
        {
            AddValuesPrivate(startIndex, count, std::move(value), std::nullopt);
        }

        void Clear()
        {
            values_.clear();
        }

        [[nodiscard]] bool Contains(int32_t const index) const
        {
            return IsCorrectRangeIndex(FindRangeIndex(index), index);
        }

        [[nodiscard]] bool ContainsAll(int32_t const startIndex, int32_t const endIndex) const
        {
            auto start = InvalidIndex;
            auto end = InvalidIndex;

            for (auto const& range : values_)
            {
                if (start == InvalidIndex && range.UpperBound() >= startIndex)
                {
                    if (startIndex < range.LowerBound())
                    {
                        return false;
                    }

                    start = startIndex;
                    end = range.UpperBound();
                    if (end >= endIndex)
                    {
                        return true;
                    }
                }
                else if (start != InvalidIndex)
                {
                    if (range.LowerBound() > end + 1)
                    {
                        return false;
                    }

                    end = range.UpperBound();
                    if (end >= endIndex)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        [[nodiscard]] bool ContainsIndexAndValue(int32_t const index, T const& value) const
        {
            auto lowerRangeIndex = FindRangeIndex(index);
            return IsCorrectRangeIndex(lowerRangeIndex, index) && values_[lowerRangeIndex].ContainsValue(value);
        }


        [[nodiscard]] IndexToValueTable Copy() const
        {
            IndexToValueTable copy;
            for (auto const& range : values_)
            {
                copy.values_.push_back(range.Copy());
            }

            return copy;
        }

        [[nodiscard]] int32_t GetNextGap(int32_t const index) const
        {
            auto const targetIndex = index + 1;
            auto rangeIndex = FindRangeIndex(targetIndex);
            if (IsCorrectRangeIndex(rangeIndex, targetIndex))
            {
                while (rangeIndex < values_.size() - 1 && values_[rangeIndex].UpperBound() == values_[rangeIndex + 1].LowerBound() - 1)
                {
                    ++rangeIndex;
                }

                return values_[rangeIndex].UpperBound() + 1;
            }

            return targetIndex;
        }

        [[nodiscard]] int32_t GetNextIndex(int32_t const index) const
        {
            auto const targetIndex = index + 1;
            auto rangeIndex = FindRangeIndex(targetIndex);
            if (IsCorrectRangeIndex(rangeIndex, targetIndex))
            {
                return targetIndex;
            }

            ++rangeIndex;
            return rangeIndex < values_.size() ? values_[rangeIndex].LowerBound() : InvalidIndex;
        }

        [[nodiscard]] int32_t GetPreviousGap(int32_t const index) const
        {
            auto const targetIndex = index - 1;
            auto rangeIndex = FindRangeIndex(targetIndex);
            if (IsCorrectRangeIndex(rangeIndex, targetIndex))
            {
                while (rangeIndex > 0 && values_[rangeIndex].LowerBound() == values_[rangeIndex - 1].UpperBound() + 1)
                {
                    --rangeIndex;
                }

                return values_[rangeIndex].LowerBound() - 1;

            }
            return targetIndex;
        }

        [[nodiscard]] int32_t GetPreviousIndex(int32_t const index) const
        {
            auto const targetIndex = index - 1;
            auto rangeIndex = FindRangeIndex(targetIndex);
            if (IsCorrectRangeIndex(rangeIndex, targetIndex))
            {
                return targetIndex;
            }

            return rangeIndex != InvalidIndex && rangeIndex < values_.size() ? values_[rangeIndex].UpperBound() : InvalidIndex;
        }

        [[nodiscard]] int32_t GetIndexCount(int32_t const lowerBound, int32_t const upperBound, T const& value) const
        {
            if (values_.empty())
            {
                return 0;
            }

            int32_t count = 0;
            auto index = FindRangeIndex(lowerBound);
            if (IsCorrectRangeIndex(index, lowerBound) && values_[index].ContainsValue(value))
            {
                count += values_[index].UpperBound() - lowerBound + 1;
            }

            ++index;
            while (static_cast<typename decltype(values_)::size_type>(index) < values_.size() && values_[index].UpperBound() <= upperBound)
            {
                if (values_[index].ContainsValue(value))
                {
                    count += values_[index].Count();
                }

                ++index;
            }

            if (index < values_.size() && IsCorrectRangeIndex(index, upperBound) && values_[index].ContainsValue(value))
            {
                count += upperBound - values_[index].LowerBound();
            }

            return count;
        }

        [[nodiscard]] int32_t GetIndexCount(int32_t const lowerBound, int32_t const upperBound) const
        {
            if (upperBound < lowerBound || values_.empty())
            {
                return 0;
            }

            int32_t count = 0;
            auto index = FindRangeIndex(lowerBound);
            if (IsCorrectRangeIndex(index, lowerBound))
            {
                count += values_[index].UpperBound() - lowerBound + 1;
            }

            ++index;
            while (index < values_.size() && values_[index].UpperBound() <= upperBound)
            {
                count += values_[index].Count();
                ++index;
            }

            if (index < values_.size() && IsCorrectRangeIndex(index, upperBound))
            {
                count += upperBound - values_[index].LowerBound();
            }

            return count;
        }

        [[nodiscard]] int32_t GetIndexCountBeforeGap(int32_t const startingIndex, int32_t const gapSize) const
        {
            if (values_.empty())
            {
                return 0;
            }

            int32_t count = 0;
            auto currentIndex = startingIndex;
            int32_t rangeIndex = 0;
            int32_t gap = 0;
            while (gap <= gapSize && rangeIndex < values_.size())
            {
                gap += values_[rangeIndex].LowerBound() - currentIndex;
                if (gap <= gapSize)
                {
                    count += values_[rangeIndex].UpperBound() - values_[rangeIndex].LowerBound() + 1;
                    currentIndex = values_[rangeIndex].UpperBound() + 1;
                    rangeIndex++;
                }
            }

            return count;
        }

        [[nodiscard]] dlg_help_utils::generator<int32_t> GetIndexes() const
        {
            for (auto const& range : values_)
            {
                for (auto i = range.LowerBound(); i <= range.UpperBound(); ++i)
                {
                    co_yield i;
                }
            }
        }

        [[nodiscard]] dlg_help_utils::generator<int32_t> GetIndexes(int32_t const startIndex) const
        {
            auto rangeIndex = FindRangeIndex(startIndex);
            if (rangeIndex == InvalidIndex)
            {
                ++rangeIndex;
            }

            while (static_cast<typename decltype(values_)::size_type>(rangeIndex) < values_.size())
            {
                for (int32_t i = values_[rangeIndex].LowerBound(); i <= values_[rangeIndex].UpperBound(); ++i)
                {
                    if (i >= startIndex)
                    {
                        co_yield i;
                    }
                }

                ++rangeIndex;
            }
        }

        [[nodiscard]] int32_t GetNthIndex(int32_t const n) const
        {
            int32_t accumulatedEntries = 0;
            for (auto const& range : values_)
            {
                if (accumulatedEntries + range.Count() > n)
                {
                    return range.LowerBound() + n - accumulatedEntries;
                }

                accumulatedEntries += range.Count();
            }

            return InvalidIndex;
        }

        [[nodiscard]] T GetValueAt(int32_t const index) const
        {
            bool found;
            return GetValueAt(index, found);
        }

        [[nodiscard]] T GetValueAt(int32_t const index, bool& found) const
        {
            auto const rangeIndex = FindRangeIndex(index);
            if (IsCorrectRangeIndex(rangeIndex, index))
            {
                found = true;
                return values_[rangeIndex].Value();
            }
            else
            {
                found = false;
                return T{};
            }
        }

        [[nodiscard]] int32_t IndexOf(int32_t index) const
        {
            int32_t accumulatedIndexes = 0;
            for (auto const& range : values_)
            {
                if (range.UpperBound() >= index)
                {
                    accumulatedIndexes += index - range.LowerBound();
                    break;
                }
                accumulatedIndexes += range.Count();
            }

            return accumulatedIndexes;
        }

        void InsertIndex(int32_t const index)
        {
            InsertIndexes(index, 1);
        }

        void InsertIndexAndValue(int32_t const index, T value)
        {
            InsertIndexesAndValues(index, 1, std::move(value));
        }

        void InsertIndexes(int32_t const startIndex, int32_t const count)
        {
            InsertIndexesPrivate(startIndex, count, FindRangeIndex(startIndex));
        }

        void InsertIndexesAndValues(int32_t const startIndex, int32_t const count, T value)
        {
            auto lowerRangeIndex = FindRangeIndex(startIndex);
            InsertIndexesPrivate(startIndex, count, lowerRangeIndex);
            if ((lowerRangeIndex != InvalidIndex) && (values_[lowerRangeIndex].LowerBound() > startIndex))
            {
                // Because of the insert, the original range no longer contains the startIndex
                --lowerRangeIndex;
            }

            AddValuesPrivate(startIndex, count, value, lowerRangeIndex);
        }

        void RemoveIndex(int32_t const index)
        {
            RemoveIndexes(index, 1);
        }

        void RemoveIndexAndValue(int32_t const index)
        {
            RemoveIndexesAndValues(index, 1);
        }

        void RemoveIndexes(int32_t const startIndex, int32_t const count)
        {
            auto lowerRangeIndex = FindRangeIndex(startIndex);
            if (lowerRangeIndex < 0)
            {
                lowerRangeIndex = 0;
            }

            auto i = lowerRangeIndex;
            while (i < values_.size())
            {
                if (auto& range = values_[i]; range.UpperBound() >= startIndex)
                {
                    if (range.LowerBound() >= startIndex + count)
                    {
                        // Both bounds will remain after the removal
                        range.LowerBound(range.LowerBound() - count);
                        range.UpperBound(range.UpperBound() - count);
                    }
                    else
                    {
                        auto currentIndex = i;
                        if (range.LowerBound() <= startIndex)
                        {
                            // Range gets split up
                            if (range.UpperBound() >= startIndex + count)
                            {
                                ++i;
                                values_.insert(values_.begin() + i, Range<T>{startIndex, range.UpperBound() - count, range.Value()});
                            }

                            range.UpperBound(startIndex - 1);
                        }
                        else
                        {
                            range.LowerBound(startIndex);
                            range.UpperBound(range.UpperBound() - count);
                        }

                        if (RemoveRangeIfInvalid(range, currentIndex))
                        {
                            --i;
                        }
                    }
                }

                ++i;
            }

            if (!Merge(lowerRangeIndex))
            {
                Merge(lowerRangeIndex + 1);
            }
        }

        void RemoveIndexesAndValues(int32_t const startIndex, int32_t const count)
        {
            RemoveValues(startIndex, count);
            RemoveIndexes(startIndex, count);
        }

        void RemoveValue(int32_t const index)
        {
            RemoveValues(index, 1);
        }

        void RemoveValues(int32_t const startIndex, int32_t const count)
        {
            auto lowerRangeIndex = FindRangeIndex(startIndex);
            if (lowerRangeIndex < 0)
            {
                lowerRangeIndex = 0;
            }

            while ((static_cast<typename decltype(values_)::size_type>(lowerRangeIndex) < values_.size()) && 
                (static_cast<decltype(startIndex)>(values_[lowerRangeIndex].UpperBound()) < startIndex))
            {
                ++lowerRangeIndex;
            }

            if (static_cast<typename decltype(values_)::size_type>(lowerRangeIndex) >= values_.size() || 
                static_cast<decltype(startIndex)>(values_[lowerRangeIndex].LowerBound()) > startIndex + count - 1)
            {
                // If all the values are above our below our values, we have nothing to remove
                return;
            }

            if (values_[lowerRangeIndex].LowerBound() < startIndex)
            {
                // Need to split this up
                values_.insert(values_.begin() + lowerRangeIndex, Range<T>{values_[lowerRangeIndex].LowerBound(), startIndex - 1, values_[lowerRangeIndex].Value()});
                ++lowerRangeIndex;
            }

            values_[lowerRangeIndex].LowerBound(startIndex + count);
            if (!RemoveRangeIfInvalid(values_[lowerRangeIndex], lowerRangeIndex))
            {
                ++lowerRangeIndex;
            }

            while ((static_cast<typename decltype(values_)::size_type>(lowerRangeIndex) < values_.size()) && 
                (static_cast<decltype(startIndex)>(values_[lowerRangeIndex].UpperBound()) < startIndex + count))
            {
                values_.erase(values_.begin() + lowerRangeIndex);
            }

            if ((static_cast<typename decltype(values_)::size_type>(lowerRangeIndex) < values_.size()) && 
                (static_cast<decltype(startIndex)>(values_[lowerRangeIndex].UpperBound()) >= startIndex + count) &&
                (static_cast<decltype(startIndex)>(values_[lowerRangeIndex].LowerBound()) < startIndex + count))
            {
                // Chop off the start of the remaining Range if it contains values that we're removing
                values_[lowerRangeIndex].LowerBound(startIndex + count);
                RemoveRangeIfInvalid(values_[lowerRangeIndex], lowerRangeIndex);
            }
        }

    private:
        void AddValuesPrivate(int32_t const startIndex, int32_t const count, T value, std::optional<int32_t> const startRangeIndex)
        {
            auto endIndex = startIndex + count - 1;
            Range<T> newRange{startIndex, endIndex, value};
            if (values_.empty())
            {
                values_.push_back(std::move(newRange));
            }
            else
            {
                auto lowerRangeIndex = startRangeIndex.value_or(FindRangeIndex(startIndex));
                if (lowerRangeIndex < 0)
                {
                    if (lowerRangeIndex < 0)
                    {
                        lowerRangeIndex = 0;
                    }

                    values_.insert(values_.begin() + lowerRangeIndex, newRange);
                }
                else if (auto& lowerRange = values_[lowerRangeIndex];
                        lowerRange.Value() != value && (lowerRange.UpperBound() >= startIndex))
                {
                    // Split up the range
                    if (lowerRange.UpperBound() > endIndex)
                    {
                        values_.insert(values_.begin() + lowerRangeIndex + 1, Range<T>{endIndex + 1, lowerRange.UpperBound(), lowerRange.Value()});
                    }

                    lowerRange.UpperBound(startIndex - 1);
                    if (!RemoveRangeIfInvalid(lowerRange, lowerRangeIndex))
                    {
                        ++lowerRangeIndex;
                    }

                    values_.insert(values_.begin() + lowerRangeIndex, newRange);
                }
                else
                {
                    values_.insert(values_.begin() + lowerRangeIndex + 1, newRange);
                    if (!Merge(lowerRangeIndex))
                    {
                        ++lowerRangeIndex;
                    }
                }

                // At this point the newRange has been inserted in the correct place, now we need to remove
                // any subsequent ranges that no longer make sense and possibly update the one at newRange.UpperBound
                auto upperRangeIndex = lowerRangeIndex + 1;
                while ((upperRangeIndex < values_.size()) && (values_[upperRangeIndex].UpperBound() < endIndex))
                {
                    values_.erase(values_.begin() + upperRangeIndex);
                }

                if (upperRangeIndex < values_.size())
                {
                    if (auto& upperRange = values_[upperRangeIndex];
                        upperRange.LowerBound() <= endIndex)
                    {
                        // Update the range
                        upperRange.LowerBound(endIndex + 1);
                        RemoveRangeIfInvalid(upperRange, upperRangeIndex);
                    }

                    Merge(lowerRangeIndex);
                }
            }
        }

        [[nodiscard]] int32_t FindRangeIndex(int32_t const index) const
        {
            if (values_.size() == 0)
            {
                return InvalidIndex;
            }

            // Do a binary search for the index
            int32_t front = 0;
            auto end = static_cast<int32_t>(values_.size()) - 1;
            Range<T> range{0, 0, T{}};
            while (end > front)
            {
                auto median = (front + end) / 2;
                range = values_[median];
                if (range.UpperBound() < index)
                {
                    front = median + 1;
                }
                else if (range.LowerBound() > index)
                {
                    end = median - 1;
                }
                else
                {
                    // we found it
                    return median;
                }
            }

            if (front == end)
            {
                range = values_[front];
                if (range.ContainsIndex(index) || (range.UpperBound() < index))
                {
                    // we found it or the index isn't there, and we're one range before
                    return front;
                }

                // not found and we're one range after
                return front - 1;
            }

            // end is one index before front in this case, so it's the range before
            return end;
        }

        bool Merge(int32_t const lowerRangeIndex)
        {
            auto upperRangeIndex = lowerRangeIndex + 1;
            if (lowerRangeIndex != InvalidIndex && upperRangeIndex < values_.size())
            {
                auto& lowerRange = values_[lowerRangeIndex];
                if (auto const& upperRange = values_[upperRangeIndex];
                    lowerRange.UpperBound() + 1 >= upperRange.LowerBound() && lowerRange.Value() == upperRange.Value())
                {
                    lowerRange.UpperBound(std::max(lowerRange.UpperBound(), upperRange.UpperBound()));
                    values_.erase(values_.begin() + upperRangeIndex);
                    return true;
                }
            }

            return false;
        }

        void InsertIndexesPrivate(int32_t const startIndex, int32_t const count, int32_t const lowerRangeIndex)
        {
            // Same as AddRange after we fix the indices affected by the insertion
            auto const startRangeIndex = lowerRangeIndex != InvalidIndex ? lowerRangeIndex : 0;
            for (auto i = startRangeIndex; i < values_.size(); i++)
            {
                auto& range = values_[i];
                if (range.LowerBound() >= startIndex)
                {
                    range.LowerBound(range.LowerBound() + count);
                }
                else
                {
                    if (range.UpperBound() >= startIndex)
                    {
                        // Split up this range
                        i++;
                        values_.insert(values_.begin() + i, Range<T>{startIndex, range.UpperBound() + count, range.Value()});
                        range.UpperBound(startIndex - 1);
                        continue;
                    }
                }

                if (range.UpperBound() >= startIndex)
                {
                    range.UpperBound(range.UpperBound() + count);
                }
            }
        }

        [[nodiscard]] bool IsCorrectRangeIndex(int32_t const rangeIndex, int32_t const index) const
        {
            return rangeIndex != InvalidIndex && values_[rangeIndex].ContainsIndex(index);
        }

        bool RemoveRangeIfInvalid(Range<T> const& range, int32_t const rangeIndex)
        {
            if (range.UpperBound() < range.LowerBound())
            {
                values_.erase(values_.begin() + rangeIndex);
                return true;
            }

            return false;
        }

    private:
        std::vector<Range<T>> values_;
    };
}