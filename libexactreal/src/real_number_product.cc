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

#include <cassert>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <memory>
#include <set>
#include <vector>
#include <variant>

#include "../exact-real/cereal.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/yap/arf.hpp"
#include "external/unique-factory/unique-factory/unique-factory.hpp"
#include "impl/real_number_base.hpp"
#include "util/assert.ipp"

namespace exactreal {
namespace {

// A product of transcendental reals
class RealNumberProduct final : public RealNumberBase {
 public:
  using Monomials = std::vector<std::shared_ptr<const RealNumber>>;
  using Exponents = std::vector<int>;

  RealNumberProduct(Monomials monomials, Exponents exponents, size_t hash);

  RealNumberProduct(Monomials monomials, Exponents exponents);

  RealNumber const& operator>>(std::ostream& os) const override;

  explicit operator std::optional<mpq_class>() const override;

  std::optional<std::shared_ptr<const RealNumber>> operator/(const RealNumber& rhs) const override;

  Arf arf_(long prec) const override;

  int totalDegree() const;

  std::vector<std::shared_ptr<const RealNumber>> monomials;
  std::vector<int> exponents;
  size_t hash;

  template <typename Archive>
  void save(Archive& archive) const;
};

class Key {
  struct Factors {
    const RealNumber* lhs;
    const RealNumber* rhs;
  };

  struct MonomialsExponents {
    MonomialsExponents(const std::vector<std::shared_ptr<const RealNumber>>& monomials, std::vector<int> exponents) : MonomialsExponents(
      [&]() {
        std::vector<size_t> monomials_id;
        for (const auto& monomial : monomials)
          monomials_id.push_back(RealNumberBase::id(*monomial));
        return monomials_id;
      }(),
      std::move(exponents)) {}

    MonomialsExponents(std::vector<size_t> monomials, std::vector<int> exponents) : monomials(std::move(monomials)), exponents(std::move(exponents)), hash([&]() {
      size_t hash = 1;

      const std::function<size_t(size_t, size_t)> pow = [&](size_t base, size_t exponent) -> size_t {
        if (exponent == 0)
          return 1;
        else if (exponent == 2)
          return base * base;
        else if (exponent % 2)
          return base * pow(base, exponent-1);
        else
          return pow(pow(base, exponent / 2), 2);
      };

      for (size_t i = 0; i < this->monomials.size(); i++)
        hash *= pow(this->monomials[i], this->exponents[i]);
      return hash;
    }()) {
    }

    MonomialsExponents(std::vector<size_t> monomials, std::vector<int> exponents, size_t hash) : monomials(std::move(monomials)), exponents(std::move(exponents)), hash(hash) {}
  
    std::vector<size_t> monomials;
    std::vector<int> exponents;
    size_t hash;
  };
 public:
  struct Hash {
    size_t operator()(const Key& key) const {
      return std::visit([&](const auto& data) -> size_t {
        return this->operator()(data);
      }, key.data);
    }

    size_t operator()(const Key::MonomialsExponents& monomials) const {
      return monomials.hash;
    }

    size_t operator()(const Key::Factors& factors) const {
        const auto hash = [](const auto& x) {
          if (typeid(x) == typeid(const RealNumberProduct&)) {
            return static_cast<const RealNumberProduct&>(x).hash;
          } else {
            return RealNumberBase::id(x);
          }
        };

        return hash(*factors.lhs) * hash(*factors.rhs);
    }
  };

  Key(const RealNumber& lhs, const RealNumber& rhs) : data(Factors{&lhs, &rhs}) {}

  Key(const RealNumberProduct::Monomials& monomials, const RealNumberProduct::Exponents& exponents) : data(MonomialsExponents{monomials, exponents}) {
  }

