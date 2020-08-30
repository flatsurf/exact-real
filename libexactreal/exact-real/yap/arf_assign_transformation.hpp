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

#ifndef LIBEXACTREAL_YAP_ARF_ASSIGN_TRANSFORMATION_HPP
#define LIBEXACTREAL_YAP_ARF_ASSIGN_TRANSFORMATION_HPP

#include <optional>

#include "../arf.hpp"
#include "assign_transformation.hpp"

namespace exactreal::yap {

// Evaluates an expression and assigns its value to an actual Arf instance.
template <bool precisionBound, bool roundBound>
struct ArfAssignTransformation : AssignTransformation<Arf, ArfAssignTransformation<precisionBound, roundBound>> {
  using Base = AssignTransformation<Arf, ArfAssignTransformation<precisionBound, roundBound>>;
  using Base::operator();

  std::optional<prec> precision;
  std::optional<Arf::Round> round;

  ArfAssignTransformation(Arf& value, std::optional<prec> precision = {}, std::optional<Arf::Round> round = {})
      : Base{value}, precision(precision), round(round) {
    assert(precisionBound == precision.has_value());
    assert(roundBound == round.has_value());
  }

  template <typename Arg>
  void operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&& arg, Arf::Round round) {
    boost::yap::transform_strict(as_expr(std::forward<Arg>(arg)),
                                 ArfAssignTransformation<precisionBound, true>(this->value, precision, round));
  }

  template <typename Arg>
  void operator()(boost::yap::expr_tag<boost::yap::expr_kind::call>, Arg&& arg, prec prec) {
    boost::yap::transform_strict(as_expr(std::forward<Arg>(arg)),
                                 ArfAssignTransformation<true, roundBound>(this->value, prec, round));
  }

  template <typename Arg>
  Arf eval(Arg&& arg) {
    Arf ret;
    boost::yap::transform_strict(as_expr(std::forward<Arg>(arg)),
                                 ArfAssignTransformation<precisionBound, roundBound>(ret, precision, round));
    return ret;
  }

  template <typename Arg>
  static auto as_expr(Arg&& arg) {
    return boost::yap::as_expr<ArfExpr>(std::forward<Arg>(arg));
  }

  template <boost::yap::expr_kind tag>
  void unary() {
    if constexpr (tag == boost::yap::expr_kind::negate) {
      arf_neg(this->value.arf_t(), this->value.arf_t());
    } else {
      static_assert(false_v<tag>, "unsupported unary operation");
    }
  }

  template <boost::yap::expr_kind tag>
  void binary(Arf&& rhs) {
    static_assert(precisionBound,
                  "Expression can not be evaluated. The binary operator requires the precision to be bound. Did you "
                  "forget to wrap your expression in a (…)(64) to set the precision explicitly?");
    static_assert(roundBound,
                  "Expression can not be evaluated. The binary operator requires the rounding to be specified. Did you "
                  "forget to wrap your expression in a (…)(Arf::Round::NEAR) to set the rounding explicitly?");
    // Note that cling as of mid 2019 ignores the static_asserts above, i.e.,
    // it's strangely possible that roundBound is actually 0 here.

    if constexpr (tag == boost::yap::expr_kind::plus) {
      arf_add(this->value.arf_t(), this->value.arf_t(), rhs.arf_t(), *precision, static_cast<int>(*round));
    } else if constexpr (tag == boost::yap::expr_kind::minus) {
      arf_sub(this->value.arf_t(), this->value.arf_t(), rhs.arf_t(), *precision, static_cast<int>(*round));
    } else if constexpr (tag == boost::yap::expr_kind::multiplies) {
      arf_mul(this->value.arf_t(), this->value.arf_t(), rhs.arf_t(), *precision, static_cast<int>(*round));
    } else if constexpr (tag == boost::yap::expr_kind::divides) {
      arf_div(this->value.arf_t(), this->value.arf_t(), rhs.arf_t(), *precision, static_cast<int>(*round));
    } else {
      static_assert(false_v<tag>, "unsupported binary operation");
    }
  }
};

}  // namespace exactreal::yap

#endif
