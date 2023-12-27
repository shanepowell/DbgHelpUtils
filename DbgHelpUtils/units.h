#pragma once
#include <compare>
#include <limits>
#include <ratio>

// this is based on the std::chrono::duration limitation in MSVC 2020 library changed so that we can make the
// renamed duration to length take an extra template parameter so that it makes each unit implementation its
// own type that can't be confused with other types (like std::chrono::duration and also between base_10 length
// vs base_16 length).

namespace dlg_help_utils::units
{
    template <class _Rep>
    struct treat_as_floating_point : std::is_floating_point<_Rep> {}; // tests for floating-point type

    template <typename _Rep>
    inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<_Rep>::value;

    template <class _Rep>
    struct length_values { // gets arithmetic properties of a type
        [[nodiscard]] static constexpr _Rep zero() noexcept {
            // get zero value
            return _Rep(0);
        }

        [[nodiscard]] static constexpr _Rep(min)() noexcept {
            // get smallest value
            return std::numeric_limits<_Rep>::lowest();
        }

        [[nodiscard]] static constexpr _Rep(max)() noexcept {
            // get largest value
            return (std::numeric_limits<_Rep>::max)();
        }
    };

    [[nodiscard]] constexpr intmax_t _Abs(const intmax_t _Val) noexcept
    {
        return _Val < 0 ? -_Val : _Val;
    }

    [[nodiscard]] constexpr intmax_t _Gcd(intmax_t _Ax, intmax_t _Bx) noexcept
    {
        // computes GCD of abs(_Ax) and abs(_Bx)
        if (_Ax == 0 && _Bx == 0) {
            return 1; // contrary to mathematical convention; avoids division by 0 in ratio_less
        }

        _Ax = _Abs(_Ax);
        _Bx = _Abs(_Bx);

        while (_Bx != 0) {
            const intmax_t _Ax2 = _Ax;
            _Ax                 = _Bx;
            _Bx                 = _Ax2 % _Bx;
        }

        return _Ax;
    }

    [[nodiscard]] constexpr intmax_t _Lcm(const intmax_t _Ax, const intmax_t _Bx) noexcept
    {
        return (_Ax / _Gcd(_Ax, _Bx)) * _Bx;
    }

    template <typename UnitType, class Rep, class Scale = std::ratio<1>>
    class length;

    template <typename UnitType, typename T>
    constexpr auto _is_length_unit_type(UnitType const&, T const&) -> decltype(typename T::unit_type{}, std::conditional_t<std::is_same_v<typename T::unit_type, UnitType>, std::true_type, std::false_type>{})
    {
      return std::conditional_t<std::is_same_v<typename T::unit_type, UnitType>, std::true_type, std::false_type>{};
    }

    constexpr std::false_type _is_length_unit_type(...) { return std::false_type{}; }

    template <typename UnitType, typename Ty>
    inline constexpr bool _Is_length_v = std::_Is_specialization_v<Ty, length> && _is_length_unit_type(UnitType{}, Ty{});
    
    template <class _To, typename UnitType, class _Rep, class _Scale, std::enable_if_t<_Is_length_v<UnitType, _To>, int> = 0>
    constexpr _To length_cast(const length<UnitType, _Rep, _Scale>&) noexcept(std::is_arithmetic_v<_Rep> && std::is_arithmetic_v<typename _To::rep>); // strengthened

    template <typename UnitType, typename Rep, typename Scale>
    class length { // represents a unit length
    public:
        using unit_type    = UnitType;
        using rep    = Rep;
        using scale = typename Scale::type;

        static_assert(!_Is_length_v<UnitType, Rep>, "length can't have length as first template argument");
        static_assert(std::_Is_ratio_v<Scale>, "scale not an instance of std::ratio");
        static_assert(0 < Scale::num, "scale negative or zero");

        constexpr length() = default;

        template <class _Rep2, std::enable_if_t<std::is_convertible_v<const _Rep2&, Rep> && (treat_as_floating_point_v<Rep> || !treat_as_floating_point_v<_Rep2>), int> = 0>
        constexpr explicit length(const _Rep2& _Val) noexcept(std::is_arithmetic_v<Rep> && std::is_arithmetic_v<_Rep2>) // strengthened
            : _MyRep(static_cast<Rep>(_Val))
        {
        }

