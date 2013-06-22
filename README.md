[License]: LICENSE.md
[v1]: https://github.com/timothy-shields/linq-cpp/tree/v1

[.NET LINQ]: http://msdn.microsoft.com/en-us/library/bb308959.aspx
[`IEnumerable<T>`]: http://msdn.microsoft.com/en-us/library/9eekhta0.aspx
[`IEnumerator<T>`]: http://msdn.microsoft.com/en-us/library/78dfe2yb.aspx
[Travis CI]: https://travis-ci.org/timothy-shields/linq-cpp
[Build Status]: https://api.travis-ci.org/timothy-shields/linq-cpp.png?branch=master
[.travis.yml]: .travis.yml

[CMake]: http://www.cmake.org/

# linq-cpp: LINQ for C++11 done right

If you're interested in using or contributing to this library, or if you just have questions, **I'd really like to hear from you!**<br>
Contact me at timothy.shields@live.com.

## introduction

The [`IEnumerable<T>`][] interface and associated LINQ extension methods provided by the .NET framework enable programmers to write concise, fluent, and composable query expressions using powerful abstractions.

**linq-cpp** brings equivalent functionality to the C++11 environment - and in a way that's **"done right."**

### familiarity

Users of [.NET LINQ][] and the [`IEnumerable<T>`][] interface should find **linq-cpp** immediately familiar. The precedent set by the .NET [`IEnumerable<T>`][] and [`IEnumerator<T>`][] interfaces and [`IEnumerable<T>`][] extension methods have been mimicked directly.
 
The **[methods][]** section has a list of all of the functions available in **linq-cpp**. Any .NET LINQ methods that aren't yet available should be added soon!

### portability
[portability]: README.md#portability

**linq-cpp** is written to be cross-platform and fully compliant with the C++11 standard. It **has no dependencies** outside of the C++11 standard library. It can be compiled and built using any of the following:

- Visual Studio 2012
- gcc 4.6
- clang 3.1

### build system: [CMake][]

**linq-cpp** is configured to build using the [CMake][] build system. For other projects already using [CMake][], integrating **linq-cpp** as a dependency is a simple procedure.

### continuous integration: [Travis CI][] ![Build Status][]

**linq-cpp** is built automatically by [Travis CI][]. Each time this GitHub repository updates, [Travis CI][] clones and builds the repository using the Linux compilers it provides (see the **[portability][]** section for the list of compilers). The Windows compilers (namely Visual Studio 2012) have to be run manually outside of Travis.

*(coming soon)* Automated testing using Travis CI

The Travis configuration file is [.travis.yml][].

### licensing

**linq-cpp** is licensed under the Apache License, Version 2.0. You can view the license file [here][License].

## [concepts][]

[Concepts]: http://en.cppreference.com/w/cpp/concept
[DefaultConstructible]: http://en.cppreference.com/w/cpp/concept/DefaultConstructible
[MoveConstructible]: http://en.cppreference.com/w/cpp/concept/MoveConstructible
[MoveAssignable]: http://en.cppreference.com/w/cpp/concept/MoveAssignable

The **linq-cpp** library is built on top of two foundation [concepts][], `Enumerable<T>` and `Enumerator<T>`, mimicking the [`IEnumerable<T>`][] and [`IEnumerator<T>`][] interfaces from .NET, respectively. The definitions of these two concepts follow.

### `Enumerable<T>`

A type `Type` meets the requirements of `Enumerable<T>` if it meets all of the following requirements:

- `Type` meets the [MoveConstructible][] requirements
- `Type` meets the [MoveAssignable][] requirements
- `Type::enumerator_type` is a member type that meets the `Enumerator<T>` requirements
- `Type::value_type` is a member type, where `value_type` is `T`
- `enumerator_type Type::get_enumerator()` is a member function

### `Enumerator<T>`

A type `Type` meets the requirements of `Enumerator<T>` if it meets all of the following requirements:

- `Type` meets the [DefaultConstructible][] requirements
- `Type` meets the [MoveConstructible][] requirements
- `Type` meets the [MoveAssignable][] requirements
- `Type::value_type` is a member type, where `value_type` is `T`
- `bool Type::move_first()` is a member function
- `bool Type::move_next()` is a member function
- `value_type Type::current()` is a member function

