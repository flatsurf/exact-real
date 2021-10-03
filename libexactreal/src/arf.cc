/**********************************************************************
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
 *********************************************************************/

#include <ostream>

#include "../exact-real/arf.hpp"

#include <arf.h>

#include "external/gmpxxll/gmpxxll/mpz_class.hpp"
#include "util/integer.ipp"

namespace {
std::pair<mpz_class, mpz_class> mantissa_exponent(const arf_t arf) {
  fmpz_t mantissa;
  fmpz_t exponent;
  fmpz_init(mantissa);
  fmpz_init(exponent);

  arf_get_fmpz_2exp(mantissa, exponent, arf);

  mpz_class m, e;
  fmpz_get_mpz(m.get_mpz_t(), mantissa);
  fmpz_get_mpz(e.get_mpz_t(), exponent);

  fmpz_clear(mantissa);
  fmpz_clear(exponent);

  return std::make_pair(m, e);
}

}  // namespace

namespace exactreal {
Arf::Arf() noexcept { arf_init(t); }

Arf::Arf(const std::string& mantissa, int base, long exponent) : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  fmpz_set_str(m, mantissa.c_str(), base);
  fmpz_set_si(e, exponent);
  arf_set_fmpz_2exp(t, m, e);

  fmpz_clear(e);
  fmpz_clear(m);
}

Arf::Arf(const mpz_class& mantissa, long exponent) : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  fmpz_set_mpz(m, mantissa.get_mpz_t());
  fmpz_set_si(e, exponent);
  arf_set_fmpz_2exp(t, m, e);

  fmpz_clear(e);
  fmpz_clear(m);
}

Arf::Arf(short value) : Arf(static_cast<long>(value)) {}

Arf::Arf(unsigned short value) : Arf(static_cast<unsigned long>(value)) {}

Arf::Arf(int value) : Arf(static_cast<long>(value)) {}

Arf::Arf(unsigned int value) : Arf(static_cast<unsigned long>(value)) {}

Arf::Arf(long value) : Arf() {
  arf_set_si(t, value);
}

Arf::Arf(unsigned long value) : Arf() {
  arf_set_ui(t, value);
}

Arf::Arf(long long value) : Arf() {
  *this = value;
}

Arf::Arf(unsigned long long value) : Arf() {
  *this = value;
}

Arf::Arf(const Arf& value) noexcept : Arf() { arf_set(t, value.t); }

Arf::Arf(double value) : Arf() { arf_set_d(t, value); }

Arf::Arf(Arf&& value) noexcept : Arf() { this->operator=(std::move(value)); }

Arf::~Arf() noexcept { arf_clear(t); }

arf_t& Arf::arf_t() { return t; }

const arf_t& Arf::arf_t() const { return t; }

Arf::operator double() const { return arf_get_d(t, ARF_RND_NEAR); }

Arf& Arf::operator=(const Arf& rhs) noexcept {
  arf_set(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(Arf&& rhs) noexcept {
  arf_swap(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(int rhs) {
  arf_set_si(t, rhs);
  return *this;
}

Arf& Arf::operator=(long rhs) {
  arf_set_si(t, rhs);
  return *this;
}

Arf& Arf::operator<<=(const long rhs) {
  arf_mul_2exp_si(t, t, rhs);
  return *this;
}

Arf& Arf::operator>>=(const long rhs) { return this->operator<<=(-rhs); }

bool operator<(const Arf& lhs, const Arf& rhs) { return arf_cmp(lhs.t, rhs.t) < 0; }

bool operator==(const Arf& lhs, const Arf& rhs) { return arf_equal(lhs.t, rhs.t); }

bool operator<(const Arf& lhs, long rhs) { return arf_cmp_si(lhs.t, rhs) < 0; }

bool operator>(const Arf& lhs, long rhs) { return arf_cmp_si(lhs.t, rhs) > 0; }

bool operator==(const Arf& lhs, long rhs) { return arf_equal_si(lhs.t, rhs); }

Arf Arf::operator-() const {
  Arf ret;
  arf_neg(ret.t, t);
  return ret;
}

Arf Arf::abs() const {
  Arf ret;
  arf_abs(ret.t, t);
  return ret;
}

mpz_class Arf::floor() const {
  Arf floor;
  arf_floor(floor.t, t);
  fmpz_t fmpz_floor;
  fmpz_init(fmpz_floor);
  arf_get_fmpz(fmpz_floor, floor.t, ARF_RND_NEAR);
  mpz_class ret;
  fmpz_get_mpz(ret.get_mpz_t(), fmpz_floor);
  fmpz_clear(fmpz_floor);
  return ret;
}

mpz_class Arf::ceil() const {
  Arf ceil;
  arf_ceil(ceil.t, t);
  fmpz_t fmpz_ceil;
  fmpz_init(fmpz_ceil);
  arf_get_fmpz(fmpz_ceil, ceil.t, ARF_RND_NEAR);
  mpz_class ret;
  fmpz_get_mpz(ret.get_mpz_t(), fmpz_ceil);
  fmpz_clear(fmpz_ceil);
  return ret;
}

mpz_class Arf::mantissa() const { return mantissa_exponent(t).first; }

mpz_class Arf::exponent() const { return mantissa_exponent(t).second; }

long Arf::logb() const {
  Arf _;
  fmpz_t e;
  fmpz_init(e);
  arf_frexp(_.t, e, t);
  long ret = fmpz_get_si(e) - 1;
  fmpz_clear(e);
  return ret;
}

Arf Arf::randtest(flint::frandxx& state, prec precision, prec magbits) {
  Arf ret;
  arf_randtest(ret.arf_t(), state._data(), precision, magbits);
  return ret;
}

std::ostream& operator<<(std::ostream& os, const Arf& self) {
  if (arf_is_zero(self.t)) {
    return os << 0;
  } else if (arf_is_pos_inf(self.t)) {
    return os << "+∞";
  } else if (arf_is_neg_inf(self.t)) {
    return os << "-∞";
  } else if (arf_is_nan(self.t)) {
    return os << "NaN";
  } else if (self.exponent() >= 0) {
    return os << static_cast<double>(self);
  }

  os << static_cast<double>(self) << "=" << self.mantissa() << "p" << self.exponent();

  return os;
}

}  // namespace exactreal

namespace std {

size_t hash<exactreal::Arf>::operator()(const exactreal::Arf& self) const {
  return hash<double>()(static_cast<double>(self));
}

}  // namespace std
