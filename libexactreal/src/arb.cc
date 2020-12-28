/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019-2020 Julian Rüth
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

using eantic::renf_elem_class;
using std::ostream;

namespace exactreal {
namespace {
template <typename Integer>
auto to_supported_integer(Integer value) noexcept {
  using S = std::remove_cv_t<std::remove_reference_t<Integer>>;

  using Long = std::conditional_t<std::numeric_limits<S>::is_signed, slong, ulong>;
  if constexpr (std::numeric_limits<Long>::min() <= std::numeric_limits<S>::min() && std::numeric_limits<Long>::max() >= std::numeric_limits<S>::max()) {
    // We can safely cast to a supported type without overflow
    return static_cast<Long>(value);
  } else {
    return gmpxxll::mpz_class(value);
  }
}
}  // namespace

Arb::Arb() noexcept { arb_init(arb_t()); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
Arb::Arb(const Integer x) noexcept : Arb() {
  *this = to_supported_integer(x);
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
  renf_refine_embedding(renf.parent().renf_t(), precision);
  renf_elem_set_evaluation(renf.renf_elem_t(), renf.parent().renf_t(), precision);
  arb_set(arb_t(), renf.renf_elem_t()->emb);
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

Arb Arb::zero() noexcept {
  return Arb();
}

Arb Arb::one() noexcept {
  return Arb(1);
}

Arb Arb::pos_inf() noexcept {
  return Arb(Arf(1. / 0.));
}

Arb Arb::neg_inf() noexcept {
  return Arb(Arf(-1. / 0.));
}

Arb Arb::zero_pm_inf() noexcept {
  Arb ret;
  arb_zero_pm_inf(ret.arb_t());
  return ret;
}

Arb Arb::indeterminate() noexcept {
  Arb ret;
  arb_indeterminate(ret.arb_t());
  return ret;
}

arb_t& Arb::arb_t() noexcept { return t; }

const arb_t& Arb::arb_t() const noexcept { return t; }

bool Arb::is_exact() const noexcept { return arb_is_exact(arb_t()); }

bool Arb::is_finite() const noexcept { return arb_is_finite(arb_t()); }

Arb Arb::operator-() const noexcept {
  Arb ret;
  arb_neg(ret.arb_t(), arb_t());
  return ret;
}

std::optional<bool> Arb::operator<(const Arb& rhs) const noexcept {
  if (arb_lt(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ge(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> Arb::operator>(const Arb& rhs) const noexcept {
  if (arb_gt(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_le(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> Arb::operator<=(const Arb& rhs) const noexcept {
  auto gt = *this > rhs;
  if (gt.has_value()) {
    return !*gt;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> Arb::operator>=(const Arb& rhs) const noexcept {
  auto lt = *this < rhs;
  if (lt.has_value()) {
    return !*lt;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> Arb::operator==(const Arb& rhs) const noexcept {
  if (arb_eq(arb_t(), rhs.arb_t())) {
    return true;
  } else if (arb_ne(arb_t(), rhs.arb_t())) {
    return false;
  } else {
    return std::nullopt;
  }
}

std::optional<bool> Arb::operator!=(const Arb& rhs) const noexcept {
  auto eq = this->operator==(rhs);
  if (eq) {
    return !*eq;
  } else {
    return std::nullopt;
  }
}

bool Arb::equal(const Arb& rhs) const noexcept { return arb_equal(arb_t(), rhs.arb_t()); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator<(const Integer rhs) const noexcept { return *this < Arb(rhs); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator>(const Integer rhs) const noexcept { return *this > Arb(rhs); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator<=(const Integer rhs) const noexcept { return *this <= Arb(rhs); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator>=(const Integer rhs) const noexcept { return *this >= Arb(rhs); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator==(const Integer rhs) const noexcept { return *this == Arb(rhs); }

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
std::optional<bool> Arb::operator!=(const Integer rhs) const noexcept { return *this != Arb(rhs); }

std::optional<bool> Arb::operator==(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs == Arb(rhs.get_num());
}

std::optional<bool> Arb::operator!=(const mpq_class& rhs) const noexcept {
  Arb lhs = (*this * Arb(rhs.get_den()))(ARF_PREC_EXACT);
  return lhs == Arb(rhs.get_num());
}

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

template <typename Integer, typename std::enable_if_t<std::is_integral_v<Integer>, int>>
Arb& Arb::operator=(Integer rhs) noexcept {
  auto y = to_supported_integer(rhs);
  if constexpr (std::is_same_v<decltype(y), slong>) {
    arb_set_si(arb_t(), y);
  } else if constexpr (std::is_same_v<decltype(y), ulong>) {
    arb_set_ui(arb_t(), y);
  } else {
    *this = y;
  }
  return *this;
}

Arb& Arb::operator=(const mpz_class& rhs) noexcept {
  fmpz_t z;
  fmpz_init_set_readonly(z, rhs.get_mpz_t());
  arb_set_fmpz(arb_t(), z);
  fmpz_clear_readonly(z);
  return *this;
}

Arb::operator std::pair<Arf, Arf>() const noexcept {
  std::pair<Arf, Arf> ret;
  arb_get_interval_arf(ret.first.arf_t(), ret.second.arf_t(), arb_t(), arb_rel_accuracy_bits(arb_t()));
  return ret;
}

Arb::operator double() const noexcept { return arf_get_d(arb_midref(arb_t()), ARF_RND_NEAR); }

ostream& operator<<(ostream& os, const Arb& self) {
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

template Arb::Arb(short);
template Arb::Arb(unsigned short);
template Arb::Arb(int);
template Arb::Arb(unsigned int);
template Arb::Arb(long);
template Arb::Arb(unsigned long);
template Arb::Arb(long long);
template Arb::Arb(unsigned long long);

template Arb& Arb::operator=(short);
template Arb& Arb::operator=(unsigned short);
template Arb& Arb::operator=(int);
template Arb& Arb::operator=(unsigned int);
template Arb& Arb::operator=(long);
template Arb& Arb::operator=(unsigned long);
template Arb& Arb::operator=(long long);
template Arb& Arb::operator=(unsigned long long);

template std::optional<bool> Arb::operator==(short) const noexcept;
template std::optional<bool> Arb::operator==(unsigned short) const noexcept;
template std::optional<bool> Arb::operator==(int) const noexcept;
template std::optional<bool> Arb::operator==(unsigned int) const noexcept;
template std::optional<bool> Arb::operator==(long) const noexcept;
template std::optional<bool> Arb::operator==(unsigned long) const noexcept;
template std::optional<bool> Arb::operator==(long long) const noexcept;
template std::optional<bool> Arb::operator==(unsigned long long) const noexcept;

template std::optional<bool> Arb::operator!=(short) const noexcept;
template std::optional<bool> Arb::operator!=(unsigned short) const noexcept;
template std::optional<bool> Arb::operator!=(int) const noexcept;
template std::optional<bool> Arb::operator!=(unsigned int) const noexcept;
template std::optional<bool> Arb::operator!=(long) const noexcept;
template std::optional<bool> Arb::operator!=(unsigned long) const noexcept;
template std::optional<bool> Arb::operator!=(long long) const noexcept;
template std::optional<bool> Arb::operator!=(unsigned long long) const noexcept;

template std::optional<bool> Arb::operator<(short) const noexcept;
template std::optional<bool> Arb::operator<(unsigned short) const noexcept;
template std::optional<bool> Arb::operator<(int) const noexcept;
template std::optional<bool> Arb::operator<(unsigned int) const noexcept;
template std::optional<bool> Arb::operator<(long) const noexcept;
template std::optional<bool> Arb::operator<(unsigned long) const noexcept;
template std::optional<bool> Arb::operator<(long long) const noexcept;
template std::optional<bool> Arb::operator<(unsigned long long) const noexcept;

template std::optional<bool> Arb::operator>(short) const noexcept;
template std::optional<bool> Arb::operator>(unsigned short) const noexcept;
template std::optional<bool> Arb::operator>(int) const noexcept;
template std::optional<bool> Arb::operator>(unsigned int) const noexcept;
template std::optional<bool> Arb::operator>(long) const noexcept;
template std::optional<bool> Arb::operator>(unsigned long) const noexcept;
template std::optional<bool> Arb::operator>(long long) const noexcept;
template std::optional<bool> Arb::operator>(unsigned long long) const noexcept;

template std::optional<bool> Arb::operator<=(short) const noexcept;
template std::optional<bool> Arb::operator<=(unsigned short) const noexcept;
template std::optional<bool> Arb::operator<=(int) const noexcept;
template std::optional<bool> Arb::operator<=(unsigned int) const noexcept;
template std::optional<bool> Arb::operator<=(long) const noexcept;
template std::optional<bool> Arb::operator<=(unsigned long) const noexcept;
template std::optional<bool> Arb::operator<=(long long) const noexcept;
template std::optional<bool> Arb::operator<=(unsigned long long) const noexcept;

template std::optional<bool> Arb::operator>=(short) const noexcept;
template std::optional<bool> Arb::operator>=(unsigned short) const noexcept;
template std::optional<bool> Arb::operator>=(int) const noexcept;
template std::optional<bool> Arb::operator>=(unsigned int) const noexcept;
template std::optional<bool> Arb::operator>=(long) const noexcept;
template std::optional<bool> Arb::operator>=(unsigned long) const noexcept;
template std::optional<bool> Arb::operator>=(long long) const noexcept;
template std::optional<bool> Arb::operator>=(unsigned long long) const noexcept;

}  // namespace exactreal
