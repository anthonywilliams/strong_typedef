#include "strong_typedef.hpp"
#include <type_traits>
#include <assert.h>
#include <iostream>
#include <sstream>

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
    static_assert(sizeof(conv_test(42)) == sizeof(large_result));
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
    static_assert(sizeof(conv_test_to_int(st)) == sizeof(large_result));
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
    sizeof(std::declval<T const &>() == std::declval<T const &>()) != 0,
    small_result>::type
test_equality(int);
template <typename T> large_result test_equality(...);

template <typename T>
typename std::enable_if<
    sizeof(std::declval<T const &>() != std::declval<T const &>()) != 0,
    small_result>::type
test_inequality(int);
template <typename T> large_result test_inequality(...);

void test_by_default_strong_typedef_is_not_equality_comparable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, std::string>;

    static_assert(sizeof(test_equality<ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_equality<std::string>(0)) == sizeof(small_result));
    static_assert(sizeof(test_inequality<ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_inequality<std::string>(0)) == sizeof(small_result));
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
            decltype(std::declval<ST &>().underlying_value()), X &>::value,
        "Strong typedef must expose underlying value");
    static_assert(
        std::is_same<
            decltype(std::declval<ST const &>().underlying_value()),
            X const &>::value,
        "Strong typedef must expose underlying value");
    assert(st.underlying_value().i == 42);
}

void test_strong_typedef_is_equality_comparable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, std::string,
        jss::strong_typedef_properties::equality_comparable>;

    static_assert(sizeof(test_equality<ST>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_equality<std::string>(0)) == sizeof(small_result));
    static_assert(sizeof(test_inequality<ST>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_inequality<std::string>(0)) == sizeof(small_result));
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

    static_assert(
        sizeof(test_post_incrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_post_incrementable<std::string>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_post_incrementable<int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_pre_incrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_pre_incrementable<std::string>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_pre_incrementable<int>(0)) == sizeof(small_result));
}

void test_strong_typedef_is_incrementable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_post= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::post_incrementable>;
    using ST_pre= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::pre_incrementable>;
    using ST_both= jss::strong_typedef<
        struct ST_both_tag, int, jss::strong_typedef_properties::incrementable>;

    static_assert(
        sizeof(test_pre_incrementable<ST_pre>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_pre_incrementable<ST_post>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_post_incrementable<ST_pre>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_post_incrementable<ST_post>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_pre_incrementable<ST_both>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_post_incrementable<ST_both>(0)) == sizeof(small_result));

    ST_post post1(42);
    ST_post post2= post1++;

    assert(post1.underlying_value() == 43);
    assert(post2.underlying_value() == 42);

    ST_pre pre1(42);
    ST_pre &pre2= ++pre1;

    assert(&pre2 == &pre1);

    assert(pre1.underlying_value() == 43);
}

template <typename T>
typename std::enable_if<sizeof(std::declval<T &>()--) != 0, small_result>::type
test_post_decrementable(int);
template <typename T> large_result test_post_decrementable(...);

template <typename T>
typename std::enable_if<sizeof(--std::declval<T &>()) != 0, small_result>::type
test_pre_decrementable(int);
template <typename T> large_result test_pre_decrementable(...);

void test_by_default_strong_typedef_is_not_decrementable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    static_assert(
        sizeof(test_post_decrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_post_decrementable<std::string>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_post_decrementable<int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_pre_decrementable<ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_pre_decrementable<std::string>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_pre_decrementable<int>(0)) == sizeof(small_result));
}

void test_strong_typedef_is_decrementable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_post= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::post_decrementable>;
    using ST_pre= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::pre_decrementable>;

    static_assert(
        sizeof(test_pre_decrementable<ST_pre>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_pre_decrementable<ST_post>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_post_decrementable<ST_pre>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_post_decrementable<ST_post>(0)) == sizeof(small_result));

    ST_post post1(42);
    ST_post post2= post1--;

    assert(post1.underlying_value() == 41);
    assert(post2.underlying_value() == 42);

    ST_pre pre1(42);
    ST_pre &pre2= --pre1;

    assert(&pre2 == &pre1);

    assert(pre1.underlying_value() == 41);
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() + std::declval<U &>()) != 0, small_result>::type
test_addable(int);
template <typename T, typename U> large_result test_addable(...);

void test_by_default_strong_typedef_is_not_addable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, std::string>;

    static_assert(sizeof(test_addable<ST, ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_addable<ST, std::string>(0)) == sizeof(large_result));
    static_assert(sizeof(test_addable<ST, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_addable<std::string, ST>(0)) == sizeof(large_result));
    static_assert(sizeof(test_addable<int, ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_addable<std::string, std::string>(0)) ==
        sizeof(small_result));
    static_assert(sizeof(test_addable<int, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_addable<std::string, int>(0)) == sizeof(large_result));
}

