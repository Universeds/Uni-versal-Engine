# Uni-versal Engine

A modern C++ game engine built with OpenGL.

## Features

- OpenGL rendering backend
- Modular renderer architecture
- Buffer and vertex array abstractions
- Shader management system
- Cross-platform support

## Building

This project uses CMake and vcpkg for dependency management.

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- vcpkg (included in repository)

### Build Instructions

1. Clone the repository
2. Run the build script:
   ```
   run.bat
   ```

Or manually:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Project Structure

- `src/` - Source code
  - `Core/` - Engine core systems
  - `Renderer/` - Rendering system
    - `OpenGL/` - OpenGL-specific implementations
- `build/` - Build output (generated)
- `vcpkg/` - Package manager

## Dependencies

- OpenGL
- GLFW (window management)
- GLEW (OpenGL extension loading)
