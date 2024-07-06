import os
import sys

import setuptools
from setuptools import setup

from distutils.extension import Extension
from glob import glob

try:
    from Cython.Build import cythonize
except ImportError:
    USE_CYTHON = False
else:
    USE_CYTHON = True

long_description  = """Graph planarity tools.
A wrapper to Boyer's (C) planarity algorithms: http://code.google.com/p/planarity/

Provides planarity testing, forbidden subgraph finding, and planar embeddings.

Works with the NetworkX graph package (not required).
"""

classifiers = [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Scientific/Engineering :: Information Analysis',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Topic :: Scientific/Engineering :: Physics']

ext = '.pyx' if USE_CYTHON else '.c'

sourcefiles = ['planarity/planarity'+ext]
sourcefiles.extend(glob("planarity/src/*.c"))

extensions = [Extension("planarity.planarity",
                        sourcefiles,
                        include_dirs=['planarity/src/'],
                        )]

if USE_CYTHON:
    from Cython.Build import cythonize
    extensions = cythonize(extensions)

setup(
    name= 'planarity',
    packages=setuptools.find_packages(),
    maintainer = 'Aric Hagberg',
    maintainer_email = 'aric.hagberg@gmail.com',
    author = 'Aric Hagberg',
    author_email = 'aric.hagberg@gmail.com',
    description = 'Graph planarity tools.',
    classifiers = classifiers,
    long_description = long_description,
    license = 'BSD',
    ext_modules = extensions,
    version          = '0.6',
    url = 'https://github.com/hagberg/planarity/',
    download_url='https://pypi.python.org/pypi/planarity',
    package_data = {'planarity':['tests/*.py']},
    zip_safe = False
    )

