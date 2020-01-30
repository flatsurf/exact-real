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

#ifndef LIBEXACTREAL_YAP_PARAMS_TRANSFORMATION_HPP
#define LIBEXACTREAL_YAP_PARAMS_TRANSFORMATION_HPP

#include "forward.hpp"

namespace exactreal::yap {

// Base class for transformations that determine parameters that have been set
// for an expression, such as the precision with which it should be evaluated.
template <typename T>
struct ParamsTransformation {
  struct IsNotBound {};
  struct IsTerminal {};

  template <typename Arg>
  auto operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&&, const T& t) {
    return t;
  }

  template <typename Arg, typename S>
  auto operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&& arg, const S&) {
    return boost::yap::transform_strict(std::forward<Arg>(arg), *this);
  }

  template <typename Arg>
  auto operator()(boost::yap::expr_tag<boost::yap::expr_kind::terminal>, Arg&&) {
    return IsTerminal();
  }

  // All expressions not whitelisted explicitly above, need an explicit
  // T to be set. Unless we forgot about something in the subclass.
  template <boost::yap::expr_kind Kind, typename... Arg>
  auto operator()(boost::yap::expr_tag<Kind>, Arg&&...) {
    return IsNotBound();
  }
};

}  // namespace exactreal::yap

#endif
