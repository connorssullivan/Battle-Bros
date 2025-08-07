#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "State.h"
#include "Game.h"

class Context;

class LevelSelector : public Engine::State
{
private:
    std::shared_ptr<Context> m_context;
    
    std::optional<sf::Text> m_titleText;
    std::vector<std::unique_ptr<sf::Text>> m_levelTexts;
    std::vector<std::unique_ptr<sf::RectangleShape>> m_levelButtons;
    std::vector<bool> m_levelUnlocked;
    
    int m_selectedLevel;
    int m_totalLevels;
    
    sf::Color m_unlockedColor;
    sf::Color m_lockedColor;
    sf::Color m_selectedColor;
    sf::Color m_normalColor;

public:
    LevelSelector(std::shared_ptr<Context>& context);
    ~LevelSelector();

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Draw() override;
    void Pause() override;
    void Start() override;

private:
    void CreateLevelButtons();
    void UpdateSelection(int newSelection);
    void SelectLevel(int levelIndex);
};