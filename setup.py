from setuptools import setup, Extension, find_packages

longdes = open("README.md")

module1 = Extension(
    "bostix.core",
    sources=[
        "./src/python/py_wrapper.cpp",
        "./src/cpp/Expression/EpDeriver.cpp",
        "./src/cpp/solver/DPSolver.cpp",
        "./src/cpp/complex.cpp",
    ],
    include_dirs=["./include"],
    define_macros=[
        # ("_DEBUG", None)
        ("HAHA", None)
    ],
    language="c++",
    extra_compile_args=["-fopenmp"],
    extra_link_args=["-fopenmp"],
)

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
    ext_modules=[module1],
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)