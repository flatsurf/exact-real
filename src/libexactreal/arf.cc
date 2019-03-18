/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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

#include "exact-real/arf.hpp"

#include <arf.h>

using std::ostream;
using std::string;

namespace exactreal {
Arf::Arf() { arf_init(t); }

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

Arf::Arf(const long value) : Arf() { arf_set_si(t, value); }

Arf::Arf(const int value) : Arf(static_cast<long>(value)) {}

Arf::Arf(const Arf& value) : Arf() { arf_set(t, value.t); }

Arf::Arf(const double value) : Arf() { arf_set_d(t, value); }

Arf::Arf(Arf&& value) : Arf() { this->operator=(std::move(value)); }

Arf::~Arf() { arf_clear(t); }

Arf::operator double() const { return arf_get_d(t, ARF_RND_NEAR); }

Arf& Arf::operator=(const Arf& rhs) {
  arf_set(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(Arf&& rhs) {
  arf_swap(t, rhs.t);
  return *this;
}

Arf& Arf::iadd(const Arf& rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_add(t, t, rhs.t, precision, rnd);
  return *this;
}

Arf& Arf::isub(const Arf& rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_sub(t, t, rhs.t, precision, rnd);
  return *this;
}

Arf& Arf::imul(const Arf& rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_mul(t, t, rhs.t, precision, rnd);
  return *this;
}

Arf& Arf::idiv(const Arf& rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_div(t, t, rhs.t, precision, rnd);
  return *this;
}

Arf& Arf::iadd(const long rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_add_si(t, t, rhs, precision, rnd);
  return *this;
}

Arf& Arf::isub(const long rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_sub_si(t, t, rhs, precision, rnd);
  return *this;
}

Arf& Arf::imul(const long rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_mul_si(t, t, rhs, precision, rnd);
  return *this;
}

Arf& Arf::idiv(const long rhs, mp_limb_signed_t precision, arf_rnd_t rnd) {
  arf_div_si(t, t, rhs, precision, rnd);
  return *this;
}

Arf& Arf::operator<<=(const long rhs) {
  arf_mul_2exp_si(t, t, rhs);
  return *this;
}

Arf& Arf::operator>>=(const long rhs) { return this->operator<<=(-rhs); }

bool Arf::operator<(const Arf& rhs) const { return arf_cmp(t, rhs.t) < 0; }

bool Arf::operator==(const Arf& rhs) const { return arf_equal(t, rhs.t); }

bool Arf::operator<(long rhs) const { return arf_cmp_si(t, rhs) < 0; }

bool Arf::operator>(long rhs) const { return arf_cmp_si(t, rhs) > 0; }

bool Arf::operator==(long rhs) const { return arf_equal_si(t, rhs); }

Arf Arf::abs() const {
  Arf ret;
  arf_abs(ret.t, t);
  return ret;
}

mpz_class Arf::mantissa() const {
  fmpz_t mantissa;
  fmpz_t exponent;
  fmpz_init(mantissa);
  fmpz_init(exponent);

  arf_get_fmpz_2exp(mantissa, exponent, t);

  mpz_class m;
  fmpz_get_mpz(m.get_mpz_t(), mantissa);

  fmpz_clear(mantissa);
  fmpz_clear(exponent);

  return m;
}

mpz_class Arf::exponent() const {
  fmpz_t mantissa;
  fmpz_t exponent;
  fmpz_init(mantissa);
  fmpz_init(exponent);

  arf_get_fmpz_2exp(mantissa, exponent, t);

  mpz_class e;
  fmpz_get_mpz(e.get_mpz_t(), exponent);

  fmpz_clear(mantissa);
  fmpz_clear(exponent);

  return e;
}

long Arf::logb() const {
  Arf _;
  fmpz_t e;
  fmpz_init(e);
  arf_frexp(_.t, e, t);
  long ret = fmpz_get_si(e) - 1;
  fmpz_clear(e);
  return ret;
}

ostream& operator<<(ostream& os, const Arf& self) {
  if (arf_is_zero(self.t)) {
    os << 0;
  } else if (arf_is_pos_inf(self.t)) {
    os << "+∞";
  } else if (arf_is_neg_inf(self.t)) {
    os << "-∞";
  } else if (arf_is_nan(self.t)) {
    os << "NaN";
  }

  os << self.mantissa();

  if (self.exponent() != 0) {
    os << "p" << self.exponent();
  }

  if (self.exponent() < 0) {
    os << "[∼" << static_cast<double>(self) << "]";
  }

  return os;
}
}  // namespace exactreal
