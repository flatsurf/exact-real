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

/*
 * Forward declarations of all our classes. Loaded by every other header file
 * for convenience and also to get consistent defaults for template parameters.
 */

#ifndef LIBEXACTREAL_FORWARD_HPP
#define LIBEXACTREAL_FORWARD_HPP

#include <boost/yap/algorithm_fwd.hpp>

#include "local.hpp"

namespace exactreal {

namespace yap {
template <boost::yap::expr_kind Kind, typename Tuple>
struct ArfExpr;
template <boost::yap::expr_kind Kind, typename Tuple>
struct ArbExpr;
}  // namespace yap

class Arb;
class Arf;

class RealNumber;

class Seed;

template <typename Ring>
class Element;

template <typename Ring>
class Module;

struct IntegerRing;
struct RationalField;
struct NumberField;

}  // namespace exactreal

#endif
