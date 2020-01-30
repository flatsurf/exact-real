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

#ifndef LIBEXACTREAL_YAP_ARB_EXPR_HPP
#define LIBEXACTREAL_YAP_ARB_EXPR_HPP

#include "prec_expr.hpp"

namespace exactreal {
namespace yap {

template <boost::yap::expr_kind Kind, typename Tuple>
struct ArbExpr {
  static_assert(
      Kind != boost::yap::expr_kind::terminal ||
          // terminals can be actual Arb instances
          std::is_same<Tuple, boost::hana::tuple<Arb>>{} || std::is_same<Tuple, boost::hana::tuple<Arb&>>{} ||
          std::is_same<Tuple, boost::hana::tuple<const Arb&>>{} ||
          // terminals can also be all types that can be losslessly converted to Arb
          std::is_same<Tuple, boost::hana::tuple<Arf>>{} || std::is_same<Tuple, boost::hana::tuple<Arf&>>{} ||
          std::is_same<Tuple, boost::hana::tuple<const Arf&>>{} || std::is_same<Tuple, boost::hana::tuple<int>>{} ||
          std::is_same<Tuple, boost::hana::tuple<const int&>>{} ||
          // Careful with this one: if you forget to wrap a precision as an
          // PrecExpr explicitly, it will be turned into an ArbExpr terminal.
          std::is_same<Tuple, boost::hana::tuple<slong>>{} || std::is_same<Tuple, boost::hana::tuple<const slong&>>{} ||
          std::is_same<Tuple, boost::hana::tuple<ulong>>{} || std::is_same<Tuple, boost::hana::tuple<const ulong&>>{} ||
          std::is_same<Tuple, boost::hana::tuple<double>>{} || std::is_same<Tuple, boost::hana::tuple<const double&>>{},
      "ArbExpr instantiated with an unsupported terminal type. Probably you used an unsupported type in an arithmetic "
      "expression that you should have explicitly converted to an Arb().");

  static const boost::yap::expr_kind kind = Kind;
  Tuple elements;

  decltype(auto) operator()(prec prec) && {
    return boost::yap::make_expression<ArbExpr, boost::yap::expr_kind::call>(std::move(*this), PrecExpr{{prec}});
  }
};

}  // namespace yap
}  // namespace exactreal

#endif
