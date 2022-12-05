/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#ifndef LIBEXACTREAL_INTEGER_RING_HPP
#define LIBEXACTREAL_INTEGER_RING_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>
#include <type_traits>

#include "forward.hpp"

namespace exactreal {

// Implements integer arithmetic for integer coefficients in expressions making up an Element.
struct LIBEXACTREAL_API IntegerRing : private boost::equality_comparable<IntegerRing> {
  IntegerRing();
  IntegerRing(const mpz_class&);

  static IntegerRing compositum(const IntegerRing& lhs, const IntegerRing& rhs);

  typedef mpz_class ElementClass;

  static constexpr bool contains_rationals = false;

  ElementClass coerce(const ElementClass& x) const;

  static constexpr bool isField = false;
  static bool unit(const ElementClass& x);
  static Arb arb(const ElementClass& x, long prec);
  static mpz_class floor(const ElementClass& x);
  static std::optional<mpq_class> rational(const ElementClass& x);

  // Return the result of exact multiplication of an ElementClass.
  template <typename T>
  static ElementClass& imul(ElementClass&, const T&);

  // Return the result of exact division of an ElementClass.
  template <typename T>
  static ElementClass& idiv(ElementClass&, const T&);

  bool operator==(const IntegerRing&) const;
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::IntegerRing> {
  size_t operator()(const exactreal::IntegerRing&) const;
};
}  // namespace std

#endif
