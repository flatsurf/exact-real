**Added:**

* Added arithmetic operators with short and unsigned short operators to the C++ interface.
* Added division operators for `Element<IntegerRing>`. An exact division is attempted. An exception is raised if such division is not possible in the integers.

**Changed:**

* Changed how arithmetic operators are organized in `ELement.hpp`. This is a breaking change. Existing code needs to be rebuilt to link against the new symbol names.

**Removed:**

* Removed automatic promotion of a coefficient to an element, e.g., the implicit conversion of a `renf_elem_class` to an `Element<NumberField>` which should be done explicitly since it can be quite costly. Existing code might have to replace `x` with `Element<NumberField>(x)` in some places to restore the old behavior. (The same holds for the implicit conversion from `mpz_class` to `Element<IntegerRing>` and `mpq_class` to `Element<RationalField>` but these are probably more rare.)
* Removed legacy arithmetic from the shared library that were deprecated since version 1.5.0. (There were quite a few other ABI changes, so this should not affect anybody since you'll need to rebuild things anyway.)
