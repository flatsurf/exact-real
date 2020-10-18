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

#include "../exact-real/arf.hpp"

#include <arf.h>

#include <ostream>

using std::ostream;
using std::string;

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

Arf::Arf(const string& mantissa, int base, long exponent) : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  fmpz_set_str(m, mantissa.c_str(), base);
  fmpz_set_si(e, exponent);
  arf_set_fmpz_2exp(t, m, e);

  fmpz_clear(e);
  fmpz_clear(m);
}

Arf::Arf(const mpz_class& mantissa, long exponent) noexcept : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  fmpz_set_mpz(m, mantissa.get_mpz_t());
  fmpz_set_si(e, exponent);
  arf_set_fmpz_2exp(t, m, e);

  fmpz_clear(e);
  fmpz_clear(m);
}

Arf::Arf(const long value) noexcept : Arf() { arf_set_si(t, value); }

Arf::Arf(const int value) noexcept : Arf(static_cast<long>(value)) {}

Arf::Arf(const Arf& value) noexcept : Arf() { arf_set(t, value.t); }

Arf::Arf(const double value) noexcept : Arf() { arf_set_d(t, value); }

Arf::Arf(Arf&& value) noexcept : Arf() { this->operator=(std::move(value)); }

Arf::~Arf() noexcept { arf_clear(t); }

arf_t& Arf::arf_t() noexcept { return t; }

const arf_t& Arf::arf_t() const noexcept { return t; }

Arf::operator double() const noexcept { return arf_get_d(t, ARF_RND_NEAR); }

Arf& Arf::operator=(const Arf& rhs) noexcept {
  arf_set(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(Arf&& rhs) noexcept {
  arf_swap(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(int rhs) noexcept {
  arf_set_si(t, rhs);
  return *this;
}

Arf& Arf::operator=(long rhs) noexcept {
  arf_set_si(t, rhs);
  return *this;
}

Arf& Arf::operator<<=(const long rhs) noexcept {
  arf_mul_2exp_si(t, t, rhs);
  return *this;
}

Arf& Arf::operator>>=(const long rhs) noexcept { return this->operator<<=(-rhs); }

bool Arf::operator<(const Arf& rhs) const noexcept { return arf_cmp(t, rhs.t) < 0; }

bool Arf::operator==(const Arf& rhs) const noexcept { return arf_equal(t, rhs.t); }

bool Arf::operator<(long rhs) const noexcept { return arf_cmp_si(t, rhs) < 0; }

bool Arf::operator>(long rhs) const noexcept { return arf_cmp_si(t, rhs) > 0; }

bool Arf::operator==(long rhs) const noexcept { return arf_equal_si(t, rhs); }

Arf Arf::operator-() const noexcept {
  Arf ret;
  arf_neg(ret.t, t);
  return ret;
}

Arf Arf::abs() const noexcept {
  Arf ret;
  arf_abs(ret.t, t);
  return ret;
}

mpz_class Arf::floor() const noexcept {
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

mpz_class Arf::ceil() const noexcept {
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

mpz_class Arf::mantissa() const noexcept { return mantissa_exponent(t).first; }

mpz_class Arf::exponent() const noexcept { return mantissa_exponent(t).second; }

long Arf::logb() const noexcept {
  Arf _;
  fmpz_t e;
  fmpz_init(e);
  arf_frexp(_.t, e, t);
  long ret = fmpz_get_si(e) - 1;
  fmpz_clear(e);
  return ret;
}

Arf Arf::randtest(flint::frandxx& state, prec precision, prec magbits) noexcept {
  Arf ret;
  arf_randtest(ret.arf_t(), state._data(), precision, magbits);
  return ret;
}

ostream& operator<<(ostream& os, const Arf& self) {
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
