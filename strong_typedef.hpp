#ifndef JSS_STRONG_TYPEDEF_HPP
#define JSS_STRONG_TYPEDEF_HPP
#include <type_traits>
#include <utility>

namespace jss {

    enum class strong_typedef_properties {
        none,
        equality_comparable= 1,
        pre_incrementable= 2,
        post_incrementable= 4,
        pre_decrementable= 8,
        post_decrementable= 16,
        addable= 32,
        subtractable= 64,
        ordered= 128,
        mixed_ordered= 256,
        incrementable= pre_incrementable | post_incrementable,
        decrementable= pre_decrementable | post_decrementable
    };

    constexpr strong_typedef_properties operator&(
        strong_typedef_properties lhs, strong_typedef_properties rhs) noexcept {
        typedef typename std::underlying_type<strong_typedef_properties>::type
            underlying;
        return static_cast<strong_typedef_properties>(
            static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    }

    template <
        typename Tag, typename ValueType,
        strong_typedef_properties Properties= strong_typedef_properties::none>
    class strong_typedef {
    public:
        using underlying_value_type= ValueType;

        explicit strong_typedef(ValueType value_) noexcept(
            std::is_nothrow_move_constructible<ValueType>::value) :
            value(std::move(value_)) {}

        explicit operator ValueType const &() const noexcept {
            return value;
        }

        ValueType const &underlying_value() const noexcept {
            return value;
        }

        ValueType &underlying_value() noexcept {
            return value;
        }

    private:
        ValueType value;
    };

    namespace detail {
        template <
            typename,
            strong_typedef_properties= strong_typedef_properties::none>
        struct is_strong_typedef_with_properties : std::false_type {};

        template <
            typename Tag, typename ValueType,
            strong_typedef_properties ActualProperties,
            strong_typedef_properties RequiredProperties>
        struct is_strong_typedef_with_properties<
            strong_typedef<Tag, ValueType, ActualProperties>,
            RequiredProperties>
            : std::integral_constant<
                  bool, (ActualProperties & RequiredProperties) ==
                            RequiredProperties> {};

    }

    template <typename T>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            T, strong_typedef_properties::equality_comparable>::value,
        bool>::type
    operator==(T const &lhs, T const &rhs) noexcept(noexcept(
        std::declval<T &>().underlying_value() ==
        std::declval<T &>().underlying_value())) {
        return lhs.underlying_value() == rhs.underlying_value();
    }

    template <typename T>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            T, strong_typedef_properties::equality_comparable>::value,
        bool>::type
    operator!=(T const &lhs, T const &rhs) noexcept(noexcept(
        std::declval<T &>().underlying_value() !=
        std::declval<T &>().underlying_value())) {
        return lhs.underlying_value() != rhs.underlying_value();
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    constexpr typename std::enable_if<
        (Properties & strong_typedef_properties::pre_incrementable) ==
            strong_typedef_properties::pre_incrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator++(strong_typedef<Tag, ValueType, Properties> &self) noexcept(
        noexcept(++std::declval<ValueType &>())) {
        ++self.underlying_value();
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    constexpr typename std::enable_if<
        (Properties & strong_typedef_properties::post_incrementable) ==
            strong_typedef_properties::post_incrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator++(strong_typedef<Tag, ValueType, Properties> &self, int) noexcept(
        noexcept(std::declval<ValueType &>()++)) {
        self.underlying_value()++;
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    constexpr typename std::enable_if<
        (Properties & strong_typedef_properties::pre_decrementable) ==
            strong_typedef_properties::pre_decrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator--(strong_typedef<Tag, ValueType, Properties> &self) noexcept(
        noexcept(--std::declval<ValueType &>())) {
        --self.underlying_value();
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    constexpr typename std::enable_if<
        (Properties & strong_typedef_properties::post_decrementable) ==
            strong_typedef_properties::post_decrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator--(strong_typedef<Tag, ValueType, Properties> &self, int) noexcept(
        noexcept(std::declval<ValueType &>()--)) {
        self.underlying_value()--;
        return self;
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::addable>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<
                        typename Lhs::underlying_value_type const &>() +
                    std::declval<Rhs const &>()),
                typename Lhs::underlying_value_type>::value,
        Lhs>::type
    operator+(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() +
        std::declval<Rhs const &>())) {
        return Lhs(lhs.underlying_value() + rhs);
    }

    template <typename ST>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            ST, strong_typedef_properties::addable>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename ST::underlying_value_type const &>() +
                    std::declval<typename ST::underlying_value_type const &>()),
                typename ST::underlying_value_type>::value,
        ST>::type
    operator+(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() +
        std::declval<typename ST::underlying_value_type const &>())) {
        return ST(lhs.underlying_value() + rhs.underlying_value());
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::addable>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() +
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                typename Rhs::underlying_value_type>::value,
        Rhs>::type
    operator+(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() +
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return Rhs(lhs + rhs.underlying_value());
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::subtractable>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<
                        typename Lhs::underlying_value_type const &>() -
                    std::declval<Rhs const &>()),
                typename Lhs::underlying_value_type>::value,
        Lhs>::type
    operator-(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() -
        std::declval<Rhs const &>())) {
        return Lhs(lhs.underlying_value() - rhs);
    }

    template <typename ST>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            ST, strong_typedef_properties::subtractable>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename ST::underlying_value_type const &>() -
                    std::declval<typename ST::underlying_value_type const &>()),
                typename ST::underlying_value_type>::value,
        ST>::type
    operator-(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() -
        std::declval<typename ST::underlying_value_type const &>())) {
        return ST(lhs.underlying_value() - rhs.underlying_value());
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::subtractable>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() -
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                typename Rhs::underlying_value_type>::value,
        Rhs>::type
    operator-(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() -
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return Rhs(lhs - rhs.underlying_value());
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename Lhs::underlying_value_type const
                                     &>() < std::declval<Rhs const &>()),
                bool>::value,
        bool>::type
    operator<(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() <
        std::declval<Rhs const &>())) {
        return lhs.underlying_value() < rhs;
    }

