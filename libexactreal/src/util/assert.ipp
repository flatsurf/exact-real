/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2020 Julian Rüth
 *
 *  intervalxt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  intervalxt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with intervalxt. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef LIBEXACTREAL_UTIL_ASSERT_IPP
#define LIBEXACTREAL_UTIL_ASSERT_IPP

#include <gmpxx.h>

#include <iostream>
#include <sstream>

#include <boost/config.hpp>
#include <boost/preprocessor/stringize.hpp>

#define ASSERT_(CONDITION, EXCEPTION, MESSAGE)                                \
  while (BOOST_UNLIKELY(not(CONDITION))) {                                    \
    std::stringstream user_message, assertion_message;                        \
    user_message << MESSAGE;                                                  \
    assertion_message << (#CONDITION " does not hold");                       \
    if (user_message.str().size())                                            \
      assertion_message << ": " << user_message.str();                        \
    else                                                                      \
      assertion_message << " ";                                               \
    assertion_message << " in " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__);    \
    /* Print the assertion message so we see it even in a noexcept block. */  \
    std::cerr << assertion_message.str() << std::endl;                        \
    ::flatsurf::throw_for_assert(EXCEPTION(assertion_message.str().c_str())); \
  }

// Run a (cheap) check that a (user provided) argument is valid.
// If the check should be disabled when NDEBUG is defined, e.g., because it
// occurs in a hotspot, use ASSERT... instead.
#define CHECK_ARGUMENT_(CONDITION) ASSERT_(CONDITION, std::invalid_argument, "")
#define CHECK_ARGUMENT(CONDITION, MESSAGE) ASSERT_(CONDITION, std::invalid_argument, MESSAGE)
#define CHECK(CONDITION, MESSAGE) ASSERT_(CONDITION, std::logic_error, MESSAGE);

#ifdef NDEBUG

#define ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(true || (CONDITION))
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(true || (CONDITION), MESSAGE)
#define ASSERT(CONDITION, MESSAGE) ASSERT_(true || (CONDITION), std::logic_error, MESSAGE)
#define ASSERTIONS(LAMBDA) \
  while (false) LAMBDA()
#define UNREACHABLE(MESSAGE) ASSERT_(false, std::logic_error, MESSAGE)

#else

#define ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(CONDITION)
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(CONDITION, MESSAGE)
#define ASSERT(CONDITION, MESSAGE) ASSERT_(CONDITION, std::logic_error, MESSAGE)
#define ASSERTIONS(LAMBDA) LAMBDA()
#define UNREACHABLE(MESSAGE) ASSERT_(false, std::logic_error, MESSAGE)

#endif

namespace flatsurf {

// A throw statement that can be used in noexcept marked blocks without
// triggering compiler warnings.
template <typename E>
void throw_for_assert(const E& e) { throw e; }

template <typename T = mpz_class>
class Amortized {
  T budget;

 public:
  Amortized(const T& budget = 1 << 10) :
    budget(budget) {}

  void reset(const T& budget) { this->budget = budget; }
  bool pay(const T& cost) {
    ASSERT(cost >= 0, "cost must be non-negative");
    if (cost > budget) {
      budget++;
      return false;
    }
    budget -= cost;
    return true;
  }
};

}  // namespace flatsurf

#endif
