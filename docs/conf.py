# Configuration file for the Sphinx documentation builder.

# -- Project information

project = 'berry'
copyright = '(c) 2018-2022 Guan Wenliang MIT License'
author = 'skiars@qq.com, https://github.com/Skiars/berry'

release = '1.1'
version = '1.1.0'

# -- General configuration

extensions = [
    'sphinx.ext.duration',
    'sphinx.ext.doctest',
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'sphinx.ext.intersphinx',
]

intersphinx_mapping = {
    'python': ('https://docs.python.org/3/', None),
    'sphinx': ('https://www.sphinx-doc.org/en/master/', None),
}
intersphinx_disabled_domains = ['std']

templates_path = ['_templates']

# -- Options for HTML output

html_theme = 'sphinx_rtd_theme'

# -- Options for EPUB output
epub_show_urls = 'footnote'

import subprocess
 subprocess.call('cd .. ; doxygen', shell=True)
