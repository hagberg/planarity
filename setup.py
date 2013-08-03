import os
import sys

# use setuptools with hack to build .c files with Cython
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "fake_pyrex"))
import setuptools
from setuptools import setup

from distutils.extension import Extension
from Cython.Distutils import build_ext
from glob import glob

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
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.1',
        'Programming Language :: Python :: 3.2',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Scientific/Engineering :: Information Analysis',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Topic :: Scientific/Engineering :: Physics']

sourcefiles = ['planarity/planarity.pyx']
sourcefiles.extend(glob("planarity/src/*.c"))

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
    cmdclass = {'build_ext': build_ext},
    ext_modules = [Extension("planarity.planarity",
                             sourcefiles,
                             include_dirs=['planarity/src/'],
                             )],
    version          = '0.3',
    url = 'https://github.com/hagberg/planarity/',
    download_url='https://pypi',
    package_data = {'planarity':['tests/*.py']},
        install_requires=['networkx','setuptools'],
        test_suite = 'nose.collector', 
        tests_require = ['nose >= 0.10.1'] ,
        zip_safe = False
    )

