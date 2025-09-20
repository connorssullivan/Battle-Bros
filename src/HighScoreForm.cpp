#include "HighScoreForm.h"

HighScoreForm::HighScoreForm(std::shared_ptr<Context>& context, int score, int level)
    : m_context(context), m_score(score), m_showCursor(true), m_cursorTimer(sf::Time::Zero), m_level {level}
{
}

HighScoreForm::~HighScoreForm()
{
}

void HighScoreForm::Init()
{
    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);
    
    // Title text
    m_title = std::make_unique<sf::Text>(font, "NEW HIGH SCORE: " + std::to_string(m_score), 48);
    m_title->setFillColor(sf::Color::Yellow);
    m_title->setPosition({
        (Config::SCREEN_WIDTH - m_title->getGlobalBounds().size.x) / 2,
        50
    });
    
    // Prompt text
    m_promptText = std::make_unique<sf::Text>(font, "Enter your name:", 32);
    m_promptText->setFillColor(sf::Color::White);
    m_promptText->setPosition({
        (Config::SCREEN_WIDTH - m_promptText->getGlobalBounds().size.x) / 2,
        150
    });
    
    // Input box
    m_inputBox = std::make_unique<sf::RectangleShape>(sf::Vector2f(300, 50));
    m_inputBox->setFillColor(sf::Color(50, 50, 50));
    m_inputBox->setOutlineColor(sf::Color::White);
    m_inputBox->setOutlineThickness(2);
    m_inputBox->setPosition({
        (Config::SCREEN_WIDTH - 300) / 2,
        220
    });
    
    // Name display text
    m_nameDisplay = std::make_unique<sf::Text>(font, "", 28);
    m_nameDisplay->setFillColor(sf::Color::White);
    m_nameDisplay->setPosition({
        m_inputBox->getPosition().x + 10,
        m_inputBox->getPosition().y + 10
    });
    
    // Instruction text
    m_instructionText = std::make_unique<sf::Text>(font, "Press ENTER to save | ESC to cancel", 20);
    m_instructionText->setFillColor(sf::Color(128, 128, 128)); // Gray color
    m_instructionText->setPosition({
        (Config::SCREEN_WIDTH - m_instructionText->getGlobalBounds().size.x) / 2,
        320
    });
}

void HighScoreForm::ProcessInput()
{
    while (auto event = m_context->m_window->pollEvent())
    {
        if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
        {
            handleTextInput(textEntered->unicode);
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Enter && !m_playerName.empty())
            {
                saveScore();
                // TODO: Transition to next state leaderboard
                std::vector<Records::GameRecord> top_10_records = Records::getTop10Records(m_level);
                // Save new record
                if (top_10_records.size() < 10)
                {
                    Records::GameRecord gr(m_level, m_score, Records::getCurrentDate(), m_playerName);
                    Records::saveGameRecord(gr);
                }

                //Replace Old Record
                else if (top_10_records[9].m_score <= m_score)
                {
                    Records::GameRecord gr(m_level, m_score, Records::getCurrentDate(), m_playerName);
                    Records::replaceGameRecord(top_10_records[9], gr);
                }
                m_context->m_states->Add(std::make_unique<MainMenu>(m_context), true);
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                // Cancel and go back
                m_context->m_states->Add(std::make_unique<GameOver>(m_context, m_score), true);
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
            {
                if (!m_playerName.empty())
                {
                    m_playerName.pop_back();
                    m_nameDisplay->setString(m_playerName);
                }
            }
        }
        else if (event->is<sf::Event::Closed>())
        {
            m_context->m_window->close();
        }
    }
}

void HighScoreForm::Update(sf::Time deltaTime)
{
    // Handle blinking cursor
    m_cursorTimer += deltaTime;
    if (m_cursorTimer >= sf::seconds(0.5f))
    {
        m_showCursor = !m_showCursor;
        m_cursorTimer = sf::Time::Zero;
        
        // Update display with or without cursor
        std::string displayText = m_playerName;
        if (m_showCursor && m_playerName.length() < MAX_NAME_LENGTH)
        {
            displayText += "_";
        }
        m_nameDisplay->setString(displayText);
    }
}

void HighScoreForm::Draw()
{
    m_context->m_window->clear(sf::Color::Black);
    
    // Draw UI elements
    m_context->m_window->draw(*m_title);
    m_context->m_window->draw(*m_promptText);
    m_context->m_window->draw(*m_inputBox);
    m_context->m_window->draw(*m_nameDisplay);
    m_context->m_window->draw(*m_instructionText);
    
    m_context->m_window->display();
}

void HighScoreForm::handleTextInput(char32_t unicode)
{
    // Only accept printable characters and limit length
    if (unicode >= 32 && unicode < 127 && m_playerName.length() < MAX_NAME_LENGTH)
    {
        // Convert char32_t to char (safe for ASCII)
        char character = static_cast<char>(unicode);
        
        // Only allow letters, numbers, and spaces
        if (std::isalnum(character) || character == ' ')
        {
            m_playerName += character;
            
            // Update display immediately
            std::string displayText = m_playerName;
            if (m_showCursor && m_playerName.length() < MAX_NAME_LENGTH)
            {
                displayText += "_";
            }
            m_nameDisplay->setString(displayText);
        }
    }
}

void HighScoreForm::saveScore()
{
    // TODO: Implement saving using your GameRecord system
    // GameRecord record;
    // record.m_name = m_playerName;
    // record.m_score = m_score;
    // record.m_level = /* current level */;
    // record.m_dateCompleted = getCurrentDate();
    // saveGameRecord(record);
    
    // For now, just print to console
    std::cout << "Saving score: " << m_playerName << " - " << m_score << std::endl;
}