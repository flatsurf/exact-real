/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2025 Julian Rüth
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

#include <optional>

#include <boost/operators.hpp>
#include <gmpxx.h>

#include "forward.hpp"

namespace exactreal {

/// Implements integer arithmetic for integer coefficients in expressions
/// making up an Element.
///
/// There should be no need to instantiate this struct. It's usually only used
/// as a template parameter:
///
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <exact-real/element.hpp>
///     #include <exact-real/integer_ring.hpp>
///
///     auto M = exactreal::Module<exactreal::IntegerRing>::make({
///       exactreal::RealNumber::rational(1),
///       exactreal::RealNumber::random()});
///     *M
///     // -> ℤ-Module(1, ℝ(<...>))
/// 
struct LIBEXACTREAL_API IntegerRing
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : private boost::equality_comparable<IntegerRing>
#endif
{
  /// Create the integer ring.
  IntegerRing();

  /// Create "an" integer ring that contains the argument.
  ///
  /// This is identical to \ref IntegerRing::IntegerRing() but other rings, say
  /// number fields, have different rings and need to be able to construct the
  /// smallest ring that contains a certain element.
  IntegerRing(const mpz_class&);

  /// Return the integer ring that contains both arguments.
  ///
  /// This is just the \ref IntegerRing::IntegerRing() but other rings need
  /// such functionality to compute a composite ring.
  static IntegerRing compositum(const IntegerRing& lhs, const IntegerRing& rhs);

  /// The internal representation of element of this ring, i.e., GMP integers.
  typedef mpz_class ElementClass;

  /// Whether this ring contains the rational numbers, i.e., `false`.
  static constexpr bool contains_rationals = false;

  /// Convert the integer argument into this integer ring, i.e., just return
  /// the integer unchanged.
  ///
  /// Number fields need the equivalent of this method to convert elements
  /// between different number fields.
  ElementClass coerce(const ElementClass& x) const;

  /// Whether this ring is a field, i.e., `false`.
  static constexpr bool isField = false;

  /// Return whether \p x is a unit, i.e., whether it is 1 or -1.
  static bool unit(const ElementClass& x);

  /// Return an (exact) \ref Arb approximation of the integer \p x.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     exactreal::IntegerRing::arb(1, 64)
  ///     // -> 1.00000
  ///
  static Arb arb(const ElementClass& x, long prec);

  /// Return the floor of the integer \p x, i.e., the element unchanged.
  static mpz_class floor(const ElementClass& x);

  /// Return the integer \p x as a rational number.
  ///
  ///     exactreal::IntegerRing::rational(1).value()
  ///     // -> 1
  ///
  static std::optional<mpq_class> rational(const ElementClass& x);

  /// Multiply the integer \p x with \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, or a GMP rational
  /// that is an integer.
  template <typename T>
  static ElementClass& imul(ElementClass& x, const T& t);

  /// Divide the integer \p x by \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, or a GMP rational.
  /// This operation is only supported if the result of the exact division is
  /// an integer.
  template <typename T>
  static ElementClass& idiv(ElementClass& x, const T& t);

  /// Return whether this integer ring is identical to \p R, i.e., return
  /// ``true`` since there is only one integer ring.
  bool operator==(const IntegerRing& R) const;
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::IntegerRing> {
  size_t operator()(const exactreal::IntegerRing&) const;
};
}  // namespace std

#endif
