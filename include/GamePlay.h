#pragma once

#include <memory>
#include <array>


#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio.hpp>

#include "Config.h"
#include "State.h"
#include "Game.h"
#include "BlueDude.h"

class Context;

class GamePlay : public Engine::State
{
    private:
        std::shared_ptr<Context> m_context;


        // Basic Members
        int m_score;
        bool isWalking;
        bool m_isPaused;

        sf::Music bgSound;

        //Sprites
        std::optional<sf::Sprite> m_grass;
        std::optional<sf::Sprite> m_food;
        std::array<std::unique_ptr<sf::Sprite>, 3> m_walls;
        std::unique_ptr<sf::Sprite> m_ground; 
        std::unique_ptr<Character> m_player; 
        std::optional<sf::Text> m_scoreText;
        std::vector<std::unique_ptr<sf::Sprite>> m_bricks;


        // Camera system
        sf::View m_camera;
        sf::Vector2f m_cameraTarget;  
        float m_cameraSmoothness;     

        sf::Time m_elapsedTime;

    public:
        GamePlay(std::shared_ptr<Context>& context);
        ~GamePlay();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;
        void Pause() override;
        void Start() override;

    private:
        void UpdateCamera();
        std::vector<sf::Sprite*> getPlatforms();  // Get all platform sprites for collision
};