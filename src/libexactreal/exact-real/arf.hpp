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

#ifndef LIBMODEANTIC_ARF_HPP
#define LIBMODEANTIC_ARF_HPP

#include <arf.h>
#include <gmpxx.h>
#include <boost/operators.hpp>

#include "exact-real/exact-real.hpp"

namespace exactreal {
// A simple wrapper for arf_t elements so we get C++ style memory management.
// We do not fix a precision here yet. However, after some discussion with the
// Arb author, it seems that 64 and 128 are good default precisions. After that
// there is a more noticeable decline in performance.
struct Arf : boost::field_operators<Arf>,
             boost::field_operators<Arf, long>,
             boost::totally_ordered<Arf>,
             boost::totally_ordered<Arf, long>,
             boost::shiftable<Arf, long> {
  Arf();
  Arf(const Arf&);
  Arf(Arf&&);
  Arf(const std::string& mantissa, int base, long exponent);
  Arf(const mpz_class& mantissa, long exponent);
  explicit Arf(const int value);
  explicit Arf(const long value);
  explicit Arf(const double value);
  ~Arf();

  Arf abs() const;

  mpz_class mantissa() const;
  mpz_class exponent() const;
  // log2 rounded down
  long logb() const;

  explicit operator double() const;
  Arf& operator=(const Arf&);
  Arf& operator=(Arf&&);

  Arf& iadd(const Arf& rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator+=(const Arf& rhs) { return iadd(rhs); }
  Arf& isub(const Arf& rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator-=(const Arf& rhs) { return isub(rhs); }
  Arf& imul(const Arf& rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator*=(const Arf& rhs) { return imul(rhs); }
  Arf& idiv(const Arf& rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator/=(const Arf& rhs) { return idiv(rhs); }

  Arf& iadd(const long rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator+=(const long rhs) { return iadd(rhs); }
  Arf& isub(const long rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator-=(const long rhs) { return isub(rhs); }
  Arf& imul(const long rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator*=(const long rhs) { return imul(rhs); }
  Arf& idiv(const long rhs, const mp_limb_signed_t precision = 64,
            arf_rnd_t rnd = ARF_RND_NEAR);
  Arf& operator/=(const long rhs) { return idiv(rhs); }

  Arf& operator>>=(const long rhs);
  Arf& operator<<=(const long rhs);

  bool operator<(const Arf& rhs) const;
  bool operator==(const Arf& rhs) const;

  bool operator<(long rhs) const;
  bool operator>(long rhs) const;
  bool operator==(long rhs) const;

  friend std::ostream& operator<<(std::ostream&, const Arf&);
  arf_t t;
};

}  // namespace exactreal

#endif
