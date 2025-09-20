#include <string>

#pragma once

namespace Config 
{
    inline constexpr int SCREEN_WIDTH = 1024;
    inline constexpr int SCREEN_HEIGHT = 768;

    inline constexpr int WALL_THICKNESS = 16;
    inline constexpr int TILE_SIZE = 32;
    inline constexpr int PIX_SIZE = 16;

    inline constexpr int WALK_SPEED = 200;
    inline constexpr float JUMP_STRENGTH = 500.f;  

    inline constexpr float GRAVITY = 1000.f;
    inline constexpr float ROCK_SPEED = 300.f; // Adjust for camera smoothness

    inline constexpr std::string HIGH_SCORES = "highscores.json";
}