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

#ifndef LIBEXACTREAL_YAP_ROUND_EXPR_HPP
#define LIBEXACTREAL_YAP_ROUND_EXPR_HPP

#include "../arf.hpp"
#include "forward.hpp"

namespace exactreal {
namespace yap {

struct RoundExpr {
  static boost::yap::expr_kind const kind = boost::yap::expr_kind::terminal;

  boost::hana::tuple<Arf::Round> elements;
};

}  // namespace yap
}  // namespace exactreal

#endif
