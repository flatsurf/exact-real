/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2020 Vincent Delecroix
 *        Copyright (C) 2020 Julian Rüth
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

#ifndef LIBEXACTREAL_SEED_HPP
#define LIBEXACTREAL_SEED_HPP

namespace exactreal {

struct Seed {
  Seed();
  Seed(unsigned int seed);

  unsigned int value;
};

}  // namespace exactreal

#endif
