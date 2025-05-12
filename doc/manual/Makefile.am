export ABS_SRCDIR=$(abs_srcdir)
export ABS_BUILDDIR=$(abs_builddir)

generated/html/index.html: generated/doxygen/xml/index.xml flint.inv $(srcdir)/*.rst $(srcdir)/conf.py $(srcdir)/_static/extra.css
	$(MAKE) -C ../../pyexactreal/test test-env.sh bin/python
	$(MAKE) -C ../../libexactreal/src
	source ../../pyexactreal/test/test-env.sh && \
	python $(SPHINXBUILD) -b html $(srcdir) $(builddir)/generated/html
	cp $(srcdir)/robots.txt generated/html/

generated/doxygen/xml/index.xml: $(srcdir)/Doxyfile $(srcdir)/../../libexactreal/exact-real/*.hpp
	mkdir -p generated/doxygen
	$(DOXYGEN) $(srcdir)/Doxyfile

flint.inv:
	wget -O $@ https://flintlib.org/doc/objects.inv

CLEANFILES = flint.inv

all: generated/html/index.html

EXTRA_DIST = README.md
EXTRA_DIST += robots.txt _static/extra.css
EXTRA_DIST += Doxyfile
EXTRA_DIST += conf.py index.rst
EXTRA_DIST += cxx_overview.rst cxx_arb.rst cxx_arf.rst cxx_cereal.rst cxx_element.rst cxx_integer_ring.rst cxx_module.rst cxx_number_field.rst cxx_number_field_ideal.rst cxx_ration_field.rst cxx_real_number.rst cxx_seed.rst
EXTRA_DIST += cxx_overview.rst cxx_renf_class.rst cxx_renf_elem_class.rst
EXTRA_DIST += python_overview.rst python_cppyy.rst python_exact_reals.rst

mostlyclean-local:
	[[ ! -d generated ]] || (chmod -R u+w generated && rm -rf generated)

