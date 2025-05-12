# Configuration file for the Sphinx documentation builder.

import os
from pathlib import Path

BUILDDIR = Path(os.environ.get('ABS_BUILDDIR', '.')).absolute()

project = 'pyexactreal'
copyright = '2019-2025, the exact-real authors'

release = '4.0.1'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.mathjax',
    'sphinx_book_theme',
    'breathe',
]

autodoc_default_options = {
    'members': True,
}

templates_path = ['_templates']

exclude_patterns = []

html_theme = 'sphinx_book_theme'

html_logo = 'https://github.com/flatsurf/exact-real/raw/master/logo.svg?sanitize=true'

html_theme_options = {
    "repository_url": "https://github.com/flatsurf/exact-real",
    "icon_links": [{
        "name": "flatsurf",
        "url": "https://flatsurf.github.io",
        "icon": "https://flatsurf.github.io/assets/logo.svg",
        "type": "url",
    }, {
        "name": "GitHub",
        "url": "https://github.com/flatsurf/exact-real",
        "icon": "fa-brands fa-square-github",
        "type": "fontawesome",
    }, {
        "name": "Zulip",
        "url": "https://sagemath.zulipchat.com/#narrow/channel/271193-flatsurf",
        "icon": "fa-regular fa-comments",
        "type": "fontawesome",
    },
    ],
    "use_edit_page_button": True,
    "repository_branch": "master",
    "path_to_docs": "doc/manual",
}

html_static_path = ['_static']

html_css_files = ['extra.css']

intersphinx_mapping = {
}

breathe_projects = {"libexactreal": str(BUILDDIR / "generated/doxygen/xml")}
breathe_default_project = "libexactreal"
breathe_show_include = False
