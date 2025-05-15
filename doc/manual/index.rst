.. toctree::
   :caption: C++ Interface
   :maxdepth: 1
   :hidden:

   cxx_overview
   arb.hpp<cxx_arb>
   arf.hpp<cxx_arf>
   element.hpp<cxx_element>
   integer_ring.hpp<cxx_integer_ring>
   module.hpp<cxx_module>
   number_field.hpp<cxx_number_field>
   rational_field.hpp<cxx_rational_field>
   real_number.hpp<cxx_real_number>
   seed.hpp<cxx_seed>

.. toctree::
   :maxdepth: 1
   :caption: Python Interface
   :hidden:

   python_overview
   python_cppyy_exactreal

.. toctree::
   :maxdepth: 1
   :caption: SageMath Interface
   :hidden:

   sage_overview
   sage_exact_reals

exact-real: Exact Computations with Real Numbers
================================================

exact-real is a C++/Python library which provides exact lazy computations in
the Real Numbers. In particular it implements arithmetic and comparisons of
(random) transcendental numbers.

The development of exact-real is coordinated on GitHub at https://github.com/flatsurf/exact-real.

Installation
------------

The preferred way to install exact-real is to use your package manager (e.g.,
``apt-get`` on Debian or Ubuntu, ``pacman`` on Arch Linux, ``brew`` on MacOS, etc).
See `repology for exact-real <https://repology.org/project/exact-real/packages>`_.

The following describe alternative ways of installing exact-real:

* `Build with pixi for Development <https://github.com/flatsurf/exact-real/#build-and-develop-exact-real-with-pixi>`_
* `Build from the Source Tarball <https://github.com/flatsurf/exact-real/#build-from-the-source-code-repository-or-a-tarball>`_

C++ Library
-----------

* :doc:`Tour of the C++ Interface <cxx_overview>`

Python Interface
----------------

* :doc:`Tour of the Python Interface <python_overview>`
