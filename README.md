# linq-cpp: LINQ for C++11 done right

## introduction

The `IEnumerable<T>` interface and associated LINQ extension methods provided by the .NET framework enable programmers to write concise, fluent, and composable query expressions using powerful abstractions.

**linq-cpp** brings equivalent functionality to the C++11 environment.

Readers familiar with .NET LINQ may want to skip directly to the [**methods**](README.md#methods) section.

## teaser

Suppose you have the following types.

    enum class Genders
        Male
        Female
    
    class Employee
        int ID() const
        const std::string& FirstName() const
        const std::string& LastName() const
        int Age() const
        Genders Gender() const
        
    class Customer
        int ID() const
    
    class Department
        const std::vector<Employee*>& Employees() const
        const std::vector<Customer*>& Customers() const

You're given a `std::vector<Department*>` called `departments` and the following task.

- Get the names and employee IDs of employees younger than 21 who work in departments servicing the customer with the given ID.
- The results should be grouped by age and gender, and within each group the employees should be sorted by last name then first name.

**linq-cpp** makes this a straightfoward task.
    
    vector<Department*> departments = ...;
    int customerID = ...;
    
    auto results =
        Enumerable::FromRange(departments)
        .Where([=](Department* d)
        {
            return Enumerable::FromRange(d->Customers()).Any([=](Customer* c){ return c->ID == customerID; });
        })
        .SelectMany([](Department* d){ return Enumerable::FromRange(d->Employees()); })
        .Where([](Employee* e){ return e->Age() < 21; })
        .GroupBy([](Employee* e){ return make_tuple(e->Age(), e->Gender()); })
        .Select([](pair<tuple<int, Genders>, TEnumerable<Employee*>> group)
        {
            return std::make_pair(
                group.first,
                group.second
                    .OrderBy([](Employee* e){ return make_tuple(e->LastName(), e->FirstName()); })
                    .Select([](Employee* e){ return make_tuple(e->FirstName(), e->LastName(), e->ID()); })
                    .ToVector());
        })
        .ToVector();

## definitions

### `TEnumerable<T>`
An enumerable of type `T` is an ordered sequence of zero or more values of type `T`.

### `TEnumerator<T>`
An enumerator of type `T` is the state of a traversal through a `TEnumerable<T>`.

The single operation provided by `TEnumerable<T>` is `GetEnumerator`. It returns a new `TEnumerator<T>` traversal of the `TEnumerable<T>`, starting "one before" the beginning of the sequence.

The two operations provided by `TEnumerator<T>` are `MoveNext` and `Current`. The `MoveNext` operation moves the `TEnumerator<T>` to the next value in the sequence. It returns `true` if the move to the next value was successful and `false` otherwise. `Current` returns the value `T` that the `TEnumerator<T>` is currently pointing to.

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

To make these concepts more concrete, consider the following example. Suppose `L` is a `TEnumerable<char>` representing the two-value sequence `['A', 'B']`. Then the following operations return as commented.

    std::shared_ptr<TEnumerator<char>> E = L.GetEnumerator();
    E->MoveNext(); // returns true
    E->Current();  // returns 'A'
    E->MoveNext(); // returns true
    E->Current();  // returns 'B'
    E->MoveNext(); // returns false

## classes

    class TEnumerable<T>
        std::shared_ptr<TEnumerator<T>> GetEnumerator()
        
    class TEnumerator<T>
        bool MoveNext()
        T Current()

## methods

### `Enumerable` static methods

- `TEnumerable<T> FromRange(TRange& range)`
- `TEnumerable<T> FromRange(std::shared_ptr<TRange> range)`
 - Constructs an enumerable from an STL range
- `TEnumerable<T> Factory(TFactory factory)`
 - `TFactory = TEnumerable<T>()`
 - Repeated calls to `factory` argument each create an enumerable that can only be enumerated once
 - Result is enumerable that can be enumerated as many times as desired
- `TEnumerable<T> Repeat(T x)`
 - Represents an infinite sequence of which every element is `x`
- `TEnumerable<T> Empty()`
 - Represents an empty sequence
- `TEnumerable<T> Return(T x)`
 - Represents a sequence that contains a single element `x`
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
- `bool All(TPredicate predicate)`
 - Determines whether all elements of a sequence satisfy a condition
- `T First()`
- `T First(TPredicate predicate)`
- `T Last()`
- `T Last(TPredicate predicate)`
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
- `std::string ToString(std::string separator, TWriter writer)`
- `std::string ToString(std::string separator)`
- `std::string ToString()`
 - `TWriter = void(std::stringstream&, T)`
