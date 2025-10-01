#include "LevelSelector.h"
#include "Level1.h"
#include "MainMenu.h"

LevelSelector::LevelSelector(std::shared_ptr<Context>& context)
    : m_context{context}
    , m_selectedLevel{0}
    , m_totalLevels{5} // 5 levels total
    , m_unlockedColor{sf::Color::Green}   // Green
    , m_lockedColor{100, 100, 100, 255}     // Gray
    , m_selectedColor{sf::Color::Yellow}     // Yellow
    , m_normalColor{200, 200, 200, 255}     // Light gray
{
}

LevelSelector::~LevelSelector()
{
}

void LevelSelector::Init()
{
    m_levelUnlocked.resize(m_totalLevels, false);
    m_levelUnlocked[0] = true; // Only first level unlocked
    
    m_titleText = std::make_unique<sf::Text>(sf::Text(m_context->m_assets->getFont(MAIN_FONT), "Select Level", 48));
    m_titleText->setPosition({
        (Config::SCREEN_WIDTH - m_titleText->getGlobalBounds().size.x) / 2,
        50
    });
    
    CreateLevelButtons();
}

void LevelSelector::CreateLevelButtons()
{
    const float buttonWidth = 120;
    const float buttonHeight = 80;
    const float spacing = 20;
    const float startX = (Config::SCREEN_WIDTH - (m_totalLevels * (buttonWidth + spacing) - spacing)) / 2;
    const float startY = 200;
    
    for (int i = 0; i < m_totalLevels; ++i)
    {
        // Create button background
        auto button = std::make_unique<sf::RectangleShape>(sf::Vector2f(buttonWidth, buttonHeight));
        button->setPosition({
            startX + i * (buttonWidth + spacing),
            startY
        });
        
        // Set color based on unlock status
        if (m_levelUnlocked[i])
        {
            button->setFillColor(m_normalColor);
        }
        else
        {
            button->setFillColor(m_lockedColor);
        }
        
        button->setOutlineThickness(2);
        button->setOutlineColor(sf::Color::Black);
        
        m_levelButtons.push_back(std::move(button));
        
        // Create level text
        auto levelText = std::make_unique<sf::Text>(
            m_context->m_assets->getFont(MAIN_FONT), 
            "Level " + std::to_string(i + 1), 
            24
        );
        levelText->setFillColor(sf::Color::Black);
        
        // Center text on button
        sf::FloatRect textBounds = levelText->getGlobalBounds();
        levelText->setPosition({
            startX + i * (buttonWidth + spacing) + (buttonWidth - textBounds.size.x) / 2,
            startY + (buttonHeight - textBounds.size.y) / 2
        });
        
        m_levelTexts.push_back(std::move(levelText));
    }
    
    // Highlight first level as selected
    UpdateSelection(0);
}

void LevelSelector::UpdateSelection(int newSelection)
{
    // Ensure selection is within bounds
    if (newSelection < 0) newSelection = m_totalLevels - 1;
    if (newSelection >= m_totalLevels) newSelection = 0;
    
    // Only allow selection of unlocked levels
    while (!m_levelUnlocked[newSelection])
    {
        newSelection++;
        if (newSelection >= m_totalLevels) newSelection = 0;
    }
    
    m_selectedLevel = newSelection;
    
    // Update button colors
    for (int i = 0; i < m_levelButtons.size(); ++i)
    {
        if (i == m_selectedLevel)
        {
            m_levelButtons[i]->setFillColor(m_selectedColor);
        }
        else if (m_levelUnlocked[i])
        {
            m_levelButtons[i]->setFillColor(m_normalColor);
        }
        else
        {
            m_levelButtons[i]->setFillColor(m_lockedColor);
        }
    }
}

void LevelSelector::SelectLevel(int levelIndex)
{
    if (levelIndex >= 0 && levelIndex < m_totalLevels && m_levelUnlocked[levelIndex])
    {
        // Start the selected level
        m_context->m_states->Add(std::make_unique<Level1>(m_context));
    }
}

void LevelSelector::ProcessInput()
{
    while (const std::optional<sf::Event> event = m_context->m_window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            m_context->m_window->close();
        
        else if (event->is<sf::Event::KeyPressed>())
        {
            const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
            switch (keyEvent->code)
            {
                case sf::Keyboard::Key::Left:
                    UpdateSelection(m_selectedLevel - 1);
                    break;
                    
                case sf::Keyboard::Key::Right:
                    UpdateSelection(m_selectedLevel + 1);
                    break;
                    
                case sf::Keyboard::Key::Enter:
                case sf::Keyboard::Key::Space:
                    SelectLevel(m_selectedLevel);
                    break;
                    
                case sf::Keyboard::Key::Escape:
                    // Return to main menu
                    m_context->m_states->Add(std::make_unique<MainMenu>(m_context), true);
                    break;
                    
                default:
                    break;
            }
        }
    }
}

void LevelSelector::Update(sf::Time deltaTime)
{
    
}

void LevelSelector::Draw()
{
    m_context->m_window->clear(sf::Color(50, 50, 100)); // Dark blue background
    
    // Draw title
    m_context->m_window->draw(*m_titleText);
    
    // Draw level buttons
    for (auto& button : m_levelButtons)
    {
        m_context->m_window->draw(*button);
    }
    
    // Draw level text
    for (auto& text : m_levelTexts)
    {
        m_context->m_window->draw(*text);
    }
    
    m_context->m_window->display();
}

void LevelSelector::Pause()
{
    
}

void LevelSelector::Start()
{
    
} 