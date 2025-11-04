# Wolf3D

A Wolf3D-style raycasting engine written in C using the Raylib graphics library. This project implements a classic pseudo-3D rendering technique popularized by games like Wolfenstein 3D.

## Using

- **CMake**: Version 4.0.1
- **Raylib**: Version 3.0
- **C Compiler**: C11
- **Platform**: Cross-platform (Windows, macOS, Linux)

### macOS Additional Requirements

The build system automatically links the following frameworks on macOS:

- IOKit
- Cocoa
- OpenGL

## Building

### Prerequisites

First, install Raylib. On macOS with Homebrew:

```bash
brew install raylib
```

On Ubuntu/Debian:

```bash
sudo apt-get install libraylib-dev
```

### Compilation

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make

# Run the executable
./wolf3d
```

### Build Types

**Debug Build:**

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

**Release Build:**

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
