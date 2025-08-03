# Building RTEmsg on Linux

This document describes how to build RTEmsg on Linux systems using CMake.

## Prerequisites

- CMake 3.10 or higher
- GCC or Clang compiler with C11 support
- Standard development tools (make, etc.)

## Building

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure the project:
```bash
cmake ..
```

3. Build the project:
```bash
make -j$(nproc)
```

The executable will be created in `build/bin/RTEmsg`.

## Build Options

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Release Build with Static Runtime
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DSTATIC_RUNTIME=ON ..
make
```

## Platform Compatibility

The Linux build includes a platform compatibility layer (`platform_compat.h` and `platform_compat.c`) that provides implementations for Windows-specific functions used in the codebase, including:

- Wide character file operations (`_wfopen`, `_wremove`, `_wrename`)
- Wide character directory operations (`_wchdir`, `_wgetcwd`)
- Exception handling macros (`__try`, `__except`, `__finally`)
- Various other Windows-specific functions

## Known Issues

- The build generates several format string warnings. These are non-critical and do not affect functionality.
- Stack checking on Linux uses a simplified approach compared to the Windows implementation.

## Usage

The RTEmsg tool requires format definition files and binary data files to process. Refer to the main README for usage instructions.