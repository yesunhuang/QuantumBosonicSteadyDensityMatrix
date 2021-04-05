import os
from pathlib import Path
from skbuild import setup
from setuptools import find_packages

longdes = open("README.md", encoding="utf-8")

setup(
    name="bostix",
    version="1.0.0",
    description="A tool for Solving steady density matrix problem for bosonic quantum system",
    long_description=longdes.read(),
    author="YesunHuang & Guch8017 & AyajiLin",
    author_email="yesunhuang@mail.ustc.edu.cn",
    maintainer="YesunHuang & Guch8017 & AyajiLin",
    url="https://github.com/yesunhuang/QuantumBosonicSteadyDensityMatrix",
    install_requires=["attrs"],
    packages=find_packages(),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)
