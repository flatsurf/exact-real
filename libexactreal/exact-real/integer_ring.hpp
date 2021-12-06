/* ********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019      Vincent Delecroix
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
 * *******************************************************************/

/// The Ring of Integers
///
/// The struct [IntegerRing](<> "exactreal::IntegerRing") models some features
/// of the ring of integers. In practice, you won't be using this struct
/// direcly, but just pass it into the [constructor](<>
/// "exactreal::Module::make") of [Module](<> "exactreal::Module") to specify
/// that the module should be considered over the integers.
///
///     #include <exact-real/integer_ring.hpp>
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <gmpxx.h>
///
///     auto M = exactreal::Module<exactreal::IntegerRing>::make({exactreal::RealNumber::rational(mpq_class{1, 2})}, exactreal::IntegerRing{});
///     std::cout << *M;
///     // -> ℤ-Module(1/2)
///

#ifndef LIBEXACTREAL_INTEGER_RING_HPP
#define LIBEXACTREAL_INTEGER_RING_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>
#include <type_traits>
#include <iosfwd>

#include "forward.hpp"

namespace exactreal {

/// Traits of the ring of integers.
/// This struct models the ring of integers, or rather describes how
/// arithmetic works in that ring. Integers themselves are modeled by GMP's
/// [`mpz_class`](https://gmplib.org/manual/C_002b_002b-Interface-Integers).
///
///     #include <exact-real/integer_ring.hpp>
///     #include <exact-real/module.hpp>
///
///     auto M = exactreal::Module<exactreal::IntegerRing>::make({}, exactreal::IntegerRing{});
///     std::cout << *M;
///     // -> ℤ-Module()
///
struct LIBEXACTREAL_API IntegerRing : private boost::equality_comparable<IntegerRing> {
  /// Create the ring of integers.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///
  ///     exactreal::IntegerRing Z;
  ///     std::cout << Z;
  ///     // -> ℤ
  ///
  IntegerRing();

  /// Create the ring of integers containing the argument.
  /// The argument is ignored. This method exists for symmetry with other
  /// coefficient rings.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///
  ///     exactreal::IntegerRing Z{1337};
  ///     std::cout << Z;
  ///     // -> ℤ
  ///
  IntegerRing(const mpz_class&);

  /// Return the smallest integer ring containing `lhs` and `rhs`, i.e., the ring of integers itself.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///
  ///     exactreal::IntegerRing Z;
  ///     std::cout << exactreal::IntegerRing::compositum(Z, Z);
  ///     // -> ℤ
  ///
  static IntegerRing compositum(const IntegerRing& lhs, const IntegerRing& rhs);

  /// The type modeling the elements of this ring, GMP's
  /// [mpz_class](https://gmplib.org/manual/C_002b_002b-Interface-Integers).
  typedef mpz_class ElementClass;

  /// The result of multiplication of an integer and a `T`.
  template <typename T, typename M = decltype(std::declval<const ElementClass&>() * std::declval<const T&>())>
  using multiplication_t = M;

  /// The result of dividing an integer by a `T`.
  template <typename T, typename Q = decltype(std::declval<const ElementClass&>() / std::declval<const T&>())>
  using division_t = std::conditional_t<std::is_same_v<Q, mpz_class>, mpq_class, Q>;

  /// Whether the integers contain the rational field, i.e., `false`.
  static constexpr bool contains_rationals = false;

  /// Return the integer `x` as an element of this ring, i.e., return `x`
  /// unchanged.
  ElementClass coerce(const ElementClass& x) const { return x; }

  /// Whether the integer ring is a field, i.e., `false`.
  static constexpr bool isField = false;

  /// Return whether the integer `x` is a unit in this ring, i.e., whether it
  /// is ±1.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///
  ///     exactreal::IntegerRing::unit(1)
  ///     // -> true
  ///
  ///     exactreal::IntegerRing::unit(-1)
  ///     // -> true
  ///
  ///     exactreal::IntegerRing::unit(2)
  ///     // -> false
  ///
  static bool unit(const ElementClass& x);

  /// Return an approximation to the integer `x` in ball arithmetic.
  /// The argument `prec` is ignored.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << exactreal::IntegerRing::arb(1, 64);
  ///     // -> 1.00000
  ///
  static Arb arb(const ElementClass& x, long prec);

  /// Return the floor of the integer `x`, i.e., `x` unchanged.
  static mpz_class floor(const ElementClass& x);

  /// Return the integer `x` as a rational number.
  static std::optional<mpq_class> rational(const ElementClass& x);

  /// Return whether this ring is equal to another integer ring, i.e., `true`.
  ///
  ///     #include <exact-real/integer_ring.hpp>
  ///
  ///     exactreal::IntegerRing{} == exactreal::IntegerRing{1337}
  ///     // -> true
  ///
  bool operator==(const IntegerRing&) const { return true; }

  friend std::ostream& operator<<(std::ostream&, const IntegerRing&);
};

LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const IntegerRing&);

}  // namespace exactreal

namespace std {
template <>
struct LIBEXACTREAL_API hash<exactreal::IntegerRing> {
  size_t LIBEXACTREAL_API operator()(const exactreal::IntegerRing&) const { return 0; }
};
}  // namespace std

#endif