void test_strong_typedef_is_addable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, std::string, jss::strong_typedef_properties::addable>;
    static_assert(sizeof(test_addable<ST, ST>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_addable<ST, std::string>(0)) == sizeof(small_result));
    static_assert(sizeof(test_addable<ST, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_addable<std::string, ST>(0)) == sizeof(small_result));
    static_assert(sizeof(test_addable<int, ST>(0)) == sizeof(large_result));

    ST st1("hello");
    ST st2= st1 + " world";
    assert(st1.underlying_value() == "hello");
    assert(st2.underlying_value() == "hello world");
    auto st3= "goodbye" + st1;
    assert(st1.underlying_value() == "hello");
    assert(st3.underlying_value() == "goodbyehello");
    auto st4= st1 + st1;
    assert(st1.underlying_value() == "hello");
    assert(st4.underlying_value() == "hellohello");
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() - std::declval<U &>()) != 0, small_result>::type
test_subtractable(int);
template <typename T, typename U> large_result test_subtractable(...);

void test_by_default_strong_typedef_is_not_subtractable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    static_assert(sizeof(test_subtractable<ST, ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_subtractable<ST, std::string>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_subtractable<ST, int>(0)) == sizeof(large_result));
    assert(
        sizeof(test_subtractable<std::string, ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_subtractable<int, ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_subtractable<std::string, std::string>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_subtractable<int, int>(0)) == sizeof(small_result));
    assert(
        sizeof(test_subtractable<std::string, int>(0)) == sizeof(large_result));
}

void test_strong_typedef_is_subtractable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::subtractable>;
    static_assert(sizeof(test_subtractable<ST, ST>(0)) == sizeof(small_result));
    assert(
        sizeof(test_subtractable<ST, std::string>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_subtractable<ST, int>(0)) == sizeof(small_result));
    assert(
        sizeof(test_subtractable<std::string, ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_subtractable<int, ST>(0)) == sizeof(small_result));
}

template <typename T, typename U>
typename std::enable_if<
    std::is_same<
        bool, decltype(std::declval<T const &>() < std::declval<U const &>())>::
            value &&
        std::is_same<
            bool, decltype(
                      std::declval<T const &>() >
                      std::declval<U const &>())>::value &&
        std::is_same<
            bool, decltype(
                      std::declval<T const &>() <=
                      std::declval<U const &>())>::value &&
        std::is_same<
            bool,
            decltype(
                std::declval<T const &>() >= std::declval<U const &>())>::value,
    small_result>::type
test_ordered(int);
template <typename T, typename U> large_result test_ordered(...);

void test_by_default_strong_typedef_is_not_ordered() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    static_assert(sizeof(test_ordered<ST, ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_ordered<ST, std::string>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<ST, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_ordered<std::string, ST>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<int, ST>(0)) == sizeof(large_result));
    assert(
        sizeof(test_ordered<std::string, std::string>(0)) ==
        sizeof(small_result));
    static_assert(sizeof(test_ordered<int, int>(0)) == sizeof(small_result));
    static_assert(sizeof(test_ordered<float, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_ordered<std::string, int>(0)) == sizeof(large_result));
}

void test_strong_typedef_is_ordered_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::ordered>;
    using ST2= jss::strong_typedef<
        struct Tag2, int, jss::strong_typedef_properties::ordered>;
    static_assert(sizeof(test_ordered<ST, ST>(0)) == sizeof(small_result));
    static_assert(sizeof(test_ordered<ST, ST2>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_ordered<ST, std::string>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<ST, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_ordered<std::string, ST>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<int, ST>(0)) == sizeof(large_result));

    ST const st1(42);
    ST const st2(43);

    assert(!(st1 < st1));
    assert(!(st1 > st1));
    assert(st1 <= st1);
    assert(st1 >= st1);
    assert(st1 < st2);
    assert(st1 <= st2);
    assert(!(st2 < st1));
    assert(!(st2 <= st1));
    assert(st2 > st1);
    assert(st2 >= st1);
    assert(!(st1 > st2));
    assert(!(st1 >= st2));
}

void test_strong_typedef_is_mixed_ordered_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::mixed_ordered<int>>;
    using ST2= jss::strong_typedef<
        struct Tag2, int, jss::strong_typedef_properties::mixed_ordered<int>>;
    static_assert(sizeof(test_ordered<ST, ST>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<ST, ST2>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_ordered<ST, std::string>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<ST, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_ordered<std::string, ST>(0)) == sizeof(large_result));
    static_assert(sizeof(test_ordered<int, ST>(0)) == sizeof(small_result));

    ST constexpr st1(42);
    int const st2(43);

    assert(st1 < st2);
    assert(st1 <= st2);
    assert(!(st2 < st1));
    assert(!(st2 <= st1));
    assert(st2 > st1);
    assert(st2 >= st1);
    assert(!(st1 > st2));
    assert(!(st1 >= st2));
}

