# src/dataframe/__init__.py
import os
import sys

# Add the bin directory to PATH so the .dylib can be found
bin_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), 'bin')
if sys.platform == 'darwin':  # For macOS
    os.environ['DYLD_LIBRARY_PATH'] = bin_dir
elif sys.platform == 'linux':
    os.environ['LD_LIBRARY_PATH'] = bin_dir

from .dataframe import *  # Import symbols from your compiled module
del dataframe