import subprocess
import os
import csv
import sys

def run_bench(binary_path):
    print(f"--- Running {os.path.basename(binary_path)} ---")
    try:
        # We run the binary once as it now handles its own internal loop
        # For professional benchmarking, one might run the binary multiple times 
        # but here we rely on the internal counts.
        result = subprocess.run([binary_path], capture_output=True, text=True, check=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error running {binary_path}: {e}")
        print(e.stderr)

def main():
    # Assuming the script is run from project root or bench/
    # Find build directory
    possible_build_dirs = ["build", "../build"]
    build_dir = None
    for d in possible_build_dirs:
        if os.path.exists(os.path.join(d, "bench_delaunay")):
            build_dir = d
            break
    
    if not build_dir:
        print("Error: Could not find build directory with benchmark binaries.")
        print("Please build the project first (e.g., cd build && cmake .. && make)")
        sys.exit(1)

    binaries = [
        "bench_delaunay",
        "bench_convex_hull",
        "bench_voronoi",
        "bench_cdt"
    ]

    for b in binaries:
        path = os.path.join(build_dir, b)
        if os.path.exists(path):
            run_bench(path)
        else:
            print(f"Warning: Binary {b} not found in {build_dir}")

    print("\nAll benchmarks completed. CSV files generated in the build directory.")

if __name__ == "__main__":
    main()
