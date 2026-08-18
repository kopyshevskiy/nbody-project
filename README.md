# N-Body Course Project

This project implements the direct gravitational $N$-body problem. The same
physical model is evaluated through sequential, SIMD, OpenMP and CUDA
implementations, so that the effect of the data layout and of the different
parallelization strategies can be studied independently.

The simulation uses the softened gravitational interaction and a constant
time step. The time integration is performed with the Forward Euler scheme.
All implementations evaluate the direct all-pairs interaction, whose cost is
$O(N^2)$ per integration step.

## Project Structure

```text
src/
|-- common/
|   |-- core/       Shared data structures, physics and simulation interface
|   |-- ogl/        Optional OpenGL trajectory viewer
|   `-- utils/      CLI parsing and timing utilities
|-- nbody/
|   |-- serial/     Serial AoS and SoA implementations
|   |-- simd/       AVX2 SIMD implementation based on SoA
|   |-- openmp/     OpenMP implementation based on SoA
|   |-- cuda/       CUDA tiled implementation based on SoA
|   `-- main.cpp    Common command-line entry point
`-- test/           Correctness and layout-comparison tests
```

The two memory layouts used by the project are:

- `SerialAoS`: bodies are stored as complete records in an array of
  structures;
- `SerialSoA`: each physical quantity is stored in a separate contiguous
  array. This layout is also used by `SIMDSoA`, `OpenMPSoA` and `CudaSoA`.

## Initial Conditions

When `--input` is not specified, the initial state is generated according to
`--scheme`.

### Galaxy

`galaxy` generates a disk of orbiting bodies around a central massive body.
The orbital speeds are initialized from the gravitational constant and the
distance from the centre. Masses, radii and vertical positions vary between
bodies, which makes this condition suitable for visualization.

### Spiral

`spiral` generates four arms distributed in the $x$-$y$ plane. The arms make
approximately `0.7` turns and extend to a radius of about `8` in the project
units. A small radial and vertical perturbation gives the system a
three-dimensional appearance. Masses, radii and tangential velocities are
also varied between bodies.

### Circular

`circular` places the bodies regularly on a unit circle with tangential
velocities. It is useful for small, controlled tests and for checking that
different implementations start from the same state.

## Requirements

Required:

- CMake 3.20 or newer;
- a C++ compiler with C++20 support;
- OpenMP support, required by the build configuration.

Optional:

- an x86-64 compiler and processor with AVX2 and FMA for explicit SIMD;
- the CUDA Toolkit and an NVIDIA GPU for the CUDA implementation;
- OpenGL, GLFW and their development packages for the viewer.

CUDA is not available on Apple Silicon machines such as the MacBook M1. The
CUDA sources can still remain in the project, but the CUDA target must be
configured and tested on a Linux system with an NVIDIA GPU.

## Build

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The default build enables the normal executable, the tests and the SIMD
source. On x86-64, AVX2 is enabled automatically when
`NBODY_ENABLE_AVX2` is on. It can be disabled explicitly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    -DNBODY_ENABLE_AVX2=OFF
cmake --build build
```

On macOS, use a compiler installation that provides OpenMP. For example, with
Homebrew LLVM:

```bash
cmake -S . -B build-openmp -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
    -DNBODY_ENABLE_AVX2=OFF
cmake --build build-openmp
```

## Running the Simulation

The executable is `nbody`. The main options are:

```text
--implementation serial-aos|serial-soa|simd|openmp|cuda
--scheme galaxy|spiral|circular
--bodies N
--steps N
--dt VALUE
--g VALUE
--softening VALUE
--input PATH
--output PATH
--no-output
```

The default implementation is `serial-aos`. The default scheme is `galaxy`,
the default time step is `0.001` and the default number of steps is `100`.
Use `--no-output` when only the execution time is needed.

### Serial AoS

