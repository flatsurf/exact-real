import json
import glob
import os
import os.path
import subprocess

ASV_BUILD_DIR = os.environ.get('ASV_BUILD_DIR', os.path.dirname(os.path.abspath(__file__)) + "/../../")

BENCHMARKS = {}
PREFIX = "track_"

os.chdir(ASV_BUILD_DIR)
for benchmark in glob.glob('test/**/*.*.cc'):
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

        asv_name = "%s%s"%(PREFIX, name.replace("/", "_"), )
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
