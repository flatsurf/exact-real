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

#ifndef LIBEXACTREAL_SMART_OSTREAM_HPP
#define LIBEXACTREAL_SMART_OSTREAM_HPP

#include <memory>
#include <ostream>

namespace exactreal {
// A convenience print wrapper for easier debugging in GDB with
// https://stackoverflow.com/a/55550136/812379
template <class T>
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<T>& self) {
  if (self == nullptr) {
    return os << "nullptr";
  } else {
    return os << *self;
  }
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::unique_ptr<T>& self) {
  if (self == nullptr) {
    return os << "nullptr";
  } else {
    return os << *self;
  }
}
}  // namespace exactreal

#endif
