# Missile Interception Simulator

A small 2D C++ simulation project exploring radar sweeps, aircraft movement, missile guidance, and collisions. The player pilots an aircraft while a rotating radar sweep detects nearby targets. Fire missiles to engage the nearest enemy and watch the guidance and physics systems play out on screen.

This project is a personal/educational simulation and is not intended for real-world targeting or operational use.

## Features

- SDL2 window and real-time 2D rendering
- Player and enemy aircraft with simple movement behavior
- Rotating radar sweep with range-limited contacts
- Missile flight and proportional-navigation-style guidance
- Collision and damage handling
- On-screen debug overlay

## Requirements

- A C++17 compiler (MSVC, GCC, or Clang)
- CMake 3.16 or newer
- SDL2 development files (headers and libraries)

CMake must be able to locate SDL2 on your system. Install SDL2 using your operating system's package manager or a supported package manager such as vcpkg, then follow its setup instructions if CMake does not find it automatically.

## Build and run

From the project root, configure and build with CMake:

```sh
cmake -S . -B build
cmake --build build --config Release
```

Run the resulting executable:

- **Windows:** `build/Release/MissileSim.exe` with a Visual Studio generator; some generators place it at `build/MissileSim.exe`.
- **Linux/macOS:** `./build/MissileSim`

If your SDL2 installation is outside the system's normal search paths, provide the SDL2 package location when configuring CMake. For example, with a vcpkg toolchain file:

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Controls

| Key | Action |
| --- | --- |
| Arrow keys | Move the player aircraft |
| Space | Fire a missile toward the nearest enemy |
| E | Spawn another enemy aircraft |
| D | Toggle the debug overlay |
| Esc | Quit |

## Project layout

- `engine/` — game loop and renderer
- `entities/` — aircraft and missile entities
- `guidance/` — missile guidance logic
- `physics/` — movement and vector math
- `radar/` — radar sweep and contact detection
- `ui/` — debug overlay
- `main.cpp` — program entry point
- `CMakeLists.txt` — CMake build configuration

## Notes

The CMake configuration currently requires SDL2 through `find_package(SDL2 REQUIRED)`. If configuration reports that SDL2 cannot be found, install the SDL2 development package and make its CMake package configuration discoverable to CMake.

