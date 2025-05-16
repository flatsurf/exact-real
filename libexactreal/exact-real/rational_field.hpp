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

#ifndef LIBEXACTREAL_RATIONAL_FIELD_HPP
#define LIBEXACTREAL_RATIONAL_FIELD_HPP

#include <gmpxx.h>

#include <boost/operators.hpp>
#include <optional>

#include "integer_ring.hpp"

namespace exactreal {

/// Models some features of a number field with an embedding into the real. In
/// practice you won't be using instances of this but just use it as a type
/// parameter to specify that a module should be considered over the rationals.
///
///     #include <exact-real/rational_field.hpp>
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <gmpxx.h>
///
///     auto M = exactreal::Module<exactreal::RationalField>::make({exactreal::RealNumber::rational(mpq_class{1, 2})}, exactreal::RationalField{});
///
struct LIBEXACTREAL_API RationalField
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : boost::equality_comparable<RationalField>
#endif
{
  /// Create the rational field.
  RationalField();

  /// Create "a" rational field that contains the argument.
  ///
  /// This is identical to \ref RationalField::RationalField() but other rings,
  /// say number fields, have different rings and need to be able to construct
  /// the smallest ring that contains a certain element.
  RationalField(const mpq_class&);

  /// Return the rational field that contains both arguments.
  ///
  /// This is just the \ref RationalField::RationalField() but other rings need
  /// such functionality to compute a composite ring.
  static RationalField compositum(const RationalField& lhs, const RationalField& rhs);

  /// The internal representation of element of this ring, i.e., GMP rationals.
  typedef mpq_class ElementClass;

  /// Whether this ring contains the rational numbers, i.e., `true`.
  static constexpr bool contains_rationals = true;

  /// Convert the rational argument into this rational field, i.e., just return
  /// the rational unchanged.
  ///
  /// Number fields need the equivalent of this method to convert elements
  /// between different number fields.
  ElementClass coerce(const ElementClass& x) const;

  /// Whether this ring is a field, i.e., `true`.
  static constexpr bool isField = true;

  /// Return whether \p x is a unit, i.e., whether it is non-zero.
  static bool unit(const ElementClass& x);

  /// Return an \ref Arb approximation of the rational \p x.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     exactreal::RationalField::arb(mpq_class{1, 3}, 64)
  ///     // -> [0.333333 +/- 3.34e-7]
  ///
  static Arb arb(const ElementClass& x, long prec);

  /// Return the integer floor of the rational \p x.
  static mpz_class floor(const ElementClass& x);

  /// Return the rational \p x as a rational number, i.e, \p x unchanged.
  static std::optional<mpq_class> rational(const ElementClass& x);

  /// Multiply the rational \p x with \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, or a GMP
  /// rational.
  template <typename T>
  static ElementClass& imul(ElementClass&, const T&);

  /// Divide the rational \p x by \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, or a GMP rational.
  template <typename T>
  static ElementClass& idiv(ElementClass&, const T&);

  /// Return whether this field is equal to another rational field, i.e., `true`.
  ///
  ///     exactreal::RationalField{} == exactreal::RationalField{1337}
  ///     // -> true
  ///
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
