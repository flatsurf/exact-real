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

#ifndef LIBEXACTREAL_RING_TRAITS_HPP
#define LIBEXACTREAL_RING_TRAITS_HPP

#include <type_traits>

#include "exact-real/exact-real.hpp"

namespace exactreal {

// true_type iff Ring is the traits class of a ring that needs additional
// parameters to function, e.g., a number field which has a "renf_class" which
// singles out the specific ring.
template <typename Ring, typename = std::void_t<>>
struct is_parametrized : std::false_type {};

template <typename Ring>
struct is_parametrized<Ring, std::void_t<typename Ring::Parameters>> : std::true_type {};

template <typename Ring>
inline constexpr bool is_parametrized_v = is_parametrized<Ring>::value;

}  // namespace exactreal

#endif
