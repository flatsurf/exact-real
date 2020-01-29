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

#ifndef LIBEXACTREAL_YAP_ASSIGN_TRANSFORMATION_HPP
#define LIBEXACTREAL_YAP_ASSIGN_TRANSFORMATION_HPP

#include "forward.hpp"

namespace exactreal {
namespace yap {

// A transformation that evaluate an expression and assigns its value to an
// actual instance of a T.
template <typename T, typename TypedAssignTransformation>
struct AssignTransformation {
  // Forward calls such as (a, b, c) to (a)(b)(c). The subclass must implement the single parameter version.
  template <typename Arg, typename Param1, typename Param2, typename... Params>
  void operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&& arg, Param1&& param1, Param2&& param2,
                  Params&&... params) {
    this->operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>{}, std::forward<Arg>(arg),
                     std::forward<Param1>(param1))(std::forward<Param2>(param2))(std::forward<Params>(params)...);
  }

  template <typename S>
  void operator()(boost::yap::expr_tag<boost::yap::expr_kind::terminal>, S&& rhs) {
    value = std::forward<S>(rhs);
  }

  // Evaluate an unary operator
  // by moving the value of the operand into "value" and applying the operator.
  template <boost::yap::expr_kind tag, typename Arg>
  void operator()(boost::yap::expr_tag<tag>, Arg&& arg) {
    boost::yap::transform_strict(TypedAssignTransformation::as_expr(std::forward<Arg>(arg)), *this);
    static_cast<TypedAssignTransformation*>(this)->template unary<tag>();
  }

  template <boost::yap::expr_kind tag, typename L, typename R>
  void operator()(boost::yap::expr_tag<tag>, L&& left, R&& right) {
    T rhs = static_cast<TypedAssignTransformation*>(this)->eval(std::forward<R>(right));
    boost::yap::transform_strict(TypedAssignTransformation::as_expr(std::forward<L>(left)), *this);
    static_cast<TypedAssignTransformation*>(this)->template binary<tag>(std::move(rhs));
  }

  T& value;
};

}  // namespace yap
}  // namespace exactreal

#endif