  bool operator==(const Key& rhs) const {
    return std::visit([](const auto& lhs, const auto& rhs) -> bool {
      using L = std::decay_t<decltype(lhs)>;
      using R = std::decay_t<decltype(rhs)>;

      if constexpr (std::is_same_v<L, MonomialsExponents> && std::is_same_v<R, MonomialsExponents>) {
        return lhs.monomials == rhs.monomials && lhs.exponents == rhs.exponents;
      } else if constexpr (std::is_same_v<L, Factors> && std::is_same_v<R, Factors>) {
        LIBEXACTREAL_UNREACHABLE("There should never be a need to compare two Factors since they are persisted to MonomialsExponents once they are inserted into the cache.");
      } else {
        const auto eq = [](const auto& lhs, const auto& rhs) {
          const auto eq = [](const auto& lhs_lhs_monomials, const auto& lhs_lhs_exponents, const auto& lhs_rhs_monomials, const auto& lhs_rhs_exponents, const auto& rhs_monomials, const auto& rhs_exponents) {
            using std::begin, std::end;

            const size_t lhs_lhs_size = end(lhs_lhs_monomials) - begin(lhs_lhs_monomials);
            const size_t lhs_rhs_size = end(lhs_rhs_monomials) - begin(lhs_rhs_monomials);
            const size_t rhs_size = end(rhs_monomials) - begin(rhs_monomials);

            size_t lhs_lhs_i = 0;
            size_t lhs_rhs_i = 0;
            size_t rhs_i = 0;

            while(true) {
              if (rhs_i == rhs_size) {
                return lhs_lhs_i == lhs_lhs_size && lhs_rhs_i == lhs_rhs_size;
              }

              const size_t rhs_monomial = rhs_monomials[rhs_i];
              const int rhs_exponent = rhs_exponents[rhs_i];
              rhs_i++;

              if (lhs_lhs_i != lhs_lhs_size) {
                const size_t lhs_lhs_monomial = RealNumberBase::id(*lhs_lhs_monomials[lhs_lhs_i]);
                const int lhs_lhs_exponent = lhs_lhs_exponents[lhs_lhs_i];

                if (lhs_lhs_monomial < rhs_monomial) {
                  return false;
                }

                if (lhs_rhs_i != lhs_rhs_size) {
                  const size_t lhs_rhs_monomial = RealNumberBase::id(*lhs_rhs_monomials[lhs_rhs_i]);
                  const int lhs_rhs_exponent = lhs_rhs_exponents[lhs_rhs_i];

                  if (lhs_rhs_monomial < rhs_monomial) {
                    return false;
                  }

                  if (lhs_lhs_monomial == rhs_monomial && lhs_rhs_monomial == rhs_monomial) {
                    if (lhs_lhs_exponent + lhs_rhs_exponent != rhs_exponent) {
                      return false;
                    }

                    lhs_lhs_i++;
                    lhs_rhs_i++;
                  } else if (lhs_lhs_monomial == rhs_monomial) {
                    if (lhs_lhs_exponent != rhs_exponent) {
                      return false;
                    }

                    lhs_lhs_i++;
                  } else  if (lhs_rhs_monomial == rhs_monomial) {
                    if (lhs_rhs_exponent != rhs_exponent) {
                      return false;
                    }

                    lhs_rhs_i++;
                  } else {
                    return false;
                  }
                } else {
                  if (lhs_lhs_monomial != rhs_monomial || lhs_lhs_exponent != rhs_exponent) {
                    return false;
                  }

                  lhs_lhs_i++;
                }
              } else if (lhs_rhs_i != lhs_rhs_size) {
                const size_t lhs_rhs_monomial = RealNumberBase::id(*lhs_rhs_monomials[lhs_rhs_i]);
                const int lhs_rhs_exponent = lhs_rhs_exponents[lhs_rhs_i];

                if (lhs_rhs_monomial != rhs_monomial || lhs_rhs_exponent != rhs_exponent) {
                  return false;
                }

                lhs_rhs_i++;
              } else {
                return false;
              }
            }
          };

          if (typeid(*lhs.lhs) == typeid(const RealNumberProduct&)) {
            const auto& lhs_lhs = static_cast<const RealNumberProduct&>(*lhs.lhs);

            if (typeid(*lhs.rhs) == typeid(const RealNumberProduct&)) {
              const auto& lhs_rhs = static_cast<const RealNumberProduct&>(*lhs.rhs);

              return eq(lhs_lhs.monomials, lhs_lhs.exponents, lhs_rhs.monomials, lhs_rhs.exponents, rhs.monomials, rhs.exponents);
            } else {
              const RealNumber* lhs_rhs_monomials[]{ lhs.rhs };
              int lhs_rhs_exponents[]{ 1 };

              return eq(lhs_lhs.monomials, lhs_lhs.exponents, lhs_rhs_monomials, lhs_rhs_exponents, rhs.monomials, rhs.exponents);
            }
          } else {
            const RealNumber* lhs_lhs_monomials[]{ lhs.lhs };
            int lhs_lhs_exponents[]{ 1 };

            if (typeid(*lhs.rhs) == typeid(const RealNumberProduct&)) {
              const auto& lhs_rhs = static_cast<const RealNumberProduct&>(*lhs.rhs);

              return eq(lhs_lhs_monomials, lhs_lhs_exponents, lhs_rhs.monomials, lhs_rhs.exponents, rhs.monomials, rhs.exponents);
            } else {
              const RealNumber* lhs_rhs_monomials[]{ lhs.rhs };
              int lhs_rhs_exponents[]{ 1 };

              return eq(lhs_lhs_monomials, lhs_lhs_exponents, lhs_rhs_monomials, lhs_rhs_exponents, rhs.monomials, rhs.exponents);
            }
          }
        };

        if constexpr (std::is_same_v<L, MonomialsExponents> && std::is_same_v<R, Factors>) {
          return eq(rhs, lhs);
        } else if constexpr (std::is_same_v<L, Factors> && std::is_same_v<R, MonomialsExponents>) {
          return eq(lhs, rhs);
        } else {
          static_assert(false_t<L> && false_t<R>, "Unsuported cache key types.");
        }
      }
    }, this->data, rhs.data);
  }

