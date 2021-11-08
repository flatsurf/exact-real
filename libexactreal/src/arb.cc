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

#include "../exact-real/arb.hpp"

#include <arb.h>
#include <e-antic/renfxx.h>
#include <flint/fmpz.h>

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <ostream>

#include "../exact-real/arf.hpp"
#include "../exact-real/yap/arb.hpp"
#include "external/gmpxxll/gmpxxll/mpz_class.hpp"
#include "util/integer.ipp"

namespace exactreal {

Arb::Arb() noexcept { arb_init(arb_t()); }

Arb::Arb(short x) : Arb(static_cast<long>(x)) {}

Arb::Arb(unsigned short x) : Arb(static_cast<unsigned long>(x)) {}

Arb::Arb(int x) : Arb(static_cast<long>(x)) {}

Arb::Arb(unsigned int x) : Arb(static_cast<unsigned long>(x)) {}

Arb::Arb(long x) : Arb() {
  *this = x;
}

Arb::Arb(unsigned long x) : Arb() {
  *this = x;
}

Arb::Arb(long long x) : Arb() {
  *this = x;
}

Arb::Arb(unsigned long long x) : Arb() {
  *this = x;
}

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

Arb::Arb(const mpq_class& rat) : Arb(rat, ARB_PRECISION_FAST) {}

Arb::Arb(const mpq_class& rat, const mp_limb_signed_t precision) : Arb() {
  fmpq_t x;
  fmpq_init_set_readonly(x, rat.get_mpq_t());
  arb_set_fmpq(arb_t(), x, precision);
  fmpq_clear_readonly(x);
}

Arb::Arb(const mpz_class& value) : Arb() {
  fmpz_t x;
  fmpz_init_set_readonly(x, value.get_mpz_t());
  arb_set_fmpz(arb_t(), x);
  fmpz_clear_readonly(x);
}

Arb::Arb(const std::string& value, const prec precision) : Arb() { arb_set_str(arb_t(), value.c_str(), precision); }

Arb::Arb(const eantic::renf_elem_class& renf) : Arb(renf, ARB_PRECISION_FAST) {}

Arb::Arb(const eantic::renf_elem_class& renf, const mp_limb_signed_t precision) : Arb() {
  renf_refine_embedding(renf.parent().renf_t(), precision);
  renf_elem_set_evaluation(renf.renf_elem_t(), renf.parent().renf_t(), precision);
  arb_set(arb_t(), renf.renf_elem_t()->emb);
}

Arb::~Arb() noexcept { arb_clear(arb_t()); }

Arb Arb::randtest_exact(flint::frandxx& state, prec precision, prec magbits) {
  Arb ret;
  arb_randtest_exact(ret.arb_t(), state._data(), precision, magbits);
  return ret;
}

Arb Arb::randtest(flint::frandxx& state, prec precision, prec magbits) {
  Arb ret;
  arb_randtest(ret.arb_t(), state._data(), precision, magbits);
  return ret;
}

Arb Arb::zero() {
  return Arb();
}

Arb Arb::one() {
  return Arb(1);
}

Arb Arb::pos_inf() {
  Arb ret;
  arb_pos_inf(ret.arb_t());
  return ret;
}

Arb Arb::neg_inf() {
  Arb ret;
  arb_neg_inf(ret.arb_t());
  return ret;
}

Arb Arb::zero_pm_one() {
  Arb ret;
  arb_zero_pm_one(ret.arb_t());
  return ret;
}

Arb Arb::zero_pm_inf() {
  Arb ret;
  arb_zero_pm_inf(ret.arb_t());
  return ret;
}

Arb Arb::indeterminate() {
  Arb ret;
  arb_indeterminate(ret.arb_t());
  return ret;
}

arb_t& Arb::arb_t() { return t; }

const arb_t& Arb::arb_t() const { return t; }

bool Arb::is_exact() const { return arb_is_exact(arb_t()); }

bool Arb::is_finite() const { return arb_is_finite(arb_t()); }

Arb Arb::operator-() const {
  Arb ret;
  arb_neg(ret.arb_t(), arb_t());
  return ret;
}

std::optional<bool> operator<(const Arb& lhs, const Arb& rhs) {
  if (arb_lt(lhs.arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ge(lhs.arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> operator>(const Arb& lhs, const Arb& rhs) {
  if (arb_gt(lhs.arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_le(lhs.arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> operator<=(const Arb& lhs, const Arb& rhs) {
  auto gt = lhs > rhs;
  if (gt.has_value()) {
    return !*gt;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> operator>=(const Arb& lhs, const Arb& rhs) {
  auto lt = lhs < rhs;
  if (lt.has_value()) {
    return !*lt;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> operator==(const Arb& lhs, const Arb& rhs) {
  if (arb_eq(lhs.arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ne(lhs.arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> operator!=(const Arb& lhs, const Arb& rhs) {
  auto eq = operator==(lhs, rhs);
  if (eq) {
    return !*eq;
  } else {
    return std::nullopt;
  }
}

bool Arb::equal(const Arb& rhs) const { return arb_equal(arb_t(), rhs.arb_t()); }

std::optional<bool> operator==(const Arb& lhs, short rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, short rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, short rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, short rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, short rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, short rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(short lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(short lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(short lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(short lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(short lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(short lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, unsigned short rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, unsigned short rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, unsigned short rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, unsigned short rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, unsigned short rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, unsigned short rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(unsigned short lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(unsigned short lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(unsigned short lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(unsigned short lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(unsigned short lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(unsigned short lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, int rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, int rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, int rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, int rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, int rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, int rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(int lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(int lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(int lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(int lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(int lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(int lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, unsigned int rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, unsigned int rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, unsigned int rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, unsigned int rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, unsigned int rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, unsigned int rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(unsigned int lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(unsigned int lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(unsigned int lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(unsigned int lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(unsigned int lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(unsigned int lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, long rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, long rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, long rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, long rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, long rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, long rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(long lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(long lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(long lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(long lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(long lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(long lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, unsigned long rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, unsigned long rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, unsigned long rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, unsigned long rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, unsigned long rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, unsigned long rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(unsigned long lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(unsigned long lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(unsigned long lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(unsigned long lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(unsigned long lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(unsigned long lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, long long rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, long long rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, long long rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, long long rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, long long rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, long long rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(long long lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(long long lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(long long lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(long long lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(long long lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(long long lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, unsigned long long rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, unsigned long long rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, unsigned long long rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, unsigned long long rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, unsigned long long rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, unsigned long long rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(unsigned long long lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, const mpz_class& rhs) { return lhs == Arb(rhs); }
std::optional<bool> operator!=(const Arb& lhs, const mpz_class& rhs) { return lhs != Arb(rhs); }
std::optional<bool> operator<(const Arb& lhs, const mpz_class& rhs) { return lhs < Arb(rhs); }
std::optional<bool> operator>(const Arb& lhs, const mpz_class& rhs) { return lhs > Arb(rhs); }
std::optional<bool> operator<=(const Arb& lhs, const mpz_class& rhs) { return lhs <= Arb(rhs); }
std::optional<bool> operator>=(const Arb& lhs, const mpz_class& rhs) { return lhs >= Arb(rhs); }
std::optional<bool> operator==(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) == rhs; }
std::optional<bool> operator!=(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) != rhs; }
std::optional<bool> operator<(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) < rhs; }
std::optional<bool> operator>(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) > rhs; }
std::optional<bool> operator<=(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) <= rhs; }
std::optional<bool> operator>=(const mpz_class& lhs, const Arb& rhs) { return Arb(lhs) >= rhs; }

std::optional<bool> operator==(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) == rhs.get_num(); }
std::optional<bool> operator!=(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) != rhs.get_num(); }
std::optional<bool> operator<(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) < rhs.get_num(); }
std::optional<bool> operator>(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) > rhs.get_num(); }
std::optional<bool> operator<=(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) <= rhs.get_num(); }
std::optional<bool> operator>=(const Arb& lhs, const mpq_class& rhs) { return (lhs * Arb(rhs.get_den()))(ARF_PREC_EXACT) >= rhs.get_num(); }
std::optional<bool> operator==(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() == (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }
std::optional<bool> operator!=(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() != (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }
std::optional<bool> operator<(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() < (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }
std::optional<bool> operator>(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() > (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }
std::optional<bool> operator<=(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() <= (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }
std::optional<bool> operator>=(const mpq_class& lhs, const Arb& rhs) { return lhs.get_num() >= (rhs * Arb(lhs.get_den()))(ARF_PREC_EXACT); }

Arb& Arb::operator=(const Arb& rhs) noexcept {
  arb_set(arb_t(), rhs.arb_t());
  return *this;
}

Arb& Arb::operator=(Arb&& rhs) noexcept {
  arb_swap(arb_t(), rhs.arb_t());
  return *this;
}

Arb& Arb::operator=(short rhs) {
  return *this = static_cast<long>(rhs);
}

Arb& Arb::operator=(unsigned short rhs) {
  return *this = static_cast<unsigned long>(rhs);
}

Arb& Arb::operator=(int rhs) {
  return *this = static_cast<long>(rhs);
}

Arb& Arb::operator=(unsigned int rhs) {
  return *this = static_cast<unsigned long>(rhs);
}

Arb& Arb::operator=(long rhs) {
  arb_set_si(arb_t(), rhs);
  return *this;
}

Arb& Arb::operator=(unsigned long rhs) {
  arb_set_ui(arb_t(), rhs);
  return *this;
}

Arb& Arb::operator=(long long rhs) {
  return *this = to_supported_integer(rhs);
}

Arb& Arb::operator=(unsigned long long rhs) {
  return *this = to_supported_integer(rhs);
}

Arb& Arb::operator=(const mpz_class& rhs) {
  fmpz_t z;
  fmpz_init_set_readonly(z, rhs.get_mpz_t());
  arb_set_fmpz(arb_t(), z);
  fmpz_clear_readonly(z);
  return *this;
}

Arb::operator std::pair<Arf, Arf>() const {
  std::pair<Arf, Arf> ret;
  arb_get_interval_arf(ret.first.arf_t(), ret.second.arf_t(), arb_t(), arb_rel_accuracy_bits(arb_t()));
  return ret;
}

Arb::operator double() const { return arf_get_d(arb_midref(arb_t()), ARF_RND_NEAR); }

Arb::operator Arf() const {
  Arf midpoint;
  arf_set(midpoint.arf_t(), arb_midref(arb_t()));
  return midpoint;
}

std::ostream& operator<<(std::ostream& os, const Arb& self) {
  // ARB_STR_MORE is essential. Otherwise, arb prints things such as [1.5 +/- .6]
  // as [+/- something] since not a single digit of the midpoint is correct.
  // (At least that's our theory of what's happening…)
  // Note that the radius is also printed with respect to os.precision() which
  // means that the radius might be shortened, e.g., [0.5 +/- 1.08e-78] might
  // print as [0.5 +/- 1e-10].
  char* s = arb_get_str(self.arb_t(), os.precision(), ARB_STR_MORE);
  os << s;
  flint_free(s);
  return os;
}

}  // namespace exactreal
