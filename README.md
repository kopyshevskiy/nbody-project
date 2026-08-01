# N-Body Course Project

Minimal C++ N-body project organized for a sequential parallel-programming study.

Current stage:

- shared core data structures and utilities;
- serial AoS and SoA implementations;
- portable SIMD implementation based on the SoA layout;
- OpenMP implementation based on the SoA layout;
- optional tiled CUDA implementation based on the SoA layout;
- optional OpenGL trajectory visualizer;
- trajectory input/output;
- single CLI entry point.

Build:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

On macOS, configure with a compiler that supports OpenMP. With Homebrew LLVM:

```bash
cmake -S . -B build-openmp -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
    -DNBODY_ENABLE_AVX2=OFF
cmake --build build-openmp
ctest --test-dir build-openmp
```

On Linux x86-64, leave `NBODY_ENABLE_AVX2` enabled (the default) on CPUs
with AVX2 and FMA support. Disable it only when a scalar fallback is needed.

Example:

```bash
./build/nbody --implementation serial-aos --scheme galaxy --bodies 1000 --output trajectory.csv --steps 600 --dt 0.001
```

Without `--input`, the default initial condition is a galaxy with a central
massive body and orbiting bodies. Use `--scheme spiral` for a four-arm spiral
or `--scheme circular` for the regular test orbit.

Use `--implementation serial-soa` to run the same simulation with the SoA layout.
Use `--implementation simd` to use explicit AVX2 vector registers on Linux
x86-64 (`__m256d`, FMA and vector loads/stores). The number of bodies must be
divisible by four. Other architectures use the scalar fallback in the same solver.
Use `--implementation openmp` to run the OpenMP-only SoA implementation.

The visualizer is an independent executable that reads the CSV trajectory written
by the simulator. It uses the position and radius stored in each `Body`:

```bash
cmake -S . -B build-viewer -DNBODY_ENABLE_OPENGL=ON
cmake --build build-viewer --target nbody_viewer
./build-viewer/nbody_viewer trajectory.csv
```

The left mouse button rotates the view, the mouse wheel changes the zoom and
Escape closes the window. The trajectory format contains the columns
`step,time,id,mass,x,y,z,vx,vy,vz,radius`.

For a longer visual example, use 1000 bodies and 600 steps. The direct
interaction algorithm has quadratic cost, and the viewer displays different
colors according to the bodies' speeds.

On a system with an NVIDIA GPU and the CUDA toolkit, enable the CUDA version
explicitly:

```bash
cmake -S . -B build-cuda -DCMAKE_BUILD_TYPE=Release -DNBODY_ENABLE_CUDA=ON
cmake --build build-cuda
ctest --test-dir build-cuda
./build-cuda/nbody --implementation cuda --bodies 4096 --steps 100 --no-output
```