---

***Below this point has not yet been updated from v1***

---

## teaser

Suppose you have the following types.

    enum class Genders
        Male
        Female
    
    class Employee
        const std::string& FirstName() const
        const std::string& LastName() const
        int Age() const
        Genders Gender() const
        
    class Customer
        int ID() const
    
    class Department
        const std::vector<Employee*>& Employees() const
        const std::vector<Customer*>& Customers() const

You're given `vector<Department*> departments`, `int customerID`, and the following task.

- Get the employees younger than 21 who work in departments servicing the customer with the given ID.
- The results should be grouped by age and gender, and within each group the employee data should be sorted by last name then first name.
- An employee may work in multiple departments, but the results shouldn't contain any employee more than once.

**linq-cpp** makes this complex task straightforward.
    
    vector<Department*> departments = ...;
    int customerID = ...;
    
	// Could use "auto" to have the compiler determine the query's return type
	// but an explicit breakdown makes it clearer.
	typedef tuple<int, Genders> EmployeeGroupKey;
	typedef pair<EmployeeGroupKey, vector<Employee*>> EmployeeGroup;
    vector<EmployeeGroup> results =
        Enumerable::FromRange(departments)
        .Where([=](Department* d)
        {
            return Enumerable::FromRange(d->Customers()).Any([=](Customer* c){ return c->ID == customerID; });
        })
        .SelectMany([](Department* d){ return Enumerable::FromRange(d->Employees()); })
        .Where([](Employee* e){ return e->Age() < 21; })
        .Distinct()
        .GroupBy([](Employee* e){ return make_tuple(e->Age(), e->Gender()); })
        .Select([](pair<tuple<int, Genders>, TEnumerable<Employee*>> group)
        {
            return std::make_pair(
                group.first,
                group.second
                    .OrderBy([](Employee* e){ return make_tuple(e->LastName(), e->FirstName()); })
                    .ToVector());
        })
        .ToVector();

## definitions

### classes

    class TEnumerable<T>
        std::shared_ptr<TEnumerator<T>> GetEnumerator()
        
    class TEnumerator<T>
        bool MoveNext()
        T Current()

### `TEnumerable<T>`

- A sequence of zero or more values of type `T`

### `TEnumerator<T>`

- The state of a traversal through a `TEnumerable<T>`

### `TEnumerable<T>::GetEnumerator`

- Returns a new `TEnumerator<T>` traversal of the `TEnumerable<T>`
- Starts "one before" the beginning of the sequence

### `TEnumerator<T>::MoveNext`

- Moves the `TEnumerator<T>` to the next value in the sequence
- Returns `true` if the move to the next value was successful and `false` otherwise

### `TEnumerator<T>::Current`

- Returns the value `T` that the `TEnumerator<T>` is currently pointing to

### analogues in STL

It may help initially to associate these new concepts with familiar analogues in the standard template library.

<table>
  <tr>
    <th>new concept</th>
    <th>familiar analogue</th>
  </tr>
  <tr>
    <td><code>TEnumerable&lt;T&gt; L;</code></td>
    <td><code>std::vector&lt;T&gt; V;</code></td>
  </tr>
  <tr>
    <td><code>std::shared_ptr&lt;TEnumerator&lt;T&gt;&gt; E = L.GetEnumerator();</code></td>
    <td><code>std::vector&lt;T&gt;::iterator I = V.begin();</code></td>
  </tr>
  <tr>
    <td><code>bool success = E->MoveNext();</code></td>
    <td><code>bool success = (++I != V.end());</code></td>
  </tr>
  <tr>
    <td><code>T value = E->Current();</code></td>
    <td><code>T value = *I;</code></td>
  </tr>
</table>

### example

To make these concepts more concrete, consider the following example. Suppose `L` is a `TEnumerable<char>` representing the two-value sequence `['A', 'B']`. Then the following operations return as commented.

    std::shared_ptr<TEnumerator<char>> E = L.GetEnumerator();
    E->MoveNext(); // returns true
    E->Current();  // returns 'A'
    E->MoveNext(); // returns true
    E->Current();  // returns 'B'
    E->MoveNext(); // returns false

