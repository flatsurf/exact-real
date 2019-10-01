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

#include <cassert>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <memory>
#include <set>
#include <vector>

#include "exact-real/cereal.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arf.hpp"
#include "external/unique-factory/unique_factory.hpp"

using namespace exactreal;
using std::make_shared;
using std::map;
using std::optional;
using std::shared_ptr;
using std::vector;

using Factors = map<shared_ptr<const RealNumber>, int>;

namespace {
// A product of transcendental reals
class RealNumberProduct final : public RealNumber {
 public:
  explicit RealNumberProduct(const Factors& factors) : factors(factors) {
    assert(std::all_of(factors.begin(), factors.end(), [](auto& factor) { return factor.second >= 1; }) && "factors must appear at least once");
    assert(std::all_of(factors.begin(), factors.end(), [](auto& factor) { return !static_cast<std::optional<mpq_class>>(*factor.first); }) && "factors must be transcendental");
  }

  RealNumber const& operator>>(std::ostream& os) const override {
    bool first = true;
    for (auto& factor : factors) {
      if (!first) os << "*";
      first = false;
      os << *factor.first;

      if (factor.second != 1) {
        os << "^" << factor.second;
      }
    }
    return *this;
  }

  explicit operator std::optional<mpq_class>() const override {
    return {};
  }

  Arf arf(long prec) const override {
    // We naively compute the product of all the factors, i.e., we do
    // nothing smart about repeated factors.
    // The following analysis could certainly be done much more sharply but
    // since we usually only have very few factors, it does not matter that
    // much that it's using too much precision.
    // We get an approximation for every factor with arf(). Since this
    // rounds down, every factor has an error of 1 ulp.
    // These errors essentially sum (plus the product of the errors.)
    // Additionally, each multiplication introduces a rounding error of 1
    // ulp.
    long nfactors = 0;
    for (auto& factor : factors)
      nfactors += factor.second;

    long workingPrec = prec + static_cast<long>(ceil(log2(nfactors * 2 + (nfactors - 1)) + 1));

    Arf ret(1);
    for (auto& factor : factors)
      for (int i = 0; i < factor.second; i++)
        ret *= factor.first->arf(workingPrec)(workingPrec, Arf::Round::NEAR);

    // The calling code assumes that no extra digits are present, so we drop
    // everything beyond prec bits (we must use NEAR here since otherwise
    // the error might bee too big. As a consequence, the bit sequence here
    // might not be stable, unlike for the random numbers, i.e., for one
    // digit we might see 0.1 but for two digits 0.01.
    fmpz_t m, e;
    fmpz_init(m);
    fmpz_init(e);
    arf_get_fmpz_2exp(m, e, ret.arf_t());
    arf_set_round_fmpz_2exp(ret.arf_t(), m, e, prec + 1, ARF_RND_NEAR);
    fmpz_clear(m);
    fmpz_clear(e);

    return ret;
  }

  Factors factors;

  template <typename Archive>
  void save(Archive& archive) const {
    archive(cereal::make_nvp("factors", factors));
  }
};

auto& factory() {
  static unique_factory::UniqueFactory<std::weak_ptr<RealNumberProduct>, Factors> factory;
  return factory;
}
}  // namespace

namespace exactreal {
shared_ptr<const RealNumber> RealNumber::operator*(const RealNumber& rhs) const {
  auto rational = static_cast<std::optional<mpq_class>>(rhs);
  if (rational) {
    return rhs * *this;
  }

  Factors factors;
  for (auto factor : {this->shared_from_this(), rhs.shared_from_this()}) {
    assert(!static_cast<optional<mpq_class>>(*factor) && "All factors must be transcendental");
    auto product = std::dynamic_pointer_cast<const RealNumberProduct>(factor);
    if (product) {
      for (auto& f : product->factors) {
        factors[f.first] += f.second;
      }
    } else {
      factors[factor] += 1;
    }
  }

  return factory().get(factors, [&]() { return new RealNumberProduct(factors); });
}

void save_product(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& base) {
  std::dynamic_pointer_cast<const RealNumberProduct>(base)->save(archive);
}

void load_product(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& base) {
  Factors factors;
  archive(cereal::make_nvp("factors", factors));
  base = factory().get(factors, [&]() {
    return new RealNumberProduct(Factors(factors.begin(), factors.end()));
  });
}
}  // namespace exactreal
