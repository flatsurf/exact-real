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

// Implements e-antic number field arithmetic for coefficients in expressions
// making up an Element.
class LIBEXACTREAL_API NumberField : boost::equality_comparable<NumberField> {
 public:
  NumberField();
  NumberField(const eantic::renf_class&);
  NumberField(boost::intrusive_ptr<const eantic::renf_class>);
  NumberField(const eantic::renf_elem_class&);

  boost::intrusive_ptr<const eantic::renf_class> parameters;

  static NumberField compositum(const NumberField& lhs, const NumberField& rhs);

  typedef eantic::renf_elem_class ElementClass;

  static constexpr bool contains_rationals = true;

  ElementClass coerce(const ElementClass&) const;

  static constexpr bool isField = true;
  static bool unit(const ElementClass& x);
  static mpz_class floor(const ElementClass& x);
  static Arb arb(const ElementClass& x, long prec);
  static std::optional<mpq_class> rational(const ElementClass& x);

  // Return the result of exact multiplication of an ElementClass.
  template <typename T>
  static ElementClass& imul(ElementClass&, const T&);

  // Return the result of exact division of an ElementClass.
  template <typename T>
  static ElementClass& idiv(ElementClass&, const T&);

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