    template <typename ST>
    constexpr typename std::enable_if<
        (detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::ordered>::value ||
         detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::mixed_ordered>::value) &&
            std::is_convertible<
                decltype(
                    std::declval<typename ST::underlying_value_type const &>() <
                    std::declval<typename ST::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator<(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() <
        std::declval<typename ST::underlying_value_type const &>())) {
        return lhs.underlying_value() < rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() <
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator<(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() <
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return lhs < rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename Lhs::underlying_value_type const
                                     &>() <= std::declval<Rhs const &>()),
                typename Lhs::underlying_value_type>::value,
        bool>::type
    operator<=(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() <=
        std::declval<Rhs const &>())) {
        return lhs.underlying_value() <= rhs;
    }

    template <typename ST>
    constexpr typename std::enable_if<
        (detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::ordered>::value ||
         detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::mixed_ordered>::value) &&
            std::is_convertible<
                decltype(
                    std::declval<
                        typename ST::underlying_value_type const &>() <=
                    std::declval<typename ST::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator<=(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() <=
        std::declval<typename ST::underlying_value_type const &>())) {
        return lhs.underlying_value() <= rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() <=
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator<=(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() <=
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return lhs <= rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename Lhs::underlying_value_type const
                                     &>() >= std::declval<Rhs const &>()),
                typename Lhs::underlying_value_type>::value,
        bool>::type
    operator>=(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() >=
        std::declval<Rhs const &>())) {
        return lhs.underlying_value() >= rhs;
    }

    template <typename ST>
    constexpr typename std::enable_if<
        (detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::ordered>::value ||
         detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::mixed_ordered>::value) &&
            std::is_convertible<
                decltype(
                    std::declval<
                        typename ST::underlying_value_type const &>() >=
                    std::declval<typename ST::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator>=(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() >=
        std::declval<typename ST::underlying_value_type const &>())) {
        return lhs.underlying_value() >= rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() >=
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator>=(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() >=
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return lhs >= rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Lhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Rhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<typename Lhs::underlying_value_type const
                                     &>() > std::declval<Rhs const &>()),
                typename Lhs::underlying_value_type>::value,
        bool>::type
    operator>(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<typename Lhs::underlying_value_type const &>() >
        std::declval<Rhs const &>())) {
        return lhs.underlying_value() > rhs;
    }

    template <typename ST>
    constexpr typename std::enable_if<
        (detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::ordered>::value ||
         detail::is_strong_typedef_with_properties<
             ST, strong_typedef_properties::mixed_ordered>::value) &&
            std::is_convertible<
                decltype(
                    std::declval<typename ST::underlying_value_type const &>() >
                    std::declval<typename ST::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator>(ST const &lhs, ST const &rhs) noexcept(noexcept(
        std::declval<typename ST::underlying_value_type const &>() >
        std::declval<typename ST::underlying_value_type const &>())) {
        return lhs.underlying_value() > rhs.underlying_value();
    }

    template <typename Lhs, typename Rhs>
    constexpr typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            Rhs, strong_typedef_properties::mixed_ordered>::value &&
            !detail::is_strong_typedef_with_properties<Lhs>::value &&
            std::is_convertible<
                decltype(
                    std::declval<Lhs const &>() >
                    std::declval<
                        typename Rhs::underlying_value_type const &>()),
                bool>::value,
        bool>::type
    operator>(Lhs const &lhs, Rhs const &rhs) noexcept(noexcept(
        std::declval<Lhs const &>() >
        std::declval<typename Rhs::underlying_value_type const &>())) {
        return lhs > rhs.underlying_value();
    }

}

#endif
