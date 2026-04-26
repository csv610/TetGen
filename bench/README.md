# TetGen Benchmarks

This directory contains performance benchmarks for various TetGen functionalities:
- **Delaunay Triangulation**: Standard 3D Delaunay.
- **Convex Hull**: Computation of the convex hull of a point set.
- **Voronoi Diagram**: Generation of Voronoi cells.
- **Constrained Delaunay (CDT)**: Triangulation of random points with PLC constraints.

## Requirements
- C++ compiler with C++11 support.
- CMake 3.5+.
- Python 3 (optional, for the automated runner and plotting).

## How to Run

### Manual Execution
If you have built the project in the `build/` directory:
```bash
cd build
./bench_delaunay
./bench_convex_hull
./bench_voronoi
./bench_cdt
```

### Automated Runner
The `run_benchmarks.py` script runs all benchmarks, performs multiple iterations to ensure statistical accuracy (median filtering), and generates CSV reports.

```bash
python3 bench/run_benchmarks.py
```

## Output
Each benchmark produces a `.csv` file with the following columns:
- `Name`: The specific test name.
- `InputSize`: Number of points.
- `DurationMS`: Time taken in milliseconds.
- `OutputTets`: Number of tetrahedra generated.
