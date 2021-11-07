import os
import sys
sys.path.insert(0, os.path.abspath('../../pyexactreal/src/'))


project = 'pyexactreal'
copyright = '2021, the exact-real authors'
author = 'the exact-real authors'

release = '2.1.0'


extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx_rtd_theme',
]

autodoc_default_options = {
    'members': True,
}

templates_path = ['_templates']

exclude_patterns = []

html_theme = 'sphinx_rtd_theme'

html_static_path = []

intersphinx_mapping = {
    'libexactreal': ('/', ('../../libexactreal/inventory/objects.inv')),
}
