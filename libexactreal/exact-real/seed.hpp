/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2020 Vincent Delecroix
 *        Copyright (C) 2020-2025 Julian Rüth
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

#include "forward.hpp"

namespace exactreal {

/// A random seed that can be used to make randomization in exact-real
/// reproducible and deterministic.
class LIBEXACTREAL_API Seed {
  public:
  /// Create a random seed that is different from any previously
  /// generated random seed in this process.
  ///
  /// \note This seed is not really random. It's just a global counter so you
  /// are going to see the same seeds in an otherwise deterministic use of
  /// exact-real.
  /// 
  ///     #include <exact-real/seed.hpp>
  ///     const exactreal::Seed seed;
  ///     seed.value
  ///     // -> 1337
  ///
  Seed();
 
  /// Create a fixed seed with value \p seed.
  Seed(unsigned int seed);

  /// The underlying value used as a seed in random number generators.
  unsigned int value;
};

}  // namespace exactreal

#endif
