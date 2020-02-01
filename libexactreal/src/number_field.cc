/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
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

#include <e-antic/renfxx.h>

#include "../exact-real/arb.hpp"
#include "../exact-real/number_field.hpp"

namespace exactreal {
NumberField::NumberField() : NumberField(eantic::renf_class::make()) {}

NumberField::NumberField(const std::shared_ptr<const eantic::renf_class>& parameters) : parameters(parameters) {
  // parameters might contain a null pointer if this is the rational field
}

NumberField::NumberField(const eantic::renf_elem_class& value) : NumberField(value.parent()) {}

NumberField NumberField::compositum(const NumberField& lhs, const NumberField& rhs) {
  if (lhs == rhs) return lhs;
  if (!lhs.parameters) return rhs;
  if (!rhs.parameters) return lhs;
  throw std::logic_error("not implemented: compositum of number fields");
}

Arb NumberField::arb(const ElementClass& x, mp_limb_signed_t prec) { return Arb(x, prec); }

bool NumberField::operator==(const NumberField& rhs) const { return parameters == rhs.parameters; }

mpz_class NumberField::floor(const ElementClass& x) { return x.floor(); }
}  // namespace exactreal
