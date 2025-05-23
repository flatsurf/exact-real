Tour of the Python Interface
============================

The ``pyexactreal`` package provides three interfaces to exact-real from Python.

There is a very low-level interface that is provided by `cppyy
<https://cppyy.readthedocs.io/en/latest/>`_. This interface is very low-level
and just exposes the C++ namespace ``exactreal`` at ``pyexactreal.exactreal``.
While in principle this interface is fully functional, it can be very tedious
to work with. The translation from C++ to Python is not without problems and
you'll likely run into segfaults at some point.

There is a higher-level interface which works around most of the oddities
coming from the C++ interface. We showcase some of its features below. More
examples can be found in the :doc:`module documentation
<python_cppyy_exactreal>`. In principle most objects exposed in this interface
are just somewhat tweaked wrappers of the C++ objects so most functions that
you may find in the C++ documentation should be available.

Finally, there is a :doc:`SageMath interface <sage_overview>` that wraps the
exact-real machinery in the language of SageMath parents and elements.

Example
-------

We want to construct a module over the field of rational real numbers. We fix
our generators, one is a :cpp:func:`random real number
<exactreal::RealNumber::random>` in the interval (0, 1).

.. code-block:: python

   >>> from pyexactreal import RealNumber, QQModule  # byexample: +timeout=30

   >>> a = RealNumber.rational(1)
   >>> b = RealNumber.random()

We construct the :py:func:`module <pyexactreal.cppyy_exactreal.QQModule>` generated by these
reals:

.. code-block:: python

   >>> M = QQModule(a, b)
   >>> M
   ℚ-Module(1, ℝ(0.303644…))

We construct some :cpp:class:`elements <exactreal::Element>` in this module, namely
its :cpp:func:`generators <exactreal::Module::gen>`:

.. code-block:: python

   >>> x = M.gen(0)
   >>> y = M.gen(1)

We perform some arithmetic in this module:

.. code-block:: python

   >>> 2*x
   2

   >>> y + x - y == x
   True

Note that we can also :cpp:func:`multiply elements
<exactreal::Element::operator*=>`, however the result then typically lives in a
larger module:

.. code-block:: python

   >>> (y * y).module() == M
   False

:cpp:func:`Divisions <exactreal::Element::truediv>` are supported when the
result can be determined exactly:

.. code-block:: python

   >>> (y * y) / y == y
   True

Otherwise, only :cpp:func;`floor division <exactreal::Element::floordiv>` is
possible:

.. code-block:: python

   >>> x.floordiv(y) >= 1
   True
