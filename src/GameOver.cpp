#include "GameOver.h"
#include "GamePlay.h"


GameOver::GameOver(std::shared_ptr<Context>& context, int score)
: m_context (context)
, m_isPlayButtonSelected (true), m_isPlayButtonPressed (false)
, m_isExitButtonSelected (false), m_isExitButtonPressed (false)
, m_scoreValue(score)
{

}


 GameOver::~GameOver()
 {

 }

void GameOver::Init()
{

    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);

    m_score = sf::Text(font, ("YOUR SCORE: " + std::to_string(m_scoreValue)), 50);

    m_gameTitle = sf::Text(font, "GAME OVER", 50);

    sf::FloatRect bounds = m_gameTitle->getLocalBounds();

    m_score->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f 
    ));

    m_score->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f - 250.f
    ));

     m_gameTitle->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    ));


    m_gameTitle->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f - 150.f
    ));


    m_playButton = sf::Text(font, "Retry", 30);

    bounds = m_playButton->getLocalBounds();

    m_playButton->setOrigin({
        bounds.position.x + bounds.size.x / 2.f ,
        bounds.position.y + bounds.size.y/2.f
    });

    m_playButton->setPosition({
        static_cast<float>(Config::SCREEN_WIDTH) / 2.f,
        static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 25.f,
    });


     m_exitButton = sf::Text(font, "Exit", 30);

    bounds = m_exitButton->getLocalBounds();

    m_exitButton->setOrigin({
        bounds.position.x + bounds.size.x / 2.f ,
        bounds.position.y + bounds.size.y/2.f
    });

    m_exitButton->setPosition({
        static_cast<float>(Config::SCREEN_WIDTH) / 2.f,
        static_cast<float>(Config::SCREEN_HEIGHT) / 2.f + 25.f,
    });

}


void GameOver::ProcessInput()
{
     while (const std::optional<sf::Event> event = m_context->m_window->pollEvent())
    {
        if(event->is<sf::Event::Closed>())
            m_context->m_window->close();
        
        else if (event->is<sf::Event::KeyPressed>())
        {
            const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
            switch (keyEvent->code)
            {
            case sf::Keyboard::Key::Up:
                if (!m_isPlayButtonSelected)
                {
                    m_isPlayButtonSelected = true;
                    m_isExitButtonSelected = false;
                }
                break;
            case sf::Keyboard::Key::Down:
                if (!m_isExitButtonSelected)
                {
                    m_isExitButtonSelected= true;
                    m_isPlayButtonSelected = false;
                }
                break;
            case sf::Keyboard::Key::Enter:
                m_isPlayButtonPressed = false;
                m_isExitButtonPressed = false;

                if (m_isPlayButtonSelected)
                {
                    m_isPlayButtonPressed = true;
                }
                if (m_isExitButtonSelected)
                {
                    m_isExitButtonPressed = true;
                }
            default:
                break;
           }
        }
        
    }
}


void GameOver::Update(sf::Time deltaTime)
{
    if (m_isPlayButtonSelected)
    {
        m_playButton->setFillColor(sf::Color::Yellow);
        m_exitButton->setFillColor(sf::Color::White);
    }
    else 
    {
        m_exitButton->setFillColor(sf::Color::Yellow);
        m_playButton->setFillColor(sf::Color::White);
    }

    if (m_isPlayButtonPressed)
    {
        m_context->m_states->Add(std::make_unique<GamePlay>(m_context), true);

    }

    if (m_isExitButtonPressed)
    {
        m_context->m_states->Add(std::make_unique<MainMenu>(m_context), true);
    } 
}


void GameOver::Draw() 
{
    m_context->m_window->clear(sf::Color::Red);
    m_context->m_window->draw(*m_score);
    m_context->m_window->draw(*m_gameTitle);
    m_context->m_window->draw(*m_playButton);
    m_context->m_window->draw(*m_exitButton);
    m_context->m_window->display();
}



