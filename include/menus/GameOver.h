#pragma once


#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "State.h"
#include "Config.h"

class Context;

class GameOver : public Engine::State
{
    private:
        int m_scoreValue;
        bool m_isExitButtonSelected;
        bool m_isExitButtonPressed;
        bool m_isPlayButtonSelected;
        bool m_isPlayButtonPressed;
        std::shared_ptr<Context> m_context;

        std::unique_ptr<sf::Text> m_gameTitle;
        std::unique_ptr<sf::Text> m_playButton;
        std::unique_ptr<sf::Text> m_exitButton;
        std::unique_ptr<sf::Text> m_score;

    public:
        GameOver(std::shared_ptr<Context>& context, int score);
        ~GameOver();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;

};

