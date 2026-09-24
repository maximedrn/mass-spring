# Mass Spring

A C++ mass-spring system simulation using Qt5 and QGLViewer for visualization.

## Prerequisites

- [CMake](https://cmake.org/)
- [Qt5](https://www.qt.io/)
- [Clang](https://clang.llvm.org/)
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

## Usage

### Build and run

#### Euler

```sh
cmake -S . -B .build -DCMAKE_BUILD_TYPE=Release -DINTEGRATOR=euler
cmake --build .build --parallel
cmake --build .build --target run
```

![Euler demo](assets/euler.mp4)

### Runge-Kutta 4

```sh
cmake -S . -B .build -DINTEGRATOR=rk4
cmake --build .build --parallel
cmake --build .build --target run
```

![RK4 demo](assets/rk4.mp4)

### Format

```sh
cmake --build .build --target format
cmake --build .build --target format-check
```
