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

#ifndef LIBEXACTREAL_RATIONAL_FIELD_HPP
#define LIBEXACTREAL_RATIONAL_FIELD_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>

#include "integer_ring.hpp"

namespace exactreal {

// Implements rational arithmetic for rational coefficients in expressions making up an Element.
struct LIBEXACTREAL_API RationalField : boost::equality_comparable<RationalField> {
  RationalField();
  RationalField(const mpq_class&);

  static RationalField compositum(const RationalField& lhs, const RationalField& rhs);

  typedef mpq_class ElementClass;

  static constexpr bool contains_rationals = true;

  ElementClass coerce(const ElementClass& x) const;

  static constexpr bool isField = true;
  static bool unit(const ElementClass&);
  static Arb arb(const ElementClass& x, long prec);
  static mpz_class floor(const ElementClass& x);
  static std::optional<mpq_class> rational(const ElementClass& x);

  // Return the result of exact multiplication of an ElementClass.
  template <typename T>
  static ElementClass& imul(ElementClass&, const T&);

  // Return the result of exact division of an ElementClass.
  template <typename T>
  static ElementClass& idiv(ElementClass&, const T&);

  bool operator==(const RationalField&) const;
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::RationalField> {
  size_t operator()(const exactreal::RationalField&) const;
};
}  // namespace std

#endif
