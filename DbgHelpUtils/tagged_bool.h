#pragma once
namespace dlg_help_utils
{
    namespace tagged_bool_ns // artificial namespace to prevent ADL into namespace explicit
    {
        template <typename Tag>
        class tagged_bool
        {
            bool value_;

            template <typename /*OtherTag*/>
            friend class tagged_bool;

        public:

            constexpr explicit tagged_bool(const bool v) : value_{ v } {}

            constexpr explicit tagged_bool(int) = delete;
            constexpr explicit tagged_bool(double) = delete;
            constexpr explicit tagged_bool(void*) = delete;

            template <typename OtherTag>
            constexpr explicit tagged_bool(tagged_bool<OtherTag> b) : value_{ b.value } {}

            constexpr explicit operator bool() const { return value_; }
            constexpr tagged_bool operator!() const { return tagged_bool{ !value_ }; }

            friend constexpr bool operator==(tagged_bool l, tagged_bool r) { return l.value_ == r.value_; }
            friend constexpr bool operator!=(tagged_bool l, tagged_bool r) { return l.value_ != r.value_; }
        };
    }

    using tagged_bool_ns::tagged_bool; // with this tagged_bool is in namespace explicit but with disabled ADL
}