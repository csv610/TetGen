# Set  the minimum  required version  of cmake  for a  project.
cmake_minimum_required(VERSION 3.5)

project(TetGen LANGUAGES CXX)

# Use C++14 or higher for Catch2 v3 compatibility
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Main Library and Executable
# -----------------------------------------------------------------------------

# Find Assimp
find_package(assimp REQUIRED)

# Add a static library for TetGen core
add_library(tet STATIC 
    src/tetgen.cxx 
    src/predicates.cxx
)

# Add a static library for C++ wrappers
add_library(tet_cpp STATIC
    cpp/TetDelMesher.cpp
    cpp/MeshOptimizer.cpp
    cpp/Mesh.cpp
)

# Set TETLIBRARY definition for the library and include headers from src/ and cpp/
target_compile_definitions(tet PUBLIC TETLIBRARY)
target_include_directories(tet PUBLIC src)

target_link_libraries(tet_cpp PUBLIC tet)
target_include_directories(tet_cpp PUBLIC cpp src)

# Add the main tetgen CLI tool
add_executable(tetgen src/tetgen.cxx src/predicates.cxx)
target_include_directories(tetgen PRIVATE src)

# -----------------------------------------------------------------------------
# Applications
# -----------------------------------------------------------------------------

add_executable(random_points_delaunay apps/random_points_delaunay.cpp)
target_link_libraries(random_points_delaunay PRIVATE tet_cpp)
target_include_directories(random_points_delaunay PRIVATE src cpp)

add_executable(random_convex_hull apps/random_convex_hull.cpp)
target_link_libraries(random_convex_hull PRIVATE tet_cpp)
target_include_directories(random_convex_hull PRIVATE src cpp)

add_executable(cdt apps/cdt.cpp)
target_link_libraries(cdt PRIVATE tet_cpp assimp::assimp)
target_include_directories(cdt PRIVATE src cpp)

add_executable(mesh_convex_hull apps/mesh_convex_hull.cpp)
target_link_libraries(mesh_convex_hull PRIVATE tet_cpp assimp::assimp)
target_include_directories(mesh_convex_hull PRIVATE src cpp)

# -----------------------------------------------------------------------------
# Benchmarks
# -----------------------------------------------------------------------------

macro(add_tetgen_bench bench_name)
    add_executable(${bench_name} bench/${bench_name}.cxx)
    target_link_libraries(${bench_name} PRIVATE tet_cpp)
    target_include_directories(${bench_name} PRIVATE bench src cpp)
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
target_link_libraries(run_tests PRIVATE tet_cpp Catch2::Catch2WithMain)
target_include_directories(run_tests PRIVATE tests src cpp)

# Add a simple test for the isotropic mesh generator
add_executable(test_isotropic tests/test_isotropic.cpp)
target_link_libraries(test_isotropic PRIVATE tet_cpp)
target_include_directories(test_isotropic PRIVATE src cpp)

# Register the test case with CTest
add_test(NAME AllTests COMMAND run_tests)
add_test(NAME IsotropicTest COMMAND test_isotropic)
