#pragma once
#include "Game.h"
#include "Config.h"
#include "GameRecords.h"
#include "GameOver.h"

#include <SFML/Graphics.hpp>
#include <cctype>
#include <iostream>


class Context;

class LeaderBoard : public Engine::State
{
    private:
        std::shared_ptr<Context> m_context;
        
        std::optional<sf::Text> m_titleText;
        std::vector<std::unique_ptr<sf::Text>> m_levelTexts;
        std::vector<std::unique_ptr<sf::RectangleShape>> m_levelButtons;
        std::vector<std::unique_ptr<sf::RectangleShape>> m_leaderBoxes;
        std::vector<bool> m_levelUnlocked;
        
        int m_selectedLevel;
        int m_totalLevels;
        
        sf::Color m_unlockedColor;
        sf::Color m_lockedColor;
        sf::Color m_selectedColor;
        sf::Color m_normalColor;

    public:
        LeaderBoard(std::shared_ptr<Context>& context);
        ~LeaderBoard();

        void Init() override;
        void ProcessInput() override;
        void Update(sf::Time deltaTime) override;
        void Draw() override;
        void Pause() override;
        void Start() override;

    private:
        void CreateLevelButtons();
        void CreateLevelLeaderBoard(int level);
        void UpdateSelection(int newSelection);
        void SelectLevel(int levelIndex);

};
