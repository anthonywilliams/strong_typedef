# `jss::strong_typedef <Tag, ValueType, Properties...>`

[![Build Status](https://travis-ci.com/anthonywilliams/strong_typedef.svg?branch=master)](https://travis-ci.com/anthonywilliams/strong_typedef)

This is an implementation of a class template that provides a wrapper type that is convertible to
and from the underlying type, but is distinct from it.

The purpose of this is primarily to enable things like indexes and IDs to be unique types. This is
achieved by specifying a `Tag` type for each `jss::strong_typedef` use, alongside the `ValueType`
which is being wrapped. This `Tag` type can be an incomplete type, and can be forward-declared in
the template parameter directly. For example:

~~~cplusplus
#include "strong_typedef.hpp"

using FirstIndex=jss::strong_typedef<struct FirstTag,int>;
using SecondIndex=jss::strong_typedef<struct SecondTag,int>;
~~~

`FirstIndex` and `SecondIndex` are both explicitly convertible to/from `int`, but they are separate
types and are thus not inter-convertible. They also have no additional operations beyond retrieving
the underlying value, as no properties are specified.

The third template parameter (`Properties...`) is an optional parameter pack that specifies the
operations you wish the strong typedef to support. By default it is empty, so no operations are
supported. See [below](#properties) for a full list.

### Declaring Types

You create a typedef by specifying these parameters:

~~~cplusplus
using type1=jss::strong_typedef<struct type1_tag,int>;
using type2=jss::strong_typedef<struct type2_tag,int>;
using type3=jss::strong_typedef<struct type3_tag,std::string,
    jss::strong_typedef_properties::comparable>;
~~~

`type1`, `type2` and `type3` are now separate types. They cannot be implicitly converted
to or from each other or anything else.

### Creating Values

If the underlying type is default-constructible, then so is the new type. You
can also construct the objects from an object of the wrapped type:

~~~cplusplus
type1 t1;
type2 t2(42);
// type2 e2(t1); // error, type1 cannot be converted to type2
~~~

### Accessing the Value

`strong_typedef` can wrap built-in or class type, but that's only useful if you
can access the value. There are two ways to access the value:

* Cast to the stored type: `static_cast<unsigned>(my_channel_index)`
* Use the `underlying_value` member function: `my_channel_index.underlying_value()`

Using the `underlying_value` member function returns a reference to the stored
value, which can thus be used to modify non-`const` values, or to call member
functions on the stored value without taking a copy. This makes it particularly
useful for class types such as `std::string`.

~~~cplusplus
using transaction_id=jss::strong_typedef<struct transaction_tag,std::string>;

bool is_a_foo(transaction_id id){
    auto &s= id.underlying_value();
    return s.find("foo") != s.end();
}
~~~

### Other Operations

Depending on the [properties](#properties) you've assigned to your type you may
be able to do other operations on that type, such as compare `a == b` or 
`a < b`, increment with `++a`, or add two values with `a + b`. You might also be
able to hash the values with `std::hash<my_typedef>`, or write them to a
`std::ostream` with `os << a`. Only the behaviours enabled by the `Properties`
template parameter will be available on any given type. For anything else, you
need to extract the wrapped value and use that.

## Examples

### IDs

An ID of some description might essentially be a number, but it makes no sense
to perform much in the way of operations on it. You probably want to be able to
compare IDs, possibly with an ordering so you can use them as keys in a
`std::map`, or with hashing so you can use them as keys in `std::unordered_map`,
and maybe you want to be able to write them to a stream. Such an ID type might
be declared as follows:

~~~cplusplus
using widget_id=jss::strong_typedef<struct widget_id_tag,unsigned long long,
    jss::strong_typedef_properties::comparable,
    jss::strong_typedef_properties::hashable,
    jss::strong_typedef_properties::streamable>;

using froob_id=jss::strong_typedef<struct froob_id_tag,unsigned long long,
    jss::strong_typedef_properties::comparable,
    jss::strong_typedef_properties::hashable,
    jss::strong_typedef_properties::streamable>;
~~~

Note that `froob_id` and `widget_id` are now different types due to the
different tags used, even though they are both based on `unsigned long
long`. Therefore any attempt to use a `widget_id` as a `froob_id` or vice-versa
will lead to a compiler error. It also means you can overload on them:

~~~cplusplus
void do_stuff(widget_id my_widget);
void do_stuff(froob_id my_froob);

widget_id some_widget(421982);
do_stuff(some_widget);
~~~

Alternatively, an ID might be a string, such as a purchase order number of
transaction ID:

~~~cplusplus
using transaction_id=jss::strong_typedef<struct transaction_id_tag,std::string,
    jss::strong_typedef_properties::comparable,
    jss::strong_typedef_properties::hashable,
    jss::strong_typedef_properties::streamable>;
    
transaction_id some_transaction("GBA283-HT9X");
~~~

That works too, since `strong_typedef` can wrap any built-in or class type.

### Indexes

Suppose you have a device that supports a number of channels, so you want to be
able to retrieve the data for a given channel. Each channel yields a number of
data items, so you also want to access the data items by index. You could use
`strong_typedef` to wrap the channel index and the data item index, so they
can't be confused. You can also make the index types `incrementable` and
`decrementable` so they can be used in a `for` loop:

~~~cplusplus
using channel_index=jss::strong_typedef<struct channel_index_tag,unsigned,
    jss::strong_typedef_properties::comparable,
    jss::strong_typedef_properties::incrementable,
    jss::strong_typedef_properties::decrementable>;
    
using data_index=jss::strong_typedef<struct data_index_tag,unsigned,
    jss::strong_typedef_properties::comparable,
    jss::strong_typedef_properties::incrementable,
    jss::strong_typedef_properties::decrementable>;
    
Data get_data_item(channel_index channel,data_index item);
data_index get_num_items(channel_index channel);
void process_data(Data data);

void foo(){
    channel_index const num_channels(99);
    for(channel_index channel(0); channel < num_channels; ++channel) {
        data_index const num_data_items(get_num_items(channel));
        for(data_index item(0); item < num_data_items; ++item) {
            process_data(get_data_item(channel, item));
        }
    }
}
~~~

The compiler will complain if you pass the wrong parameters, or compare the
`channel` against the `item`.

## <a name="properties"></a>Behaviour Properties

The third template parameter (`Properties...`) specifies behavioural properties for the new type. It
can be any combination of the types in the `jss::strong_typedef_properties` namespace,
(e.g. `jss::strong_typedef_properties::hashable, jss::strong_typedef_properties::streamable,
jss::strong_typedef_properties::comparable`). Each property adds some behaviour. The available
properties are:

* `jss::strong_typedef_properties::equality_comparable` => Can be compared for equality (`st==st2`)
  and inequality (`st!=st2`)
* `jss::strong_typedef_properties::pre_incrementable` => Supports preincrement (`++st`)
* `jss::strong_typedef_properties::post_incrementable` => Supports postincrement (`st++`)
* `jss::strong_typedef_properties::pre_decrementable` => Supports predecrement (`--st`)
* `jss::strong_typedef_properties::post_decrementable` => Supports postdecrement (`st--`)
* `jss::strong_typedef_properties::self_addable` => Supports addition of two objects of the strong
  typedef (`st+st2`) where the result is convertible to the underlying type. The result is a new
  instance of the strong typedef.
* `jss::strong_typedef_properties::mixed_addable<T>` => Supports addition of an object of the strong
  typedef with another object of type `T` either way round (`st+value` or `value+st`) where the
  result is convertible to the underlying type. The result is a new instance of the strong typedef.
* `jss::strong_typedef_properties::addable` => Combines `self_addable` and
  `mixed_addable<ValueType>`
* `jss::strong_typedef_properties::generic_mixed_addable` => Supports addition of an object of the
  strong typedef with another object of any type either way round (`st+value` or `value+st`) where
  the result is convertible to the underlying type. The result is a new instance of the strong
  typedef.
* `jss::strong_typedef_properties::self_subtractable` => Supports subtraction of two objects of the
  strong typedef (`st-st2`) where the result is convertible to the underlying type. The result is a
  new instance of the strong typedef.
* `jss::strong_typedef_properties::mixed_subtractable<T>` => Supports subtraction of an object of
  the strong typedef with another object of type `T` either way round (`st-value` or `value-st`)
  where the result is convertible to the underlying type. The result is a new instance of the strong
  typedef.
* `jss::strong_typedef_properties::generic_mixed_subtractable` => Supports subtraction of an object
  of the strong typedef with another object of any type either way round (`st-value` or `value-st`)
  where the result is convertible to the underlying type. The result is a new instance of the strong
  typedef.
* `jss::strong_typedef_properties::subtractable` => Combines `self_subtractable` and
  `mixed_subtractable<ValueType>`
* `jss::strong_typedef_properties::difference<T>` => Supports subtraction of two objects of the
  strong typedef (`st-st2`) where the result is convertible to `T`. The result is an object of type `T`.
* `jss::strong_typedef_properties::self_multiplicable` => Supports multiplication of two objects of
  the strong typedef (`st*st2`) where the result is convertible to the underlying type. The result
  is a new instance of the strong typedef.
* `jss::strong_typedef_properties::mixed_multiplicable<T>` => Supports multiplication of an object
  of the strong typedef with another object of type `T` either way round (`st*value` or `value*st`)
  where the result is convertible to the underlying type. The result is a new instance of the strong
  typedef.
* `jss::strong_typedef_properties::multiplicable` => Combines `self_multiplicable` and
  `mixed_multiplicable<ValueType>`
* `jss::strong_typedef_properties::self_divisible` => Supports division of two objects of the strong
  typedef (`st/st2`) where the result is convertible to the underlying type. The result is a new
  instance of the strong typedef.
* `jss::strong_typedef_properties::mixed_divisible<T>` => Supports division of an object of the
  strong typedef with another object of type `T` either way round (`st/value` or `value/st`) where
  the result is convertible to the underlying type. The result is a new instance of the strong
  typedef.
* `jss::strong_typedef_properties::divisible` => Combines `self_divisible` and
  `mixed_divisible<ValueType>`
* `jss::strong_typedef_properties::ratio<T>` => Supports division of two objects of the strong
  typedef (`st/st2`) where the result is convertible to `T`. The result is an object of type `T`.
* `jss::strong_typedef_properties::ordered` => Supports ordering comparisons (`st<st2`, `st>st2`,
  `st<=st2`, `st>=st2`)
* `jss::strong_typedef_properties::mixed_ordered<T>` => Supports ordering comparisons where only one
  of the values is a strong typedef and the other is `T`
* `jss::strong_typedef_properties::hashable` => Supports hashing with `std::hash`
* `jss::strong_typedef_properties::streamable` => Can be written to a `std::ostream` with
  `operator<<`
* `jss::strong_typedef_properties::incrementable` => Combines
  `jss::strong_typedef_properties::pre_incrementable` and
  `jss::strong_typedef_properties::post_incrementable`
* `jss::strong_typedef_properties::decrementable` => Combines
  `jss::strong_typedef_properties::pre_decrementable` and
  `jss::strong_typedef_properties::post_decrementable`
* `jss::strong_typedef_properties::comparable` => Combines `jss::strong_typedef_properties::ordered`
  and `jss::strong_typedef_properties::equality_comparable`

## Writing new properties

You can write a new property by creating a class with the following structure:

~~~cplusplus
struct my_property{
    template <typename Derived, typename ValueType> struct mixin {};
};
~~~

Each strong typedef that uses the new property (e.g. `jss::strong_typedef<struct
MyTag,int,my_property`) is a class derived from an instantiation of the `mixin` template. The
`Derived` parameter is the type of the strong typedef class itself, and `ValueType` is the
corresponding `ValueType` parameter.

The mixin can therefore add members and/or friends, and can be used as a marker for
`enable_if`-based overload resolution.

## License

This code is released under the [Boost Software License](https://www.boost.org/LICENSE_1_0.txt):

> Boost Software License - Version 1.0 - August 17th, 2003
>
> Permission is hereby granted, free of charge, to any person or organization
> obtaining a copy of the software and accompanying documentation covered by
> this license (the "Software") to use, reproduce, display, distribute,
> execute, and transmit the Software, and to prepare derivative works of the
> Software, and to permit third-parties to whom the Software is furnished to
> do so, all subject to the following:
>
> The copyright notices in the Software and this entire statement, including
> the above license grant, this restriction and the following disclaimer,
> must be included in all copies of the Software, in whole or in part, and
> all derivative works of the Software, unless such copies or derivative
> works are solely in the form of machine-executable object code generated by
> a source language processor.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
> SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
> FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
> ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
> DEALINGS IN THE SOFTWARE.
