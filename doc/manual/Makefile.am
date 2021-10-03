PORT ?= 8080
EXTRA_STANDARDESE_FLAGS ?=

STANDARDESE_FLAGS = --warn-as-error --free-file-comments -X
# Configure search paths for #include statements
STANDARDESE_FLAGS += -I $(srcdir)/../../libexactreal/exact-real -I $(builddir)/../../libexactreal/exact-real -I $(includedir) -I $(srcdir)/../../libexactreal/test/external/cereal/include
# Link against e-antic's Sphinx documentation.
STANDARDESE_FLAGS += --external sphinx:libeantic:libeantic.inv=https://flatsurf.github.io/
# Link against Arb's Sphinx documentation.
STANDARDESE_FLAGS += --external sphinx:arb:arb.inv=https://arblib.org/
# Allow === Output Section === Syntax
STANDARDESE_FLAGS += -p output_section='=== (.*) ==='
# Allow ==* Group *== Syntax
STANDARDESE_FLAGS += -p group='==\* ((?:.(?!\*==))+)() \*=='
# Allow :param:`name` syntax like in Sphinx.
STANDARDESE_FLAGS += -p param=':param:`([^`]*)`'
# Use C++ 17 to parse code.
STANDARDESE_FLAGS += --std=c++17
# Work around bugs in C++ parser.
STANDARDESE_FLAGS += -DLIBEXACTREAL_DOCBUILD
# We will serve the C/C++ documentation from the exact-real/libexactreal/ subdirectory
STANDARDESE_FLAGS += --vpath 'exact-real/libexactreal/doc_{{ sanitize_basename(name) }}'
# Shorten type names in the documentation that reference common namespaces or use complicated aliases.
STANDARDESE_FLAGS += --format type='{% set formatted = format(option("default:type_format")) %}{% set formatted = replace(formatted, "std::|exactreal:", "") %}{{ formatted }}'

STANDARDESE_FLAGS += ${EXTRA_STANDARDESE_FLAGS}

# Process all header files with standardese.
STANDARDESE_INPUTS = `find $(srcdir)/../../libexactreal/exact-real/ \( -name '*.h' -o -name '*.hpp' \) -not -path '*external*'`
# Progress all MarkDown files with standardese.
STANDARDESE_INPUTS += `find $(srcdir)/libexactreal/ -name '*.md'`

$(builddir)/generated/pyexactreal/source:
	@# Create a copy of the source dir with some generated assets since Sphinx cannot read from more than one directory.
	mkdir -p $(builddir)/generated/pyexactreal
	cp -RT $(srcdir)/pyexactreal/ $(builddir)/generated/pyexactreal/source

$(builddir)/generated/pyexactreal/inventory: $(builddir)/generated/pyexactreal/source
	@# Create an inventory of pyexactreal so standardese can link against it.
	source $(builddir)/../../pyexactreal/test/test-env.sh && \
	python $(SPHINXBUILD) -b json $(builddir)/generated/pyexactreal/source $(builddir)/build/pyexactreal/inventory -Q

$(builddir)/generated/pyexactreal/html: $(builddir)/generated/libexactreal/inventory $(builddir)/generated/pyexactreal/source
	@# Create HTML pages for pyexactreal with Sphinx
	source $(builddir)/../../pyexactreal/test/test-env.sh && \
	python $(SPHINXBUILD) -b html -n $(builddir)/generated/pyexactreal/source $(builddir)/generated/pyexactreal/html --color -a -E -W

$(builddir)/generated/libexactreal/inventory: $(builddir)/generated/pyexactreal/inventory $(builddir)/arb.inv $(builddir)/libeantic.inv
	@# Create an inventory of libexactreal so Sphinx can link against it. (This also creates the entire documentation since there is no flag in standardese yet to only generate the inventory.)
	mkdir -p $(builddir)/generated/libexactreal/inventory
	$(STANDARDESE) --outdir $(builddir)/generated/libexactreal/inventory/ $(STANDARDESE_FLAGS) $(STANDARDESE_INPUTS)

$(builddir)/generated/libexactreal/markdown: $(builddir)/generated/pyexactreal/inventory $(builddir)/arb.inv $(builddir)/libeantic.inv
	@# Build markdown files from exact-real C/C++ headers
	mkdir -p $(builddir)/generated/libexactreal/
	cp -RT $(srcdir)/libexactreal/ $(builddir)/generated/libexactreal/markdown
	$(STANDARDESE) --outdir $(builddir)/generated/libexactreal/markdown/ $(STANDARDESE_FLAGS) $(STANDARDESE_INPUTS)

$(builddir)/generated/libexactreal/html: $(builddir)/generated/libexactreal/markdown $(builddir)/generated/libexactreal/markdown/mkdocs.yml
	@# Create html documentation from markdown files
	cd $(builddir)/generated/libexactreal/markdown &&\
	$(MKDOCS) build --config-file mkdocs.yml --site-dir $(abs_builddir)/generated/libexactreal/html/

$(builddir)/generated/libexactreal/markdown/mkdocs.yml: $(srcdir)/libexactreal/mkdocs.yml.in Makefile
	mkdir -p $(builddir)/generated/libexactreal/markdown
	sed -e 's,[@]srcdir[@],$(srcdir),g' -e 's,[@]builddir[@],$(builddir),g' < $< > $@

$(builddir)/arb.inv:
	wget -O $@ https://arblib.org/objects.inv

CLEANFILES = arb.inv

$(builddir)/libeantic.inv:
	wget -O $@ https://flatsurf.github.io/e-antic/libeantic/objects.inv

CLEANFILES += libeantic.inv

html: $(builddir)/generated/libexactreal/html $(builddir)/generated/pyexactreal/html
	mkdir -p $(builddir)/generated/html/exact-real
	cp -RT $(builddir)/generated/libexactreal/html/ $(builddir)/generated/html/exact-real/libexactreal
	cp -RT $(builddir)/generated/pyexactreal/html/ $(builddir)/generated/html/exact-real/pyexactreal

serve: html
	python -m http.server ${PORT} --bind 127.0.0.1 --directory $(builddir)/generated/html

all: html

.PHONY: serve $(builddir)/generated/libexactreal/html $(builddir)/generated/libexactreal/markdown $(builddir)/generated/libexactreal/inventory $(builddir)/generated/pyexactreal/html $(builddir)/generated/pyexactreal/inventory $(builddir)/generated/pyexactreal/source

EXTRA_DIST = README.md libexactreal pyexactreal

mostlyclean-local:
	[[ ! -d generated ]] || (chmod -R u+w generated && rm -rf generated)
	[[ ! -d build ]] || (chmod -R u+w build && rm -rf build)
