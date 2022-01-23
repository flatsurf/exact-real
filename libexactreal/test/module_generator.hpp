/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2022 Julian Rüth
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

#ifndef EXACT_REAL_TEST_MODULE_GENERATOR_HPP
#define EXACT_REAL_TEST_MODULE_GENERATOR_HPP

#include <memory>

#include <e-antic/renf_class.hpp>

#include "../exact-real/module.hpp"
#include "../exact-real/real_number.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

namespace {

template<typename R>
struct ModuleGenerator : public Catch::Generators::IGenerator<const exactreal::Module<R>&>
{
  mutable std::shared_ptr<const exactreal::Module<R>> current;

  int generators = -1;

  ModuleGenerator() {}

  bool next() override {
    generators++;

    return true;
  }

  const exactreal::Module<R>& get() const override {
    if (current == nullptr || current->rank() != generators) {
      typename exactreal::Module<R>::Basis basis;
      for (int g = 0; g < generators; g++) {
        if (g == 0) {
          basis.push_back(exactreal::RealNumber::rational(1));
        } else {
          basis.push_back(exactreal::RealNumber::random());
        }
      }

      if constexpr (std::is_same_v<R, exactreal::NumberField>) {
        const auto K = eantic::renf_class::make("a^2 - 2", "a", "1.41 +/- 0.1", 64);
        current = exactreal::Module<R>::make(basis, K);
      } else {
        current = exactreal::Module<R>::make(basis);
      }
    }
    return *current;
  }
};

/*
 * Wrap ModuleGenerator for use as GENERATE(modules())
 */
template <typename R>
inline Catch::Generators::GeneratorWrapper<const exactreal::Module<R>&> modules() {
    return Catch::Generators::GeneratorWrapper<const exactreal::Module<R>&>(std::unique_ptr<Catch::Generators::IGenerator<const exactreal::Module<R>&>>(new ModuleGenerator<R>()));
}

}

#endif

