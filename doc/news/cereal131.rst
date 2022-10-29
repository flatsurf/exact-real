**Added:**

* Added support for Python 3.10. We now test automatically that everything in pyexactreal works in our CI for this version of Python.

**Removed:**

* Removed explicit support for Python 3.6. We do not explicitly test things for Python 3.6 anymore which had reached its end-of-life in 2021.

**Fixed:**

* Fixed serialization with cereal 1.3.1+ in the same way we did for e-antic in https://github.com/flatsurf/e-antic/pull/242. We also fix a segfault that could happen when mixing different versions of cereal.
