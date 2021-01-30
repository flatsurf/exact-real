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
#include <iostream>
#include <sstream>

namespace exactreal {
namespace {

// A throw statement that can be used in noexcept marked blocks without
// triggering compiler warnings.
template <typename E>
void throw_for_assert(const E& e) { throw e; }

}
}  // namespace exactreal

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
    ::exactreal::throw_for_assert(EXCEPTION(assertion_message.str().c_str())); \
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

#endif
