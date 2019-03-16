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

#include <arb.h>
#include <e-antic/renfxx.h>
#include <ostream>

#include "exact-real/arb.hpp"
#include "exact-real/arf.hpp"

using std::ostream;

namespace exactreal {
Arb::Arb() { arb_init(t); }

Arb::Arb(const slong x) : Arb() {
  arb_set_si(t, x);
}

Arb::Arb(const Arb& arb) : Arb() {
  arb_set(t, arb.t);
}

Arb::Arb(Arb&& arb) : Arb() {
  arb_swap(t, arb.t);
}

Arb::Arb(const Arf& lower, const Arf& upper, const mp_limb_signed_t precision) : Arb() {
	arb_set_interval_arf(t, lower.t, upper.t, precision);	
}

Arb::Arb(const mpq_class& rat, const mp_limb_signed_t precision) : Arb() {
	fmpq_t x;
	fmpq_init_set_readonly(x, rat.get_mpq_t());
	arb_set_fmpq(t, x, precision);
	fmpq_clear_readonly(x);
}

Arb::Arb(const mpz_class& value) : Arb() {
	fmpz_t x;
	fmpz_init_set_readonly(x, value.get_mpz_t());
	arb_set_fmpz(t, x);
	fmpz_clear_readonly(x);
}

Arb::Arb(const renf_elem_class& renf, const mp_limb_signed_t precision) : Arb() {
  if (renf.is_fmpq()) {
    arb_set_fmpq(t, renf.get_fmpq(), precision);
  } else {
    arb_set(t, renf.get_renf_elem()->emb);
  }
}

Arb::~Arb() { arb_clear(t); }

Arb Arb::any() {
	Arb ret;
	arb_zero_pm_inf(ret.t);
	return ret;
}

bool Arb::isExact() const { return arb_is_exact(t); }

Arb& Arb::iadd(const renf_elem_class& rhs, const mp_limb_signed_t precision) {
  arb_add(t, t, Arb(rhs).t, precision);
  return *this;
}

Arb& Arb::imul(const Arb& rhs, const mp_limb_signed_t precision) {
  arb_mul(t, t, rhs.t, precision);
  return *this;
}

std::optional<int> Arb::cmp(const Arb& rhs) const {
  if (arb_gt(t, rhs.t)) {
    return 1;
  } else if (arb_lt(t, rhs.t)) {
    return -1;
  } else if (arb_eq(t, rhs.t)) {
    return 0;
  } else {
    return {};
  }
}

std::optional<int> Arb::cmp(const long long rhs_) const {
  Arb rhs(rhs_);
  if (arb_gt(t, rhs.t)) {
    return 1;
  } else if (arb_lt(t, rhs.t)) {
    return -1;
  } else if (arb_eq(t, rhs.t)) {
    return 0;
  } else {
    return {};
  }
}

Arb& Arb::iadd(const Arb& rhs, const mp_limb_signed_t precision) {
  arb_add(t, t, rhs.t, precision);
  return *this;
}

Arb& Arb::operator=(const Arb& rhs) {
  arb_set(t, rhs.t);
  return *this;
}

std::pair<Arf, Arf> Arb::arf() const {
	std::pair<Arf, Arf> ret;
	arb_get_interval_arf(ret.first.t, ret.second.t, t, arb_bits(t));
	return ret;	
}

ostream& operator<<(ostream& os, const Arb& self) {
  char* out = arb_get_str(self.t, 64, ARB_STR_CONDENSE);
  os << out;
  delete (out);
  return os;
}

}  // namespace exactreal
