# coding: utf-8

from setuptools import setup, find_packages

NAME = "mpctool"
VERSION = "1.0.0"

# To install the library, run the following
#
# python setup.py install
#
# prerequisite: setuptools
# http://pypi.python.org/pypi/setuptools

REQUIRES = [
    "numpy",
    "pandas",
    "psutil",
    "setuptools"
]

setup(
    name=NAME,
    version=VERSION,
    description="mpctool",
    install_requires=REQUIRES,
    packages=find_packages(),
    data_files=[
        ("/usr/lib/systemd/system", ["mpctool.service"])
    ],
    entry_points={
        "console_scripts": ["mpctool=mpctool.mpctool:main"]
    },
    zip_safe=False
)
