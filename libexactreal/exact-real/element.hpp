/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2021 Julian Rüth
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

#include <gmpxx.h>

#include <boost/blank.hpp>
#include <boost/operators.hpp>
#include <e-antic/renfxx_fwd.hpp>
#include <type_traits>
#include <vector>

#include "external/spimpl/spimpl.h"
#include "forward.hpp"
#include "integer_ring.hpp"
#include "rational_field.hpp"

namespace exactreal {

template <typename Ring>
class LIBEXACTREAL_API Element : boost::additive<Element<Ring>>,
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

  template <bool Enabled = !std::is_same_v<Ring, RationalField> && Ring::contains_rationals, std::enable_if_t<Enabled, bool> = true>
  Element(const Element<RationalField>& value);

  typename Ring::ElementClass operator[](const size) const;
  std::conditional<Ring::isField, mpq_class, mpz_class> operator[](const std::pair<size, size>&) const;

  template <typename C>
  [[deprecated("Use the non-template coefficients() or rationalCoefficients() instead.")]] std::vector<C> coefficients() const;

  std::vector<typename Ring::ElementClass> coefficients() const;

  std::vector<mpq_class> rationalCoefficients() const;

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

  // Multiply this element with `c`.
  // This overload is available for every type that multiplies with the
  // coefficient type. However, we need to define multiplication separately for
  // mpz_class and mpq_class since we cannot export symbols with these as
  // template arguments when building on clang with -fvisibility=hidden.
  template <typename T, typename = typename Ring::template multiplication_t<T>>
  Element& operator*=(const T& c);
  Element& operator*=(const mpz_class&);
  Element& operator*=(const mpq_class&);

  // Divide this element by `c`.
  // As with multiplication, we provide specialized overloads for GMP types.
  template <typename T, typename Q = typename Ring::template division_t<T>, typename = 
    // Unt
    std::enable_if_t<
      (std::is_same_v<Ring, RationalField> && std::is_same_v<T, int>) ||
      (std::is_same_v<Ring, NumberField> && (std::is_same_v<T, int> || std::is_same_v<T, eantic::renf_elem_class>))
    , void>>
  Element& operator/=(const T& c);
  Element& operator/=(const mpz_class&);
  Element& operator/=(const mpq_class&);

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

  // Make this element an element of a module defined over the same ring which
  // has only the generators necessary to represent this element. This method
  // is especially useful after chains of arithmetic operations involving
  // products as these introduce lots of generators that might not be necessary
  // in the final result.
  Element& simplify();

  template <typename R>
  friend std::ostream& operator<<(std::ostream&, const Element<R>&);

 private:
  struct LIBEXACTREAL_LOCAL Implementation;
  spimpl::impl_ptr<Implementation> impl;
};

template <typename Ring, typename... Args>
Element(const std::shared_ptr<const Module<Ring>>&, Args...) -> Element<Ring>;

Element(int)->Element<IntegerRing>;

Element(unsigned int)->Element<IntegerRing>;

Element(long)->Element<IntegerRing>;

Element(unsigned long)->Element<IntegerRing>;

Element(const mpz_class&)->Element<IntegerRing>;

Element(const mpq_class&)->Element<RationalField>;

Element(const RealNumber&)->Element<IntegerRing>;

template <typename R>
LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const Element<R>&);

}  // namespace exactreal

namespace std {
template <typename Ring>
struct LIBEXACTREAL_API hash<exactreal::Element<Ring>> {
  size_t LIBEXACTREAL_API operator()(const exactreal::Element<Ring>&) const noexcept;
};
}  // namespace std

#endif
