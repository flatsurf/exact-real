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

// We define some helpers so we can put the template parameters explicitly when
// calling these functions. (Something that is simply impossible to do with C++
// constructors.)
template <typename Ring>
auto makeModule(const typename Module<Ring>::Basis &basis) { return Module<Ring>(basis); }
template <typename Ring>
auto makeModule(const typename Module<Ring>::Basis &basis, typename Ring::Parameters parameters) { return Module<Ring>(basis, parameters); }

}  // namespace exactreal

// Work around https://bitbucket.org/wlav/cppyy/issues/96/cannot-make-wrapper-for-a-function
extern template std::ostream &exactreal::operator<<<exactreal::IntegerRingTraits>(std::ostream &, const exactreal::Module<exactreal::IntegerRingTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::RationalFieldTraits>(std::ostream &, const exactreal::Module<exactreal::RationalFieldTraits> &);
extern template std::ostream &exactreal::operator<<<exactreal::NumberFieldTraits>(std::ostream &, const exactreal::Module<exactreal::NumberFieldTraits> &);

// Squash some compiler warnings that come out of cppyy otherwise
extern template const std::shared_ptr<const exactreal::Module<exactreal::IntegerRingTraits>> exactreal::Module<exactreal::IntegerRingTraits>::trivial;
extern template const std::shared_ptr<const exactreal::Module<exactreal::NumberFieldTraits>> exactreal::Module<exactreal::NumberFieldTraits>::trivial;
extern template const std::shared_ptr<const exactreal::Module<exactreal::RationalFieldTraits>> exactreal::Module<exactreal::RationalFieldTraits>::trivial;

#endif
