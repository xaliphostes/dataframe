from setuptools import setup, find_packages
from setuptools.dist import Distribution

class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True

setup(
    name="dataframe",
    version="0.1.0",
    packages=find_packages(),
    include_package_data=True,
    package_data={
        'dataframe': ['*.so', '*.dylib'],
    },
    distclass=BinaryDistribution,
)