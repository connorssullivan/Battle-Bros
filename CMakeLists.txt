cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(SFML)

add_executable(main 
    src/main.cpp
    src/Game.cpp
    src/StateMan.cpp
    src/AssetMan.cpp
    src/MainMenu.cpp
    src/Level1.cpp
    src/GameOver.cpp
    src/PauseGame.cpp
    src/Character.cpp
    src/BlueDude.cpp
    src/Coin.cpp
    src/Monster.cpp
    src/LevelSelector.cpp
    src/Star.cpp
    src/HighScoreForm.cpp
    src/LeaderBoard.cpp
)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio nlohmann_json::nlohmann_json)

# INclude my libraries 
target_include_directories(main PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/levels
    ${CMAKE_SOURCE_DIR}/include/scenes
    ${CMAKE_SOURCE_DIR}/include/menus
    ${CMAKE_SOURCE_DIR}/include/sprites
    ${CMAKE_SOURCE_DIR}/include/objects
)

