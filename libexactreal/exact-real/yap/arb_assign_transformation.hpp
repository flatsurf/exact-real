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

#ifndef LIBEXACTREAL_YAP_ARB_ASSIGN_TRANSFORMATION_HPP
#define LIBEXACTREAL_YAP_ARB_ASSIGN_TRANSFORMATION_HPP

#include "../arb.hpp"
#include "assign_transformation.hpp"

namespace exactreal {
namespace yap {

// Evaluates an expression and assigns its value to an actual Arb instance.
// This could be improved quite a lot. There are way too many temporaries
// created at the moment. Also this does not do any reordering. Also logging,
// probably controlled through a template parameter, would be very helpful to
// see what an expression actually turns into. Finally, we blindly promote
// everything to an Arb at the moment. Instead we should leave some primitives
// alone and use the specialized methods from Arb such as arb_mul_si instead
// of arb_mul.
template <bool precisionBound>
struct ArbAssignTransformation : AssignTransformation<Arb, ArbAssignTransformation<precisionBound>> {
  using Base = AssignTransformation<Arb, ArbAssignTransformation<precisionBound>>;
  using Base::operator();

  ArbAssignTransformation(Arb& value, std::optional<prec> precision = {}) : Base{value}, precision(precision) {
    assert(precisionBound == precision.has_value());
  }

  template <boost::yap::expr_kind tag>
  void unary() {
    if constexpr (tag == boost::yap::expr_kind::negate) {
      arb_neg(this->value.arb_t(), this->value.arb_t());
    } else {
      static_assert(false_v<tag>, "unsupported unary operation");
    }
  }

  template <boost::yap::expr_kind tag>
  void binary(Arb&& rhs) {
    static_assert(precisionBound,
                  "Expression can not be evaluated. This binary operator requires the precision to be bound. Did you "
                  "forget to wrap your expression in a (…)(64) to set the precision explicitly?");
    if constexpr (tag == boost::yap::expr_kind::plus) {
      arb_add(this->value.arb_t(), this->value.arb_t(), rhs.arb_t(), *precision);
    } else if constexpr (tag == boost::yap::expr_kind::minus) {
      arb_sub(this->value.arb_t(), this->value.arb_t(), rhs.arb_t(), *precision);
    } else if constexpr (tag == boost::yap::expr_kind::multiplies) {
      arb_mul(this->value.arb_t(), this->value.arb_t(), rhs.arb_t(), *precision);
    } else if constexpr (tag == boost::yap::expr_kind::divides) {
      arb_div(this->value.arb_t(), this->value.arb_t(), rhs.arb_t(), *precision);
    } else {
      static_assert(false_v<tag>, "unsupported binary operation");
    }
  }

  template <typename Arg>
  static decltype(auto) as_expr(Arg&& arg) {
    return boost::yap::as_expr<ArbExpr>(std::forward<Arg>(arg));
  }

  template <typename Arg>
  void operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&& arg, prec prec) {
    boost::yap::transform_strict(as_expr(std::forward<Arg>(arg)), ArbAssignTransformation<true>(this->value, prec));
  }

  template <typename Arg>
  Arb eval(Arg&& arg) {
    Arb ret;
    boost::yap::transform_strict(as_expr(std::forward<Arg>(arg)),
                                 ArbAssignTransformation<precisionBound>(ret, precision));
    return ret;
  }

  std::optional<prec> precision;
};

}  // namespace yap
}  // namespace exactreal

#endif