template <typename T>
typename std::enable_if<
    std::is_convertible<
        decltype(std::hash<T>()(std::declval<T const &>())), size_t>::value,
    small_result>::type
test_hashable(int);
template <typename T> large_result test_hashable(...);

void test_by_default_strong_typedef_is_not_hashable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    static_assert(sizeof(test_hashable<ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_hashable<std::string>(0)) == sizeof(small_result));
    static_assert(sizeof(test_hashable<int>(0)) == sizeof(small_result));
}

void test_strong_typedef_is_hashable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, std::string, jss::strong_typedef_properties::hashable>;
    static_assert(sizeof(test_hashable<ST>(0)) == sizeof(small_result));

    std::string s("hello");
    ST st(s);
    assert(std::hash<ST>()(st) == std::hash<std::string>()(s));
}

template <typename T>
typename std::enable_if<
    std::is_convertible<
        decltype(std::declval<std::ostream &>() << std::declval<T const &>()),
        std::ostream &>::value,
    small_result>::type
test_streamable(int);
template <typename T> large_result test_streamable(...);

void test_by_default_strong_typedef_is_not_streamable() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<struct Tag, int>;

    static_assert(sizeof(test_streamable<ST>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_streamable<std::string>(0)) == sizeof(small_result));
    static_assert(sizeof(test_streamable<int>(0)) == sizeof(small_result));
}

void test_strong_typedef_is_streamable_if_tagged_as_such() {
    std::cout << __FUNCTION__ << std::endl;

    using ST= jss::strong_typedef<
        struct Tag, std::string, jss::strong_typedef_properties::streamable>;
    static_assert(
        sizeof(test_streamable<ST>(0)) == sizeof(small_result),
        "Must be streamable when tagged");

    std::string s("hello");
    ST st(s);
    std::stringstream os;
    os << st;

    assert(os.str() == s);
}

void test_properties_can_be_combined() {
    std::cout << __FUNCTION__ << std::endl;
    using ST= jss::strong_typedef<
        struct Tag, std::string, jss::strong_typedef_properties::streamable,
        jss::strong_typedef_properties::hashable,
        jss::strong_typedef_properties::comparable>;
    static_assert(
        sizeof(test_streamable<ST>(0)) == sizeof(small_result),
        "Must be streamable when tagged");
    static_assert(
        sizeof(test_hashable<ST>(0)) == sizeof(small_result),
        "Must be hashable when tagged");
    static_assert(
        sizeof(test_ordered<ST, ST>(0)) == sizeof(small_result),
        "Must be ordered when tagged");
    static_assert(
        sizeof(test_equality<ST>(0)) == sizeof(small_result),
        "Must be equality-comparable when tagged");
}

void test_strong_typedef_is_default_constructible() {
    std::cout << __FUNCTION__ << std::endl;

    struct X {
        int value;

        X() : value(42) {}
    };

    using ST= jss::strong_typedef<struct sometag, X>;

    ST st;

    assert(static_cast<X>(st).value == 42);

    jss::strong_typedef<struct footag, int> i;

    assert(static_cast<int>(i) == 0);
}

void test_can_support_difference_with_other_type() {
    std::cout << __FUNCTION__ << std::endl;

    using difference_type= jss::strong_typedef<struct difftag, int>;
    using ST= jss::strong_typedef<
        struct sometag, int,
        jss::strong_typedef_properties::difference<difference_type>>;

    static_assert(sizeof(test_subtractable<ST, ST>(0)) == sizeof(small_result));
    static_assert(
        std::is_same<
            decltype(std::declval<ST const &>() - std::declval<ST const &>()),
            difference_type>::value);

    ST st1(45);
    ST st2(99);

    difference_type res= st2 - st1;
    assert(res.underlying_value() == 54);
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() * std::declval<U &>()) != 0, small_result>::type
test_multiplicable(int);
template <typename T, typename U> large_result test_multiplicable(...);

void test_self_multiplication() {
    std::cout << __FUNCTION__ << std::endl;

    static_assert(
        sizeof(test_multiplicable<int, int>(0)) == sizeof(small_result));

    using ST1= jss::strong_typedef<struct Tag1, int>;
    static_assert(
        sizeof(test_multiplicable<ST1, ST1>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_multiplicable<ST1, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_multiplicable<int, ST1>(0)) == sizeof(large_result));
}

