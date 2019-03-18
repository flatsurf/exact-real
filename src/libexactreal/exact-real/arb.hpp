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

#ifndef LIBMODEANTIC_ARB_HPP
#define LIBMODEANTIC_ARB_HPP

#include <arb.h>
#include <gmpxx.h>
#include <boost/operators.hpp>
#include <optional>

#include "exact-real/exact-real.hpp"

struct renf_elem_class;

namespace exactreal {
struct Arf;

// A simple wrapper for arb_t elements so we get C++ style memory management.
// We do not fix a precision here yet. However, after some discussion with the
// Arb author, it seems that 64 and 128 are good default precisions. After that
// there is a more noticeable decline in performance.
struct Arb : boost::additive<Arb>,
             boost::additive<renf_elem_class>,
             boost::multipliable<Arb> {
  // An exact zero element
  Arb();
  Arb(const Arb&);
  Arb(Arb&&);
  explicit Arb(const mpz_class& rat);
  explicit Arb(const mpq_class& rat, const mp_limb_signed_t precision = 64);
  explicit Arb(const renf_elem_class& renf,
               const mp_limb_signed_t precision = 64);
  Arb(const Arf& lower, const Arf& upper,
      const mp_limb_signed_t precision = 64);
  // cppcheck-suppress noExplicitConstructor
  explicit Arb(slong x);
  ~Arb();

  // Returns the who extended real line [-∞,∞]
  static Arb any();

  bool isExact() const;

  Arb& operator=(const Arb&);
  Arb& operator=(Arb&&);

  Arb& iadd(const Arb& rhs, const mp_limb_signed_t precision = 64);
  Arb& operator+=(const Arb& rhs) { return iadd(rhs); }
  Arb& isub(const Arb& rhs, const mp_limb_signed_t precision = 64);
  Arb& operator-=(const Arb& rhs) { return isub(rhs); }

  Arb& imul(const Arb& rhs, const mp_limb_signed_t precision = 64);
  Arb& operator*=(const Arb& rhs) { return imul(rhs); }

  Arb& iadd(const renf_elem_class& rhs, const mp_limb_signed_t precision = 64);
  Arb& operator+=(const renf_elem_class& rhs) { return iadd(rhs); }
  Arb& isub(const renf_elem_class& rhs, const mp_limb_signed_t precision = 64);
  Arb& operator-=(const renf_elem_class& rhs) { return isub(rhs); }

  std::optional<int> cmp(const long long) const;
  std::optional<int> cmp(const Arb& rhs) const;

  std::pair<Arf, Arf> arf() const;

  explicit operator double();

  friend std::ostream& operator<<(std::ostream&, const Arb&);

  arb_t t;
};
}  // namespace exactreal

#endif
