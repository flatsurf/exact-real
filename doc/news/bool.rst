**Added:**

* Added `Module::one()` convenience method; returns a 1 element if one exists in the module.

**Fixed:**

* Added missing implementation of `Element<NumberField>::Element(const Element<IntegerRing>&)` and `Element<Numberfield>::Element(const Element<RationalField>&)`.

**Performance:**

* Improved performance of explicit bool cast, i.e., non-zero check, for `Element`.
