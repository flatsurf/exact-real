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

#ifndef LIBEXACTREAL_ARB_HPP
#define LIBEXACTREAL_ARB_HPP

#include <arb.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>
#include <boost/yap/algorithm_fwd.hpp>
#include <optional>

#include "exact-real/arf.hpp"
#include "exact-real/exact-real.hpp"

struct renf_elem_class;

namespace exactreal {
// After some discussion with the Arb author, it seems that 64 and 128 are good
// default precisions for which Arb should be particularly fast. We use this
// only when we need some random precision to at which to start an algorithm.
// For callers of the library, this should not have any effect other than
// changing the performance of certain calls slightly.
inline constexpr const prec ARB_PRECISION_FAST = 64;

// A wrapper for arb_t elements so we get C++ style memory management.
// We use some Yap magic to get nice operators (which is tricky otherwise
// because we cannot pass the additional prec and rnd parameters to operators
// in C++.)
//
// If you don't like that magic and only want memory management, just create
// elements
//
//   Arb x, y;
//
// and then use the Arb functions directly:
//
//   arb_add(x.arb_t(), x.arb_t(), y.arb_t(), 64);
//
// Using yap this can be rewritten as any of the following:
//
//   #include <exact-real/yap/arb.hpp>
//
//   x += y(64);
//   x = (x + y)(64);
//
// Note that the latter might use an additional temporary Arb. See the
// yap/arb.hpp header for more details.
//
// Note that methods here are usually named as their counterparts in arb.h with
// the leading arb_ removed.
struct Arb : yap::Terminal<Arb, yap::ArbExpr> {
  // An exact zero element
  Arb() noexcept;
  Arb(const Arb&) noexcept;
  Arb(Arb&&) noexcept;
  explicit Arb(const mpz_class&) noexcept;
  explicit Arb(const mpq_class&, const prec) noexcept;
  explicit Arb(const renf_elem_class&, const prec) noexcept;
  explicit Arb(const std::pair<Arf, Arf>&, const prec = ARF_PREC_EXACT);
  explicit Arb(const Arf& midpoint);
  explicit Arb(slong) noexcept;
  explicit Arb(ulong) noexcept;
  explicit Arb(int) noexcept;
  explicit Arb(long long) noexcept;
  explicit Arb(const std::string&, const prec);

  ~Arb() noexcept;

  Arb& operator=(const Arb&) noexcept;
  Arb& operator=(Arb&&) noexcept;
  Arb& operator=(slong) noexcept;
  Arb& operator=(ulong) noexcept;
  Arb& operator=(int) noexcept;
  Arb& operator=(long long) noexcept;

  // The comparison operators return a value if the relation is true for all
  // elements in the ball described by this element, e.g., x < y, returns true
  // if the relation is true for every element in x and y, they return false if
  // the relation is false for every element in x and y, and nothing otherwise.
  // Note that this is different from the semantic in Arb where false is
  // returned in both of the latter cases.
  std::optional<bool> operator<(const Arb&) const noexcept;
  std::optional<bool> operator>(const Arb&) const noexcept;
  std::optional<bool> operator<=(const Arb&) const noexcept;
  std::optional<bool> operator>=(const Arb&) const noexcept;
  std::optional<bool> operator<(const long long) const noexcept;
  std::optional<bool> operator>(const long long) const noexcept;
  std::optional<bool> operator<=(const long long) const noexcept;
  std::optional<bool> operator>=(const long long) const noexcept;
  std::optional<bool> operator==(const Arb&) const noexcept;
  std::optional<bool> operator!=(const Arb&) const noexcept;

  bool is_exact() const noexcept;
  bool is_finite() const noexcept;

  // The endpoints of this ball; wraps as arb_get_interval_arf
  explicit operator std::pair<Arf, Arf>() const noexcept;
  // A double representation, actually a double representation of the midpoint of this ball.
  explicit operator double() const noexcept;

  friend std::ostream& operator<<(std::ostream&, const Arb&);

  // A reference to the underlying arb_t element for direct manipulation with Arb
  ::arb_t& arb_t() noexcept;
  // A const reference to the underlying arb_t element for direct usage in Arb
  const ::arb_t& arb_t() const noexcept;

  static Arb zero() noexcept;
  static Arb one() noexcept;
  static Arb pos_inf() noexcept;
  static Arb neg_inf() noexcept;
  // The extended real line [-∞,∞]
  static Arb zero_pm_inf() noexcept;
  static Arb indeterminate() noexcept;
  static Arb zero_pm_one() noexcept;
  static Arb unit_interval() noexcept;
  static Arb randtest(flint::frandxx&, prec precision, prec magbits) noexcept;
  static Arb randtest_exact(flint::frandxx&, prec precision, prec magbits) noexcept;

  // Return whether elements have the same midpoint and radius.
  bool equal(const Arb&) const noexcept;

  // Syntactic sugar for Yap, so that expresions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  decltype(auto) operator()(Args&&...) const noexcept;

  // Construction from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arb(const yap::ArbExpr<Kind, Tuple>&) noexcept;
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arb(yap::ArbExpr<Kind, Tuple>&&) noexcept;

  // Assignment from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arb& operator=(const yap::ArbExpr<Kind, Tuple>&) noexcept;

 private:
  // The underlying arb_t; use arb_t() to get a reference to it.
  ::arb_t t;
};

// Provide swap for STL containers
void swap(Arb& lhs, Arb& rhs);
}  // namespace exactreal

#endif
