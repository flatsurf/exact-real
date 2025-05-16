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

#ifndef LIBEXACTREAL_NUMBER_FIELD_HPP
#define LIBEXACTREAL_NUMBER_FIELD_HPP

#include <gmpxx.h>

#include <boost/intrusive_ptr.hpp>
#include <boost/operators.hpp>
#include <e-antic/renfxx_fwd.hpp>
#include <memory>
#include <optional>

#include "rational_field.hpp"

namespace exactreal {

/// Models some features of a number field with an embedding into the real. In
/// practice you won't be using this struct directly, but just pass it into the
/// \ref exactreal::Module::make "factory" of \ref exactreal::Module to specify
/// that the module should be considered over a certain number field.
///
///     #include <exact-real/number_field.hpp>
///     #include <exact-real/module.hpp>
///     #include <exact-real/real_number.hpp>
///     #include <e-antic/renf_class.hpp>
///     #include <e-antic/renf_elem_class.hpp>
///     #include <gmpxx.h>
///
///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
///     auto M = exactreal::Module<exactreal::NumberField>::make({exactreal::RealNumber::rational(1)}, exactreal::NumberField{K});
///     *M->ring().parameters
///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
///
struct LIBEXACTREAL_API NumberField
#ifndef DOXYGEN_DOCUMENTATION_BUILD
  : boost::equality_comparable<NumberField>
#endif
{
  /// Create the trivial number field, i.e., the rational field.
  ///
  ///     exactreal::NumberField Q;
  ///     *Q.parameters
  ///     // -> NumberField(a-1, [+/- 2.01])
  ///
  NumberField();

  /// Create the number field given by an e-antic field.
  ///
  ///     auto K = exactreal::NumberField{*eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     *K.parameters
  ///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
  ///
  NumberField(const eantic::renf_class&);

  /// Create the number field given by an e-antic field.
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     *K.parameters
  ///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
  ///
  NumberField(boost::intrusive_ptr<const eantic::renf_class>);

  /// Create the number field containing this number field element.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     auto N = exactreal::NumberField{K->one()};
  ///     *N.parameters
  ///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
  ///
  NumberField(const eantic::renf_elem_class&);

  /// Return the e-antic number field underlying this number field.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField{K}.parameters == K
  ///     // -> true
  ///
  boost::intrusive_ptr<const eantic::renf_class> parameters;

  /// Return the smallest number field containing \p lhs and \p rhs.
  ///
  /// Note that this method is only implemented in trivial cases currently.
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{};
  ///     
  ///     auto M = exactreal::NumberField::compositum(K, L);
  ///     *M.parameters
  ///     // -> NumberField(x^2 - 2, [1.414213562373095048801688724209698 +/- 1.96e-34])
  ///
  static NumberField compositum(const NumberField& lhs, const NumberField& rhs);

  /// The type modeling the elements of this field, i.e., e-antic's
  /// \rst{:cpp:class:`~eantic::renf_elem_class`.}
  typedef eantic::renf_elem_class ElementClass;

  /// Whether this number field contains the rational field, i.e., `true`.
  static constexpr bool contains_rationals = true;

  /// Return the number field element \p x as an element of this field.
  ///
  /// Note that this is only implemented in trivial cases.
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{eantic::renf_class::make("y^3 - 2", "y", "1.4 +/- 1")};
  ///     auto x = K.coerce(L.parameters->one());
  ///     x.parent() == *K.parameters
  ///     // -> true
  ///
  ElementClass coerce(const ElementClass& x) const;

  /// Whether this number field is a field, i.e., `true`.
  static constexpr bool isField = true;

  /// Return whether the element \p x is a unit, i.e., whether it is non-zero.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::unit(K->gen())
  ///     // -> true
  ///
  ///     exactreal::NumberField::unit(K->zero())
  ///     // -> false
  ///     
  static bool unit(const ElementClass& x);

  /// Return the integer floor of this number field element.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::floor(K->gen())
  ///     // -> 1
  ///
  static mpz_class floor(const ElementClass& x);

  /// Return an approximation to the number field element \p x in ball arithmetic.
  ///
  ///     #include <exact-real/arb.hpp>
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::arb(K->gen(), 64)
  ///     // -> [1.41421 +/- 3.57e-6]
  ///
  static Arb arb(const ElementClass& x, long prec);

  /// Return the number field element \p x as a rational number, if possible.
  ///
  ///     auto K = eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1");
  ///     exactreal::NumberField::rational(K->gen()).has_value()
  ///     // -> false
  ///
  ///     exactreal::NumberField::rational(K->one()).value()
  ///     // -> 1
  ///     
  static std::optional<mpq_class> rational(const ElementClass& x);

  /// Multiply the number field element \p x with \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, a GMP rational,
  /// or another number field element.
  template <typename T>
  static ElementClass& imul(ElementClass& x, const T& t);

  /// Divide the number field element \p x by \p t.
  ///
  /// The argument may be a primitive integer, a GMP integer, a GMP rational,
  /// or another number field element.
  template <typename T>
  static ElementClass& idiv(ElementClass& x, const T& t);

  /// Return whether two number fields are identical.
  ///
  ///     auto K = exactreal::NumberField{eantic::renf_class::make("x^2 - 2", "x", "1.4 +/- 1")};
  ///     auto L = exactreal::NumberField{eantic::renf_class::make("y^2 - 2", "y", "1.4 +/- 1")};
  ///     K == L
  ///     // -> false
  ///
  bool operator==(const NumberField&) const;
};

}  // namespace exactreal

namespace std {
template <>
struct hash<exactreal::NumberField> {
  size_t operator()(const exactreal::NumberField&) const;
};
}  // namespace std

#endif