void test_mixed_multiplication() {
    std::cout << __FUNCTION__ << std::endl;

    using ST2= jss::strong_typedef<
        struct Tag2, int, jss::strong_typedef_properties::self_multiplicable>;
    static_assert(
        sizeof(test_multiplicable<ST2, ST2>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_multiplicable<ST2, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_multiplicable<int, ST2>(0)) == sizeof(large_result));

    ST2 a(5);
    ST2 b(6);
    ST2 c= a * b;
    assert(c.underlying_value() == 30);

    using ST3= jss::strong_typedef<
        struct Tag3, int,
        jss::strong_typedef_properties::mixed_multiplicable<int>>;
    static_assert(
        sizeof(test_multiplicable<ST3, ST3>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_multiplicable<ST3, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_multiplicable<int, ST3>(0)) == sizeof(small_result));

    ST3 d(9);
    int e(7);
    ST3 f= d * e;
    assert(f.underlying_value() == 63);

    using ST4= jss::strong_typedef<
        struct Tag4, int, jss::strong_typedef_properties::multiplicable>;
    static_assert(
        sizeof(test_multiplicable<ST4, ST4>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_multiplicable<ST4, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_multiplicable<int, ST4>(0)) == sizeof(small_result));
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() / std::declval<U &>()) != 0, small_result>::type
test_divisible(int);
template <typename T, typename U> large_result test_divisible(...);

void test_self_division() {
    std::cout << __FUNCTION__ << std::endl;

    static_assert(sizeof(test_divisible<int, int>(0)) == sizeof(small_result));

    using ST1= jss::strong_typedef<struct Tag1, int>;
    static_assert(sizeof(test_divisible<ST1, ST1>(0)) == sizeof(large_result));
    static_assert(sizeof(test_divisible<ST1, int>(0)) == sizeof(large_result));
    static_assert(sizeof(test_divisible<int, ST1>(0)) == sizeof(large_result));
}

void test_mixed_division() {
    std::cout << __FUNCTION__ << std::endl;

    using ST2= jss::strong_typedef<
        struct Tag2, int, jss::strong_typedef_properties::self_divisible>;
    static_assert(sizeof(test_divisible<ST2, ST2>(0)) == sizeof(small_result));
    static_assert(sizeof(test_divisible<ST2, int>(0)) == sizeof(large_result));
    static_assert(sizeof(test_divisible<int, ST2>(0)) == sizeof(large_result));

    ST2 a(42);
    ST2 b(6);
    ST2 c= a / b;
    assert(c.underlying_value() == 7);

    using ST3= jss::strong_typedef<
        struct Tag3, int, jss::strong_typedef_properties::mixed_divisible<int>>;
    static_assert(sizeof(test_divisible<ST3, ST3>(0)) == sizeof(large_result));
    static_assert(sizeof(test_divisible<ST3, int>(0)) == sizeof(small_result));
    static_assert(sizeof(test_divisible<int, ST3>(0)) == sizeof(small_result));

    ST3 d(99);
    int e(11);
    ST3 f= d / e;
    assert(f.underlying_value() == 9);

    using ST4= jss::strong_typedef<
        struct Tag4, int, jss::strong_typedef_properties::divisible>;
    static_assert(sizeof(test_divisible<ST4, ST4>(0)) == sizeof(small_result));
    static_assert(sizeof(test_divisible<ST4, int>(0)) == sizeof(small_result));
    static_assert(sizeof(test_divisible<int, ST4>(0)) == sizeof(small_result));
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() % std::declval<U &>()) != 0, small_result>::type
test_modulus(int);
template <typename T, typename U> large_result test_modulus(...);

void test_self_modulus() {
    std::cout << __FUNCTION__ << std::endl;

    static_assert(sizeof(test_modulus<int, int>(0)) == sizeof(small_result));

    using ST1= jss::strong_typedef<struct Tag1, int>;
    static_assert(sizeof(test_modulus<ST1, ST1>(0)) == sizeof(large_result));
    static_assert(sizeof(test_modulus<ST1, int>(0)) == sizeof(large_result));
    static_assert(sizeof(test_modulus<int, ST1>(0)) == sizeof(large_result));
}

void test_mixed_modulus() {
    std::cout << __FUNCTION__ << std::endl;

    using ST2= jss::strong_typedef<
        struct Tag2, int, jss::strong_typedef_properties::self_modulus>;
    static_assert(sizeof(test_modulus<ST2, ST2>(0)) == sizeof(small_result));
    static_assert(sizeof(test_modulus<ST2, int>(0)) == sizeof(large_result));
    static_assert(sizeof(test_modulus<int, ST2>(0)) == sizeof(large_result));

    constexpr ST2 a(42);
    constexpr ST2 b(5);
    constexpr ST2 c= a % b;
    static_assert(c.underlying_value() == 2);

    using ST3= jss::strong_typedef<
        struct Tag3, int, jss::strong_typedef_properties::mixed_modulus<int>>;
    static_assert(sizeof(test_modulus<ST3, ST3>(0)) == sizeof(large_result));
    static_assert(sizeof(test_modulus<ST3, int>(0)) == sizeof(small_result));
    static_assert(sizeof(test_modulus<int, ST3>(0)) == sizeof(small_result));

    constexpr ST3 d(99);
    constexpr int e(8);
    constexpr ST3 f= d % e;
    static_assert(f.underlying_value() == 3);

    using ST4= jss::strong_typedef<
        struct Tag4, int, jss::strong_typedef_properties::modulus>;
    static_assert(sizeof(test_modulus<ST4, ST4>(0)) == sizeof(small_result));
    static_assert(sizeof(test_modulus<ST4, int>(0)) == sizeof(small_result));
    static_assert(sizeof(test_modulus<int, ST4>(0)) == sizeof(small_result));
}

