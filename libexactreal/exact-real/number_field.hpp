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

/// A Number Field embedded into the Reals
///
/// The struct [NumberField](<> "exactreal::NumberField") models some features
/// of a number field with an embedded into the real numbers. In practice you
/// won't be using this struct directly, but just pass it into the
/// [constructor](<> "exactreal::Module::make") of [Module](<>
/// "exactreal::Module") to specify that the module should be considered over
/// a certain number field.
///
///     #include <exact-real/number_field.hpp>
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <e-antic/renf_class.hpp>
///     #include <gmpxx.h>
///
///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
///     auto M = exactreal::Module::make({exactreal::RealNumber::rational(1)}, exactreal::NumberField{K});
///     std::cout << *M;
///     // -> ...
///

#ifndef LIBEXACTREAL_NUMBER_FIELD_HPP
#define LIBEXACTREAL_NUMBER_FIELD_HPP

#include <gmpxx.h>

#include <boost/intrusive_ptr.hpp>
#include <boost/operators.hpp>
#include <e-antic/renfxx_fwd.hpp>
#include <memory>
#include <optional>

#include "forward.hpp"

namespace exactreal {

/// Traits of a real-embedded number field.
/// This struct models a number field embedded into the reals, or rather
/// describes how arithmetic works in that field. Number fields and their
/// elements themselves are modeled by e-antic's [`renf_class`](<>
/// "renf_class") and [`renf_elem_class`](<>
/// "renf_elem_class").
///
///     #include <exact-real/number_field.hpp>
///     #include <exact-real/module.hpp>
///     #include <e-antic/renf_class.hpp>
///
///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
///     auto M = exactreal::Module::make({}, exactreal::NumberField{K});
///     std::cout << *M;
///     // -> ...
///
struct LIBEXACTREAL_API NumberField : boost::equality_comparable<NumberField> {

  /// Create the trivial number field, i.e., the rational field.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///
  ///     exactreal::NumberField Q;
  ///     std::cout << Q;
  ///     // -> ...
  ///
  NumberField();

  /// Create the number field given by an e-antic field.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = exactreal::NumberField{*eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     std::cout << K;
  ///     // -> ...
  ///
  NumberField(const eantic::renf_class&);

  /// Create the number field given by an e-antic field.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     std::cout << K;
  ///     // -> ...
  ///
  NumberField(boost::intrusive_ptr<const eantic::renf_class>);

  /// Create the number containing this number field element.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto N = exactreal::NumberField{K->one()};
  ///     std::cout << K;
  ///     // -> ...
  ///
  NumberField(const eantic::renf_elem_class&);

  /// Return the e-antic number field underlying this number field.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField{K}.parameters == K
  ///     // -> true
  ///
  boost::intrusive_ptr<const eantic::renf_class> parameters;

  /// Return the smallest number field containing `lhs` and `rhs`.
  /// Note that this method is only implemented in trivial cases currently.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{};
  ///     
  ///     auto M = exactreal::NumberField::compositum(K, L);
  ///     std::cout << M;
  ///     // -> ...
  ///
  static NumberField compositum(const NumberField& lhs, const NumberField& rhs);

  /// Return whether two number fields are identical.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{eantic::renf_class::make("y^2 - 2", "y", "1.4 +/- 1")};
  ///     K == L
  ///     // -> false
  ///
  bool operator==(const NumberField&) const;

  /// The type modeling the elements of this field, i.e., e-antic's
  /// [renf_elem_class]().
  typedef eantic::renf_elem_class ElementClass;

  /// The result of multiplication of an element of this field and a `T`.
  template <typename T, typename M = decltype(std::declval<const ElementClass&>() * std::declval<const T&>())>
  using multiplication_t = M;

  /// The result of dividing an element of this field by a `T`.
  template <typename T, typename Q = decltype(std::declval<const ElementClass&>() / std::declval<const T&>())>
  using division_t = Q;

  /// Whether this number field contians the rational field, i.e., `true`.
  static constexpr bool contains_rationals = true;

  /// Return the number field element `x` as an element of this field.
  /// Note that this is only implemented in trivial cases.
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{eantic::renf_class::make("y^3 - 2", "y", "1.4 +/- 1")};
  ///     auto x = exactreal::NumberField{K}.coerce(L.one());
  ///     x.parent() == K
  ///     // -> true
  ///
  ElementClass coerce(const ElementClass&) const;

  /// Whether this number field is a field, i.e., `true`.
  static constexpr bool isField = true;

  /// Return whether the element `x` is a unit, i.e., whether it is non-zero.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::unit(K.gen())
  ///     // -> true
  ///
  ///     exactreal::NumberField::unit(K.zero())
  ///     // -> false
  ///     
  static bool unit(const ElementClass& x);

  /// Return an approximation to the number field element `x` in ball arithmetic.
  static Arb arb(const ElementClass& x, long prec);

  /// Return the integer floor of this number field element.
  static mpz_class floor(const ElementClass& x);

  /// Return the number field element `x` as a rational number, if possible.
  ///
  ///     #include <exact-real/number_field.hpp>
  ///     #include <e-antic/renf_class.hpp>
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::rational(K.gen()).has_value()
  ///     // -> false
  ///
  ///     exactreal::NumberField::rational(K.one()).has_value()
  ///     // -> true
  ///     
  static std::optional<mpq_class> rational(const ElementClass& x);
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::NumberField> {
  size_t operator()(const exactreal::NumberField&) const;
};
}  // namespace std

#endif
