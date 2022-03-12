**Added:**

* Deployed documentation for libexactreal and pyexactreal to
  flatsurf.github.io/exact-real.

* Added some convenience constructors to `exactreal::Arb` such as `Arb(const
  mpq_class&)` and `Arb(const eantic::renf_elem_class&)`.

* Added more `Arf` constructors from primitive integer types.

* Added more `Arf::operator=` from primitive integer types.

**Removed:**

* Removed superfluous `noexcept` specifiers from `Arb` and `Arf` methods. These
  were originally added in the hope to improve performance. However, this is
  not how `noexcept` works for most methods. At the same time it tends to make
  debugging much harder since any exception leads to program termination. (Note
  that this is a breaking ABI change.)

* Removed `NumberFieldIdeal` which was not implemented at all anyway.

* Removed `Element::operator[](const std::pair<size, size>&) const` which was
  not implemented anyway.

**Changed:**

* Replaced templatized `Arb(Integer)` constructor with explicit constructors
  for all the integer types. (Note that this is a breaking ABI change.)

* Replaced templatized `Arb::operator=(Integer)` methods with explicit
  operators for all the integer types. (Note that this is a breaking ABI
  change.)

* Replaced templatized comparison operators in `Arb` with explicit operators
  for all integer types. (Note that this is a breaking ABI change.)

* Replaced all comparison operators in `Arb` and `Arf` with free comparison
  operators.  (Note that this is a breaking ABI change.)