void test_ratio() {
    std::cout << __FUNCTION__ << std::endl;

    using ratio_type= jss::strong_typedef<struct difftag, int>;
    using ST= jss::strong_typedef<
        struct sometag, int, jss::strong_typedef_properties::ratio<ratio_type>>;

    static_assert(sizeof(test_divisible<ST, ST>(0)) == sizeof(small_result));
    static_assert(
        std::is_same<
            decltype(std::declval<ST const &>() / std::declval<ST const &>()),
            ratio_type>::value);

    ST st1(125);
    ST st2(5);

    ratio_type res= st1 / st2;
    assert(res.underlying_value() == 25);
}

void test_constexpr_comparison() {
    std::cout << __FUNCTION__ << std::endl;
    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::comparable>;

    constexpr ST st1(42);
    constexpr ST st2(43);
    constexpr bool r1= st1 == st2;
    constexpr bool r2= st1 < st2;
    constexpr bool r3= st1 > st2;
    constexpr bool r4= st1 <= st2;
    constexpr bool r5= st1 >= st2;
    constexpr bool r6= st1 != st2;
    static_assert(!r1);
    static_assert(r2);
    static_assert(!r3);
    static_assert(r4);
    static_assert(!r5);
    static_assert(r6);
}

void test_constexpr_addition() {
    std::cout << __FUNCTION__ << std::endl;
    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::addable>;

    constexpr ST st1(42);
    constexpr ST st2(3);

    constexpr ST st3= st1 + st2;
    constexpr ST st4= st1 + 1;
    constexpr ST st5= -1 + st1;

    static_assert(st3.underlying_value() == 45);
    static_assert(st4.underlying_value() == 43);
    static_assert(st5.underlying_value() == 41);
}

void test_constexpr_subtraction() {
    std::cout << __FUNCTION__ << std::endl;
    using ST= jss::strong_typedef<
        struct Tag, int, jss::strong_typedef_properties::subtractable>;

    constexpr ST st1(42);
    constexpr ST st2(3);

    constexpr ST st3= st1 - st2;
    constexpr ST st4= st1 - 1;
    constexpr ST st5= -1 - st1;

    static_assert(st3.underlying_value() == 39);
    static_assert(st4.underlying_value() == 41);
    static_assert(st5.underlying_value() == -43);
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() | std::declval<U &>()) != 0, small_result>::type
test_bitwise_or(int);
template <typename T, typename U> large_result test_bitwise_or(...);

void test_bitwise_or() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_self= jss::strong_typedef<
        struct Self, int, jss::strong_typedef_properties::self_bitwise_or>;
    using ST_mixed= jss::strong_typedef<
        struct Mixed, int,
        jss::strong_typedef_properties::mixed_bitwise_or<int>>;

    static_assert(
        sizeof(test_bitwise_or<ST_plain, ST_plain>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_or<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_or<int, ST_plain>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_or<ST_self, ST_self>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_or<ST_self, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_or<int, ST_self>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_or<ST_mixed, ST_mixed>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_or<ST_mixed, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_or<int, ST_mixed>(0)) == sizeof(small_result));

    constexpr ST_self st1{0x1842};
    constexpr ST_self st2(0x8214);

    constexpr ST_self st3= st1 | st2;

    static_assert(st3.underlying_value() == 0x9a56);

    constexpr ST_mixed st4{0x1842a5};
    constexpr int i1(0x82145a);

    constexpr ST_mixed st5= st4 | i1;
    constexpr ST_mixed st6= i1 | st4;

    static_assert(st5.underlying_value() == 0x9a56ff);
    static_assert(st6.underlying_value() == 0x9a56ff);
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() & std::declval<U &>()) != 0, small_result>::type
test_bitwise_and(int);
template <typename T, typename U> large_result test_bitwise_and(...);