        template <class _Rep2, class _Scale2, std::enable_if_t<treat_as_floating_point_v<Rep> || (std::_Ratio_divide_sfinae<_Scale2, Scale>::den == 1 && !treat_as_floating_point_v<_Rep2>), int> = 0>
        constexpr length(const length<UnitType, _Rep2, _Scale2>& _Dur) noexcept(std::is_arithmetic_v<Rep> && std::is_arithmetic_v<_Rep2>) // strengthened
            : _MyRep(length_cast<length<UnitType, Rep, Scale>>(_Dur).count())
        {
        }

        [[nodiscard]] constexpr Rep count() const noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            return _MyRep;
        }

        [[nodiscard]] constexpr std::common_type_t<length> operator+() const noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            return std::common_type_t<length>(*this);
        }

        [[nodiscard]] constexpr std::common_type_t<length> operator-() const noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            return std::common_type_t<length>(-_MyRep);
        }

        constexpr length& operator++() noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            ++_MyRep;
            return *this;
        }

        constexpr length operator++(int) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            return length(_MyRep++);
        }

        constexpr length& operator--() noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            --_MyRep;
            return *this;
        }

        constexpr length operator--(int) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            return length(_MyRep--);
        }

        constexpr length& operator+=(const length& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep += _Right._MyRep;
            return *this;
        }

        constexpr length& operator-=(const length& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep -= _Right._MyRep;
            return *this;
        }

        constexpr length& operator*=(const Rep& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep *= _Right;
            return *this;
        }

        constexpr length& operator/=(const Rep& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep /= _Right;
            return *this;
        }

        constexpr length& operator%=(const Rep& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep %= _Right;
            return *this;
        }

        constexpr length& operator%=(const length& _Right) noexcept(std::is_arithmetic_v<Rep>) /* strengthened */
        {
            _MyRep %= _Right.count();
            return *this;
        }

        [[nodiscard]] static constexpr length zero() noexcept
        {
            // get zero value
            return length(length_values<Rep>::zero());
        }

        [[nodiscard]] static constexpr length(min)() noexcept
        {
            // get minimum value
            return length((length_values<Rep>::min)());
        }

        [[nodiscard]] static constexpr length(max)() noexcept
        {
            // get maximum value
            return length((length_values<Rep>::max)());
        }

    private:
        Rep _MyRep; // the stored rep
    };
}


template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
struct std::common_type<dlg_help_utils::units::length<UnitType, _Rep1, _Scale1>, dlg_help_utils::units::length<UnitType, _Rep2, _Scale2>>  // NOLINT(cert-dcl58-cpp)
{
    using type = dlg_help_utils::units::length<UnitType, std::common_type_t<_Rep1, _Rep2>, std::ratio<dlg_help_utils::units::_Gcd(_Scale1::num, _Scale2::num), dlg_help_utils::units::_Lcm(_Scale1::den, _Scale2::den)>>;
};

