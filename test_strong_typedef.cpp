#include "strong_typedef.hpp"
#include <type_traits>
#include <assert.h>
#include <iostream>

void test_strong_typedef_is_not_original() {
    std::cout << __FUNCTION__ << std::endl;

    static_assert(
        !std::is_same<int, jss::strong_typedef<struct MyTag, int>>::value,
        "Strong typedef should not be the same");
    static_assert(
        !std::is_same<
            jss::strong_typedef<struct MyTag, int>,
            jss::strong_typedef<struct MyTag2, int>>::value,
        "Strong typedefs with different tags should not be the same");
}

void test_strong_typedef_explicitly_convertible_from_source() {
    std::cout << __FUNCTION__ << std::endl;

    jss::strong_typedef<struct Tag, int> st(42);
}

using ConvTestTypedef= jss::strong_typedef<struct ConvTest, int>;

using small_result= char;
struct large_result {
    small_result dummy[2];
};

small_result conv_test(ConvTestTypedef);
large_result conv_test(...);

void test_strong_typedef_not_implicitly_convertible_from_source() {
    std::cout << __FUNCTION__ << std::endl;
    assert(sizeof(conv_test(42)) == sizeof(large_result));
}

void test_strong_typedef_explicitly_convertible_to_source() {
    std::cout << __FUNCTION__ << std::endl;

    jss::strong_typedef<struct Tag, int> st(42);
    assert(static_cast<int>(st) == 42);
}

small_result conv_test_to_int(int);
large_result conv_test_to_int(...);

void test_strong_typedef_not_implicitly_convertible_to_source() {
    std::cout << __FUNCTION__ << std::endl;

    jss::strong_typedef<struct Tag, int> st(42);
    assert(sizeof(conv_test_to_int(st)) == sizeof(large_result));
}

void test_strong_typedef_is_copyable_and_movable() {
    std::cout << __FUNCTION__ << std::endl;
    std::string const s=
        "hello there this is quote a long string abcdefghijklmnopoq";
    using ST= jss::strong_typedef<struct Tag, std::string>;
    ST st(s);

    static_assert(
        std::is_copy_constructible<ST>::value,
        "strong typedef should be copy constructible");
    static_assert(
        std::is_copy_assignable<ST>::value,
        "strong typedef should be copy assignable");
    static_assert(
        std::is_move_constructible<ST>::value,
        "strong typedef should be move constructible");
    static_assert(
        std::is_move_assignable<ST>::value,
        "strong typedef should be move assignable");

    ST st2(st);

    assert(static_cast<std::string>(st) == s);
    assert(static_cast<std::string>(st2) == s);

    ST st3(std::move(st));
    assert(static_cast<std::string>(st) == "");
    assert(static_cast<std::string>(st2) == s);
    assert(static_cast<std::string>(st3) == s);

    st= std::move(st2);
    assert(static_cast<std::string>(st) == s);
    assert(static_cast<std::string>(st2) == "");
    assert(static_cast<std::string>(st3) == s);

    st2= st;
    assert(static_cast<std::string>(st) == s);
    assert(static_cast<std::string>(st2) == s);
    assert(static_cast<std::string>(st3) == s);
}

template <typename T>
typename std::enable_if<
    sizeof(std::declval<T const &>() == std::declval<T const&>()) != 0, small_result>::type
test_equality(int);
template <typename T> large_result test_equality(...);

void test_by_default_strong_typedef_is_not_equality_comparable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, std::string>;

    assert(sizeof(test_equality<ST>(0)) == sizeof(large_result));
    assert(sizeof(test_equality<std::string>(0)) == sizeof(small_result));
}

void test_can_get_underlying_value_and_type() {
    std::cout << __FUNCTION__ << std::endl;

    struct LocalTag;
    struct X {
        int i;
    };

    using ST= jss::strong_typedef<LocalTag, X>;

    ST st({42});

    static_assert(
        std::is_same<typename ST::underlying_value_type, X>::value,
        "Strong typedef must expose underlying type");
    static_assert(
        std::is_same<
            decltype(std::declval<ST &>().underlying_value()),
            X&>::value,
        "Strong typedef must expose underlying value");
    static_assert(
        std::is_same<
            decltype(std::declval<ST const &>().underlying_value()),
            X const&>::value,
        "Strong typedef must expose underlying value");
    assert(st.underlying_value().i == 42);
}

void test_strong_typedef_is_equality_comparable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, std::string,
        jss::strong_typedef_properties::equality_comparable>;

    assert(sizeof(test_equality<ST>(0)) == sizeof(small_result));
    assert(sizeof(test_equality<std::string>(0)) == sizeof(small_result));
}

template <typename T>
typename std::enable_if<sizeof(std::declval<T &>()++) != 0, small_result>::type
test_post_incrementable(int);
template <typename T> large_result test_post_incrementable(...);

template <typename T>
typename std::enable_if<sizeof(++std::declval<T &>()) != 0, small_result>::type
test_pre_incrementable(int);
template <typename T> large_result test_pre_incrementable(...);

void test_by_default_strong_typedef_is_not_incrementable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    assert(sizeof(test_post_incrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_post_incrementable<std::string>(0)) ==
        sizeof(large_result));
    assert(sizeof(test_post_incrementable<int>(0)) == sizeof(small_result));
    assert(sizeof(test_pre_incrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_pre_incrementable<std::string>(0)) == sizeof(large_result));
    assert(sizeof(test_pre_incrementable<int>(0)) == sizeof(small_result));
}

void test_strong_typedef_is_incrementable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_post= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::post_incrementable>;
    using ST_pre= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::pre_incrementable>;

    assert(sizeof(test_pre_incrementable<ST_pre>(0)) == sizeof(small_result));
    assert(sizeof(test_pre_incrementable<ST_post>(0)) == sizeof(large_result));
    assert(sizeof(test_post_incrementable<ST_pre>(0)) == sizeof(large_result));
    assert(sizeof(test_post_incrementable<ST_post>(0)) == sizeof(small_result));
}

int main() {
    test_strong_typedef_is_not_original();
    test_strong_typedef_explicitly_convertible_from_source();
    test_strong_typedef_not_implicitly_convertible_from_source();
    test_strong_typedef_explicitly_convertible_to_source();
    test_strong_typedef_not_implicitly_convertible_to_source();
    test_strong_typedef_is_copyable_and_movable();
    test_by_default_strong_typedef_is_not_equality_comparable();
    test_can_get_underlying_value_and_type();
    test_strong_typedef_is_equality_comparable_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_incrementable();
    test_strong_typedef_is_incrementable_if_tagged_as_such();
}
