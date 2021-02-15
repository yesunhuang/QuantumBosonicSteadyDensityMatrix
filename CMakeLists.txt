project(Quantum)
cmake_minimum_required( VERSION 3.0 )

include_directories( include )
find_package( OpenMP REQUIRED )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_executable( main src/main.cpp
        src/cpp/solver/TDSolver.cpp
        src/cpp/solver/DPSolver.cpp
        src/cpp/complex.cpp
        src/cpp/Expression/EpDeriver.cpp
        )