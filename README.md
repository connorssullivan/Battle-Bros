# 🐺 Battle Bros — 2D Platformer (C++ SFML)

<div align="center">
  <img src="assets/gifs/battle-bros.gif" alt="Battle Bros Level1" width="800"/>

</div>

A modern, arcade-style platformer built in C++ with SFML. Jump across platforms, throw rocks, dodge monsters, collect coins, and chase the star.

![C++](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=cplusplus)
![SFML](https://img.shields.io/badge/SFML-3.0.1-green?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-3.28-orange?style=for-the-badge&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

## 🎮 Features

- **Responsive Platforming**: Gravity, jump arcs, and precise horizontal/vertical collision resolution
- **Projectile System**: Throw rocks with top/bottom/side collision handling and anti-tunneling
- **State Management**: Clean separation between main menu, Level1, pause, and game over
- **Asset Management**: Centralized resource loading/caching for textures, fonts, and audio
- **Modern C++**: C++20 codebase with RAII and smart pointers
- **Cross-Platform**: CMake-based builds on Windows, macOS, and Linux
- **Audio Integration**: Background music and effects

## 🏗️ Architecture

### Core Components

- **Game Engine**: Main loop and view/camera system
- **State Machine**: `MainMenu`, `Level1`, `PauseGame`, `GameOver`
- **Asset Manager**: Texture, font, and audio loading/caching
- **Character/BlueDude**: Movement, animation, jump, and throwing
- **Monsters / Coins / Star**: Interactables and simple AI/collectibles
- **Input System**: Keyboard-driven controls

### Design Patterns

- **State Pattern** for scene management
- **Resource Manager Pattern** for assets
- **RAII** with smart pointers for safe ownership
- **Modular Design** for maintainable Level1 code

## 🛠️ Technical Stack

- **Language**: C++20
- **Graphics/Audio**: SFML 3.0.1
- **Build System**: CMake 3.28
- **Platform**: Windows, macOS, Linux

## 🚀 Getting Started

### Prerequisites

- C++20-compatible compiler (Clang, GCC, or MSVC)
- CMake 3.28+
- Git
- Linux-only deps (example for Debian/Ubuntu):
  ```bash
  sudo apt update && sudo apt install -y \
    libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
    libfreetype-dev libflac-dev libvorbis-dev \
    libgl1-mesa-dev libegl1-mesa-dev
  ```

### Installation

1. Clone the repository
   ```bash
   git clone https://github.com/connorssullivan/Battle-Bros.git
   cd Battle-Bros
   ```

2. Build the project
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j
   ```

3. Run the game
   ```bash
   ./build/bin/main
   ```

### Development Setup

- **VS Code**: Use the CMake Tools extension
- **Visual Studio**: Open `CMakeLists.txt`
- **CLion**: Open the project directory
- **Qt Creator**: Open `CMakeLists.txt`

## 🎮 Controls

- Left/Right: Move
- Up: Jump (when grounded)
- Space: Throw rock
- Esc: Pause

## 📁 Project Structure

```
Battle-Bros/
├── assets/
│   ├── audio/
│   ├── fonts/
│   └── textures/
├── include/
│   ├── core/         # Config, Game, State, StateMan, AssetMan
│   ├── menus/        # MainMenu, PauseGame, GameOver
│   ├── sprites/      # Character, BlueDude
│   └── objects/      # Monster, Coin, Star
├── src/
│   ├── main.cpp
│   ├── Game.cpp, Level1.cpp, StateMan.cpp, AssetMan.cpp
│   ├── Character.cpp, BlueDude.cpp
│   ├── Monster.cpp, Coin.cpp, Star.cpp
│   └── MainMenu.cpp, PauseGame.cpp, GameOver.cpp
└── CMakeLists.txt
```

## 🎯 Key Implementation Details

### State Management
Manages transitions across `MainMenu`, `Level1`, `PauseGame`, and `GameOver` while keeping logic isolated per scene.

### Player & Rocks
- Separate velocities for player and rock
- Rock collisions use previous-frame bounds to classify top/bottom/side impacts and prevent tunneling
- Landed rocks remain supported on platform tops and can be picked up

### Collisions
- Character resolves horizontal then vertical movement each frame
- Landing computes corrected position on top of platforms
- Walls and ground are treated as platforms for consistent behavior

### Asset Management
Centralized loading/caching for textures, fonts, and audio, minimizing IO and ensuring single ownership.

## 🔧 Customization

### Adding New Features
1. Create a new state (derive from `State`) or entity class
2. Register assets in the asset system
3. Implement logic and wire into `Level1`
4. Add sources to `CMakeLists.txt`

### Modifying Level1
- Tune physics in `include/core/Config.h`
- Adjust collisions in `src/Character.cpp`
- Add mechanics in `src/Level1.cpp`

## 📊 Performance Considerations

- 60 FPS target
- Efficient sprite bounds checks and simple intersection math
- Cached assets and minimal per-frame allocations

## 📝 License

This project is licensed under the MIT License — see `LICENSE.md`.

Note: Some bundled assets (e.g., fonts) include their own licenses.

## 🙏 Acknowledgments

- SFML team for the excellent library
- CMake community for the build system

---

Built with C++ and SFML. Designed to showcase clean architecture, modern C++, and practical game engineering suitable for SWE roles.