namespace dlg_help_utils::units
{
    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>
        operator+(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CD = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CD(_CD(_Left).count() + _CD(_Right).count());
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>
        operator-(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CD = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CD(_CD(_Left).count() - _CD(_Right).count());
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2,
              std::enable_if_t<std::is_convertible_v<const _Rep2&, std::common_type_t<_Rep1, _Rep2>>, int> = 0>
    [[nodiscard]] constexpr length<UnitType, std::common_type_t<_Rep1, _Rep2>, _Scale1> operator*(const length<UnitType, _Rep1, _Scale1>& _Left, const _Rep2& _Right) noexcept(std::is_arithmetic_v<_Rep1>&& std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CR = std::common_type_t<_Rep1, _Rep2>;
        using _CD = length<UnitType, _CR, _Scale1>;
        return _CD(_CD(_Left).count() * _Right);
    }

    template <typename UnitType, class _Rep1, class _Rep2, class _Scale2,
              std::enable_if_t<std::is_convertible_v<const _Rep1&, std::common_type_t<_Rep1, _Rep2>>, int> = 0>
    [[nodiscard]] constexpr length<UnitType, std::common_type_t<_Rep1, _Rep2>, _Scale2>
        operator*(const _Rep1& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */ {
        return _Right * _Left;
    }

    template <class _CR, typename UnitType, class _Scale1, class _Rep2, bool = std::is_convertible_v<const _Rep2&, _CR>>
    struct _length_div_mod1
    { // return type for length / rep and length % rep
        using type = length<UnitType, _CR, _Scale1>;
    };

    template <class _CR, typename UnitType, class _Scale1, class _Rep2>
    struct _length_div_mod1<_CR, UnitType, _Scale1, _Rep2, false> {}; // no return type

    template <class _CR, typename UnitType, class _Scale1, class _Rep2, bool = _Is_length_v<UnitType, _Rep2>>
    struct _length_div_mod {}; // no return type

    template <class _CR, typename UnitType, class _Scale1, class _Rep2>
    struct _length_div_mod<_CR, UnitType, _Scale1, _Rep2, false> : _length_div_mod1<_CR, UnitType, _Scale1, _Rep2>
    {
        // return type for length / rep and length % rep
    };

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2>
    [[nodiscard]] constexpr typename _length_div_mod<std::common_type_t<_Rep1, _Rep2>, UnitType, _Scale1, _Rep2>::type operator/(const length<UnitType, _Rep1, _Scale1>& _Left, const _Rep2& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CR = std::common_type_t<_Rep1, _Rep2>;
        using _CD = length<UnitType, _CR, _Scale1>;
        return _CD(_CD(_Left).count() / _Right);
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr std::common_type_t<_Rep1, _Rep2> operator/(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CD = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CD(_Left).count() / _CD(_Right).count();
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2>
    [[nodiscard]] constexpr typename _length_div_mod<std::common_type_t<_Rep1, _Rep2>, UnitType, _Scale1, _Rep2>::type operator%(const length<UnitType, _Rep1, _Scale1>& _Left, const _Rep2& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CR = std::common_type_t<_Rep1, _Rep2>;
        using _CD = length<UnitType, _CR, _Scale1>;
        return _CD(_CD(_Left).count() % _Right);
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>> operator%(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CD = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CD(_CD(_Left).count() % _CD(_Right).count());
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr bool operator==(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CT = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CT(_Left).count() == _CT(_Right).count();
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr bool operator<(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        using _CT = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CT(_Left).count() < _CT(_Right).count();
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr bool operator<=(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        return !(_Right < _Left);
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr bool operator>(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        return _Right < _Left;
    }

    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2>
    [[nodiscard]] constexpr bool operator>=(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        return !(_Left < _Right);
    }

    // clang-format off
    template <typename UnitType, class _Rep1, class _Scale1, class _Rep2, class _Scale2> requires std::three_way_comparable<typename std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>::rep>
    [[nodiscard]] constexpr auto operator<=>(const length<UnitType, _Rep1, _Scale1>& _Left, const length<UnitType, _Rep2, _Scale2>& _Right) noexcept(std::is_arithmetic_v<_Rep1> && std::is_arithmetic_v<_Rep2>) /* strengthened */
    {
        // clang-format on
        using _CT = std::common_type_t<length<UnitType, _Rep1, _Scale1>, length<UnitType, _Rep2, _Scale2>>;
        return _CT(_Left).count() <=> _CT(_Right).count();
    }

    template <class _To, typename UnitType, class _Rep, class _Scale, std::enable_if_t<_Is_length_v<UnitType, _To>, int> _Enabled>
    [[nodiscard]] constexpr _To length_cast(const length<UnitType, _Rep, _Scale>& _Dur) noexcept(std::is_arithmetic_v<_Rep> && std::is_arithmetic_v<typename _To::rep>) /* strengthened */
    {
        // convert length to another length; truncate
        using _CF = std::ratio_divide<_Scale, typename _To::scale>;

        using _ToRep = typename _To::rep;
        using _CR    = std::common_type_t<_ToRep, _Rep, intmax_t>;

        constexpr bool _Num_is_one = _CF::num == 1;
        // ReSharper disable once CppTooWideScope
        constexpr bool _Den_is_one = _CF::den == 1;

        if (_Den_is_one) {
            if (_Num_is_one) {
                return static_cast<_To>(static_cast<_ToRep>(_Dur.count()));
            } else {
                return static_cast<_To>(
                    static_cast<_ToRep>(static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num)));
            }
        } else {
            if (_Num_is_one) {
                return static_cast<_To>(
                    static_cast<_ToRep>(static_cast<_CR>(_Dur.count()) / static_cast<_CR>(_CF::den)));
            } else {
                return static_cast<_To>(static_cast<_ToRep>(
                    static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num) / static_cast<_CR>(_CF::den)));
            }
        }
    }

    template <class _To, typename UnitType, class _Rep, class _Scale, std::enable_if_t<_Is_length_v<UnitType, _To>, int> = 0>
    [[nodiscard]] constexpr _To floor(const length<UnitType, _Rep, _Scale>& _Dur) noexcept(std::is_arithmetic_v<_Rep> && std::is_arithmetic_v<typename _To::rep>) /* strengthened */
    {
        // convert length to another length; round towards negative infinity
        // i.e. the greatest integral result such that the result <= _Dur
        // ReSharper disable once CppInconsistentNaming
        // ReSharper disable once IdentifierTypo
        const _To _Casted{units::length_cast<_To>(_Dur)};
        if (_Casted > _Dur) {
            return _To{_Casted.count() - static_cast<typename _To::rep>(1)};
        }

        return _Casted;
    }

    template <class _To, typename UnitType, class _Rep, class _Scale, std::enable_if_t<_Is_length_v<UnitType, _To>, int> = 0>
    [[nodiscard]] constexpr _To ceil(const length<UnitType, _Rep, _Scale>& _Dur) noexcept(std::is_arithmetic_v<_Rep> && std::is_arithmetic_v<typename _To::rep>) /* strengthened */
    {
        // convert length to another length; round towards positive infinity
        // i.e. the least integral result such that _Dur <= the result
        // ReSharper disable once CppInconsistentNaming
        // ReSharper disable once IdentifierTypo
        const _To _Casted{units::length_cast<_To>(_Dur)};
        if (_Casted < _Dur) {
            return _To{_Casted.count() + static_cast<typename _To::rep>(1)};
        }

        return _Casted;
    }

    template <class _Rep>
    constexpr bool _Is_even(_Rep _Val) noexcept(std::is_arithmetic_v<_Rep>) /* strengthened */ {
        // Tests whether _Val is even
        return _Val % 2 == 0;
    }

    template <class _To, typename UnitType, class _Rep, class _Scale, std::enable_if_t<_Is_length_v<UnitType, _To> && !treat_as_floating_point_v<typename _To::rep>, int> = 0>
    [[nodiscard]] constexpr _To round(const length<UnitType, _Rep, _Scale>& _Dur) noexcept(std::is_arithmetic_v<_Rep> && std::is_arithmetic_v<typename _To::rep>) /* strengthened */
    {
        // convert length to another length, round to nearest, ties to even
        const _To _Floored{units::floor<_To>(_Dur)};
        // ReSharper disable once CppInconsistentNaming
        // ReSharper disable once IdentifierTypo
        const _To _Ceiled{_Floored + _To{1}};
        const auto _Floor_adjustment = _Dur - _Floored;
        const auto _Ceil_adjustment  = _Ceiled - _Dur;
        if (_Floor_adjustment < _Ceil_adjustment
            || (_Floor_adjustment == _Ceil_adjustment && _Is_even(_Floored.count()))) {
            return _Floored;
        }

        return _Ceiled;
    }

    template <typename UnitType, class _Rep, class _Scale, std::enable_if_t<std::numeric_limits<_Rep>::is_signed, int> = 0>
    [[nodiscard]] constexpr length<UnitType, _Rep, _Scale> abs(const length<UnitType, _Rep, _Scale> _Dur) noexcept(std::is_arithmetic_v<_Rep>) /* strengthened */
    {
        // create a length with count() the absolute value of _Dur.count()
        return _Dur < length<UnitType, _Rep, _Scale>::zero() ? length<UnitType, _Rep, _Scale>::zero() - _Dur : _Dur;
    }
}