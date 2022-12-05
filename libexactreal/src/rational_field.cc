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

#include "../exact-real/rational_field.hpp"

#include "../exact-real/arb.hpp"

#include "external/gmpxxll/gmpxxll/mpz_class.hpp"

namespace exactreal {

RationalField::RationalField() {}

RationalField::RationalField(const mpq_class&) {}

RationalField RationalField::compositum(const RationalField&, const RationalField&) {
  return RationalField();
}

Arb RationalField::arb(const ElementClass& x, prec prec) {
  return Arb(x, prec);
}

bool RationalField::unit(const ElementClass& x) {
  return x != 0;
}

std::optional<mpq_class> RationalField::rational(const ElementClass& x) {
  return x;
}

mpz_class RationalField::floor(const ElementClass& x) {
  return x.get_num() / x.get_den();
}

RationalField::ElementClass RationalField::coerce(const ElementClass& x) const {
  return x;
}

bool RationalField::operator==(const RationalField&) const {
  return true;
}

template <typename T>
RationalField::ElementClass& RationalField::imul(RationalField::ElementClass& lhs, const T& rhs) {
  if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>) {
    return lhs *= gmpxxll::mpz_class(rhs);
  } else {
    return lhs *= rhs;
  }
}

template <typename T>
RationalField::ElementClass& RationalField::idiv(RationalField::ElementClass& lhs, const T& rhs) {
  if (!rhs)
    throw std::invalid_argument("division by zero");

  if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>) {
    return lhs /= gmpxxll::mpz_class(rhs);
  } else {
    return lhs /= rhs;
  }
}

}  // namespace exactreal

namespace std {

size_t hash<exactreal::RationalField>::operator()(const exactreal::RationalField&) const {
  return 0;
}

}

// Instantiations of templates so implementations are generated for the linker
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<char>(RationalField::ElementClass&, const char&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<unsigned char>(RationalField::ElementClass&, const unsigned char&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<short>(RationalField::ElementClass&, const short&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<unsigned short>(RationalField::ElementClass&, const unsigned short&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<int>(RationalField::ElementClass&, const int&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<unsigned int>(RationalField::ElementClass&, const unsigned int&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<long>(RationalField::ElementClass&, const long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<unsigned long>(RationalField::ElementClass&, const unsigned long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<long long>(RationalField::ElementClass&, const long long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<unsigned long long>(RationalField::ElementClass&, const unsigned long long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<mpz_class>(RationalField::ElementClass&, const mpz_class&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::imul<mpq_class>(RationalField::ElementClass&, const mpq_class&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<char>(RationalField::ElementClass&, const char&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<unsigned char>(RationalField::ElementClass&, const unsigned char&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<short>(RationalField::ElementClass&, const short&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<unsigned short>(RationalField::ElementClass&, const unsigned short&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<int>(RationalField::ElementClass&, const int&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<unsigned int>(RationalField::ElementClass&, const unsigned int&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<long>(RationalField::ElementClass&, const long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<unsigned long>(RationalField::ElementClass&, const unsigned long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<long long>(RationalField::ElementClass&, const long long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<unsigned long long>(RationalField::ElementClass&, const unsigned long long&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<mpz_class>(RationalField::ElementClass&, const mpz_class&);
template exactreal::RationalField::ElementClass& exactreal::RationalField::idiv<mpq_class>(RationalField::ElementClass&, const mpq_class&);
