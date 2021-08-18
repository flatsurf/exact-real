/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2021 Julian Rüth
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
 * Include this file to get the definitions for the entire API of libexactreal.
 * To improve build times, consider only including some of the headers below or
 * only forward.hpp instead.
 */

#ifndef LIBEXACTREAL_EXACT_REAL_HPP
#define LIBEXACTREAL_EXACT_REAL_HPP

#include "arb.hpp"
#include "arf.hpp"
#include "element.hpp"
#include "integer_ring.hpp"
#include "module.hpp"
#include "number_field.hpp"
#include "number_field_ideal.hpp"
#include "rational_field.hpp"
#include "real_number.hpp"
#include "seed.hpp"
#include "yap/arb.hpp"
#include "yap/arf.hpp"

// Do not include extensions to the API which integrate with other libraries.
// #include "cereal.hpp"
// #include "cppyy.hpp"

#endif