## methods
[methods]: README.md#methods

### `Enumerable` static methods

- `TEnumerable<T> FromRange(TRange& range)`
- `TEnumerable<T> FromRange(std::shared_ptr<TRange> range)`
 - Constructs an enumerable from an STL range
- `TEnumerable<T> Factory(TFactory factory)`
 - `TFactory = TEnumerable<T>()`
 - Repeated calls to `factory` argument each create an enumerable that can only be enumerated once
 - Result is enumerable that can be enumerated as many times as desired
- `TEnumerable<T> Repeat(T item)`
 - Represents an infinite sequence of which every element is `item`
- `TEnumerable<T> Empty()`
 - Represents an empty sequence
- `TEnumerable<T> Return(T item)`
 - Represents a sequence that contains a single element `item`
- `TEnumerable<T> Generate(TFactory factory)`
 - Repeated calls to factory generate elements of sequence
- `TEnumerable<T> Sequence(T start, TPredicate predicate, TNext next)`
- `TEnumerable<T> Sequence(T start, TNext next)`
- `TEnumerable<T> Sequence(T start)`
- `TEnumerable<T> Sequence()`
 - `TPredicate = bool()`
 - `TNext = T(T)`
 - The sequence generated by a typical `for` loop
- `TEnumerable<T> Range(T start, T count)`
 - The sequence `start, start+1, ..., start+count`
- `TEnumerable<T> Concat(TEnumerable<T> first, TEnumerable<T> second)`
 - Concatenates two sequences
- `TEnumerable<TResult> Zip(TEnumerable<T1> first, TEnumerable<T2> second, TSelector selector)`
 - `TSelector = TResult(T1, T2)`
 - Merges two sequences by using the specified `selector` function

### `TEnumerable<T>` instance methods

- `TEnumerable<TResult> Select(TSelector selector)`
 - `TSelector = TResult(T)`
 - Projects each element of a sequence into a new form
- `TEnumerable<TResult> StaticCast()`
 - Static casts the elements of a sequence to the specified type
- `TEnumerable<TResult> DynamicCast()`
 - Dynamic casts the elements of a sequence to the specified type
- `TEnumerable<TResult> SelectMany(TSelector selector)`
 - `TSelector = TEnumerable<TResult>(T)`
 - Projects each element of a sequence to a new sequence and flattens the resulting sequences into one sequence
- `T SelectMany()`
 - Only available when `T = TEnumerable<S>`
 - Concatenates a sequence of sequences
- `TEnumerable<T> Where(TPredicate predicate)`
 - `TPredicate = bool(T)`
 - Filters a sequence of values based on a predicate
- `TEnumerable<std::pair<T, int>> Index()`
 - Projects each element `x` of a sequence to the pair `(x, i)` where `i` is the index of `x`
- `TEnumerable<TResult> SelectIndexed(TSelector selector)`
 - `TSelector = TResult(T, int)`
 - Projects each element of a sequence into a new form by incorporating the element's index
- `TEnumerable<T> WhereIndexed(TPredicate predicate)`
 - `TPredicate = bool(T, int)`
 - Filters a sequence of values based on a predicate incorporating the element's index
- `TEnumerable<T> Skip(int count)`
 - Bypasses a specified number of elements in a sequence and then returns the remaining elements
- `TEnumerable<T> SkipWhile(TPredicate predicate)`
 - `TPredicate = bool(T)`
 - Bypasses elements in a sequence as long as a specified condition is true and then returns the remaining elements
- `TEnumerable<T> SkipWhileIndexed(TPredicate predicate)`
 - TODO
- `TEnumerable<T> Take(int count)`
 - Returns a specified number of contiguous elements from the start of a sequence
- `TEnumerable<T> TakeWhile(TPredicate predicate)`
 - `TPredicate = bool(T, int)`
 - Returns elements from a sequence as long as a specified condition is true, and then skips the remaining elements
