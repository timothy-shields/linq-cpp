`interactive<void>` enters the monad

`interactive<Enumerable>` transforms within the monad and exits the monad

things like `select`, `where`, etc. work on actual types (no pointers or references) so template/inlining magic happens

can end a query expression with one of these:
- `std::unique_ptr<Enumerable> interactive<Enumerable>::capture_unique()`
- `std::shared_ptr<Enumerable> interactive<Enumerable>::capture_shared()`
