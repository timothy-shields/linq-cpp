linq-cpp
========

LINQ for C++11 done right

introduction
------------

The `IEnumerable<T>` interface and associated LINQ extension methods provided by the .NET framework enable .NET programmers to write concise, fluent, and composable query expressions. For anyone familiar with these tools, their incredible utility shouldn't need explaining. linq-cpp brings equivalent functionality to the C++11 environment.

If you are a programmer not familiar with .NET's `IEnumerable<T>` and LINQ, the rest of this section aims to quickly give you a basic understanding of what this library provides. (TODO) 

classes
=======
    class TEnumerable<T>
        std::shared_ptr<TEnumerator<T>> GetEnumerator()
        
    class TEnumerator<T>
        bool MoveNext()
        T Current()

methods
=======
`static Enumerable class`
------------
- `TEnumerable<T> FromRange(TRange& range)`
 - Same as `FromRange(range.begin(), range.end())`
- `TEnumerable<T> FromRange(TIter begin, TIter end)`
 - Construct an `Enumerable` from stl iterators
- `TEnumerable<T> Factory(const TFactory& factory)`
 - `TFactory = TEnumerable<T>()`
 - Repeated calls to `factory` argument each create an enumerable that can only be enumerated once
 - Result is Enumerable that can be enumerated as many times as desired
