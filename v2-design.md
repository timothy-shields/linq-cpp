`interactive<void>` enters the monad

`interactive<Enumerable>` transforms within the monad and exits the monad

things like `select`, `where`, etc. work on actual types (no pointers or references) so template/inlining magic happens

can end a query expression with one of these:
- `std::unique_ptr<Enumerable> interactive<Enumerable>::capture_unique()`
- `std::shared_ptr<Enumerable> interactive<Enumerable>::capture_shared()`

can continue a query expression with:
- `interactive<Enumerable> interactive<Enumerable>::make(std::unique_ptr<Enumerable>&& e)`
- `interactive<Enumerable> interactive<Enumerable>::make(std::shared_ptr<Enumerable>&& e)`
- `interactive<Enumerable> interactive<Enumerable>::make(std::shared_ptr<Enumerable> e)`

make a function `make_interactive` that has analagous signatures so that `Enumerable` can be inferred

should be able to do something like this:

auto a = interactive<void>::range(0, 10)
    .select([](int n){ return 3 * n; })
    .capture_unique();
    
auto b = make_interactive(a)
    .where([](int n){ return (n % 2) == 0; })
    .to_vector();
    
