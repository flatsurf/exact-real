exact-real Reference Manual
===========================

Our reference manual is built in two steps:

* doxygen builds documentation for the C/C++ headers in XML format,
* sphinx builds the actual HTML documentation which includes the doxygen bits with breathe.

Build the Reference Manual
--------------------------

Running `make` builds the documentation to generated/html. Use `pixi run doc`
to build the documentation and serve it on `http://localhost:8880`.

For faster rebuilds, you can also just run ``make`` in a ``pixi shell -e dev``
in this directory.

If you are working on documentation in headers, this triggers rebuilds of the
libexactreal library. If you do not make any functional changes, you can also
run ``make sphinx`` which should be much faster.
