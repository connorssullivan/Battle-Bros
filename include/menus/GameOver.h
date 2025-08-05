#pragma once


#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "State.h"
#include "Config.h"

class Context;

class GameOver : public Engine::State
{
    private:
        std::shared_ptr<Context> m_context;

        std::optional<sf::Text> m_gameTitle;
        std::optional<sf::Text> m_playButton;
        std::optional<sf::Text> m_exitButton;

        bool m_isExitButtonSelected;
        bool m_isExitButtonPressed;
        bool m_isPlayButtonSelected;
        bool m_isPlayButtonPressed;

    public:
        GameOver(std::shared_ptr<Context>& context);
        ~GameOver();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;

};

