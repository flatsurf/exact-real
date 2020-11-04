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

#ifndef LIBEXACTREAL_ARF_HPP
#define LIBEXACTREAL_ARF_HPP

#include <arf.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>

#include <boost/operators.hpp>
#include <memory>

#include "yap/terminal.hpp"

namespace exactreal {
// A wrapper for arf_t elements so we get C++ style memory management.
// We use some Yap magic to get nice operators (which is tricky otherwise
// because we cannot pass the additional prec and rnd parameters to operators
// in C++.)
//
// If you don't like that magic and only want memory management, just create
// elements
//
//   Arf x, y;
//
// and then use the Arf functions directly:
//
//   arf_add(x.arb_t(), x.arb_t(), y.arb_t(), 64, ARF_RND_NEAR);
//
// Using yap this can be rewritten as any of the following:
//
//   #include <exact-real/yap/arb.hpp>
//
//   x += y(64)(Arf::Round::NEAR);
//   x = (x + y)(64, Arf::Round::NEAR);
//
// Note that the latter might use an additional temporary Arf. See the
// yap/arf.hpp header for more details.
//
// Note that methods here are usually named as their counterparts in arf.h with
// the leading arf_ removed.
class LIBEXACTREAL_API Arf : yap::Terminal<Arf, yap::ArfExpr>, boost::totally_ordered<Arf>, boost::totally_ordered<Arf, long>, boost::shiftable<Arf, long> {
 public:
  enum class Round {
    NEAR = ARF_RND_NEAR,
    DOWN = ARF_RND_DOWN,
    UP = ARF_RND_UP,
    FLOOR = ARF_RND_FLOOR,
    CEIL = ARF_RND_CEIL,
  };

  // A zero element
  Arf() noexcept;
  Arf(const Arf&) noexcept;
  Arf(Arf&&) noexcept;
  Arf(const std::string& mantissa, int base, long exponent);
  Arf(const mpz_class& mantissa, long exponent) noexcept;
  explicit Arf(const int) noexcept;
  explicit Arf(const slong) noexcept;
  explicit Arf(const double) noexcept;
  ~Arf() noexcept;

  Arf& operator=(const Arf&) noexcept;
  Arf& operator=(Arf&&) noexcept;
  Arf& operator=(int) noexcept;
  Arf& operator=(slong) noexcept;
  Arf& operator=(double) noexcept;

  Arf operator-() const noexcept;

  explicit operator double() const noexcept;

  Arf abs() const noexcept;
  mpz_class floor() const noexcept;
  mpz_class ceil() const noexcept;

  mpz_class mantissa() const noexcept;
  mpz_class exponent() const noexcept;
  // log2 rounded down
  long logb() const noexcept;

  Arf& operator>>=(const long) noexcept;
  Arf& operator<<=(const long) noexcept;

  bool operator<(const Arf&) const noexcept;
  bool operator==(const Arf&) const noexcept;

  bool operator<(long) const noexcept;
  bool operator>(long) const noexcept;
  bool operator==(long) const noexcept;

  static Arf randtest(flint::frandxx&, prec precision, prec magbits) noexcept;

  friend std::ostream& operator<<(std::ostream&, const Arf&) LIBEXACTREAL_API;

  // A reference to the underlying arf_t element for direct manipulation with Arb
  ::arf_t& arf_t() noexcept;
  // A const reference to the underlying arf_t element for direct usage in Arb
  const ::arf_t& arf_t() const noexcept;

  // Syntactic sugar for Yap, so that expresions such as x += y(64, Arf::Rount::NEAR) work.
  template <typename... Args>
  decltype(auto) operator()(Args&&...) const noexcept LIBEXACTREAL_LOCAL;

  // Construction from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arf(const yap::ArfExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;
  template <boost::yap::expr_kind Kind, typename Tuple>
  // cppcheck-suppress  noExplicitConstructor
  Arf(yap::ArfExpr<Kind, Tuple>&&) noexcept LIBEXACTREAL_LOCAL;

  // Assignment from evaluating a Yap expression
  template <boost::yap::expr_kind Kind, typename Tuple>
  Arf& operator=(const yap::ArfExpr<Kind, Tuple>&) noexcept LIBEXACTREAL_LOCAL;

 private:
  // The underlying arf_t; use arf_t() to get a reference to it.
  ::arf_t t;
};

// Provide swap for STL containers
void swap(Arf& lhs, Arf& rhs);

}  // namespace exactreal

namespace std {

template <>
struct hash<exactreal::Arf> {
  size_t operator()(const exactreal::Arf&) const;
};

}  // namespace std

#endif
