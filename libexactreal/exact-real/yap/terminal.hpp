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

#ifndef LIBEXACTREAL_YAP_TERMINAL_HPP
#define LIBEXACTREAL_YAP_TERMINAL_HPP

#include "../forward.hpp"

namespace exactreal {
namespace yap {
template <typename T, template <boost::yap::expr_kind, typename> typename Expr>
class Terminal {
  template <boost::yap::expr_kind Kind, typename Tuple>
  friend T& operator+=(T& self, Expr<Kind, Tuple>&& expr) noexcept {
    return inplace_binop(self, std::move(expr), [](auto&& a, auto&& b) -> decltype(auto) {
      return std::forward<decltype(a)>(a) + std::forward<decltype(b)>(b);
    });
  }
  template <boost::yap::expr_kind Kind, typename Tuple>
  friend T& operator-=(T& self, Expr<Kind, Tuple>&& expr) noexcept {
    return inplace_binop(self, std::move(expr), [](auto&& a, auto&& b) -> decltype(auto) {
      return std::forward<decltype(a)>(a) - std::forward<decltype(b)>(b);
    });
  }
  template <boost::yap::expr_kind Kind, typename Tuple>
  friend T& operator*=(T& self, Expr<Kind, Tuple>&& expr) noexcept {
    return inplace_binop(self, std::move(expr), [](auto&& a, auto&& b) -> decltype(auto) {
      return std::forward<decltype(a)>(a) * std::forward<decltype(b)>(b);
    });
  }
  template <boost::yap::expr_kind Kind, typename Tuple>
  friend T& operator/=(T& self, Expr<Kind, Tuple>&& expr) noexcept {
    return inplace_binop(self, std::move(expr), [](auto&& a, auto&& b) -> decltype(auto) {
      return std::forward<decltype(a)>(a) / std::forward<decltype(b)>(b);
    });
  }
};
}  // namespace yap
}  // namespace exactreal

#endif
