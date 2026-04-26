# TetGen (Mac-M1 Port)

This repository is a specialized port of **TetGen** for Apple Silicon (Mac M1/M2/M3). 

### 📜 Attribution & Credits
**All credits for the original research, algorithms, and core implementation go to [Hang Si](https://github.com/sihang0592)**. This software was developed during his affiliation with the [Weierstrass Institute for Applied Analysis and Stochastics (WIAS) Berlin](https://www.wias-berlin.de/software/tetgen). 

The primary development repository for the original project is [https://codeberg.org/TetGen/TetGen](https://codeberg.org/TetGen/TetGen).

---

### 🚀 Contributions in this Port
This fork introduces several enhancements to make TetGen more robust and easier to evaluate on modern hardware:

1.  **Mac-M1 Optimization**: Build configurations and source adjustments specifically for Apple Silicon (ARM64) architecture.
2.  **Comprehensive Benchmarking Suite**:
    *   New high-performance benchmarks for **Delaunay Triangulation**, **Convex Hull**, **Voronoi Diagrams**, and **Constrained Delaunay (CDT)**.
    *   Scalability testing from **10,000 to 1,000,000 points**.
    *   Automated Python runner (`bench/run_benchmarks.py`) with CSV data export for performance analysis.
3.  **Unit Testing Framework**:
    *   Added a structured testing suite in `tests/` to ensure geometric correctness and stability during the porting process.
4.  **Project Modernization**:
    *   Restructured source code into a clean `src/` directory.
    *   Integrated CMake build system for seamless compilation.

---

## Getting Started

### Prerequisites
- macOS (Optimized for M1/M2/M3)
- CMake 3.5+
- C++11 compatible compiler (Clang/GCC)

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Running Benchmarks
To run the full suite and generate performance data:
```bash
python3 bench/run_benchmarks.py
```

## Documentation & Licensing
TetGen is distributed under the **AGPLv3 license**. For original documentation, manuals, and technical papers by Hang Si, please visit the [official WIAS TetGen page](https://www.wias-berlin.de/software/tetgen).

Commercial licenses for TetGen are offered by the Weierstrass Institute. Please contact `tetgen at wias-berlin.de` for details.