  RealNumber* persist() const {
    return std::visit([&](const auto& data) -> RealNumber* {
      using T = std::decay_t<decltype(data)>;
      if constexpr (std::is_same_v<T, Factors>) {
        const auto persist = [&](const auto& lhs_monomials, const auto& lhs_exponents, const auto& rhs_monomials, const auto& rhs_exponents) -> RealNumber* {
          size_t lhs_size = end(lhs_monomials) - begin(lhs_monomials);
          size_t rhs_size = end(rhs_monomials) - begin(rhs_monomials);

          size_t lhs_i = 0;
          size_t rhs_i = 0;

          RealNumberProduct::Monomials monomials;
          std::vector<size_t> monomials_id;
          RealNumberProduct::Exponents exponents;

          while (true) {
            if (lhs_i < lhs_size) {
              const auto lhs_id = RealNumberBase::id(*lhs_monomials[lhs_i]);

              if (rhs_i < rhs_size) {
                const auto rhs_id = RealNumberBase::id(*rhs_monomials[rhs_i]);

                if (lhs_id < rhs_id) {
                  monomials.push_back(lhs_monomials[lhs_i]);
                  monomials_id.push_back(lhs_id);
                  exponents.push_back(lhs_exponents[lhs_i]);

                  lhs_i++;
                } else if (lhs_id == rhs_id) {
                  monomials.push_back(lhs_monomials[lhs_i]);
                  monomials_id.push_back(lhs_id);
                  exponents.push_back(lhs_exponents[lhs_i] + rhs_exponents[rhs_i]);

                  lhs_i++;
                  rhs_i++;
                } else {
                  monomials.push_back(rhs_monomials[rhs_i]);
                  monomials_id.push_back(rhs_id);
                  exponents.push_back(rhs_exponents[rhs_i]);

                  rhs_i++;
                }
              } else {
                monomials.push_back(lhs_monomials[lhs_i]);
                monomials_id.push_back(lhs_id);
                exponents.push_back(lhs_exponents[lhs_i]);

                lhs_i++;
              }
            } else if (rhs_i < rhs_size) {
                const auto rhs_id = RealNumberBase::id(*rhs_monomials[rhs_i]);

                monomials.push_back(rhs_monomials[rhs_i]);
                monomials_id.push_back(rhs_id);
                exponents.push_back(rhs_exponents[rhs_i]);

                rhs_i++;
            } else {
              break;
            }
          }

          const size_t hash = Hash{}(data);

          this->data = MonomialsExponents{std::move(monomials_id), exponents, hash};

          return new RealNumberProduct{std::move(monomials), std::move(exponents), hash};
        };

        if (data.lhs == data.rhs) {
          if (typeid(*data.lhs) == typeid(const RealNumberProduct&)) {
            const auto& base = static_cast<const RealNumberProduct&>(*data.lhs);

            auto exponents = base.exponents;
            for (auto& e : exponents)
              e *= 2;

            std::vector<size_t> monomials_id;
            for (const auto& monomial : base.monomials)
              monomials_id.push_back(RealNumberBase::id(*monomial));

            size_t hash = base.hash * base.hash;

            this->data = MonomialsExponents{std::move(monomials_id), exponents, hash};

            return new RealNumberProduct{base.monomials, std::move(exponents), hash};
          } else {
            RealNumberProduct* product = new RealNumberProduct{{data.lhs->shared_from_this()}, {2}, RealNumberBase::id(*data.lhs) * RealNumberBase::id(*data.lhs)};
            this->data = MonomialsExponents{{RealNumberBase::id(*data.lhs)}, {2}, product->hash};
            return product;
          }
        } else {
          if (typeid(*data.lhs) == typeid(const RealNumberProduct&)) {
            const auto& lhs = static_cast<const RealNumberProduct&>(*data.lhs);

            if (typeid(*data.rhs) == typeid(const RealNumberProduct&)) {
              const auto& rhs = static_cast<const RealNumberProduct&>(*data.rhs);

              return persist(lhs.monomials, lhs.exponents, rhs.monomials, rhs.exponents);
            } else {
              std::shared_ptr<const RealNumber> rhs_monomials[]{ data.rhs->shared_from_this() };
              int rhs_exponents[]{ 1 };

              return persist(lhs.monomials, lhs.exponents, rhs_monomials, rhs_exponents);
            }
          } else {
            std::shared_ptr<const RealNumber> lhs_monomials[] { data.lhs->shared_from_this() };
            int lhs_exponents[]{ 1 };

            if (typeid(*data.rhs) == typeid(const RealNumberProduct&)) {
              const auto& rhs = static_cast<const RealNumberProduct&>(*data.rhs);

              return persist(lhs_monomials, lhs_exponents, rhs.monomials, rhs.exponents);
            } else {
              std::shared_ptr<const RealNumber> rhs_monomials[]{ data.rhs->shared_from_this() };
              int rhs_exponents[]{ 1 };

              return persist(lhs_monomials, lhs_exponents, rhs_monomials, rhs_exponents);
            }
          }
        }
      } else if constexpr (std::is_same_v<T, MonomialsExponents>) {
        LIBEXACTREAL_UNREACHABLE("Cannot persist a MonomialsExponents object since we do not know which reals are underlying it actually.");
      } else {
        static_assert(false_t<T>, "Unexpected kind of product factory cache key.");
      }

      LIBEXACTREAL_UNREACHABLE("persist() did not produce anything.");
    }, this->data);
  }

