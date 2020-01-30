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

#ifndef LIBEXACTREAL_YAP_ARB_HPP
#define LIBEXACTREAL_YAP_ARB_HPP

#include <boost/hana/fwd/all.hpp>
#include <boost/yap/expression.hpp>
#include <optional>

#include "../arb.hpp"
#include "arb_assign_transformation.hpp"
#include "arb_expr.hpp"
#include "prec_expr.hpp"
#include "prec_transformation.hpp"

namespace exactreal {
namespace yap {
using exactreal::Arb;

template <typename T>
using isArb = std::is_same<Arb, T>;

}  // namespace yap

BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(multiplies, yap::ArbExpr, yap::isArb)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(plus, yap::ArbExpr, yap::isArb)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(minus, yap::ArbExpr, yap::isArb)
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(divides, yap::ArbExpr, yap::isArb)
BOOST_YAP_USER_UDT_UNARY_OPERATOR(negate, yap::ArbExpr, yap::isArb)
BOOST_YAP_USER_BINARY_OPERATOR(plus, yap::ArbExpr, yap::ArbExpr)
BOOST_YAP_USER_BINARY_OPERATOR(minus, yap::ArbExpr, yap::ArbExpr)
BOOST_YAP_USER_BINARY_OPERATOR(multiplies, yap::ArbExpr, yap::ArbExpr)
BOOST_YAP_USER_BINARY_OPERATOR(divides, yap::ArbExpr, yap::ArbExpr)
BOOST_YAP_USER_UNARY_OPERATOR(negate, yap::ArbExpr, yap::ArbExpr)

template <boost::yap::expr_kind Kind, typename Tuple>
Arb::Arb(const yap::ArbExpr<Kind, Tuple>& expr) noexcept : Arb() {
  *this = expr;
}

template <boost::yap::expr_kind Kind, typename Tuple>
Arb::Arb(yap::ArbExpr<Kind, Tuple>&& expr) noexcept : Arb() {
  *this = std::move(expr);
}

template <boost::yap::expr_kind Kind, typename Tuple>
Arb& Arb::operator=(const yap::ArbExpr<Kind, Tuple>& expr) noexcept {
  boost::yap::transform_strict(expr, yap::ArbAssignTransformation(*this));
  return *this;
}

template <boost::yap::expr_kind Kind, typename Tuple, typename Lambda>
Arb& inplace_binop(Arb& self, const yap::ArbExpr<Kind, Tuple>& expr, Lambda op) noexcept {
  auto prec = boost::yap::transform_strict(expr, yap::PrecTransformation{});
  if constexpr (!std::is_same_v<decltype(prec), ::exactreal::prec>) {
    static_assert(false_t<decltype(prec)>,
                  "Expression can not be used in @= operator because the right hand side has no precision set. Did you "
                  "forget to wrap the right hand side in a (…)(64) to set the precision explicitly?");
  } else {
    auto binary = op(self, expr)(prec);
    boost::yap::transform_strict(std::move(binary), yap::ArbAssignTransformation(self));
    return self;
  }
}

template <typename... Args>
decltype(auto) Arb::operator()(Args&&... args) const noexcept {
  return boost::yap::as_expr<yap::ArbExpr>(*this)(std::forward<Args>(args)...);
}

}  // namespace exactreal

#endif
