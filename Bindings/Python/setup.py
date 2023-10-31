#!/usr/bin/env python

import os
import sys
from setuptools import setup
import fileinput

# update __init__.py in local installation
install_path = os.path.abspath('../../bin').replace(os.sep, '/')
if (sys.platform.startswith('win') and os.path.exists(install_path)):
    print ('install path found as '+ install_path)
    with fileinput.FileInput('opensim/__init__.py', inplace=True, backup='.bak') as file:
        for line in file:
            print(line.replace('DLL_PATH', "'" + install_path + "'"), end='')

# This provides the variable `__version__`.
if sys.version_info[0] < 3:
    execfile('opensim/version.py')
else:
    exec(compile(open('opensim/version.py').read(), 'opensim/version.py', 'exec'))

setup(name='opensim',
      version=__version__,
      description='OpenSim Simulation Framework',
      author='OpenSim Team',
      author_email='ahabib@stanford.edu',
      url='http://opensim.stanford.edu/',
      license='Apache 2.0',
      packages=['opensim'],
      # The last 3 entries are for if OPENSIM_PYTHON_STANDALONE is ON.
      # The asterisk after the extension is to handle version numbers on Linux.
      package_data={'opensim': ['_*.*', '*.dylib', '*.dll', '*.so*']},
      # Create a command-line tool for generating a report.
      entry_points={
          'console_scripts': [
              'opensim-moco-generate-report = opensim.report:main',
          ],
      },
      include_package_data=True,
      classifiers=[
          'Intended Audience :: Science/Research',
          'Operating System :: OS Independent',
          'Programming Language :: Python :: 2.7',
          'Programming Language :: Python :: 3',
          'Topic :: Scientific/Engineering :: Physics',
          ],
      )
