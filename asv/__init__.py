# -*- coding: utf-8 -*-
#*********************************************************************
#  This file is part of flatsurf.
#
#        Copyright (C) 2019 Julian Rüth
#
#  Flatsurf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  Flatsurf is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with flatsurf. If not, see <https://www.gnu.org/licenses/>.
#*********************************************************************

# This module turn google (C++) benchmarks into a Python module that can be run
# by AirSpeedVelocity which provides a nice web interface to see how benchmarks
# perform over time.

# If you want to use this module, make sure NOT to add the conda-forge package
# 'benchmark' (google benchmark) to your asv.conf.json matrix as that is a
# reserved key in ASV. Instead add an exact pin such as 'benchmark==1.4.1'.

import json
import glob
import os
import os.path
import subprocess

ASV_BUILD_DIR = os.environ.get('ASV_BUILD_DIR', os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))

BENCHMARKS = {}
PREFIX = "track_"

os.chdir(ASV_BUILD_DIR)
for benchmark in glob.glob('**/test/*.*.cc'):
    os.system("echo 'Processing %s'"%benchmark)
    os.chdir(ASV_BUILD_DIR)
    os.chdir(os.path.dirname(benchmark))
    benchmark = os.path.basename(benchmark).replace('.', '_')[:-3]
    if benchmark.endswith("_test"): benchmark = benchmark[:-5]
    subprocess.check_call(["make", benchmark])
    subprocess.check_call(["./%s"%(benchmark,), "--benchmark_format=json", "--benchmark_out=%s.json"%(benchmark,)])
    if os.lstat("%s.json"%(benchmark,)).st_size == 0: continue
    results = json.load(open("%s.json"%(benchmark,), "r"))
    for bench in results["benchmarks"]:
        name = bench["name"]
        time = float(bench["cpu_time"])
        unit = bench["time_unit"]

        asv_name = "%s%s"%(PREFIX, name.replace("/", "_").replace("<", "_").replace(">", "_"), )
        def asv_method(self, t=time): return t
        asv_method.unit = unit
        asv_method.number = 1
        asv_method.repeat = 1
        BENCHMARKS[asv_name] = asv_method

class BenchmarkMetaclass(type):
    def __dir__(cls):
        return list(BENCHMARKS.keys())
    def __getattr__(cls, name):
        if not name.startswith(PREFIX):
            raise AttributeError
        setattr(cls, name, BENCHMARKS[name])
        return getattr(cls, name)

class Benchmarks(metaclass=BenchmarkMetaclass):
    def __getattr__(self, name):
        if not name.startswith(PREFIX):
            raise AttributeError
        setattr(type(self), name, BENCHMARKS[name])
        return getattr(self, name)
