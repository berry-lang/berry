# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'berry'
copyright = '2022, Guan Wenliang & Stephan Hadinger'
author = 'Guan Wenliang & Stephan Hadinger'

# The full version, including alpha/beta/rc tags
release = '1.1.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
        'myst_parser',
        'sphinx.ext.autodoc',
        'sphinx.ext.doctest',
        'sphinx.ext.mathjax',
        'sphinx.ext.viewcode',
        'sphinx.ext.imgmath', 
        'sphinx.ext.todo',
        'breathe'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['conf.py', 'Doxyfile']

# on_rtd is whether we are on readthedocs.org
import os
on_rtd = os.environ.get('READTHEDOCS', None) == 'True'

import subprocess
subprocess.call('doxygen', shell=True)


# -- Options for HTML output -------------------------------------------------

if not on_rtd:  # only import and set the theme if we're building docs locally
    import sphinx_rtd_theme

    # The theme to use for HTML and HTML Help pages.  See the documentation for
    # a list of builtin themes.
    #
    # html_theme = 'alabaster'
    html_theme = 'sphinx_rtd_theme'
    html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

    # Theme options are theme-specific and customize the look and feel of a theme
    # further.  For a list of options available for each theme, see the
    # documentation.
    #
    # html_theme_options = {}

    # Add any paths that contain custom static files (such as style sheets) here,
    # relative to this directory. They are copied after the builtin static files,
    # so a file named "default.css" will overwrite the builtin "default.css".
    # html_static_path = ['_static']

    # Custom sidebar templates, must be a dictionary that maps document names
    # to template names.
    #
    # The default sidebars (for documents that don't match any pattern) are
    # defined by theme itself.  Builtin themes are using these templates by
    # default: ``['localtoc.html', 'relations.html', 'sourcelink.html',
    # 'searchbox.html']``.
    #
    # html_sidebars = {}

# otherwise, readthedocs.org uses their theme by default, so no need to specify it

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = ['_static']

source_suffix = {
     '.md': 'markdown',
    '.rst': 'restructuredtext',
}

# Breathe Configuration
breathe_projects = {"berry" : "../src"}
breathe_default_project = "berry"
breathe_projects_source = {
    "berry" : (
        "../", ["src/berry.h"]
    )
}

c_id_attributes = ["BERRY_API"]
c_paren_attributes = ["BERRY_API"]

cpp_id_attributes = ["BERRY_API"]
cpp_paren_attributes = ["BERRY_API"]

import sys
def setup(sphinx):
    sys.path.insert(0, os.path.abspath('../tools/highlighters/Pygments'))
    from berry import BerryLexer
    sphinx.add_lexer("berry", BerryLexer)
