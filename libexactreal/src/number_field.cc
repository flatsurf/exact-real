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

#include "../exact-real/number_field.hpp"

#include <e-antic/renfxx.h>

#include "../exact-real/arb.hpp"

namespace exactreal {
NumberField::NumberField() : NumberField(eantic::renf_class::make()) {}

NumberField::NumberField(const eantic::renf_class& parameters) : parameters(&parameters) {}

NumberField::NumberField(boost::intrusive_ptr<const eantic::renf_class> parameters) : parameters(std::move(parameters)) {}

NumberField::NumberField(const eantic::renf_elem_class& value) : NumberField(value.parent()) {}

NumberField NumberField::compositum(const NumberField& lhs, const NumberField& rhs) {
  if (lhs == rhs) return lhs;
  if (lhs.parameters->degree() == 1) return rhs;
  if (rhs.parameters->degree() == 1) return lhs;
  throw std::logic_error("not implemented: compositum of number fields");
}

Arb NumberField::arb(const ElementClass& x, mp_limb_signed_t prec) {
  return Arb(x, prec);
}

bool NumberField::unit(const ElementClass& x) {
  return x != 0;
}

std::optional<mpq_class> NumberField::rational(const ElementClass& x) {
  return x.is_rational() ? std::optional{static_cast<mpq_class>(x)} : std::nullopt;
}

bool NumberField::operator==(const NumberField& rhs) const {
  return *parameters == *rhs.parameters;
}

mpz_class NumberField::floor(const ElementClass& x) {
  return x.floor();
}

typename NumberField::ElementClass NumberField::coerce(const ElementClass& x) const {
  if (x.parent() == *parameters)
    return x;

  // Use https://github.com/videlec/e-antic/pull/120 instead, once it has been merged.
  if (x.is_rational())
    return ElementClass(*parameters, static_cast<mpq_class>(x));

  throw std::logic_error("not implemented: coercion to this number field");
}

template <typename T>
NumberField::ElementClass& NumberField::imul(NumberField::ElementClass& lhs, const T& rhs) {
  return lhs *= rhs;
}

template <typename T>
NumberField::ElementClass& NumberField::idiv(NumberField::ElementClass& lhs, const T& rhs) {
  if (!rhs)
    throw std::invalid_argument("division by zero");

  return lhs /= rhs;
}

}  // namespace exactreal

namespace std {

size_t hash<exactreal::NumberField>::operator()(const exactreal::NumberField& self) const {
  return std::hash<eantic::renf_class>()(*self.parameters);
}

}  // namespace std

// Instantiations of templates so implementations are generated for the linker
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<char>(NumberField::ElementClass&, const char&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<unsigned char>(NumberField::ElementClass&, const unsigned char&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<short>(NumberField::ElementClass&, const short&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<unsigned short>(NumberField::ElementClass&, const unsigned short&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<int>(NumberField::ElementClass&, const int&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<unsigned int>(NumberField::ElementClass&, const unsigned int&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<long>(NumberField::ElementClass&, const long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<unsigned long>(NumberField::ElementClass&, const unsigned long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<long long>(NumberField::ElementClass&, const long long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<unsigned long long>(NumberField::ElementClass&, const unsigned long long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<mpz_class>(NumberField::ElementClass&, const mpz_class&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<mpq_class>(NumberField::ElementClass&, const mpq_class&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::imul<eantic::renf_elem_class>(NumberField::ElementClass&, const eantic::renf_elem_class&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<char>(NumberField::ElementClass&, const char&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<unsigned char>(NumberField::ElementClass&, const unsigned char&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<short>(NumberField::ElementClass&, const short&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<unsigned short>(NumberField::ElementClass&, const unsigned short&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<int>(NumberField::ElementClass&, const int&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<unsigned int>(NumberField::ElementClass&, const unsigned int&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<long>(NumberField::ElementClass&, const long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<unsigned long>(NumberField::ElementClass&, const unsigned long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<long long>(NumberField::ElementClass&, const long long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<unsigned long long>(NumberField::ElementClass&, const unsigned long long&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<mpz_class>(NumberField::ElementClass&, const mpz_class&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<mpq_class>(NumberField::ElementClass&, const mpq_class&);
template exactreal::NumberField::ElementClass& exactreal::NumberField::idiv<eantic::renf_elem_class>(NumberField::ElementClass&, const eantic::renf_elem_class&);
