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
#include "Coin.h"
#include "Monster.h"
#include "Star.h"

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
        std::unique_ptr<sf::Sprite> m_background;
        std::unique_ptr<sf::Sprite> m_background2; // Second background for repeating
        std::array<std::unique_ptr<sf::Sprite>, 3> m_walls;
        std::unique_ptr<sf::Sprite> m_ground; 
        std::unique_ptr<Character> m_player; 
        std::optional<sf::Text> m_scoreText;
        std::vector<std::unique_ptr<sf::Sprite>> m_bricks;
        std::vector<std::unique_ptr<Coin>> m_coins;
        std::vector<std::unique_ptr<Monster>> m_monsters;
        std::unique_ptr<Star> m_star;


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
        void CheckMonsterHit();

    private:
        void UpdateCamera();
        std::vector<sf::Sprite*> getPlatforms();  // Get all platform sprites for collision
        bool checkMonsterAttack(Monster* monster);
        bool checkGotStar();
};