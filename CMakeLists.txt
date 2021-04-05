project(Quantum)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(include/solver)
include_directories(include/expression)

# openmp
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS})
# add debug param
# set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0)

set(SOURCES 
        src/main.cpp
        src/cpp/solver/DPSolver.cpp
        src/cpp/complex.cpp
        src/cpp/Expression/EpDeriver.cpp
        )

# cpp version
add_executable(main ${SOURCES})
target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)

if (SKBUILD)
        find_package(PythonExtensions REQUIRED)
        add_library(core MODULE ${SOURCES} src/python/py_wrapper.cpp)
        python_extension_module(core)
        install(TARGETS core LIBRARY DESTINATION bostix)
endif()