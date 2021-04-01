import os
from pathlib import Path

from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name):
        # don't invoke the original build_ext for this special extension
        super().__init__(name, sources=[])


class cmake_build_ext(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        cwd = Path().absolute()

        # these dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        extdir = Path(self.get_ext_fullpath(ext.name))
        extdir.mkdir(parents=True, exist_ok=True)

        # example of cmake args
        config = "Debug" if self.debug else "Release"
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + str(extdir.parent.joinpath("bostix").absolute()),
            "-DCMAKE_BUILD_TYPE=" + config,
        ]

        # example of build args
        build_args = ["--config", config, "--target", "python", "-j4"]

        os.chdir(str(build_temp))
        self.spawn(["cmake", str(cwd)] + cmake_args)
        if not self.dry_run:
            self.spawn(["cmake", "--build", "."] + build_args)
        # Troubleshooting: if fail on line above then delete all possible
        # temporary CMake files including "CMakeCache.txt" in top level dir.
        os.chdir(str(cwd))


# copts =  {
#     "msvc": ["/openmp"],
#     "mingw32" : ["-fopenmp"],
#     "unix": ["-fopenmp"],
#     "default": ["-fopenmp"],
# }
# lopts =  {
#     "msvc": [],
#     "mingw32" : ["-fopenmp"],
#     "unix": ["-fopenmp"],
#     "default": ["-fopenmp"],
# }

# class build_ext_subclass(build_ext):
#     def build_extensions(self):
#         c = self.compiler.compiler_type
#         copt = copts.get(c)
#         if copt is None:
#             copt = copts["default"]
#         lopt = lopts.get(c)
#         if lopt is None:
#             lopt = lopts["default"]
#         # assign option
#         for e in self.extensions:
#             e.extra_compile_args = copt
#             e.extra_link_args = lopt
#         build_ext.build_extensions(self)

longdes = open("README.md", encoding="utf-8")

# module1 = Extension(
#     "bostix.core",
#     sources=[
#         "./src/python/py_wrapper.cpp",
#         "./src/cpp/Expression/EpDeriver.cpp",
#         "./src/cpp/solver/DPSolver.cpp",
#         "./src/cpp/complex.cpp",
#     ],
#     include_dirs=["./include"],
#     define_macros=[
#         # ("_DEBUG", None)
#         ("HAHA", None)
#     ],
#     language="c++",
# )

bostix_cmake_ext = CMakeExtension(".")

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
    ext_modules=[bostix_cmake_ext],
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Physics",
    ],
    cmdclass={"build_ext": cmake_build_ext},
)
