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

// Include this file once at the end of your .test.cc file to get a main that
// runs unit tests and benchmarks.

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <vector>

using std::istream_iterator;
using std::istringstream;
using std::string;
using std::vector;

class AbortBenchmarksOnError : public ::testing::EmptyTestEventListener {
  virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
    if (test_part_result.failed()) {
      throw std::logic_error("Benchmark aborted due to failing assertion.");
    }
  }
};

int main(int argc, char** argv) {
  vector<string> args;
  for (int i = 0; i < argc; i++) {
    args.push_back(argv[i]);
  }

  char* env_argv = std::getenv("EXACTREAL_CHECK");
  if (env_argv != nullptr) {
    auto iss = istringstream(env_argv);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(args));
  }

  argc = int(args.size());
  argv = new char*[argc];
  for (int i = 0; i < argc; i++) {
    const char* arg = args[i].c_str();
    argv[i] = new char[strlen(arg) + 1];
    strcpy(argv[i], arg);
  }

  testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();

  if (result) {
    return result;
  }

  testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new AbortBenchmarksOnError);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();

  return result;
}
