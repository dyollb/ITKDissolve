# -*- coding: utf-8 -*-
from os import sys

try:
    from skbuild import setup
except ImportError:
    print("scikit-build is required to build from source.", file=sys.stderr)
    print("Please run:", file=sys.stderr)
    print("", file=sys.stderr)
    print("  python -m pip install scikit-build")
    sys.exit(1)

setup(
    name="itk-dissolve",
    version="0.9.0",
    author="Bryn Lloyd",
    author_email="lloyd@itis.swiss",
    packages=["itk"],
    package_dir={"itk": "itk"},
    download_url=r"https://github.com/dyollb/ITKDissolve",
    description=r"This is a module for the Insight Toolkit (ITK) that provides filters to replace regions within a masked region with the surrounding labels.",
    long_description="ITK is an open-source, cross-platform library that provides developers with an extensive suite of software tools for image analysis. Developed through extreme programming methodologies, ITK employs leading-edge algorithms for registering and segmenting multidimensional scientific images.",
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Healthcare Industry",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
        "Topic :: Scientific/Engineering :: Information Analysis",
        "Topic :: Software Development :: Libraries",
        "Operating System :: Android",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Operating System :: MacOS",
    ],
    license="MIT",
    keywords="ITK InsightToolkit",
    url=r"https://itk.org/",
    install_requires=[r"itk>=5.2.1.post1"],
)
