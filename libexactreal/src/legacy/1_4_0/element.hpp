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

// Provide backwards compatible implements of Element::coefficients,
// Element::operator*= and Element::operator/= as defined in the 1.4.0 release.
// In the 2.2.0 release we had to remove mpz_class and mpq_class from the
// template arguments of all methods in the public API of libexactreal since
// otherwise clang deos not export these symbols when building with
// -fvisibility=hidden.
// Therefore, we split coefficients() into coefficinets() and
// rationalCoefficients(), and added specializations of operator*= and
// operator/=. However, we can not instantiate the old versions of the
// templated versions due to
// http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1665, i.e., we would
// break ABI compatibility with this change.
// We now implement the old ABI in a separate translation unit. The underlying
// implementation is still in the original element.cc, implementing the
// functions below.

#ifndef LIBEXACTREAL_LEGACY_1_4_0_ELEMENT_HPP
#define LIBEXACTREAL_LEGACY_1_4_0_ELEMENT_HPP

#include <gmpxx.h>

#include <vector>

#include "../../../exact-real/forward.hpp"

namespace exactreal {

template <typename Ring, typename C>
std::vector<C> Element_coefficients_1_4_0(const Element<Ring>&);

template <typename Ring, typename T>
Element<Ring>& Element_operator_mul_1_4_0(Element<Ring>&, const T&);

template <typename Ring, typename T>
Element<Ring>& Element_operator_div_1_4_0(Element<Ring>&, const T&);

}  // namespace exactreal

#endif
