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

#ifndef LIBEXACTREAL_SMART_LESS_HPP
#define LIBEXACTREAL_SMART_LESS_HPP

#include <algorithm>

namespace exactreal {
// An operator< for smart pointers so they can be used more transparently in
// STL containers. (This should exist somewhere in Boost but I could not find
// anything like that preexisting.)
template <class P>
struct smart_less {
  constexpr bool operator()(const P& lhs, const P& rhs) const {
    if (lhs == nullptr) {
      return rhs == nullptr;
    }
    if (rhs == nullptr) {
      return false;
    }
    return *lhs < *rhs;
  }
};

// Return whether two sets are equal (after dereferencing smart pointers.)
// The above does not help when actually comparing sets since comparison of
// sets iterates over the set and calls operator== for every element (and not
// the operator< defined above.)
template <class S>
bool smart_eq(const S& lhs, const S& rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](const auto& x, const auto& y) {
    if (x == nullptr)
      return y == nullptr;
    if (y == nullptr)
      return false;
    return *x == *y;
  });
}
}  // namespace exactreal

#endif