- `TEnumerable<T> TakeWhileIndexed(TPredicate predicate)`
 - TODO
- `TEnumerable<T> ToInclusive(T end)`
 - Take elements while they are less than or equal to end
- `TEnumerable<T> ToExclusive(T end)`
 - Take elements while they are less than end
- `TEnumerable<T> Do(TAction action)`
 - `TAction = void(T)`
 - Inject a side effect of enumerating
- `TEnumerable<T> DoIndexed(TAction action)`
 - `TAction = void(T,int)`
 - Inject a side effect of enumerating
- `TEnumerable<T> Order(TComparer comparer)`
 - `TComparer = int(T, T)`
 - Sorts the elements of a sequence in ascending order using the given comparer
- `TEnumerable<T> Order()`
 - Sorts the elements of a sequence in ascending order using the default comparer
- `TEnumerable<T> OrderBy(TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
 - Sorts the elements of a sequence in ascending order with respect to selected keys
- `TEnumerable<std::pair<TKey, TEnumerable<T>> GroupBy(TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
 - Groups the elements of a sequence by selected keys
 - The groups are returned in ascending order with respect to selected keys
- `bool Any()`
 - Determines whether a sequence contains any elements
- `bool Any(TPredicate predicate)`
 - Determines whether any element of a sequence satisfies a condition
- `bool Contains(T item)`
- `bool All(TPredicate predicate)`
 - Determines whether all elements of a sequence satisfy a condition
- `T First()`
- `T First(TPredicate predicate)`
- `T Last()`
- `T Last(TPredicate predicate)`
- `T ElementAt(int i)`
- `T Single()`
- `T Single(TPredicate predicate)`
- `int Count()`
- `int Count(TPredicate predicate)`
- `TAccumulate Aggregate(TAccumulate seed, TAccumulator accumulator)`
 - `TAccumulator = TAccumulate(TAccumulate, T)`
- `T Aggregate(TAccumulator accumulator)`
 - `TAccumulator = T(T, T)`
- `T Sum()`
- `T Product()`
- `double Average()`
- `TResult Min(TSelector selector)`
- `T Min()`
 - `TSelector = TResult(T)`
- `TResult Max(TSelector selector)`
- `T Max()`
 - `TSelector = TResult(T)`
- `T MinBy(TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
- `T MaxBy(TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
- `int MinIndex(TKeySelector keySelector)`
- `int MinIndex()`
 - `TKeySelector = TKey(T)`
- `int MaxIndex(TKeySelector keySelector)`
- `int MaxIndex()`
 - `TKeySelector = TKey(T)`
- `void ForEach(TAction action)`
 - `TAction = void(T)`
- `void ForEachIndexed(TAction action)`
 - `TAction = void(T, int)`
- `void IntoVector(std::vector<T>& _vector)`
- `std::vector<T> ToVector()`
- `void IntoSet(std::set<T>& _set)`
- `std::set<T> ToSet()`
- `void IntoMap(std::map<TKey, TValue>& _map, TKeySelector keySelector, TValueSelector valueSelector)`
- `void IntoMap(std::map<TKey, T>& _map, TKeySelector keySelector)`
- `void IntoMap(std::map<TKey, TValue>& _map)`
 - `TKeySelector = TKey(T)`
 - `TValueSelector = TValue(T)`
- `std::map<TKey, TValue> ToMap(TKeySelector keySelector, TValueSelector valueSelector)`
- `std::map<TKey, T> ToMap(TKeySelector keySelector)`
- `std::map<TKey, TValue> ToMap()`
 - `TKeySelector = TKey(T)`
 - `TValueSelector = TValue(T)`
- `void IntoLookup(std::map<TKey, std::shared_ptr<std::vector<T>>>& _map, TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
- `std::map<TKey, std::shared_ptr<std::vector<T>>> ToLookup(TKeySelector keySelector)`
 - `TKeySelector = TKey(T)`
- `std::string ToString(std::string separator, TWriter writer)`
- `std::string ToString(std::string separator)`
- `std::string ToString()`
 - `TWriter = void(std::stringstream&, T)`
