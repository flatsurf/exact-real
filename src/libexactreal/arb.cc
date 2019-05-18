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
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <ostream>

#include "exact-real/arb.hpp"
#include "exact-real/arf.hpp"
#include "exact-real/yap/arb.hpp"

using eantic::renf_elem_class;
using std::ostream;

namespace exactreal {
Arb::Arb() noexcept { arb_init(arb_t()); }

Arb::Arb(const slong x) noexcept : Arb() { arb_set_si(arb_t(), x); }

Arb::Arb(const int x) noexcept : Arb() { arb_set_si(arb_t(), x); }

Arb::Arb(const long long x) noexcept : Arb(mpz_class(boost::lexical_cast<std::string>(x))) {}

Arb::Arb(const Arb& arb) noexcept : Arb() { arb_set(arb_t(), arb.arb_t()); }

Arb::Arb(Arb&& arb) noexcept {
  *t = *arb.t;
  arb_init(arb.t);
}

Arb::Arb(const std::pair<Arf, Arf>& bounds, const mp_limb_signed_t precision) : Arb() {
  arb_set_interval_arf(arb_t(), bounds.first.arf_t(), bounds.second.arf_t(), precision);
}

Arb::Arb(const Arf& midpoint) : Arb() {
  arb_set_arf(arb_t(), midpoint.arf_t());
}

Arb::Arb(const mpq_class& rat, const mp_limb_signed_t precision) noexcept : Arb() {
  fmpq_t x;
  fmpq_init_set_readonly(x, rat.get_mpq_t());
  arb_set_fmpq(arb_t(), x, precision);
  fmpq_clear_readonly(x);
}

Arb::Arb(const mpz_class& value) noexcept : Arb() {
  fmpz_t x;
  fmpz_init_set_readonly(x, value.get_mpz_t());
  arb_set_fmpz(arb_t(), x);
  fmpz_clear_readonly(x);
}

Arb::Arb(const std::string& value, const prec precision) : Arb() { arb_set_str(arb_t(), value.c_str(), precision); }

Arb::Arb(const renf_elem_class& renf, const mp_limb_signed_t precision) noexcept : Arb() {
  if (renf.is_fmpq()) {
    arb_set_fmpq(arb_t(), renf.fmpq_t(), precision);
  } else {
    renf_refine_embedding(renf.parent().renf_t(), precision);
    renf_elem_set_evaluation(renf.renf_elem_t(), renf.parent().renf_t(), precision);
    arb_set(arb_t(), renf.renf_elem_t()->emb);
  }
}

Arb::~Arb() noexcept { arb_clear(arb_t()); }

Arb Arb::randtest_exact(flint::frandxx& state, prec precision, prec magbits) noexcept {
  Arb ret;
  arb_randtest_exact(ret.arb_t(), state._data(), precision, magbits);
  return ret;
}

Arb Arb::randtest(flint::frandxx& state, prec precision, prec magbits) noexcept {
  Arb ret;
  arb_randtest(ret.arb_t(), state._data(), precision, magbits);
  return ret;
}

Arb Arb::zero_pm_inf() noexcept {
  Arb ret;
  arb_zero_pm_inf(ret.arb_t());
  return ret;
}

arb_t& Arb::arb_t() noexcept { return t; }

const arb_t& Arb::arb_t() const noexcept { return t; }

bool Arb::is_exact() const noexcept { return arb_is_exact(arb_t()); }

bool Arb::is_finite() const noexcept { return arb_is_finite(arb_t()); }

std::optional<bool> Arb::operator<(const Arb& rhs) const noexcept {
  if (arb_lt(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ge(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return {};
  }
}

std::optional<bool> Arb::operator>(const Arb& rhs) const noexcept {
  if (arb_gt(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_le(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return {};
  }
}

std::optional<bool> Arb::operator<=(const Arb& rhs) const noexcept {
  auto gt = *this > rhs;
  if (gt.has_value()) {
    return !*gt;
  } else {
    return {};
  }
}

std::optional<bool> Arb::operator>=(const Arb& rhs) const noexcept {
  auto lt = *this < rhs;
  if (lt.has_value()) {
    return !*lt;
  } else {
    return {};
  }
}

std::optional<bool> Arb::operator==(const Arb& rhs) const noexcept {
  if (arb_eq(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ne(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return {};
  }
}

std::optional<bool> Arb::operator!=(const Arb& rhs) const noexcept {
  auto eq = this->operator==(rhs);
  if (eq) {
    return !*eq;
  } else {
    return {};
  }
}

bool Arb::equal(const Arb& rhs) const noexcept { return arb_equal(arb_t(), rhs.arb_t()); }

std::optional<bool> Arb::operator<(const long long rhs) const noexcept { return *this < Arb(rhs); }

std::optional<bool> Arb::operator>(const long long rhs) const noexcept { return *this > Arb(rhs); }

std::optional<bool> Arb::operator<=(const long long rhs) const noexcept { return *this <= Arb(rhs); }

std::optional<bool> Arb::operator>=(const long long rhs) const noexcept { return *this >= Arb(rhs); }

std::optional<bool> Arb::operator<(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs < Arb(rhs.get_num());
}

std::optional<bool> Arb::operator>(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs > Arb(rhs.get_num());
}

std::optional<bool> Arb::operator<=(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs <= Arb(rhs.get_num());
}

std::optional<bool> Arb::operator>=(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs >= Arb(rhs.get_num());
}

Arb& Arb::operator=(const Arb& rhs) noexcept {
  arb_set(arb_t(), rhs.arb_t());
  return *this;
}

Arb& Arb::operator=(Arb&& rhs) noexcept {
  arb_swap(arb_t(), rhs.arb_t());
  return *this;
}

Arb::operator std::pair<Arf, Arf>() const noexcept {
  std::pair<Arf, Arf> ret;
  arb_get_interval_arf(ret.first.arf_t(), ret.second.arf_t(), arb_t(), arb_bits(arb_t()));
  return ret;
}

Arb::operator double() const noexcept { return arf_get_d(arb_midref(arb_t()), ARF_RND_NEAR); }

ostream& operator<<(ostream& os, const Arb& self) { return os << arb_get_str(self.arb_t(), os.precision(), 0); }

}  // namespace exactreal
