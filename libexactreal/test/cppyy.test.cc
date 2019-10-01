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

#include <gtest/gtest.h>

#include <exact-real/cppyy.hpp>
#include <exact-real/yap/arb.hpp>

namespace exactreal {
TEST(ArbCppyyTest, Eval) {
  Arb x(1);
  auto y = x + x;
  Arb z = exactreal::cppyy::eval(std::move(y), 10);
  ASSERT_TRUE(z.equal(Arb(2)));
}
}  // namespace exactreal

#include "main.hpp"
