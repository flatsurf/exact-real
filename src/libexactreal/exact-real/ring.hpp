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

#ifndef LIBMODEANTIC_RING_HPP
#define LIBMODEANTIC_RING_HPP

#include <type_traits>

#include "exact-real/exact-real.hpp"

namespace exactreal {

template<class Ring, class = std::void_t<> >
struct has_parameters : std::false_type { };
 
template< class Ring >
struct has_parameters<Ring, std::void_t<typename Ring::Parameters>> : std::true_type { };

}


#endif

