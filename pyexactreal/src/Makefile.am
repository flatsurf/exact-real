all-local:
	mkdir -p $(builddir)/build
	cd $(srcdir) && $(PYTHON) $(abs_top_builddir)/src/setup.py build --verbose --build-base $(abs_top_builddir)/src/build

install-exec-local:
	$(PYTHON) setup.py install --prefix $(DESTDIR)$(prefix) --single-version-externally-managed --record $(DESTDIR)$(pythondir)/pyexactreal/install_files.txt --verbose

uninstall-local:
	cat $(DESTDIR)$(pythondir)/pyexactreal/install_files.txt | xargs rm -rf
	rm -rf $(DESTDIR)$(pythondir)/pyexactreal

clean-local:
	-rm -rf pyexactreal/__pycache__ pyexactreal.egg-info build .pytest_cache

BUILT_SOURCES = setup.py MANIFEST.in
EXTRA_DIST = setup.py.in MANIFEST.in.in pyexactreal/__init__.py pyexactreal/exact_reals.py pyexactreal/cppyy_exactreal.py pyexactreal/include
CLEANFILES = setup.py MANIFEST.in
$(builddir)/setup.py: $(srcdir)/setup.py.in Makefile
	sed -e 's,[@]abs_top_srcdir[@],$(abs_top_srcdir),g' -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' < $< > $@
$(builddir)/MANIFEST.in: $(srcdir)/MANIFEST.in.in Makefile
	sed -e 's,[@]srcdir[@],$(srcdir),g' < $< > $@
