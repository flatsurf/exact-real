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

#include "../exact-real/integer_ring.hpp"

#include "../exact-real/arb.hpp"

#include "external/gmpxxll/gmpxxll/mpz_class.hpp"

namespace exactreal {

IntegerRing::IntegerRing() {}

IntegerRing::IntegerRing(const mpz_class&) {}

IntegerRing IntegerRing::compositum(const IntegerRing&, const IntegerRing&) {
  return IntegerRing();
}

Arb IntegerRing::arb(const ElementClass& x, long) {
  return Arb(x);
}

bool IntegerRing::unit(const ElementClass& x) {
  return x == 1 || x == -1;
}

std::optional<mpq_class> IntegerRing::rational(const ElementClass& x) {
  return mpq_class(x);
}

mpz_class IntegerRing::floor(const ElementClass& x) {
  return x;
}

IntegerRing::ElementClass IntegerRing::coerce(const ElementClass& x) const {
  return x;
}

bool IntegerRing::operator==(const IntegerRing&) const {
  return true;
}

template <typename T>
IntegerRing::ElementClass& IntegerRing::imul(IntegerRing::ElementClass& lhs, const T& rhs) {
  if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>) {
    return lhs *= gmpxxll::mpz_class(rhs);
  } else if constexpr (std::is_same_v<T, mpq_class>) {
    return idiv(imul(lhs, rhs.get_num()), rhs.get_den());
  } else {
    return lhs *= rhs;
  }
}

template <typename T>
IntegerRing::ElementClass& IntegerRing::idiv(IntegerRing::ElementClass& lhs, const T& rhs) {
  if (!rhs)
    throw std::invalid_argument("division by zero");

  if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long>) {
    return idiv(lhs, static_cast<mpz_class>(gmpxxll::mpz_class(rhs)));
  } else if constexpr (std::is_same_v<T, mpq_class>) {
    return idiv(imul(lhs, rhs.get_den()), rhs.get_num());
  } else {
    if (lhs % rhs != 0)
      throw std::invalid_argument("cannot divide these integers");
    return lhs /= rhs;
  }
}

}  // namespace exactreal

namespace std {

size_t hash<exactreal::IntegerRing>::operator()(const exactreal::IntegerRing&) const {
  return 0;
}

}

// Instantiations of templates so implementations are generated for the linker
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<short>(IntegerRing::ElementClass&, const short&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<unsigned short>(IntegerRing::ElementClass&, const unsigned short&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<int>(IntegerRing::ElementClass&, const int&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<unsigned int>(IntegerRing::ElementClass&, const unsigned int&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<long>(IntegerRing::ElementClass&, const long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<unsigned long>(IntegerRing::ElementClass&, const unsigned long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<long long>(IntegerRing::ElementClass&, const long long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<unsigned long long>(IntegerRing::ElementClass&, const unsigned long long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<mpz_class>(IntegerRing::ElementClass&, const mpz_class&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::imul<mpq_class>(IntegerRing::ElementClass&, const mpq_class&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<short>(IntegerRing::ElementClass&, const short&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<unsigned short>(IntegerRing::ElementClass&, const unsigned short&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<int>(IntegerRing::ElementClass&, const int&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<unsigned int>(IntegerRing::ElementClass&, const unsigned int&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<long>(IntegerRing::ElementClass&, const long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<unsigned long>(IntegerRing::ElementClass&, const unsigned long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<long long>(IntegerRing::ElementClass&, const long long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<unsigned long long>(IntegerRing::ElementClass&, const unsigned long long&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<mpz_class>(IntegerRing::ElementClass&, const mpz_class&);
template exactreal::IntegerRing::ElementClass& exactreal::IntegerRing::idiv<mpq_class>(IntegerRing::ElementClass&, const mpq_class&);
