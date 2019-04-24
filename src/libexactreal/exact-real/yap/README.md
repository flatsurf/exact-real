Expression Magic powered by Boost's Yap
=======================================

We try to give a convenient interface to Arb & our types by using Yap.
Basically, this creates quite complex recursive types that describe
expressions, e.g., `(x + y)*z` would be conceptually something like an
`ArbExpr<multiplies, ArbExpr<plus, ArbExpr<Terminal>, ArbExpr<Terminal>, ArbExpr<Terminal>>>`. 
These types typically only exist at compile time and get optimized away by the
compiler to create standard Arb calls in the above example.

We mostly do this so we can pass in the parameters that Arb expects (precision)
in a syntactically nice way. The above expression for example can not be
assigned to an `Arb t` because we detect at compile time that no precision has
been defined. However, something like `Arb t = ((x+y)*z)(64)` would use 64 as
the precision in both computations to obtain a result that can be assigned to
an actual Arb instance.

This approach also allows us to optimize expressions (mostly at compile time.)
This has only been done to varying degree but in principle we could rewrite
expressions equivalently at compile time to reduce the number of temporaries
used: `x = (y + x)(64)` can be rewritten to `arb_add(x, x, y, 64)` without
needing any temporaries at all.

Since all of this relies heavily on C++ template machinery, the relevant code
all has to live in header files and we do not get the compilation firewall that
we got elsewhere in this library.

Finally, note that all of this is opt-in. If you don't want to use any of this,
only inclode the `exact-real/arb.hpp` header and create `Arb x` to get C++
style memory management and manipulate the underlying `arb_t` directly with
`x.arb_t()`.
