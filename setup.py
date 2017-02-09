import io
import re
from setuptools import setup, find_packages

with open("rig_cpp_key_allocation/version.py", "r") as f:
    exec(f.read())

setup(
    name="rig_cpp_key_allocation",
    version=__version__,
    packages=find_packages(),
    
    # Files required by CFFI wrapper
    package_data = {
        "rig_cpp_key_allocation": [
            "bitset.h", "bitset.cpp", 
            "constraints_graph.h", "constraints_graph.cpp"
        ]
    },

    # Metadata for PyPi
    url="https://github.com/project-rig/rig_cpp_key_allocation",
    author="The Rig Authors",
    description=("A C++ library (and CFFI Python interface) for allocating " +
                 "SpiNNaker multicast keys."),
    license="GPLv2",
    classifiers=[
        "Development Status :: 3 - Alpha",

        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",

        "License :: OSI Approved :: GNU General Public License v2 (GPLv2)",

        "Operating System :: POSIX :: Linux",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: MacOS",

        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",

        "Topic :: Software Development :: Libraries",
    ],
    keywords="spinnaker cffi graph-coloring",

    # Build CFFI Interface
    cffi_modules=["rig_cpp_key_allocation/cffi_compile.py:ffi"],
    setup_requires=["cffi>=1.0.0"],
    install_requires=["cffi>=1.0.0"],
)
