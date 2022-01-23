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

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <memory>
#include <sstream>

#include "../exact-real/cereal.hpp"
#include "../exact-real/real_number.hpp"
#include "../exact-real/seed.hpp"
#include "../exact-real/yap/arf.hpp"
#include "external/hash-combine/hash.hpp"
#include "external/unique-factory/unique-factory/unique-factory.hpp"
#include "impl/real_number_base.hpp"

using namespace exactreal;
using boost::lexical_cast;
using std::isfinite;
using std::nextafter;
using std::numeric_limits;
using std::ostream;
using std::shared_ptr;
using std::string;

namespace {

// A random real number in [a, b]
class ConstrainedRandomRealNumber final : public RealNumberBase {
 public:
  ConstrainedRandomRealNumber(const Arf& initial, long e, const shared_ptr<const RealNumber>& inner) : initial(initial), e(e), inner(inner) {}

  virtual Arf arf_(long prec) const override {
    if (prec < 1) {
      prec = 0;
    }

    long necessary_digits = prec + 1;

    Arf ret(initial);
    long initial_digits = initial == 0 ? 0 : (mpz_sizeinbase(initial.mantissa().get_mpz_t(), 2));
    long missing_digits = necessary_digits - initial_digits;

    // initial does not have enough valid digits, we need to ask inner for more
    // digits
    if (missing_digits > 0) {
      // If inner is smaller than ½, then this might be asking for a few
      // digits too many, but it does not matter in practice. (And we round
      // them away below anyway.)
      Arf additional(inner->arf(missing_digits - 1));
      additional <<= (e - 1);
      ret += additional(prec + 1)(Arf::Round::NEAR);
    }

    arf_set_round(ret.arf_t(), ret.arf_t(), prec + 1, ARF_RND_NEAR);
    return ret;
  }

  explicit operator std::optional<mpq_class>() const override {
    return std::nullopt;
  }

  RealNumber const& operator>>(ostream& out) const override {
    out << "ℝ(" << initial << " + " << *inner << "p" << e << ")";
    if (lexical_cast<string>(static_cast<double>(initial)).compare(lexical_cast<string>(static_cast<double>(*this)))) {
      out << "[∼" << static_cast<double>(*this) << "]";
    }
    return *this;
  }

  template <typename Archive>
  void save(Archive& archive) const {
    archive(cereal::make_nvp("initial", initial),
            cereal::make_nvp("e", e),
            cereal::make_nvp("inner", inner));
  }

 private:
  Arf initial;
  long e;
  shared_ptr<const RealNumber> inner;
};

auto& factory() {
  using Key = std::tuple<Arf, long, std::shared_ptr<const RealNumber>>;
  struct Hash {
    size_t operator()(const Key& key) const {
      using flatsurf::hash_combine, flatsurf::hash;
      return hash_combine(hash(std::get<0>(key)), hash(std::get<1>(key)), hash(static_cast<double>(*std::get<2>(key))));
    }
  };
  static unique_factory::UniqueFactory<Key, RealNumber, unique_factory::KeepNothingAlive, Hash> factory;
  return factory;
}
}  // namespace

namespace exactreal {
shared_ptr<const RealNumber> RealNumber::random(const Arf& lower, const Arf& upper) {
  return RealNumber::random(lower, upper, Seed());
}

shared_ptr<const RealNumber> RealNumber::random(const Arf& lower, const Arf& upper, Seed seed) {
  if (lower == 0 && upper == 1) {
    return RealNumber::random(seed);
  } else {
    if (lower >= upper) {
      throw std::logic_error("interval must have an interior");
    }

    // We first rewrite a and b as mantissa + 2^exponent with the same exponent
    // and then select a random mantissa. Our random number is then going to be
    // this "mantissa" followed by random bits. Specifically, if we write
    // initial = m·2^e, then this returns initial + inner·2^(e - 1)

    auto lower_mantissa = lower.mantissa();
    auto lower_exponent = lower.exponent();

    auto upper_mantissa = upper.mantissa();
    auto upper_exponent = upper.exponent();

    // If a and b are very different in size, these shifts could lead to Out Of
    // Memory situations.

    if (lower_exponent < upper_exponent) {
      mpz_class delta = upper_exponent - lower_exponent;
      mpz_mul_2exp(upper_mantissa.get_mpz_t(), upper_mantissa.get_mpz_t(), delta.get_si());
      upper_exponent = lower_exponent;
    }

    if (upper_exponent < lower_exponent) {
      mpz_class delta = lower_exponent - upper_exponent;
      mpz_mul_2exp(lower_mantissa.get_mpz_t(), lower_mantissa.get_mpz_t(), delta.get_si());
      lower_exponent = upper_exponent;
    }

    mpz_class length = upper_mantissa - lower_mantissa;
    assert(length > 0);

    mpz_class mantissa;

    gmp_randstate_t rnd;
    gmp_randinit_default(rnd);
    gmp_randseed_ui(rnd, seed.value);
    mpz_urandomm(mantissa.get_mpz_t(), rnd, length.get_mpz_t());
    gmp_randclear(rnd);

    mantissa += lower_mantissa;

    long e = lower_exponent.get_si();
    Arf initial = Arf(mantissa, e);
    auto inner = RealNumber::random(seed);

    return factory().get(std::tuple{initial, e, inner}, [&]() {
      return new ConstrainedRandomRealNumber(initial, e, inner);
    });
  }
}

shared_ptr<const RealNumber> RealNumber::random(const double x) {
  return RealNumber::random(x, Seed());
}

shared_ptr<const RealNumber> RealNumber::random(const double x, Seed seed) {
  if (!isfinite(x)) {
    throw std::logic_error("not implemented - random number close to non-finite");
  }

  // We rewrite x as a small interval such that any element of that interval
  // is closer to x than to another double.
  // We start with an interval that ends at the preceding and following
  // double…
  Arf lower = Arf(nextafter(x, -numeric_limits<double>::infinity()));
  Arf midpoint = Arf(x);
  Arf upper = Arf(nextafter(x, numeric_limits<double>::infinity()));

  // … and shrink it so it only contains midpoint.
  // We use 64 bits of precision, more than a double can possibly have:
  lower = ((lower + 2 * midpoint) / 3)(64, Arf::Round::NEAR);
  upper = ((2 * midpoint + upper) / 3)(64, Arf::Round::NEAR);

  // Check that x is the nearest double to [lower, upper]
  assert(static_cast<double>(lower) == x);
  assert(static_cast<double>(upper) == x);

  // Note that the result is smaller than 2^-1023 if x == 0 (which is probably
  // not what you wanted.)
  return random(lower, upper, seed);
}

void save_constrained(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& base) {
  std::dynamic_pointer_cast<const ConstrainedRandomRealNumber>(base)->save(archive);
}

void load_constrained(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& base) {
  Arf initial;
  long e;
  shared_ptr<const RealNumber> inner;
  archive(initial, e, inner);

  base = factory().get(std::tuple{initial, e, inner}, [&]() { return new ConstrainedRandomRealNumber(initial, e, inner); });
}
}  // namespace exactreal
