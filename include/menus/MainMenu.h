#pragma once

#include <memory>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "Config.h"
#include "State.h"
#include "Game.h"

class Context;

class MainMenu : public Engine::State
{
    private:
        std::shared_ptr<Context> m_context;

        std::optional<sf::Text> m_gameTitle;
        std::optional<sf::Text> m_levelsButton;
        std::optional<sf::Text> m_exitButton;
        std::optional<sf::Sprite> m_background;
        std::optional<sf::Sprite> m_floor;

        // Animated background elements
        std::optional<sf::Sprite> m_blueDude;
        std::optional<sf::Sprite> m_monster;
        std::optional<sf::Sprite> m_rock;
        std::vector<sf::IntRect> m_blueDudeFrames;
        std::vector<sf::IntRect> m_monsterFrames;
        std::vector<sf::IntRect> m_rockFrames;
        int m_currentBlueDudeFrame;
        int m_currentMonsterFrame;
        int m_currentRockFrame;
        float m_animationTimer;
        float m_frameTime;
        float m_rockTimer;
        bool m_rockFlying;
        sf::Vector2f m_rockVelocity;
        sf::Vector2f m_rockStartPos;
        sf::Vector2f m_rockTargetPos;

        bool m_isExitButtonSelected;
        bool m_isExitButtonPressed;
        bool m_isLevelsButtonSelected;
        bool m_isLevelsButtonPressed;

    public:
        MainMenu(std::shared_ptr<Context>& context);
        ~MainMenu();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;

    private:
        void InitAnimatedBackground();
        void UpdateAnimation(sf::Time deltaTime);


};