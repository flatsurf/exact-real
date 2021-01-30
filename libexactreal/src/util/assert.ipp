/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2020-2021 Julian Rüth
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

#ifndef LIBEXACTREAL_UTIL_ASSERT_IPP
#define LIBEXACTREAL_UTIL_ASSERT_IPP

#include <boost/preprocessor/stringize.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <sstream>
#include <iostream>

namespace exactreal {
namespace {

// A throw statement that can be used in noexcept marked blocks without
// triggering compiler warnings.
template <typename E>
void throw_for_assert(const E& e) { throw e; }

// Return whether an environment variable should be considered as set.
bool isSet(const char* env) {
  const auto* ptr = std::getenv(env);
  if (ptr == nullptr) return false;

  std::string value = ptr;
  boost::trim(value);

  if (value == "0") return false;
  if (boost::iequals(value, "no")) return false;
  if (boost::iequals(value, "false")) return false;

  return true;
}

// Return whether all CHECK_ and ASSERT_ macros have been disabled at runtime
// through the LIBEXACTREAL_NOCHECK environment variable.
bool nocheck() {
  static bool value = isSet("LIBEXACTREAL_NOCHECK");
  return value;
}

// Return whether all ASSERT_ macros have been disabled at runtime through the
// LIBEXACTREAL_NOASSERT environment variable.
bool noassert() {
  if (nocheck()) return true;

  static bool value = isSet("LIBEXACTREAL_NOASSERT");
  return value;
}

}
} // namespace exactreal

#define ASSERT_(CONDITION, EXCEPTION, MESSAGE)                                  \
  while (BOOST_UNLIKELY(not(CONDITION))) {                                      \
    std::stringstream user_message, assertion_message;                          \
    user_message << MESSAGE;                                                    \
    assertion_message << (#CONDITION " does not hold");                         \
    if (user_message.str().size())                                              \
      assertion_message << ": " << user_message.str();                          \
    else                                                                        \
      assertion_message << " ";                                                 \
    assertion_message << " in " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__);      \
    /* show messages in noexcept blocks */                                      \
    std::cerr << assertion_message.str() << std::endl;                          \
    ::exactreal::throw_for_assert(EXCEPTION(assertion_message.str().c_str()));  \
  }

// Run a (cheap) check that a (user provided) argument is valid.
// If the check should be disabled when NDEBUG is defined, e.g., because it
// occurs in a hotspot, use ASSERT_ARGUMENT instead.
#define CHECK_ARGUMENT_(CONDITION) ASSERT_(nocheck() || (CONDITION), std::invalid_argument, "")
#define CHECK_ARGUMENT(CONDITION, MESSAGE) ASSERT_(nocheck() || (CONDITION), std::invalid_argument, MESSAGE)

#ifdef NDEBUG

#define ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(true || (CONDITION))
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(true || (CONDITION), MESSAGE)
#define ASSERT(CONDITION, MESSAGE) ASSERT_(true || (CONDITION), std::logic_error, MESSAGE)

#else

#define ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(noassert() || (CONDITION))
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(noassert() || (CONDITION), MESSAGE)
#define ASSERT(CONDITION, MESSAGE) ASSERT_(noassert() || (CONDITION), std::logic_error, MESSAGE)

#endif

#define UNREACHABLE(MESSAGE)                  \
  {                                           \
    ASSERT_(false, std::logic_error, MESSAGE) \
    __builtin_unreachable();                  \
  }

#endif
