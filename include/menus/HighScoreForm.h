#pragma once

#include "Game.h"
#include "Config.h"
#include "GameRecords.h"
#include "GameOver.h"

#include <SFML/Graphics.hpp>
#include <cctype>
#include <iostream>


class Context;

class HighScoreForm : public Engine::State
{
    private:
        std::unique_ptr<sf::Text> m_title;
        std::unique_ptr<sf::Text> m_promptText;
        std::unique_ptr<sf::Text> m_nameDisplay;
        std::unique_ptr<sf::Text> m_instructionText;
        std::unique_ptr<sf::RectangleShape> m_inputBox;
        std::shared_ptr<Context> m_context;

        std::string m_playerName;
        int m_score;
        int m_level;
        bool m_showCursor;
        sf::Time m_cursorTimer;


        static const size_t MAX_NAME_LENGTH = 10;

    public:
        HighScoreForm(std::shared_ptr<Context>& context, int score, int level);
        ~HighScoreForm();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;
    
    private: 
        void handleTextInput(char32_t unicode);
        void saveScore();

};