void test_bitwise_and() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_self= jss::strong_typedef<
        struct Self, int, jss::strong_typedef_properties::self_bitwise_and>;
    using ST_mixed= jss::strong_typedef<
        struct Mixed, int,
        jss::strong_typedef_properties::mixed_bitwise_and<int>>;

    static_assert(
        sizeof(test_bitwise_and<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_and<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_and<int, ST_plain>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_and<ST_self, ST_self>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_and<ST_self, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_and<int, ST_self>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_and<ST_mixed, ST_mixed>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_and<ST_mixed, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_and<int, ST_mixed>(0)) == sizeof(small_result));

    constexpr ST_self st1{0x1876};
    constexpr ST_self st2(0x9af4);

    constexpr ST_self st3= st1 & st2;

    static_assert(st3.underlying_value() == 0x1874);

    constexpr ST_mixed st4{0xf3c5a5};
    constexpr int i1(0x83945a);

    constexpr ST_mixed st5= st4 & i1;
    constexpr ST_mixed st6= i1 & st4;

    static_assert(st5.underlying_value() == 0x838400);
    static_assert(st6.underlying_value() == 0x838400);
}
template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() ^ std::declval<U &>()) != 0, small_result>::type
test_bitwise_xor(int);
template <typename T, typename U> large_result test_bitwise_xor(...);

void test_bitwise_xor() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_self= jss::strong_typedef<
        struct Self, int, jss::strong_typedef_properties::self_bitwise_xor>;
    using ST_mixed= jss::strong_typedef<
        struct Mixed, int,
        jss::strong_typedef_properties::mixed_bitwise_xor<int>>;

    static_assert(
        sizeof(test_bitwise_xor<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_xor<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_xor<int, ST_plain>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_xor<ST_self, ST_self>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_xor<ST_self, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_xor<int, ST_self>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_xor<ST_mixed, ST_mixed>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_xor<ST_mixed, int>(0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_xor<int, ST_mixed>(0)) == sizeof(small_result));

    constexpr ST_self st1{0x1876};
    constexpr ST_self st2(0x9af4);

    constexpr ST_self st3= st1 ^ st2;

    static_assert(st3.underlying_value() == 0x8282);

    constexpr ST_mixed st4{0xf3c5a5};
    constexpr int i1(0x83945a);

    constexpr ST_mixed st5= st4 ^ i1;
    constexpr ST_mixed st6= i1 ^ st4;

    static_assert(st5.underlying_value() == 0x7051ff);
    static_assert(st6.underlying_value() == 0x7051ff);
}

template <typename T>
typename std::enable_if<sizeof(~std::declval<T &>()) != 0, small_result>::type
test_bitwise_not(int);
template <typename T> large_result test_bitwise_not(...);

void test_bitwise_not() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_self= jss::strong_typedef<
        struct Self, int, jss::strong_typedef_properties::bitwise_not>;

    static_assert(
        sizeof(test_bitwise_not<ST_plain>(0)) == sizeof(large_result));

    static_assert(sizeof(test_bitwise_not<ST_self>(0)) == sizeof(small_result));

    constexpr ST_self st1{0x1876};

    constexpr ST_self st2= ~st1;

    static_assert(st2.underlying_value() == 0xffffe789);
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() << std::declval<U &>()) != 0, small_result>::type
test_bitwise_left_shift(int);
template <typename T, typename U> large_result test_bitwise_left_shift(...);

void test_bitwise_left_shift() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_mixed= jss::strong_typedef<
        struct Mixed, int,
        jss::strong_typedef_properties::bitwise_left_shift<int>>;

    static_assert(
        sizeof(test_bitwise_left_shift<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_left_shift<ST_plain, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_left_shift<int, ST_plain>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_left_shift<ST_mixed, ST_mixed>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_left_shift<ST_mixed, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_left_shift<int, ST_mixed>(0)) ==
        sizeof(large_result));

    constexpr ST_mixed st4{0x1842a5};
    constexpr int i1(3);

    constexpr ST_mixed st5= st4 << i1;

    static_assert(st5.underlying_value() == (st4.underlying_value() << i1));
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>() >> std::declval<U &>()) != 0, small_result>::type
test_bitwise_right_shift(int);
template <typename T, typename U> large_result test_bitwise_right_shift(...);

void test_bitwise_right_shift() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct Plain, int>;
    using ST_mixed= jss::strong_typedef<
        struct Mixed, int,
        jss::strong_typedef_properties::bitwise_right_shift<int>>;

    static_assert(
        sizeof(test_bitwise_right_shift<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_right_shift<ST_plain, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_right_shift<int, ST_plain>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_bitwise_right_shift<ST_mixed, ST_mixed>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bitwise_right_shift<ST_mixed, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_bitwise_right_shift<int, ST_mixed>(0)) ==
        sizeof(large_result));

    constexpr ST_mixed st4{0x1842a5};
    constexpr int i1(3);

    constexpr ST_mixed st5= st4 >> i1;

    static_assert(st5.underlying_value() == (st4.underlying_value() >> i1));
}

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()+= std::declval<U &>()) != 0, small_result>::type
test_plus_equals(int);
template <typename T, typename U> large_result test_plus_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()-= std::declval<U &>()) != 0, small_result>::type
test_minus_equals(int);
template <typename T, typename U> large_result test_minus_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()*= std::declval<U &>()) != 0, small_result>::type
test_multiply_equals(int);
template <typename T, typename U> large_result test_multiply_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()/= std::declval<U &>()) != 0, small_result>::type
test_divide_equals(int);
template <typename T, typename U> large_result test_divide_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()<<= std::declval<U &>()) != 0, small_result>::type
test_left_shift_equals(int);
template <typename T, typename U> large_result test_left_shift_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()>>= std::declval<U &>()) != 0, small_result>::type
test_right_shift_equals(int);
template <typename T, typename U> large_result test_right_shift_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()%= std::declval<U &>()) != 0, small_result>::type
test_mod_equals(int);
template <typename T, typename U> large_result test_mod_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()|= std::declval<U &>()) != 0, small_result>::type
test_bit_or_equals(int);
template <typename T, typename U> large_result test_bit_or_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()&= std::declval<U &>()) != 0, small_result>::type
test_bit_and_equals(int);
template <typename T, typename U> large_result test_bit_and_equals(...);

