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

/// The Field of Rationals
///
/// The struct [RationalField](<> "exactreal::RationalField") models some
/// features of the field of integer rationals. In practice, you won't be
/// using this struct directly, but just pass it into the [constructor](<>
/// "exactreal::Module::make") of [Module](<> "exactreal::Module") to specify
/// that the module should be considered over the rationals.
///
///     #include <exact-real/rational_field.hpp>
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <gmpxx.h>
///
///     auto M = exactreal::Module<exactreal::RationalField>::make({exactreal::RealNumber::rational(mpq_class{1, 2})}, exactreal::RationalField{});
///     std::cout << *M;
///     // -> ℚ-Module(1/2)
///

#ifndef LIBEXACTREAL_RATIONAL_FIELD_HPP
#define LIBEXACTREAL_RATIONAL_FIELD_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>

#include "forward.hpp"

namespace exactreal {

/// Traits of the rational field.
/// This struct models the field of rationals, or rather describes how
/// arithmetic works in that field. Rationals themselves are modeled by GMP's
/// [`mpq_class`](https://gmplib.org/manual/C_002b_002b-Interface-Rationals).
///
///     #include <exact-real/rational_field.hpp>
///     #include <exact-real/module.hpp>
///
///     auto M = exactreal::Module<exactreal::RationalField>::make({}, exactreal::RationalField{});
///     std::cout << *M;
///     // -> ℚ-Module()
///
struct LIBEXACTREAL_API RationalField : boost::equality_comparable<RationalField> {
  /// Create the rational field.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///
  ///     exactreal::RationalField Q;
  ///     std::cout << Q;
  ///     // -> ℚ
  ///
  RationalField();

  /// Create the rational field containing the argument.
  /// The argument is ignored. This method exists for symmetry with other
  /// coefficient rings.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///
  ///     exactreal::RationalField Q{1337};
  ///     std::cout << Q;
  ///     // -> ℚ
  ///
  RationalField(const mpq_class&);

  /// Return the smallest rational field containing `lhs` and `rhs`, i.e., the
  /// rational field itself.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///
  ///     exactreal::RationalField Q;
  ///     std::cout << exactreal::RationalField::compositum(Q, Q);
  ///     // -> ℚ
  ///
  static RationalField compositum(const RationalField& lhs, const RationalField& rhs);

  /// The type modeling the elements of this field, GMP's
  /// [mpq_class](https://gmplib.org/manual/C_002b_002b-Interface-Rationals).
  typedef mpq_class ElementClass;

  /// The result of multiplication of a rational and a `T`.
  template <typename T, typename M = decltype(std::declval<const ElementClass&>() * std::declval<const T&>())>
  using multiplication_t = M;

  /// The result of dividing a rational by a `T`.
  template <typename T, typename Q = decltype(std::declval<const ElementClass&>() / std::declval<const T&>())>
  using division_t = Q;

  /// Whether the rationals contain the rational field, i.e., `true`.
  static constexpr bool contains_rationals = true;

  /// Return the rational `x` as an element of this field, i.e., return `x`
  /// unchanged.
  ElementClass coerce(const ElementClass& x) const { return x; }

  /// Whether the rational field is a field, i.e., `true`.
  static constexpr bool isField = true;

  /// Return whether the rational `x` is a unit in this field, i.e., whether
  /// it is non-zero.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///
  ///     exactreal::RationalField::unit(2)
  ///     // -> true
  ///
  ///     exactreal::RationalField::unit(0)
  ///     // -> false
  ///
  static bool unit(const ElementClass&);

  /// Return an approximation to the rational `x` in ball arithmetic.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///     #include <exact-real/arb.hpp>
  ///
  ///     std::cout << exactreal::RationalField::arb(mpq_class{1, 3}, 64);
  ///     // -> [0.333333 +/- 3.34e-7]
  ///
  static Arb arb(const ElementClass& x, long prec);

  /// Return the integer floor of this rational.
  static mpz_class floor(const ElementClass& x);

  /// Return the rational `x` as a rational number, i.e, `x` unchanged.
  static std::optional<mpq_class> rational(const ElementClass& x);

  /// Return whether this field is equal to another rational field, i.e., `true`.
  ///
  ///     #include <exact-real/rational_field.hpp>
  ///
  ///     exactreal::RationalField{} == exactreal::RationalField{1337}
  ///     // -> true
  ///
  bool operator==(const RationalField&) const { return true; }

  friend std::ostream& operator<<(std::ostream&, const RationalField&);
};

LIBEXACTREAL_API std::ostream& operator<<(std::ostream&, const RationalField&);

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::RationalField> {
  size_t operator()(const exactreal::RationalField&) const { return 0; }
};
}  // namespace std

#endif
