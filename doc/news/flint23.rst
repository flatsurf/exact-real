**Added:**

* Added compatibility with FLINT 3.

**Changed:**

* Changed how Arb expressions print when used in Python. They now just print as "arithmetic expression" since there was a spurious segfault on macOS that we could not reliably reproduce and make sense of.
