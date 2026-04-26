# Set  the minimum  required version  of cmake  for a  project.
cmake_minimum_required(VERSION 3.5)

project(TetGen LANGUAGES CXX)

# Use C++14 or higher for Catch2 v3 compatibility
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Main Library and Executable
# -----------------------------------------------------------------------------

# Add a static library for TetGen core
add_library(tet STATIC 
    src/tetgen.cxx 
    src/predicates.cxx
)

# Set TETLIBRARY definition for the library and include headers from src/
target_compile_definitions(tet PUBLIC TETLIBRARY)
target_include_directories(tet PUBLIC src)

# Add the main tetgen CLI tool
add_executable(tetgen src/tetgen.cxx src/predicates.cxx)
target_include_directories(tetgen PRIVATE src)

# -----------------------------------------------------------------------------
# Benchmarks
# -----------------------------------------------------------------------------

macro(add_tetgen_bench bench_name)
    add_executable(${bench_name} bench/${bench_name}.cxx)
    target_link_libraries(${bench_name} PRIVATE tet)
    target_include_directories(${bench_name} PRIVATE bench src)
endmacro()

add_tetgen_bench(bench_delaunay)
add_tetgen_bench(bench_convex_hull)
add_tetgen_bench(bench_voronoi)
add_tetgen_bench(bench_cdt)

# -----------------------------------------------------------------------------
# Testing (using system Catch2 v3)
# -----------------------------------------------------------------------------

# Enable testing for the project
enable_testing()

# Find system Catch2 v3
find_package(Catch2 3 REQUIRED)

# Add the test runner executable
add_executable(run_tests tests/tests.cxx)

# Link against our library and Catch2 with its generated Main()
target_link_libraries(run_tests PRIVATE tet Catch2::Catch2WithMain)
target_include_directories(run_tests PRIVATE tests src)

# Register the test case with CTest
add_test(NAME AllTests COMMAND run_tests)
