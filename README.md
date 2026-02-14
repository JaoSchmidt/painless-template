# Pain Engine - Game Development Guide

## New Project Structure

This setup allows developers to **create their game first**, then import the Pain Engine as a dependency. No more editing engine files!

## Setup Instructions for Developers

### 1. Create Your Game Project From This Template

```bash
git clone https://github.com/JaoSchmidt/painless-template - C MyAwesomeGame
cd MyAwesomeGame
```

### 2. Add the Engine

You have two options:

**Option A: Git Submodule (Recommended)**

```bash
git submodule add https://github.com/JaoSchmidt/Pain-Engine.git Engine
git submodule update --init --recursive
```

**Option B: Manual Copy**

```bash
# Copy the Engine directory into your project
unzip Pain-Engine-master.zip -d Engine
```

### 3. Build Your Game For Linux

For builds other than linux, type `cmake --list-presets`. More on that below.

> NOTE: TARGET_NAME cannot have spaces

```bash
cmake --preset default -DTARGET_NAME="MyExecutable" -DPROJECT_NAME="My Awesome Game"
cd build
make
```

### Resource Syncing

- **Unix/Linux/macOS**: Uses `rsync` for efficient resource copying
- **Windows**: Uses CMake's `copy_directory` command

Ensure your resources are in `./resources/` directory at your project root.

### 3.1 Building Variants

The engine automatically manages its own resources in this case.

### Windows (MSVC)

```bash
cmake --preset ci-windows
```

The CMakeLists.txt automatically applies MSVC-specific flags:

- `/Zc:preprocessor` for VA_OPT support
- `/W4` for warning level 4
- `/std:c++20` for C++20 standard

### macOS / Linux (GCC/Clang)

```bash
cmake --preset ci-macos
```

## Files Explained

### Root CMakeLists.txt (Your Game Project)

Located at: `./CMakeLists.txt`

This is the main file developers interact with. It:

- **Defines your game executable** from sources in `src/`
- **Includes the Engine** via `add_subdirectory(Engine)`
- **Links the Pain library** to your executable
- **Sync your game resources folder** to your build folder
- **Handles compiler flags** for your project

### Directory Layout

```
MyGame/                          # Your game project root
├── CMakeLists.txt              # Your game's CMake config (root level)
├── src/                         # Your game source files
│   └── main.cpp
├── include/                     # Your game headers
│   └── YourGameHeader.h
├── resources/                   # Your game assets (textures, audio, scripts etc.)
│   ├── textures/
│   ├── audio/
│   └── scripts/
└── Engine/                      # Pain Engine (as submodule or directory)
    ├── CMakeLists.txt
    ├── Pain/
    │   ├── src/
    │   ├── include/
    │   ├── platform/
    │   ├── external/
    │   ├── resources/
    └── Examples/
        └── scriptGame/          # Official example game
            ├── CMakeLists.txt
            ├── src/
            ├── include/
            └── resources/
```

## Customization

### Additional Dependencies

Add before linking to Pain:

```cmake
target_link_libraries(${TARGET_NAME}
    PUBLIC
        Pain
        OtherLib
        AnotherLib
)
```

## Common Issues

### "Pain Engine not found"

**Error:** `CMake Error: Pain Engine not found at .../Engine/CMakeLists.txt`

**Fix:** Ensure the Engine directory exists with a CMakeLists.txt file:

```bash
ls Engine/CMakeLists.txt
```

### Resource Copying Not Working

**Check:**

1. You are on linux or macOS and rsync is installed
2. Resources directory exists: `ls resources/`
3. Build directory exists: `ls build`

**Windows:** If `rsync` is not available, CMake uses `copy_directory` automatically.

### Linker Errors with Pain Library

**Fix:** Ensure Pain is built before your game:

## Next Steps

1. **Create your game source files** in `src/`
2. **Add any custom headers** to `include/`
3. **Place assets** in `resources/`

## Support

For issues with the Pain Engine itself, refer to the Engine documentation in `Engine/README.md`.

For CMake build issues, check:

- CMake version: `cmake --version` (requires 3.22+)
- Compiler: `gcc --version` >= 18, `clang --version` >= 20;
- Dependencies: Ensure SDL2, OpenGL, and other required libraries are installed

---
