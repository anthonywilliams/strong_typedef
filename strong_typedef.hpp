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
        post_decrementable= 16
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
        template <typename, strong_typedef_properties>
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
    typename std::enable_if<
        detail::is_strong_typedef_with_properties<
            T, strong_typedef_properties::equality_comparable>::value,
        bool>::type
    operator==(T const &lhs, T const &rhs) {
        return lhs.underlying_value() == rhs.underlying_value();
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    typename std::enable_if<
        (Properties & strong_typedef_properties::pre_incrementable) ==
            strong_typedef_properties::pre_incrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator++(strong_typedef<Tag, ValueType, Properties> &self) {
        ++self.underlying_value();
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    typename std::enable_if<
        (Properties & strong_typedef_properties::post_incrementable) ==
            strong_typedef_properties::post_incrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator++(strong_typedef<Tag, ValueType, Properties> &self, int) {
        self.underlying_value()++;
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    typename std::enable_if<
        (Properties & strong_typedef_properties::pre_decrementable) ==
            strong_typedef_properties::pre_decrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator--(strong_typedef<Tag, ValueType, Properties> &self) {
        --self.underlying_value();
        return self;
    }

    template <
        typename Tag, typename ValueType, strong_typedef_properties Properties>
    typename std::enable_if<
        (Properties & strong_typedef_properties::post_decrementable) ==
            strong_typedef_properties::post_decrementable,
        strong_typedef<Tag, ValueType, Properties> &>::type
    operator--(strong_typedef<Tag, ValueType, Properties> &self, int) {
        self.underlying_value()--;
        return self;
    }

}

#endif
