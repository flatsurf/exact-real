all-local:
	mkdir -p $(builddir)/build
	cd $(srcdir) && $(PYTHON) $(abs_srcdir)/setup.py build --verbose --build-base $(abs_builddir)/build

install-exec-local:
	$(PYTHON) $(abs_srcdir)/setup.py install --prefix $(DESTDIR)$(prefix) --single-version-externally-managed --record $(DESTDIR)$(pythondir)/pyexactreal/install_files.txt --verbose

uninstall-local:
	cat $(DESTDIR)$(pythondir)/pyexactreal/install_files.txt | xargs rm -rf
	rm -rf $(DESTDIR)$(pythondir)/pyexactreal

clean-local:
	-rm -rf pyexactreal/__pycache__ pyexactreal.egg-info build .pytest_cache

EXTRA_DIST = setup.py pyexactreal/__init__.py pyexactreal/exact_reals.py pyexactreal/cppyy_exactreal.py
