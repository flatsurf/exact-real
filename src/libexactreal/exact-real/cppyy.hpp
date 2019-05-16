/**********************************************************************
 *  This file is part of exact-real.
 *
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

#ifndef LIBEXACTREAL_CPPYY_HPP
#define LIBEXACTREAL_CPPYY_HPP

#include <iosfwd>
#include <memory>

#include "exact-real/element.hpp"
#include "exact-real/integer_ring_traits.hpp"
#include "exact-real/module.hpp"
#include "exact-real/number_field_traits.hpp"
#include "exact-real/rational_field_traits.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arb.hpp"
#include "exact-real/yap/arf.hpp"

// See https://bitbucket.org/wlav/cppyy/issues/95/lookup-of-friend-operator
namespace exactreal {
std::ostream &operator<<(std::ostream &, const exactreal::Arb &);
std::ostream &operator<<(std::ostream &, const exactreal::Arf &);
std::ostream &operator<<(std::ostream &, const exactreal::RealNumber &);
template <typename Ring>
std::ostream &operator<<(std::ostream &, const exactreal::Module<Ring> &);
template <typename Ring>
std::ostream &operator<<(std::ostream &, const exactreal::Element<Ring> &);
}  // namespace exactreal

namespace exactreal {
// Arb & Arf do not directly support arithmetic operators in cling. This is
// not surprising as these essentially only exist at compile time.
// Here are some explicit versions for arithmetic with Arb & Arf when the full
// template magic is not possible.
Arb binary(const Arb &left, const Arb &right, char op, prec prec);
Arf binary(const Arf &left, const Arf &right, char op, prec prec, Arf::Round round);
template <typename T>
T minus(const T &lhs) {
  return -lhs;
}

// cppyy does not see the operators that come out of boost/operators.hpp.
// Why exactly is not clear to me at the moment. Since they are defined as
// non-template friends inside the template classes such as addable<>, we can
// not explicitly declare them like we did with the operator<< below.
template <typename S, typename T, char op>
auto boost_binary(const S &lhs, const T &rhs) {
  if constexpr (op == '+')
    return lhs + rhs;
  else if constexpr (op == '-')
    return lhs - rhs;
  else if constexpr (op == '*')
    return lhs * rhs;
  else if constexpr (op == '/')
    return lhs / rhs;
  else {
    static_assert(false_v<op>, "operator not implemented");
  }
}

}  // namespace exactreal

// Work around https://bitbucket.org/wlav/cppyy/issues/96/cannot-make-wrapper-for-a-function
extern template std::ostream &exactreal::operator<<<exactreal::IntegerRingTraits>(std::ostream &, const exactreal::Module<exactreal::IntegerRingTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::RationalFieldTraits>(std::ostream &, const exactreal::Module<exactreal::RationalFieldTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::NumberFieldTraits>(std::ostream &, const exactreal::Module<exactreal::NumberFieldTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::IntegerRingTraits>(std::ostream &, const exactreal::Element<exactreal::IntegerRingTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::RationalFieldTraits>(std::ostream &, const exactreal::Element<exactreal::RationalFieldTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::NumberFieldTraits>(std::ostream &, const exactreal::Element<exactreal::NumberFieldTraits> &);

// Squash some compiler warnings that come out of cppyy otherwise
extern template const std::shared_ptr<const exactreal::Module<exactreal::IntegerRingTraits>> exactreal::Module<exactreal::IntegerRingTraits>::trivial;
extern template const std::shared_ptr<const exactreal::Module<exactreal::NumberFieldTraits>> exactreal::Module<exactreal::NumberFieldTraits>::trivial;
extern template const std::shared_ptr<const exactreal::Module<exactreal::RationalFieldTraits>> exactreal::Module<exactreal::RationalFieldTraits>::trivial;

#endif
