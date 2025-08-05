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
        std::optional<sf::Text> m_playButton;
        std::optional<sf::Text> m_exitButton;
        std::optional<sf::Sprite> m_background;

        bool m_isExitButtonSelected;
        bool m_isExitButtonPressed;
        bool m_isPlayButtonSelected;
        bool m_isPlayButtonPressed;

    public:
        MainMenu(std::shared_ptr<Context>& context);
        ~MainMenu();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;


};