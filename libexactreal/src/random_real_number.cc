/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
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

#include <arb.h>
#include <gmpxx.h>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <sstream>

#include "../exact-real/arb.hpp"
#include "../exact-real/arf.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/seed.hpp"
#include "external/unique-factory/unique-factory/unique-factory.hpp"
#include "impl/real_number_base.hpp"

using namespace exactreal;
using boost::random::rand48;
using std::optional;
using std::ostream;
using std::shared_ptr;
using std::string;
using std::stringstream;

namespace {

// A random real number in [0, 1]
class RandomRealNumber final : public RealNumberBase {
 public:
  RandomRealNumber(unsigned int seed) : seed(seed) {}

  // Creates a random Arf from digits in base 2.
  // We could speed this up by caching numbers to some precision but let's wait
  // with this until this shows up in the profiler…
  virtual Arf arf_(long prec) const override {
    if (prec < 1) {
      prec = 0;
    }

    // We could work with a bigger base than 2 to speed things up here but it's
    // probably not worth it.
    const string chars = "01";

    auto rnd = rng();
    boost::random::uniform_int_distribution<size_t> index_dist(0, chars.size() - 1);

    long exp = 0;
    stringstream os;
    // With d binary digits, we get a relative error at most 2^(1-d). Since we
    // want it to be at most 2^-prec, we need prec + 1 digits.
    for (long digits = 0; digits < prec + 1; digits++, exp--) {
      char c = chars[index_dist(rnd)];
      if (digits == 0 && c == '0') {
        digits--;
        continue;
      }
      os << c;
    }

    return Arf(os.str(), 2, exp);
  }

  operator std::optional<mpq_class>() const override {
    return std::nullopt;
  }

  RealNumber const& operator>>(ostream& out) const override {
    out << "ℝ(" << static_cast<double>(*this) << "…)";
    return *this;
  }

  template <typename Archive>
  void save(Archive& archive) const {
    archive(cereal::make_nvp("seed", seed));
  }

 private:
  unsigned int seed;
  // We use a Random Number Generator that is fast and has a small memory
  // footprint as we need to create it frequently. It might have some
  // cryptographically undesirable properties but this should not matter for
  // our purposes.  Note that technically this does not produce a
  // transcendental number due to a pseudo-random number generator's
  // periodicity.
  rand48 rng() const { return rand48(seed); }
};

auto& factory() {
  static unique_factory::UniqueFactory<unsigned int, RealNumber> factory;
  return factory;
}
}  // namespace

namespace exactreal {
shared_ptr<const RealNumber> RealNumber::random() {
  return RealNumber::random(Seed());
}

shared_ptr<const RealNumber> RealNumber::random(Seed seed) {
  return factory().get(seed.value, [&]() { return new RandomRealNumber(seed.value); });
}

void save_random(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& base) {
  std::dynamic_pointer_cast<const RandomRealNumber>(base)->save(archive);
}

void load_random(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& base) {
  unsigned int seed;
  archive(cereal::make_nvp("seed", seed));
  base = factory().get(seed, [&]() { return new RandomRealNumber(seed); });
}
}  // namespace exactreal
