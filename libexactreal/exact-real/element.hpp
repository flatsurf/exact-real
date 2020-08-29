/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019-2020 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  exact-real is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef LIBEXACTREAL_ELEMENT_HPP
#define LIBEXACTREAL_ELEMENT_HPP

#include <optional>
#include <type_traits>
#include <vector>

#include <gmpxx.h>
#include <boost/blank.hpp>
#include <boost/operators.hpp>

#include "external/spimpl/spimpl.h"

#include "exact-real.hpp"
#include "forward.hpp"
#include "integer_ring.hpp"
#include "rational_field.hpp"

namespace exactreal {

template <typename Ring>
class Element : boost::additive<Element<Ring>>,
                boost::multipliable<Element<Ring>>,
                boost::totally_ordered<Element<Ring>>,
                boost::totally_ordered<Element<Ring>, RealNumber>,
                boost::totally_ordered<Element<Ring>, mpq_class>,
                boost::totally_ordered<Element<Ring>, mpz_class>,
                boost::totally_ordered<Element<Ring>, long long>,
                boost::multipliable<Element<Ring>, RealNumber>,
                boost::multiplicative<Element<Ring>, typename Ring::ElementClass> {
 public:
  Element();
  Element(const std::shared_ptr<const Module<Ring>>& parent, const std::vector<typename Ring::ElementClass>& coefficients);

  Element(const typename Ring::ElementClass& value);
  template <bool Enabled = !std::is_same_v<Ring, IntegerRing>, std::enable_if_t<Enabled, bool> = true>
  Element(const Element<IntegerRing>& value);
  template <bool Enabled = !std::is_same_v<Ring, RationalField> && std::is_convertible_v<mpq_class, typename Ring::ElementClass>, std::enable_if_t<Enabled, bool> = true>
  Element(const Element<RationalField>& value);

  typename Ring::ElementClass operator[](const size) const;
  std::conditional<Ring::isField, mpq_class, mpz_class> operator[](const std::pair<size, size>&) const;

  template <typename C = typename Ring::ElementClass>
  std::vector<C> coefficients() const;

  // Return a ball containing this element such that it's accuracy is at least
  // accuracy, defined as in http://arblib.org/arb.html#c.arb_rel_accuracy_bits, i.e.,
  // the position of the top bit of the midpoint minus the position of the top
  // bit of the radius minus one.
  Arb arb(long accuracy) const;

  Element& operator+=(const Element&);
  Element& operator-=(const Element&);
  Element& operator*=(const Element&);
  Element& operator*=(const RealNumber&);
  Element operator-() const;
  // Define operator*= for every type that multiplies with Ring::ElementClass.
  // (until we figure out how to dynamically inherit from boost::multiplicative for such T, we do not get operator* here.)
  template <typename T, typename = decltype(std::declval<const typename Ring::ElementClass&>() * std::declval<const T&>())>
  Element& operator*=(const T&);
  // Define operator/= and operator/ if we're in a field in the same way.
  // (until we figure out how to dynamically inherit from boost::multiplicative for such T, we do not get operator/ here.)
  template <typename T, typename = decltype(std::declval<const typename Ring::ElementClass&>() / std::declval<const T&>()), typename = std::enable_if_t<Ring::isField || false_t<T>, void>>
  Element& operator/=(const T&);

  std::optional<Element> truediv(const Element&) const;
  mpz_class floordiv(const Element& rhs) const;

  // Return whether this element is a unit, i.e., whether its inverse exists in the parent module.
  bool unit() const;

  mpz_class floor() const;
  mpz_class ceil() const;

  bool operator==(const Element&) const;
  bool operator<(const Element&) const;

  bool operator==(const RealNumber&) const;
  bool operator<(const RealNumber&) const;
  bool operator>(const RealNumber&) const;

  bool operator==(const mpq_class&) const;
  bool operator<(const mpq_class&) const;
  bool operator>(const mpq_class&) const;

  bool operator==(const mpz_class&) const;
  bool operator<(const mpz_class&) const;
  bool operator>(const mpz_class&) const;

  bool operator==(long long) const;
  bool operator<(long long) const;
  bool operator>(long long) const;

  explicit operator bool() const;
  // Return the closest double; ties are rounded to even.
  explicit operator double() const;

  explicit operator std::optional<mpz_class>() const;
  explicit operator std::optional<mpq_class>() const;

  const std::shared_ptr<const Module<Ring>> module() const;
  // Make this element an element of module. All the module generators used in
  // this element must be present, e.g., module may be a supermodule of this
  // element's module.
  Element& promote(const std::shared_ptr<const Module<Ring>>& module);

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Element<R>&);

 private:
  struct Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

template <typename Ring, typename... Args>
Element(const std::shared_ptr<const Module<Ring>>&, Args...)->Element<Ring>;

Element(int)->Element<IntegerRing>;

Element(unsigned int)->Element<IntegerRing>;

Element(long)->Element<IntegerRing>;

Element(unsigned long)->Element<IntegerRing>;

Element(const mpz_class&)->Element<IntegerRing>;

Element(const mpq_class&)->Element<RationalField>;

Element(const RealNumber&)->Element<IntegerRing>;

}  // namespace exactreal

namespace std {
template <typename Ring>
struct hash<exactreal::Element<Ring>> {
  size_t operator()(const exactreal::Element<Ring>&) const noexcept;
};
}  // namespace std

#endif
