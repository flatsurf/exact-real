**Added:**

* Added `Element::coefficients()` and `Element::rationalCoefficients()` for
  the deprecated `Element::coefficients<C>()`.

* Explicit overloads of `Element::operator*=` and `Element::operator/=` for
  `mpz_class` and `mpq_class` since the old template-based approach is not
  supported when building on clang with `-fvisibility=hidden`.

**Fixed:**

* Printing of elements when compiling with clang.

**Deprecated:**

* Deprecated `Element::coefficients<C>()`. The method is not visible when
  building with clang and `fvisibility=hidden`, `coefficients()` and
  `rationalCoefficients()` should be used instead.
