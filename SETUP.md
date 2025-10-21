# Uni-versal Engine - Setup Guide

Complete guide for setting up the Uni-versal Engine on a new computer.

---

## 📋 Prerequisites

### Required Software

1. **Git**
   - Download: https://git-scm.com/downloads
   - Verify: `git --version`

2. **CMake 3.16+**
   - Download: https://cmake.org/download/
   - ⚠️ During installation, select "Add CMake to system PATH"
   - Verify: `cmake --version`

3. **C++ Compiler** (choose one):

   **Option A: Visual Studio (Recommended for Windows)**
   - Visual Studio 2019 or 2022 Community Edition (Free)
   - Download: https://visualstudio.microsoft.com/downloads/
   - ⚠️ Select "Desktop development with C++" workload during installation
   - Includes: MSVC compiler, Windows SDK

   **Option B: MinGW-w64**
   - Download: https://www.mingw-w64.org/
   - ⚠️ Add bin directory to system PATH
   - Verify: `g++ --version`

4. **OpenGL-capable Graphics Drivers**
   - OpenGL 3.3+ support required
   - Update to latest drivers from your GPU manufacturer

### Optional (Recommended)

- **Visual Studio Code**
  - Download: https://code.visualstudio.com/
  - Extensions: C/C++, CMake Tools

---

## 🚀 Quick Start

### Windows (One-Command Setup)

```bash
git clone https://github.com/Universeds/Uni-versal-Engine.git
cd Uni-versal-Engine
cd vcpkg && .\bootstrap-vcpkg.bat && cd ..
run.bat
```

### Linux/macOS (One-Command Setup)

```bash
git clone https://github.com/Universeds/Uni-versal-Engine.git
cd Uni-versal-Engine
cd vcpkg && ./bootstrap-vcpkg.sh && cd ..
mkdir build && cd build && cmake .. && cmake --build . && ./UniversalEngine
```

---

## 📖 Detailed Setup Instructions

### Step 1: Clone Repository

```bash
git clone https://github.com/Universeds/Uni-versal-Engine.git
cd Uni-versal-Engine
```

### Step 2: Bootstrap vcpkg

The repository includes vcpkg for dependency management. Bootstrap it:

**Windows:**
```bash
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

**Linux/macOS:**
```bash
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
```

This builds the vcpkg executable needed to manage C++ dependencies.

### Step 3: Build the Project

#### Using the Build Script (Windows Only)

The easiest method:
```bash
run.bat
```

This script will:
- Detect your compiler (Visual Studio, MinGW, etc.)
- Configure CMake with appropriate generator
- Build the project in Release mode
- Launch the engine automatically

#### Manual Build (All Platforms)

**1. Create build directory:**
```bash
mkdir build
cd build
```

**2. Configure with CMake:**

Windows (Visual Studio 2022):
```bash
cmake .. -G "Visual Studio 17 2022"
```

Windows (Visual Studio 2019):
```bash
cmake .. -G "Visual Studio 16 2019"
```

Windows (MinGW):
```bash
cmake .. -G "MinGW Makefiles"
```

Linux/macOS:
```bash
cmake ..
```

**3. Build:**

For Visual Studio:
```bash
cmake --build . --config Release
```

For Make-based systems:
```bash
cmake --build .
```

**4. Return to project root:**
```bash
cd ..
```

### Step 4: Run the Engine

**Windows (Visual Studio):**
```bash
build\Release\UniversalEngine.exe
```

**Windows (MinGW):**
```bash
build\UniversalEngine.exe
```

**Linux/macOS:**
```bash
./build/UniversalEngine
```

---

## 📁 Project Structure

```
Uni-versal-Engine/
├── src/                        # Source code
│   ├── Core/                   # Core engine systems
│   │   ├── Engine.h/cpp        # Main engine class
│   │   ├── Components/         # ECS components
│   │   ├── ECS/                # Entity Component System
│   │   ├── Scenes/             # Scene management
│   │   └── Systems/            # ECS systems
│   ├── Renderer/               # Rendering subsystem
│   │   ├── Renderer.h/cpp      # Main renderer
│   │   ├── Buffer.h/cpp        # Buffer abstractions
│   │   ├── Shader.h/cpp        # Shader management
│   │   ├── VertexArray.h/cpp   # VAO abstractions
│   │   └── OpenGL/             # OpenGL implementation
│   └── main.cpp                # Entry point
├── tests/                      # Unit tests
│   ├── ECSTests.cpp            # ECS system tests
│   └── TestMain.cpp            # Test runner
├── imgui/                      # ImGui library (included)
│   └── backends/               # GLFW & OpenGL3 backends
├── glfw-3.4.bin.WIN64/         # Pre-built GLFW binaries (Windows)
├── vcpkg/                      # Package manager
├── build/                      # Build output (generated)
├── .vscode/                    # VS Code configuration
├── CMakeLists.txt              # Build configuration
├── run.bat                     # Windows build/run script
├── clean.bat                   # Clean build artifacts
├── README.md                   # Project overview
└── SETUP.md                    # This file
```

---

## 🔧 Dependencies

### Automatic Dependencies (via vcpkg)

These are automatically installed during CMake configuration:

- **GLFW** - Window management and input handling
- **GLEW** - OpenGL extension loading
- **GLM** - OpenGL Mathematics library

### Included Dependencies

- **ImGui** - Immediate mode GUI library (in `imgui/` directory)
- **OpenGL** - Graphics API (system-provided)

### Pre-built Binaries

- **GLFW 3.4** - Windows binaries included in `glfw-3.4.bin.WIN64/`

---

## 🛠️ Development Setup

### Using Visual Studio Code

1. Open project folder in VS Code
2. Install recommended extensions:
   - C/C++ (Microsoft)
   - CMake Tools
3. Configure: `Ctrl+Shift+P` → "CMake: Configure"
4. Build: `F7` or use CMake Tools sidebar
5. Debug: `F5`

### Using Visual Studio IDE

1. File → Open → Folder
2. Select the project root directory
3. VS will detect CMakeLists.txt automatically
4. Select configuration (Debug/Release)
5. Build: `Ctrl+Shift+B`
6. Run: `Ctrl+F5`

---

## 🧪 Running Tests

The project includes comprehensive ECS unit tests.

### Run All Tests

```bash
cd build
ctest
```

### Run Test Executable Directly

**Windows (Visual Studio):**
```bash
build\Release\UniversalEngineTests.exe
```

**Other platforms:**
```bash
build/UniversalEngineTests
```

### Run Specific Test

```bash
build\Release\UniversalEngineTests.exe --test="Entity Creation"
```

Available tests:
- Entity Creation
- Entity Validation
- Component Addition
- Component Retrieval
- Component Removal
- Multiple Components
- Entity Destruction

---

## 🧹 Cleaning Build Files

### Using Clean Script (Windows)

```bash
clean.bat
```

### Manual Cleanup

**Windows:**
```bash
rmdir /s /q build
```

**Linux/macOS:**
```bash
rm -rf build
```

After cleaning, rebuild from Step 3.

---

## ❗ Troubleshooting

### vcpkg Bootstrap Fails

**Symptoms:** 
- `bootstrap-vcpkg.bat` or `bootstrap-vcpkg.sh` fails
- "vcpkg.exe not found" error

**Solutions:**
1. Ensure internet connection (vcpkg downloads components)
2. Run terminal as Administrator (Windows) or with sudo (Linux/macOS)
3. Check that Git is installed and in PATH
4. Verify vcpkg directory is not corrupted:
   ```bash
   cd vcpkg
   git status
   ```

### CMake Cannot Find vcpkg Toolchain

**Symptoms:**
- CMake error about missing toolchain file
- "Could not find vcpkg.cmake"

**Solutions:**
1. Verify vcpkg was bootstrapped successfully
2. Check `vcpkg/scripts/buildsystems/vcpkg.cmake` exists
3. CMakeLists.txt should automatically set toolchain path

### Missing Dependencies

**Symptoms:**
- CMake errors about missing packages (GLFW, GLEW, GLM)
- Link errors during build

**Solutions:**
1. Let CMake configure completely - vcpkg installs dependencies automatically
2. First build takes longer (dependencies compile from source)
3. Manual installation:
   ```bash
   cd vcpkg
   .\vcpkg install glfw3 glew glm
   cd ..
   ```

### OpenGL Runtime Errors

**Symptoms:**
- Application crashes on startup
- "Failed to create OpenGL context"
- Black screen or rendering artifacts

**Solutions:**
1. Update graphics drivers (NVIDIA, AMD, Intel)
2. Verify OpenGL support:
   - Download OpenGL Extensions Viewer
   - Check OpenGL version ≥ 3.3
3. Check GPU compatibility with OpenGL 3.3+

### Compiler Not Found

**Symptoms:**
- "No suitable generator found"
- CMake cannot detect compiler

**Solutions:**
1. Install Visual Studio with C++ workload
2. OR install MinGW and add to PATH
3. Restart terminal after installation
4. Verify installation:
   ```bash
   # Visual Studio
   where cl

   # MinGW
   where g++
   ```

### Build Errors with MSVC

**Symptoms:**
- C++ compilation errors
- Linker errors with Visual Studio

**Solutions:**
1. Install latest Windows SDK
2. Use Release configuration:
   ```bash
   cmake --build build --config Release
   ```
3. Clean and rebuild:
   ```bash
   clean.bat
   run.bat
   ```

### Link Errors with OpenGL

**Symptoms:**
- Undefined references to `gl*` functions
- Missing OpenGL libraries

**Solutions:**
1. Windows: Ensure `opengl32.lib` is linked (automatic)
2. Linux: Install mesa development packages:
   ```bash
   sudo apt-get install libgl1-mesa-dev
   ```
3. macOS: OpenGL framework should be automatic

---

## 🌐 Platform-Specific Notes

### Windows

- Visual Studio 2019/2022 recommended
- MinGW also supported
- Pre-built GLFW binaries included
- OpenGL provided by graphics drivers

### Linux

- GCC 7+ or Clang 5+ required
- Install development packages:
  ```bash
  sudo apt-get install build-essential cmake git
  sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
  ```
- Use system package manager for basic dependencies

### macOS

- Xcode Command Line Tools required:
  ```bash
  xcode-select --install
  ```
- OpenGL framework included in macOS
- May need Homebrew for some tools

---

## 📚 Additional Resources

### Learning Resources

- [OpenGL Tutorial](https://learnopengl.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [ImGui Documentation](https://github.com/ocornut/imgui)

### Project Links

- [GitHub Repository](https://github.com/Universeds/Uni-versal-Engine.git)
- [vcpkg Documentation](https://vcpkg.io/)

---

## 🤝 Contributing

If you encounter issues not covered in this guide:

1. Check existing issues on GitHub
2. Review CMake output for specific errors
3. Verify all prerequisites are installed
4. Ensure system meets minimum requirements

---

## ✅ Verification Checklist

After setup, verify everything works:

- [ ] Repository cloned successfully
- [ ] vcpkg bootstrapped (vcpkg.exe exists)
- [ ] CMake configuration successful
- [ ] Project builds without errors
- [ ] Engine executable runs
- [ ] Tests pass (optional)

---

## 🎯 Quick Troubleshooting Flowchart

```
Problem building?
├── vcpkg issues?
│   └── Re-bootstrap: cd vcpkg && .\bootstrap-vcpkg.bat
├── CMake issues?
│   └── Check CMake version: cmake --version (need 3.16+)
├── Compiler issues?
│   └── Install Visual Studio with C++ workload
└── Runtime issues?
    └── Update graphics drivers
```

---

**Last Updated:** October 2025  
**Engine Version:** Development  
**CMake Minimum:** 3.16  
**C++ Standard:** C++17
