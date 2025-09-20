#include "LeaderBoard.h"
#include "Level1.h"



LeaderBoard::LeaderBoard(std::shared_ptr<Context>& context)
    : m_context{context}
    , m_selectedLevel{0}
    , m_totalLevels{5} // 5 levels total
    , m_unlockedColor{sf::Color::Green}   // Green
    , m_lockedColor{100, 100, 100, 255}     // Gray
    , m_selectedColor{sf::Color::Yellow}     // Bright red for testing
    , m_normalColor{200, 200, 200, 255}     // Light gray
{
    Init();
}

LeaderBoard::~LeaderBoard()
{
}

void LeaderBoard::Init()
{
    std::cout << "=== LeaderBoard::Init() called ===" << std::endl;
    
    // Clear existing data to prevent accumulation
    m_levelButtons.clear();
    m_levelTexts.clear();
    m_leaderBoxes.clear();
    m_leaderRankTexts.clear();
    m_leaderNameTexts.clear();
    m_leaderScoreTexts.clear();
    m_leaderDateTexts.clear();
    
    m_levelUnlocked.resize(m_totalLevels, false);
    m_levelUnlocked[0] = true; // Only first level unlocked
    
    m_titleText = sf::Text(m_context->m_assets->getFont(MAIN_FONT), "Select Level", 48);
    m_titleText->setPosition({
        (Config::SCREEN_WIDTH - m_titleText->getGlobalBounds().size.x) / 2,
        50
    });
    
    
    CreateLevelButtons();
    CreateLevelLeaderBoard(m_selectedLevel+1);
    
}

void LeaderBoard::CreateLevelLeaderBoard(int level)
{
    /*
        Create game records for the leaderbord, record slots will be placed vertically with the best score at the top.
    */

    // Clear existing leaderboard data
    m_leaderBoxes.clear();
    m_leaderRankTexts.clear();
    m_leaderNameTexts.clear();
    m_leaderScoreTexts.clear();
    m_leaderDateTexts.clear();

    std::vector<Records::GameRecord> records = Records::getTop5Records(level);

    const float boxWidth = Config::SCREEN_WIDTH * 0.8;
    const float boxHeight = 80;
    const float spacing = 20;

    // Center the boxes horizontally
    const float startX = (Config::SCREEN_WIDTH - boxWidth) / 2;
    const float startY = 300;
    
    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);

    // Create all the records for the desired level
    for (int i = 0; i < records.size(); i++)
    {
        const float boxY = startY + i * (boxHeight + spacing);
        
        // Create box background
        auto box = std::make_unique<sf::RectangleShape>(sf::Vector2f(boxWidth, boxHeight));
        box->setPosition({startX, boxY});
        box->setFillColor(sf::Color(40, 40, 40)); // Dark background
        box->setOutlineThickness(2);
        box->setOutlineColor(sf::Color::White);
        m_leaderBoxes.push_back(std::move(box));

        // Create rank text (1st, 2nd, etc.)
        auto rankText = std::make_unique<sf::Text>(font, std::to_string(i + 1), 24);
        rankText->setFillColor(sf::Color::Yellow);
        rankText->setPosition({startX + 20, boxY + 15});
        m_leaderRankTexts.push_back(std::move(rankText));

        // Create name text
        auto nameText = std::make_unique<sf::Text>(font, records[i].m_name, 20);
        nameText->setFillColor(sf::Color::White);
        nameText->setPosition({startX + 80, boxY + 10});
        m_leaderNameTexts.push_back(std::move(nameText));

        // Create score text
        auto scoreText = std::make_unique<sf::Text>(font, "Score: " + std::to_string(records[i].m_score), 20);
        scoreText->setFillColor(sf::Color::Green);
        scoreText->setPosition({startX + 80, boxY + 35});
        m_leaderScoreTexts.push_back(std::move(scoreText));

        // Create date text
        auto dateText = std::make_unique<sf::Text>(font, records[i].m_dateCompleted, 16);
        dateText->setFillColor(sf::Color(128, 128, 128)); // Gray color
        dateText->setPosition({startX + boxWidth - 150, boxY + 25});
        m_leaderDateTexts.push_back(std::move(dateText));
    }


}

void LeaderBoard::CreateLevelButtons()
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

void LeaderBoard::UpdateSelection(int newSelection)
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
    std::cout << "Total buttons in vector: " << m_levelButtons.size() << std::endl;
    std::cout << "m_totalLevels: " << m_totalLevels << std::endl;
    for (int i = 0; i < m_levelButtons.size(); ++i)
    {
        if (i == m_selectedLevel)
        {
            std::cout << "Setting button " << i << " to RED (selected)" << std::endl;
            m_levelButtons[i]->setFillColor(m_selectedColor);
        }
        else if (m_levelUnlocked[i])
        {
            std::cout << "Setting button " << i << " to NORMAL (unlocked)" << std::endl;
            m_levelButtons[i]->setFillColor(m_normalColor);
        }
        else
        {
            std::cout << "Setting button " << i << " to GRAY (locked)" << std::endl;
            m_levelButtons[i]->setFillColor(m_lockedColor);
        }
    }
    
    // Update leaderboard for new selection
    CreateLevelLeaderBoard(m_selectedLevel + 1);
}

void LeaderBoard::SelectLevel(int levelIndex)
{
    if (levelIndex >= 0 && levelIndex < m_totalLevels && m_levelUnlocked[levelIndex])
    {
        m_context->m_states->Add(std::make_unique<Level1>(m_context));
    }
}

void LeaderBoard::ProcessInput()
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

void LeaderBoard::Update(sf::Time deltaTime)
{
    
}

void LeaderBoard::Draw()
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

    // Draw leaderboard boxes
    for (const auto& box : m_leaderBoxes)
    {
        m_context->m_window->draw(*box);
    }
    
    // Draw leaderboard text elements
    for (const auto& text : m_leaderRankTexts)
    {
        m_context->m_window->draw(*text);
    }
    
    for (const auto& text : m_leaderNameTexts)
    {
        m_context->m_window->draw(*text);
    }
    
    for (const auto& text : m_leaderScoreTexts)
    {
        m_context->m_window->draw(*text);
    }
    
    for (const auto& text : m_leaderDateTexts)
    {
        m_context->m_window->draw(*text);
    }
    
    m_context->m_window->display();
}

void LeaderBoard::Pause()
{
    
}

void LeaderBoard::Start()
{
    
} 