template <typename T, typename U>
typename std::enable_if<
    sizeof(std::declval<T &>()^= std::declval<U &>()) != 0, small_result>::type
test_bit_xor_equals(int);
template <typename T, typename U> large_result test_bit_xor_equals(...);

void test_compound_assignment() {
    std::cout << __FUNCTION__ << std::endl;

    using ST_plain= jss::strong_typedef<struct plain, int>;
    using ST_compound_add= jss::strong_typedef<
        struct compound_add, int, jss::strong_typedef_properties::addable>;
    using ST_compound_sub= jss::strong_typedef<
        struct compound_sub, int, jss::strong_typedef_properties::subtractable>;
    using ST_compound_both= jss::strong_typedef<
        struct compound_sub, int, jss::strong_typedef_properties::subtractable,
        jss::strong_typedef_properties::addable>;
    using ST_compound_self_mult= jss::strong_typedef<
        struct compound_self_mult, int,
        jss::strong_typedef_properties::self_multiplicable>;
    using ST_compound_other_mult= jss::strong_typedef<
        struct compound_other_mult, int,
        jss::strong_typedef_properties::mixed_multiplicable<int>>;
    using ST_compound_self_divide= jss::strong_typedef<
        struct compound_self_divide, int,
        jss::strong_typedef_properties::self_divisible>;
    using ST_compound_other_divide= jss::strong_typedef<
        struct compound_other_divide, int,
        jss::strong_typedef_properties::mixed_divisible<int>>;
    using ST_compound_ratio= jss::strong_typedef<
        struct compound_ratio, int, jss::strong_typedef_properties::ratio<int>>;
    using ST_compound_self_mod= jss::strong_typedef<
        struct compound_self_mod, int,
        jss::strong_typedef_properties::self_modulus>;
    using ST_compound_other_mod= jss::strong_typedef<
        struct compound_other_mod, int,
        jss::strong_typedef_properties::mixed_modulus<int>>;
    using ST_compound_self_xor= jss::strong_typedef<
        struct compound_self_xor, int,
        jss::strong_typedef_properties::self_bitwise_xor>;
    using ST_compound_other_xor= jss::strong_typedef<
        struct compound_other_xor, int,
        jss::strong_typedef_properties::mixed_bitwise_xor<int>>;
    using ST_compound_self_or= jss::strong_typedef<
        struct compound_self_or, int,
        jss::strong_typedef_properties::self_bitwise_or>;
    using ST_compound_other_or= jss::strong_typedef<
        struct compound_other_or, int,
        jss::strong_typedef_properties::mixed_bitwise_or<int>>;
    using ST_compound_self_and= jss::strong_typedef<
        struct compound_self_and, int,
        jss::strong_typedef_properties::self_bitwise_and>;
    using ST_compound_other_and= jss::strong_typedef<
        struct compound_other_and, int,
        jss::strong_typedef_properties::mixed_bitwise_and<int>>;

    static_assert(
        sizeof(test_plus_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_add, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_sub, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_both, int>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_plus_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_add, ST_compound_add>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_sub, ST_compound_sub>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_plus_equals<ST_compound_both, ST_compound_both>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_minus_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_add, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_sub, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_both, int>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_minus_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_add, ST_compound_add>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_sub, ST_compound_sub>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_minus_equals<ST_compound_both, ST_compound_both>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_multiply_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_plain, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_plain, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_plain, int>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_plain, int>(0)) == sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_plain, ST_plain>(0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_plain, ST_plain>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_mult, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(
            test_multiply_equals<ST_compound_self_mult, ST_compound_self_mult>(
                0)) == sizeof(small_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_self_mult, ST_compound_self_mult>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_self_mult, ST_compound_self_mult>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<
               ST_compound_self_mult, ST_compound_self_mult>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<
               ST_compound_self_mult, ST_compound_self_mult>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_mult, ST_compound_self_mult>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_and_equals<ST_compound_self_mult, ST_compound_self_mult>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_xor_equals<ST_compound_self_mult, ST_compound_self_mult>(
                0)) == sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_other_mult, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<
               ST_compound_other_mult, ST_compound_other_mult>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(
            test_divide_equals<ST_compound_other_mult, ST_compound_other_mult>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_other_mult, ST_compound_other_mult>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<
               ST_compound_other_mult, ST_compound_other_mult>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<
               ST_compound_other_mult, ST_compound_other_mult>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_or_equals<ST_compound_other_mult, ST_compound_other_mult>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_and_equals<ST_compound_other_mult, ST_compound_other_mult>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_xor_equals<ST_compound_other_mult, ST_compound_other_mult>(
                0)) == sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_divide, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(
            test_mod_equals<ST_compound_self_divide, ST_compound_self_divide>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<
               ST_compound_self_divide, ST_compound_self_divide>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_other_divide, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(
            test_mod_equals<ST_compound_other_divide, ST_compound_other_divide>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<
               ST_compound_other_divide, ST_compound_other_divide>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_ratio, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_ratio, ST_compound_ratio>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_ratio, ST_compound_ratio>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_ratio, ST_compound_ratio>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_mod, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(small_result));
    static_assert(
        sizeof(
            test_left_shift_equals<ST_compound_self_mod, ST_compound_self_mod>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_right_shift_equals<ST_compound_self_mod, ST_compound_self_mod>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_mod, ST_compound_self_mod>(
            0)) == sizeof(large_result));

    static_assert(
        sizeof(test_multiply_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(small_result));
    static_assert(
        sizeof(test_left_shift_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_other_mod, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(
            test_multiply_equals<ST_compound_other_mod, ST_compound_other_mod>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_divide_equals<ST_compound_other_mod, ST_compound_other_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_mod_equals<ST_compound_other_mod, ST_compound_other_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_left_shift_equals<
               ST_compound_other_mod, ST_compound_other_mod>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_right_shift_equals<
               ST_compound_other_mod, ST_compound_other_mod>(0)) ==
        sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_mod, ST_compound_other_mod>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_and_equals<ST_compound_other_mod, ST_compound_other_mod>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(
            test_bit_xor_equals<ST_compound_other_mod, ST_compound_other_mod>(
                0)) == sizeof(large_result));

    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_xor, ST_compound_self_xor>(
            0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_self_xor, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(
            test_bit_xor_equals<ST_compound_other_xor, ST_compound_other_xor>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_xor_equals<ST_compound_other_xor, int>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_and, ST_compound_self_and>(
            0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_self_and, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(
            test_bit_and_equals<ST_compound_other_and, ST_compound_other_and>(
                0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_and_equals<ST_compound_other_and, int>(0)) ==
        sizeof(small_result));

    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_or, ST_compound_self_or>(
            0)) == sizeof(small_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_self_or, int>(0)) ==
        sizeof(large_result));

    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_or, ST_compound_other_or>(
            0)) == sizeof(large_result));
    static_assert(
        sizeof(test_bit_or_equals<ST_compound_other_or, int>(0)) ==
        sizeof(small_result));

    ST_compound_add st1(42);
    st1+= 9;
    assert(st1.underlying_value() == 51);
    ST_compound_sub st2(42);
    st2-= 3;
    assert(st2.underlying_value() == 39);
    ST_compound_self_mult st3(6);
    ST_compound_self_mult st4(7);
    st3*= st4;
    assert(st3.underlying_value() == 42);

    ST_compound_other_mult st5(9);
    st5*= 6;
    assert(st5.underlying_value() == 54);
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
    test_by_default_strong_typedef_is_not_decrementable();
    test_strong_typedef_is_decrementable_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_addable();
    test_strong_typedef_is_addable_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_subtractable();
    test_strong_typedef_is_subtractable_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_ordered();
    test_strong_typedef_is_ordered_if_tagged_as_such();
    test_strong_typedef_is_mixed_ordered_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_hashable();
    test_strong_typedef_is_hashable_if_tagged_as_such();
    test_by_default_strong_typedef_is_not_streamable();
    test_strong_typedef_is_streamable_if_tagged_as_such();
    test_properties_can_be_combined();
    test_strong_typedef_is_default_constructible();
    test_can_support_difference_with_other_type();
    test_self_multiplication();
    test_mixed_multiplication();
    test_self_division();
    test_mixed_division();
    test_self_modulus();
    test_mixed_modulus();
    test_ratio();
    test_constexpr_comparison();
    test_constexpr_addition();
    test_constexpr_subtraction();
    test_bitwise_or();
    test_bitwise_and();
    test_bitwise_xor();
    test_bitwise_not();
    test_bitwise_left_shift();
    test_bitwise_right_shift();
    test_compound_assignment();
}
