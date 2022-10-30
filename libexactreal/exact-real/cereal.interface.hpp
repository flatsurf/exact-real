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

#ifndef LIBEXACTREAL_CEREAL_INTERFACE_HPP
#define LIBEXACTREAL_CEREAL_INTERFACE_HPP

#include <string>
#include <memory>
#include <map>
#include <gmpxx.h>

#include "forward.hpp"

namespace exactreal {

struct LIBEXACTREAL_API ICerealizer {
  virtual void save(const std::string& name, const std::string& value) = 0;
  virtual void save(const std::string& name, const mpq_class& value) = 0;
  virtual void save(const std::string& name, unsigned int value) = 0;
  virtual void save(const std::string& name, long value) = 0;
  virtual void save(const std::string& name, const Arf& value) = 0;
  virtual void save(const std::string& name, const std::shared_ptr<const RealNumber>& value) = 0;
  virtual void save(const std::string& name, const std::map<std::shared_ptr<const RealNumber>, int>& value) = 0;

};

struct LIBEXACTREAL_API IDecerealizer {
  virtual void load(const std::string& name, std::string& value) = 0;
  virtual void load(const std::string& name, mpq_class& value) = 0;
  virtual void load(const std::string& name, unsigned int& value) = 0;
  virtual void load(const std::string& name, long& value) = 0;
  virtual void load(const std::string& name, Arf& value) = 0;
  virtual void load(const std::string& name, std::shared_ptr<const RealNumber>& value) = 0;
  virtual void load(const std::string& name, std::map<std::shared_ptr<const RealNumber>, int>& value) = 0;
};

}

#endif
