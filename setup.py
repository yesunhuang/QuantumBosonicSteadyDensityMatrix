from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext

copts =  {
    "msvc": ["/openmp"],
    "mingw32" : ["-fopenmp"],
    "unix": ["-fopenmp"],
    "default": ["-fopenmp"],
}
lopts =  {
    "msvc": [],
    "mingw32" : ["-fopenmp"],
    "unix": ["-fopenmp"],
    "default": ["-fopenmp"],
}

class build_ext_subclass(build_ext):
    def build_extensions(self):
        c = self.compiler.compiler_type
        copt = copts.get(c)
        if copt is None:
            copt = copts["default"]
        lopt = lopts.get(c)
        if lopt is None:
            lopt = lopts["default"]
        # assign option
        for e in self.extensions:
            e.extra_compile_args = copt
            e.extra_link_args = lopt
        build_ext.build_extensions(self)

longdes = open("README.md",encoding="utf-8")

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
    cmdclass={"build_ext": build_ext_subclass},
)
