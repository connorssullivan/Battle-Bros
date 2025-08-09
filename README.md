<div align="center">

# Battle Bros

Arcade platformer built with modern C++ and SFML 3. Throw rocks, dodge monsters, collect coins, and chase the star.

</div>

---

## Highlights

- Clean C++20 codebase with a minimal engine: `State` machine, asset manager, and scene-driven gameplay
- Deterministic-feeling platforming with gravity, jump arcs, and precise collision resolution
- Projectile system (rocks) with top/bottom/side collision handling and anti-tunneling using previous-frame bounds
- Smooth camera follow with configurable responsiveness
- Simple, extensible enemy AI (patrol/attack/kill-on-star) and score/collectible systems
- Cross-platform builds via CMake; SFML 3 fetched at configure time (no manual SDK installs required)

## Quick Start

Prerequisites

- Git, CMake ≥ 3.28, and a C++20 compiler (Clang, GCC, or MSVC)
- Linux only: install SFML runtime deps (example for Debian/Ubuntu):
  ```bash
  sudo apt update && sudo apt install -y \
    libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
    libfreetype-dev libflac-dev libvorbis-dev \
    libgl1-mesa-dev libegl1-mesa-dev
  ```

Build and Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bin/main
```

## Controls

- Left/Right: move
- Up: jump (when grounded)
- Space: throw rock
- Esc: pause

## Project Structure

```
assets/            # Textures, audio, fonts
include/           # Engine and game headers
  core/            # Config, Game, State, StateMan, AssetMan
  menus/           # Pause, GameOver
  sprites/         # Character, BlueDude, Monster, Coin, Star
src/               # Implementations (game loop, scenes, gameplay)
CMakeLists.txt     # Build config (fetches SFML 3.0.1 via FetchContent)
```

Key classes

- `Engine::State`, `StateMan`, `Game`: lightweight state machine and app loop
- `AssetMan`: centralized texture/font/audio loading
- `GamePlay`: world assembly, input, collisions, camera, scoring
- `Character`/`BlueDude`: movement, animation, jump physics, rock throw
- `Monster`, `Coin`, `Star`: interactables and simple AI/collectibles

## Architecture & Implementation Notes

- Physics and collisions
  - Character movement integrates gravity and resolves collisions in two passes (horizontal then vertical)
  - Landing detection computes a corrected top-of-platform position to avoid embedding
  - Rocks use previous-frame bounds to classify impact as top/bottom/side and prevent tunneling
  - “Supported” checks keep landed rocks resting on platform tops; otherwise they resume falling

- Camera
  - Smoothed follow via linear interpolation with configurable `m_cameraSmoothness`
  - View clamped to level bounds to avoid showing out-of-world regions

- Assets & rendering
  - Sprite-based platforms (`m_bricks`) and boundaries (`m_walls` + `m_ground`)
  - Background parallax-like offset and seamless repeat across a wide level

- Modularity
  - Each scene encapsulates its own input, update, and draw logic
  - Entities expose small, focused APIs (e.g., `throwRock`, `pickupRock`, `Update`, `Draw`)

## Extending the Game

- New enemies or power-ups: follow the `Monster`/`Star` patterns and register in `GamePlay`
- Level design: add to `m_bricks` with positions/scales, or introduce a simple level loader (e.g., JSON/TMX)
- Tuning: adjust gravity, jump strength, walk speed, and rock speed in `include/core/Config.h`

## Roadmap

- Basic level file format + loader; in-editor level placement
- Unit tests for collision helpers and state transitions
- CI builds for macOS/Linux/Windows
- Audio mixing polish and dynamic music transitions

## Building in IDEs

- VS Code (CMake Tools), CLion, Visual Studio, and Qt Creator all work out of the box with the CMake project

## License

MIT. See `LICENSE.md`.

Assets may have their own licenses (e.g., fonts in `assets/fonts/Bitcount_Grid_Double`). Review included license files before reuse.
