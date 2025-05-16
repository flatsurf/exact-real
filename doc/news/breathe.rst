**Added:**

* Added documentation built with sphinx & breathe in doc/manual/.
* Added an ``Arb(const mpq_class&)`` constructor that converts with precision 64.
* Added an ``Arb(const renf_elem_class&)`` constructor that converts with precision 64.

**Removed:**

* Removed ``Element::operator[](const std::pair<size, size>&) const`` since it had no implementation at all.
* Removed ``NumberFieldIdeal`` which was just an empty struct without any implementation at all.
* Removed ``RealNumber::liouville`` since it had no implementation at all.
* Removed ``RealNumber::pi`` since it had no implementation at all.
* Removed ``RealNumber::e`` since it had no implementation at all.

**Fixed:**

* Fixed missing implementation for ``Arb::zero_pm_one``.
* Fixed missing implementation for ``Arb::unit_interval``.
* Fixed missing implementation for ``Arb::operator Arf``.
* Fixed missing implementation for ``swap(Arb&, Arb&)``.
* Fixed missing implementation for ``Arf::operator=(double)``.
* Fixed missing implementation for ``swap(Arf&, Arf&)``.
* Fixed missing implementations for ``RealNumber::operator<(const mpz_class&)``, ``RealNumber::operator==(const mpz_class&)``, and ``RealNumber::operator>(const mpz_class&)``.
* Fixed missing implementations for ``RealNumber::operator<(const mpq_class&)`` and ``RealNumber::operator>(const mpq_class&)`.
