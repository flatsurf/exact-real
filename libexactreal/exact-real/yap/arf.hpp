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

#ifndef LIBEXACTREAL_YAP_ARF_HPP
#define LIBEXACTREAL_YAP_ARF_HPP

#include <boost/hana/fwd/all.hpp>
#include <boost/yap/expression.hpp>

#include "../arf.hpp"
#include "arf_assign_transformation.hpp"
#include "arf_expr.hpp"
#include "prec_expr.hpp"
#include "prec_transformation.hpp"
#include "round_expr.hpp"
#include "round_transformation.hpp"

namespace exactreal {

namespace yap {
template <typename T>
using isArf = std::is_same<Arf, T>;

}  // namespace yap

// Define operators: "Arf op other"
// A static assertion is raised if "other" can not be losslessly converted to
// an Arf.
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(multiplies, yap::ArfExpr, yap::isArf)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(plus, yap::ArfExpr, yap::isArf)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(minus, yap::ArfExpr, yap::isArf)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(divides, yap::ArfExpr, yap::isArf)
BOOST_YAP_USER_UDT_UNARY_OPERATOR(negate, yap::ArfExpr, yap::isArf)

// Define operators: "ArfExpr op ArfExpr"
BOOST_YAP_USER_BINARY_OPERATOR(plus, yap::ArfExpr, yap::ArfExpr)
BOOST_YAP_USER_BINARY_OPERATOR(minus, yap::ArfExpr, yap::ArfExpr)
BOOST_YAP_USER_BINARY_OPERATOR(multiplies, yap::ArfExpr, yap::ArfExpr)
BOOST_YAP_USER_BINARY_OPERATOR(divides, yap::ArfExpr, yap::ArfExpr)
BOOST_YAP_USER_UNARY_OPERATOR(negate, yap::ArfExpr, yap::ArfExpr)

template <boost::yap::expr_kind Kind, typename Tuple>
Arf::Arf(const yap::ArfExpr<Kind, Tuple>& expr) noexcept : Arf() {
  *this = expr;
}

template <boost::yap::expr_kind Kind, typename Tuple>
Arf::Arf(yap::ArfExpr<Kind, Tuple>&& expr) noexcept : Arf() {
  *this = std::move(expr);
}

template <boost::yap::expr_kind Kind, typename Tuple>
Arf& Arf::operator=(const yap::ArfExpr<Kind, Tuple>& expr) noexcept {
  boost::yap::transform_strict(expr, yap::ArfAssignTransformation(*this));
  return *this;
}

template <boost::yap::expr_kind Kind, typename Tuple, typename Lambda>
Arf& inplace_binop(Arf& self, const yap::ArfExpr<Kind, Tuple>& expr, Lambda op) noexcept {
  auto prec = boost::yap::transform_strict(expr, yap::PrecTransformation{});
  auto round = boost::yap::transform_strict(expr, yap::RoundTransformation{});
  if constexpr (!std::is_same_v<decltype(prec), ::exactreal::prec>) {
    static_assert(false_t<decltype(prec)>,
                  "Expression can not be used in @= operator because the right hand side has no precision set. Did you "
                  "forget to wrap the right hand side in a (…)(64) to set the precision explicitly?");
  } else if constexpr (!std::is_same_v<decltype(round), ::exactreal::Arf::Round>) {
    static_assert(false_t<decltype(round)>,
                  "Expression can not be used in @= operator because the right hand side has no rounding set. Did you "
                  "forget to wrap the right hand side in a (…)(Arf::Round::NEAR) to set the rounding explicitly?");
  } else {
    auto binary = op(self, expr)(prec)(round);
    boost::yap::transform_strict(std::move(binary), yap::ArfAssignTransformation(self));
  }
  return self;
}

template <typename... Args>
decltype(auto) Arf::operator()(Args&&... args) const noexcept {
  return boost::yap::as_expr<yap::ArfExpr>(*this)(std::forward<Args>(args)...);
}
}  // namespace exactreal

#endif
