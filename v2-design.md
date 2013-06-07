Try to implement everything like this:

http://community.bartdesmet.net/blogs/bart/archive/2010/01/01/the-essence-of-linq-minlinq.aspx


`ix` enters the monad

`interactive<Enumerable>` transforms within the monad and exits the monad

things like `select`, `where`, etc. work on actual types (no pointers or references) so template/inlining magic happens

can end a query expression with:
- `std::shared_ptr<Enumerable> interactive<Enumerable>::ref_count()`
- `std::shared_ptr<captured_enumerable<Enumerable::value_type>> interactive<Enumerable>::capture()`

can continue a query expression with:
- `interactive<captured_enumerable<T>> ix::capture(std::shared_ptr<captured_enumerable<T>>)`

should be able to do something like this:

    auto a = ix::range(0, 10)
        .select([](int n){ return 3 * n; })
        .capture();
    
    auto b = ix::capture(a)
        .where([](int n){ return (n % 2) == 0; })
        .to_vector();
    
and have it behave identically to this:

    auto b = ix::range(0, 10)
        .select([](int n){ return 3 * n; })
        .where([](int n){ return (n % 2) == 0; })
        .to_vector();