 private:

  mutable std::variant<Factors, MonomialsExponents> data;
};


/// Return a factory that turns a map {real: exponent} into the product π real^exponent.
auto& factory() {
  static unique_factory::UniqueFactory<Key, RealNumber, unique_factory::KeepSetAlive<RealNumber, 1024>, Key::Hash> factory;

  return factory;
}

// A product of transcendental reals
RealNumberProduct::RealNumberProduct(Monomials monomials, Exponents exponents, size_t hash) : monomials(std::move(monomials)), exponents(std::move(exponents)), hash(hash) {
  LIBEXACTREAL_ASSERT(this->monomials.size() == this->exponents.size(), "Length of monomials and exponents must be equal");
  LIBEXACTREAL_ASSERT(std::all_of(begin(this->monomials), end(this->monomials), [](const auto& monomial) { return !static_cast<std::optional<mpq_class>>(*monomial); }), "monomials must be transcendental in " << *this);
  LIBEXACTREAL_ASSERT(this->monomials.size() > 1 || (this->monomials.size() == 1 && this->exponents[0] > 1), "There must be at least two monomials or the only exponent must be at least 2 in " << *this);
}

RealNumberProduct::RealNumberProduct(Monomials monomials, Exponents exponents) : RealNumberProduct(monomials, exponents, Key::Hash{}(Key{monomials, exponents})) {}

RealNumber const& RealNumberProduct::operator>>(std::ostream& os) const {
  bool first = true;
  for (size_t i = 0; i < monomials.size(); i++) {
    if (!first) os << "*";
    first = false;
    os << *monomials[i];

    if (exponents[i] != 1) {
      os << "^" << exponents[i];
    }
  }
  return *this;
}

RealNumberProduct::operator std::optional<mpq_class>() const {
  return std::nullopt;
}

std::optional<std::shared_ptr<const RealNumber>> RealNumberProduct::operator/(const RealNumber& rhs) const {
  {
    auto rational = static_cast<std::optional<mpq_class>>(rhs);
    if (rational) {
      if (*rational == 1)
        return this->shared_from_this();
      throw std::logic_error("not implemented: division of product by rational");
    }
  }

  auto quotient = this->exponents;

  if (typeid(rhs) == typeid(const RealNumberProduct&)) {
    const auto& rhs_monomials = static_cast<const RealNumberProduct&>(rhs).monomials;
    const auto& rhs_exponents = static_cast<const RealNumberProduct&>(rhs).exponents;

    size_t lhs_i = 0;
    size_t rhs_i = 0;

    while(true) {
      if (rhs_i == rhs_monomials.size())
        break;

      if (lhs_i == monomials.size())
        return std::nullopt;

      const auto& lhs_monomial = *monomials[lhs_i];
      const auto& rhs_monomial = *rhs_monomials[rhs_i];

      const auto lhs_monomial_id = RealNumberBase::id(lhs_monomial);
      const auto rhs_monomial_id = RealNumberBase::id(rhs_monomial);

      if (lhs_monomial_id < rhs_monomial_id) {
        lhs_i++;
      } else if (lhs_monomial_id > rhs_monomial_id) {
        return std::nullopt;
      } else {
        if (rhs_exponents[rhs_i] > quotient[lhs_i])
          return std::nullopt;

        quotient[lhs_i] -= rhs_exponents[rhs_i];

        lhs_i++;
        rhs_i++;
      }
    }
  } else {
    size_t lhs_i = 0;
    for (;lhs_i < monomials.size(); lhs_i++) {
      if (*monomials[lhs_i] == rhs) {
        quotient[lhs_i]--;
        break;
      }
    }

    if (lhs_i == monomials.size())
      return std::nullopt;
  }

  RealNumberProduct::Monomials monomials;
  RealNumberProduct::Exponents exponents;

  for (size_t i = 0; i < quotient.size(); i++) {
    if (quotient[i]) {
      monomials.push_back(this->monomials[i]);
      exponents.push_back(quotient[i]);
    }
  }

  if (exponents.size() == 0)
    return RealNumber::rational(1);

  if (exponents.size() == 1 && exponents[0] == 1)
    return monomials[0];

  return factory().get({monomials, exponents}, [&]() { return new RealNumberProduct(monomials, exponents); });
}

Arf RealNumberProduct::arf_(long prec) const {
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
  long nfactors = totalDegree();

  long workingPrec = prec + static_cast<long>(ceil(log2(nfactors * 2 + (nfactors - 1)) + 1));

  Arf ret(1);
  for (size_t i = 0; i < monomials.size(); i++)
    for (int j = 0; j < exponents[i]; j++)
      ret *= monomials[i]->arf(workingPrec)(workingPrec, Arf::Round::NEAR);

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

int RealNumberProduct::totalDegree() const {
  int totalDegree = 0;
  for (auto& exponent : exponents)
    totalDegree += exponent;
  return totalDegree;
}

template <typename Archive>
void RealNumberProduct::save(Archive& archive) const {
  std::map<std::shared_ptr<const RealNumber>, int> factors;

  for (size_t i = 0; i < monomials.size(); i++)
    factors[monomials[i]] = exponents[i];

  archive(cereal::make_nvp("factors", factors));
}
}  // namespace

std::shared_ptr<const RealNumber> RealNumber::operator*(const RealNumber& rhs) const {
  if (static_cast<std::optional<mpq_class>>(rhs))
    // Call RationalRealNumber::operator* instead.
    return rhs * *this;

  return factory().get({*this, rhs}, [&](const auto& key) {
    return key.persist();
  });
}

bool RealNumber::deglex(const RealNumber& rhs_) const {
  if (*this == rhs_)
    return false;

  if (typeid(*this) != typeid(const RealNumberProduct&)) {
    if (typeid(rhs_) != typeid(const RealNumberProduct&)) {
      // Neither of the numbers is a product of real numbers, i.e., they
      // correspond to polynomials of total degree <= 1.

      const bool lhs_rational = static_cast<std::optional<mpq_class>>(*this).has_value();
      const bool rhs_rational = static_cast<std::optional<mpq_class>>(rhs_).has_value();
      if (lhs_rational)
        return !rhs_rational;
      if (rhs_rational)
        return false;

      // We order indeterminates, i.e., non-rational primitive real numbers, by
      // their internal id.
      return RealNumberBase::id(*this) < RealNumberBase::id(rhs_);
    } else {
      // The right hand side has a total degree >= 2 but the left hand side has
      // a total degree <= 1.
      return true;
    }
  } else {
    const auto* lhs = static_cast<const RealNumberProduct*>(this);

    if (typeid(rhs_) != typeid(const RealNumberProduct&)) {
      // The left hand side has a total degree >= 2 but the right hand side has
      // a total degree <= 1.
      return false;
    } else {
      const auto* rhs = static_cast<const RealNumberProduct*>(&rhs_);

      {
        const int lhs_total_degree = lhs->totalDegree();
        const int rhs_total_degree = rhs->totalDegree();
        if (lhs_total_degree != rhs_total_degree)
          return lhs_total_degree < rhs_total_degree;
      }

      for (size_t i = 0, j = 0; i < lhs->monomials.size() || j < rhs->monomials.size();) {
        if (i < lhs->monomials.size() && j < rhs->monomials.size()) {
          const size_t lhs_monomial = RealNumberBase::id(*lhs->monomials[i]);
          const size_t rhs_monomial = RealNumberBase::id(*rhs->monomials[j]);

          if (lhs_monomial < rhs_monomial) {
            // x < y
            return true;
          } else if (lhs_monomial > rhs_monomial) { 
            // y > x
            return false;
          } else {
            const int lhs_exponent = lhs->exponents[i];
            const int rhs_exponent = rhs->exponents[j];

            if (lhs_exponent < rhs_exponent) {
              // xy > x^2
              return false;
            } else if (lhs_exponent > rhs_exponent) {
              // x^2 < xy
              return true;
            } else {
              i++;
              j++;
            }
          }
        } else {
          LIBEXACTREAL_UNREACHABLE("Total degree of products inconsistent with their exponents.");
        }
      }

      LIBEXACTREAL_UNREACHABLE("Real number products pretend to be distinct values but they coincide in deglex comparison: " << *this << " and " << rhs_ << " coincide on every factor.");
    }
  }
}

void save_product(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& base) {
  std::dynamic_pointer_cast<const RealNumberProduct>(base)->save(archive);
}

void load_product(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& base) {
  std::map<std::shared_ptr<const RealNumber>, int> factors;

  archive(cereal::make_nvp("factors", factors));
  
  std::vector<std::shared_ptr<const RealNumber>> monomials;

  for (const auto& factor : factors)
    monomials.push_back(factor.first);

  std::sort(begin(monomials), end(monomials), [](const auto& lhs, const auto& rhs) {
    return RealNumberBase::id(*lhs) < RealNumberBase::id(*rhs);
  });

  std::vector<int> exponents;

  for (size_t i = 0; i < monomials.size(); i++)
    exponents.push_back(factors[monomials[i]]);

  base = factory().get({monomials, exponents}, [&]() { return new RealNumberProduct(monomials, exponents); });
}
}  // namespace exactreal
