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

#ifndef LIBEXACTREAL_CEREAL_HPP
#define LIBEXACTREAL_CEREAL_HPP

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <e-antic/cereal.hpp>

#include "arb.hpp"
#include "arf.hpp"
#include "element.hpp"
#include "integer_ring.hpp"
#include "module.hpp"
#include "number_field.hpp"
#include "rational_field.hpp"
#include "real_number.hpp"

namespace exactreal {

// A trivial wrapper for a T so we do not pollute the global namespace with our
// serialization function for T.
template <typename T>
struct CerealWrap {
  CerealWrap() : inner() {}
  CerealWrap(T&& inner) : inner(std::move(inner)) {}
  CerealWrap(const T& inner) : inner(inner) {}

  T inner;

  static bool constexpr needs_wrap() {
    if constexpr (std::is_same_v<T, mpz_class>)
      return true;
    else if constexpr (std::is_same_v<T, mpq_class>)
      return true;
    return false;
  }

  T* operator->() { return &inner; }
  const T* operator->() const { return &inner; }

  T& operator*() { return inner; }
  const T& operator*() const { return inner; }

  operator T() { return inner; }
  operator T() const { return inner; }
};

template <typename T>
using MaybeWrap = std::conditional_t<CerealWrap<T>::needs_wrap(), CerealWrap<T>, T>;

template <typename Archive>
void save(Archive& archive, const Arb& self) {
  char* serialized = arb_dump_str(self.arb_t());
  archive(
      cereal::make_nvp("data", std::string(serialized)),
      cereal::make_nvp("approximation", static_cast<double>(self)));
  flint_free(serialized);
}

template <typename Archive>
void load(Archive& archive, Arb& self) {
  std::string data;
  double ignored;
  archive(data, ignored);

  if (arb_load_str(self.arb_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arb");
  }
}

template <typename Archive>
void save(Archive& archive, const Arf& self) {
  char* serialized = arf_dump_str(self.arf_t());
  archive(
      cereal::make_nvp("data", std::string(serialized)),
      cereal::make_nvp("approximation", static_cast<double>(self)));
  flint_free(serialized);
}

template <typename Archive>
void load(Archive& archive, Arf& self) {
  std::string data;
  double ignored;
  archive(data, ignored);

  if (arf_load_str(self.arf_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arf");
  }
}

template <typename Archive, typename Ring>
void save(Archive& archive, const Element<Ring>& self) {
  archive(cereal::make_nvp("parent", self.module()));

  std::vector<MaybeWrap<typename Ring::ElementClass>> coefficients;
  for (int i = 0; i < self.module()->rank(); i++)
    coefficients.push_back(self[i]);
  archive(cereal::make_nvp("coefficients", coefficients));
}

template <typename Archive, typename Ring>
void load(Archive& archive, Element<Ring>& self) {
  std::shared_ptr<const Module<Ring>> module;
  archive(cereal::make_nvp("parent", module));

  std::vector<MaybeWrap<typename Ring::ElementClass>> coefficients;
  archive(cereal::make_nvp("coefficients", coefficients));

  std::vector<typename Ring::ElementClass> coeffs;
  for (auto& c : coefficients)
    coeffs.push_back(c);

  self = Element<Ring>(module, coeffs);
}

template <typename Archive>
void serialize(Archive&, IntegerRing&) {
  // There's nothing to do for ZZ.
}

template <typename Archive>
void serialize(Archive&, RationalField&) {
  // There's nothing to do for QQ.
}

template <typename Archive>
void serialize(Archive& archive, NumberField& self) {
  archive(cereal::make_nvp("field", self.parameters));
}

struct LIBEXACTREAL_API RealNumberCereal {
  using SupportedOutput = cereal::JSONOutputArchive;
  using SupportedInput = cereal::JSONInputArchive;

  static void save(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);
  static void load(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);
};

template <typename T, typename Cerealizer>
struct ForwardingCereal {
  template <typename Archive>
  static void save(Archive& archive, const T& self) {
    using Native = typename Cerealizer::SupportedOutput;

    if constexpr (std::is_same_v<Archive, Native>) {
      Cerealizer::save(archive, self);
    } else {
      std::stringstream s;
      save(Native(s), self);
      archive(s.str());
    }
  }

  template <typename Archive>
  static void load(Archive& archive, T& self) {
    using Native = typename Cerealizer::SupportedInput;

    if constexpr (std::is_same_v<Archive, Native>) {
      Cerealizer::load(archive, self);
    } else {
      std::string s;
      archive(s);
      load(Native(std::stringstream(s)), self);
    }
  }
};

template <typename Archive>
std::string save_minimal(const Archive&, const CerealWrap<mpq_class>& self) {
  return self->get_str();
}

template <typename Archive>
void load_minimal(const Archive&, CerealWrap<mpq_class>& self, const std::string& value) {
  self = CerealWrap{mpq_class(value)};
}

template <typename Archive>
std::string save_minimal(const Archive&, const CerealWrap<mpz_class>& self) {
  return self->get_str();
}

template <typename Archive>
void load_minimal(const Archive&, CerealWrap<mpz_class>& self, const std::string& value) {
  self = CerealWrap{mpz_class(value)};
}
}  // namespace exactreal

namespace cereal {
template <typename Archive>
void save(Archive& archive, const std::shared_ptr<const exactreal::RealNumber>& self) {
  // cereal has a fairly opinionated way of handling smart pointers to
  // polymorphic types. This is not compatibly with our hiding logic of the
  // implementation, so we hook into cereal's registerSharedPointer to get our
  // real numbers properly deduplicated but handle the actual serialization &
  // deserialization ourselves through our factory functions.
  uint32_t id = archive.registerSharedPointer(self.get());

  archive(cereal::make_nvp("shared", id));
  if (id & cereal::detail::msb_32bit) {
    exactreal::ForwardingCereal<std::shared_ptr<const exactreal::RealNumber>, exactreal::RealNumberCereal>::save(archive, self);
  }
}

template <typename Archive>
void load(Archive& archive, std::shared_ptr<const exactreal::RealNumber>& self) {
  uint32_t id;
  archive(cereal::make_nvp("shared", id));

  if (id & cereal::detail::msb_32bit) {
    exactreal::ForwardingCereal<std::shared_ptr<const exactreal::RealNumber>, exactreal::RealNumberCereal>::load(archive, self);
    archive.registerSharedPointer(id, std::static_pointer_cast<void>(std::const_pointer_cast<exactreal::RealNumber>(self)));
  } else {
    self = std::static_pointer_cast<const exactreal::RealNumber>(archive.getSharedPointer(id));
  }
}

template <typename Archive, typename Ring>
void save(Archive& archive, const std::shared_ptr<const exactreal::Module<Ring>>& self) {
  uint32_t id = archive.registerSharedPointer(self.get());

  archive(cereal::make_nvp("shared", id));
  if (id & cereal::detail::msb_32bit) {
    archive(cereal::make_nvp("basis", self->basis()));
    archive(cereal::make_nvp("ring", self->ring()));
  }
}

template <typename Archive, typename Ring>
void load(Archive& archive, std::shared_ptr<const exactreal::Module<Ring>>& self) {
  // cereal insists on creating a new instance of T when deserializing a
  // shared_ptr<T>. However this is not compatible with our uniqueness of
  // modules (if the module already exists, we do not create a new one but just
  // return a shared_ptr to the existing one.) Therefore, we hook into cereal's
  // deduplication logic for a compact output format but do not rely on its
  // deserialization for Module<Ring>.
  uint32_t id;
  archive(cereal::make_nvp("shared", id));

  if (id & cereal::detail::msb_32bit) {
    typename exactreal::Module<Ring>::Basis basis;
    archive(cereal::make_nvp("basis", basis));
    Ring ring;
    archive(cereal::make_nvp("ring", ring));
    self = exactreal::Module<Ring>::make(basis, ring);
    archive.registerSharedPointer(id, std::static_pointer_cast<void>(std::const_pointer_cast<exactreal::Module<Ring>>(self)));
  } else {
    self = std::static_pointer_cast<exactreal::Module<Ring>>(archive.getSharedPointer(id));
  }
}
}  // namespace cereal

#endif
