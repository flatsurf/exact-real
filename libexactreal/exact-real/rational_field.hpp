/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
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

struct LIBEXACTREAL_API RationalField : boost::equality_comparable<RationalField> {
  RationalField();
  RationalField(const mpq_class&);

  static RationalField compositum(const RationalField& lhs, const RationalField& rhs);

  typedef mpq_class ElementClass;

  // Whether a rational supports multiplication with a T.
  template <typename T>
  static constexpr bool can_multiply = IntegerRing::can_multiply<T> || std::is_same_v<T, mpq_class>;

  // Whether a rational supports exact division by a T.
  template <typename T>
  static constexpr bool can_divide = can_multiply<T>;

  static constexpr bool contains_rationals = true;

  ElementClass coerce(const ElementClass& x) const { return x; }

  static constexpr bool isField = true;
  static bool unit(const ElementClass&);
  static Arb arb(const ElementClass& x, long prec);
  static mpz_class floor(const ElementClass& x);
  static std::optional<mpq_class> rational(const ElementClass& x);
  bool operator==(const RationalField&) const { return true; }
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::RationalField> {
  size_t operator()(const exactreal::RationalField&) const { return 0; }
};
}  // namespace std

#endif
