cmake_minimum_required(VERSION 3.20)

project(nbody_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-Wall>
        $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
        $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
        $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
        $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>
    )
endif()

option(NBODY_BUILD_TESTS "Build small verification tests" ON)
option(NBODY_ENABLE_AVX2 "Build the explicit x86-64 AVX2 SIMD implementation" ON)
option(NBODY_ENABLE_CUDA "Build the CUDA SoA implementation" OFF)
option(NBODY_ENABLE_OPENGL "Build the OpenGL trajectory visualizer" OFF)

if(NBODY_ENABLE_CUDA)
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
endif()

find_package(OpenMP REQUIRED)

add_library(nbody_core
    src/common/core/Bodies.cpp
    src/common/core/InitialCondition.cpp
    src/common/core/Physics.cpp
    src/common/core/SimulationNBodyInterface.cpp
    src/common/core/SimulationConfig.cpp
    src/common/core/TrajectoryReader.cpp
    src/common/core/TrajectoryWriter.cpp
    src/common/utils/ArgumentsReader.cpp
    src/common/utils/Timer.cpp
    src/nbody/openmp/OpenMPSoA.cpp
    src/nbody/serial/SerialAoS.cpp
    src/nbody/serial/SerialSoA.cpp
    src/nbody/simd/SIMDSoA.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set_source_files_properties(
        src/nbody/simd/SIMDSoA.cpp
        PROPERTIES
            COMPILE_OPTIONS "-O3"
    )
    if(NBODY_ENABLE_AVX2 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
        set_source_files_properties(
            src/nbody/simd/SIMDSoA.cpp
            PROPERTIES
                COMPILE_DEFINITIONS NBODY_ENABLE_AVX2
                COMPILE_OPTIONS "-O3;-mavx2;-mfma"
        )
    endif()
endif()

target_include_directories(nbody_core PUBLIC src)
target_link_libraries(nbody_core PUBLIC OpenMP::OpenMP_CXX)

if(NBODY_ENABLE_CUDA)
    target_sources(nbody_core PRIVATE src/nbody/cuda/CudaSoA.cu)
    target_compile_definitions(nbody_core PUBLIC NBODY_ENABLE_CUDA)
    target_link_libraries(nbody_core PUBLIC CUDA::cudart)
    set_target_properties(nbody_core PROPERTIES
        CUDA_STANDARD 17
        CUDA_STANDARD_REQUIRED ON
    )
endif()

add_executable(nbody src/nbody/main.cpp)
target_link_libraries(nbody PRIVATE nbody_core)

if(NBODY_ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    add_executable(nbody_viewer
        src/common/ogl/Render.cpp
        src/common/ogl/main_view.cpp
    )
    target_link_libraries(nbody_viewer PRIVATE nbody_core OpenGL::GL glfw)
endif()

if(NBODY_BUILD_TESTS)
    enable_testing()
    add_executable(nbody_tests src/test/serial_tests.cpp)
    target_link_libraries(nbody_tests PRIVATE nbody_core)
    add_test(NAME serial_tests COMMAND nbody_tests)

    add_executable(layout_comparison_tests src/test/layout_comparison_tests.cpp)
    target_link_libraries(layout_comparison_tests PRIVATE nbody_core)
    add_test(NAME layout_comparison_tests COMMAND layout_comparison_tests)

    add_executable(simd_comparison_tests src/test/simd_comparison_tests.cpp)
    target_link_libraries(simd_comparison_tests PRIVATE nbody_core)
    add_test(NAME simd_comparison_tests COMMAND simd_comparison_tests)

    add_executable(openmp_comparison_tests src/test/openmp_comparison_tests.cpp)
    target_link_libraries(openmp_comparison_tests PRIVATE nbody_core)
    add_test(NAME openmp_comparison_tests COMMAND openmp_comparison_tests)

    if(NBODY_ENABLE_CUDA)
        add_executable(cuda_comparison_tests src/test/cuda_comparison_tests.cpp)
        target_link_libraries(cuda_comparison_tests PRIVATE nbody_core)
        add_test(NAME cuda_comparison_tests COMMAND cuda_comparison_tests)
    endif()
endif()
