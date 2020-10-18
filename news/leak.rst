**Fixed:**

* Fixed a memory leak when printing; this leak is not spotted by valgrind since
  the memory is allocated in a memory pool (we think.) We found that leak with
  gperftools.

* Free flint memory when running tests so gperftools does not get confused by
  memory pools there.
