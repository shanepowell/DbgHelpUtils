#pragma once

namespace Utility
{
    template <typename T>
    class Range
    {
    public:
        Range(int32_t const lowerBound, int32_t const upperBound, T value)
            : lowerBound_{lowerBound}
            , upperBound_{upperBound}
            , value_{std::move(value)}
        {
        }

        [[nodiscard]] int32_t LowerBound() const
        {
            return lowerBound_;
        }

        void LowerBound(int32_t const value)
        {
            lowerBound_ = value;
        }

        [[nodiscard]] int32_t UpperBound() const
        {
            return upperBound_;
        }

        void UpperBound(int32_t const value)
        {
            upperBound_ = value;
        }

        [[nodiscard]] T const& Value() const
        {
            return value_;
        }

        void Value(T value)
        {
            value_ = std::move(value);
        }

        [[nodiscard]] int32_t Count() const
        {
            return upperBound_ - lowerBound_ + 1;
        }

        [[nodiscard]] bool ContainsIndex(int32_t const index) const
        {
            return lowerBound_ <= index && upperBound_ >= index;
        }

        [[nodiscard]] bool ContainsValue(T const& value) const
        {
            return value == value_;
        }

        [[nodiscard]] Range Copy() const
        {
            return Range{lowerBound_, upperBound_, value_};
        }

        auto operator<=>(Range const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)

    private:
        int32_t lowerBound_;
        int32_t upperBound_;
        T value_;
    };
}
