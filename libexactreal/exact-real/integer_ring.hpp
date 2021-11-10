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

#ifndef LIBEXACTREAL_INTEGER_RING_HPP
#define LIBEXACTREAL_INTEGER_RING_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>
#include <type_traits>

#include "forward.hpp"

namespace exactreal {

struct LIBEXACTREAL_API IntegerRing : private boost::equality_comparable<IntegerRing> {
  IntegerRing();
  IntegerRing(const mpz_class&);

  static IntegerRing compositum(const IntegerRing& lhs, const IntegerRing& rhs);

  typedef mpz_class ElementClass;

  template <typename T, typename M = decltype(std::declval<const ElementClass&>() * std::declval<const T&>())>
  using multiplication_t = M;

  // Note that this trait is broken, see https://github.com/flatsurf/exact-real/issues/148.
  template <typename T, typename Q = decltype(std::declval<const ElementClass&>() / std::declval<const T&>())>
  using division_t = std::conditional_t<std::is_same_v<Q, mpz_class>, mpq_class, Q>;

  static constexpr bool contains_rationals = false;

  ElementClass coerce(const ElementClass& x) const { return x; }

  static constexpr bool isField = false;
  static bool unit(const ElementClass& x);
  static Arb arb(const ElementClass& x, long prec);
  static mpz_class floor(const ElementClass& x);
  static std::optional<mpq_class> rational(const ElementClass& x);
  bool operator==(const IntegerRing&) const { return true; }
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::IntegerRing> {
  size_t operator()(const exactreal::IntegerRing&) const { return 0; }
};
}  // namespace std

#endif