```bash
./build/nbody --implementation serial-aos \
    --scheme galaxy --bodies 1000 --steps 100 --dt 0.001 \
    --output trajectory.csv
```

### Serial SoA

```bash
./build/nbody --implementation serial-soa \
    --scheme galaxy --bodies 1000 --steps 100 --dt 0.001 \
    --output trajectory.csv
```

### SIMD SoA

```bash
./build/nbody --implementation simd \
    --scheme spiral --bodies 1024 --steps 100 --dt 0.001 \
    --no-output
```

The explicit AVX2 implementation processes four double-precision values at a
time. For this reason, the number of bodies must be divisible by four. It is
intended for x86-64 systems with AVX2 and FMA support.

### OpenMP SoA

```bash
OMP_NUM_THREADS=4 ./build/nbody --implementation openmp \
    --scheme spiral --bodies 4096 --steps 100 --dt 0.001 \
    --no-output
```

The OpenMP implementation parallelizes the outer loop over target bodies.
`OMP_NUM_THREADS` controls the number of OpenMP threads. The loop uses the
runtime schedule, so the scheduling policy and chunk size can be selected with
the standard OpenMP environment variables when scaling experiments are run.

### CUDA SoA

CUDA must be enabled when configuring the project:

```bash
cmake -S . -B build-cuda -DCMAKE_BUILD_TYPE=Release \
    -DNBODY_ENABLE_CUDA=ON
cmake --build build-cuda
```

Then run the CUDA implementation on a Linux system with an NVIDIA GPU:

```bash
./build-cuda/nbody --implementation cuda \
    --scheme spiral --bodies 4096 --steps 100 --dt 0.001 \
    --no-output
```

The CUDA solver uses one thread for each target body. Source bodies are loaded
in tiles into shared memory, and every thread in a block uses the tile to
compute its interactions.

## Input and Output Files

An optional input file can be supplied with `--input`. Each non-empty line
contains one body in the following order:

```text
mass position_x position_y position_z velocity_x velocity_y velocity_z
```

Lines beginning with `#` are ignored. The radius is not required in the input
file and is assigned the default value `1.0`.

The trajectory writer produces a CSV file with the header:

```text
step,time,id,mass,x,y,z,vx,vy,vz,radius
```

A frame is written before the first integration step and after every following
step. The radius is stored for use by the visualizer.

## Visualizer

The viewer is an independent executable that reads a trajectory CSV file.
Enable it with:

```bash
cmake -S . -B build-viewer \
    -DNBODY_ENABLE_OPENGL=ON \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build-viewer --target nbody_viewer
```

First generate a trajectory, for example:

```bash
./build/nbody --implementation serial-aos \
    --scheme spiral --bodies 1000 --steps 600 --dt 0.001 \
    --output trajectory.csv
```

Then open it with:

```bash
./build-viewer/nbody_viewer trajectory.csv
```

The viewer displays the bodies using their positions and radii. Body colors
are determined by their speed. The left mouse button rotates the scene, the
mouse wheel changes the zoom and `Escape` closes the window.

## Tests

Run the standard tests with:

```bash
ctest --test-dir build --output-on-failure
```

The test suite checks the serial solver, AoS/SoA conversions and comparisons
between the available CPU implementations. The CUDA comparison test is added
only when `NBODY_ENABLE_CUDA=ON`:

```bash
ctest --test-dir build-cuda --output-on-failure
```

The CUDA test compares the final state produced by `CudaSoA` with the state
produced by `SerialSoA` using a numerical tolerance. It requires a working
CUDA installation and an NVIDIA GPU.

## Performance Measurements

For timing measurements, disable trajectory output and use a Release build:

```bash
./build/nbody --implementation serial-soa \
    --scheme galaxy --bodies 4096 --steps 100 --no-output
```

The program prints the selected implementation, number of bodies, number of
steps and elapsed time. For meaningful comparisons, use the same initial
condition and simulation parameters for every implementation, repeat each run
several times and compare the median execution time.
