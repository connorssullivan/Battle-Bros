#include "PauseGame.h"
#include "GamePlay.h"


PauseGame::PauseGame(std::shared_ptr<Context>& context)
: m_context {context}
, m_isPlayButtonSelected {true}, m_isPlayButtonPressed {false}
, m_isExitButtonSelected {false}, m_isExitButtonPressed {false}
{

}

PauseGame::~PauseGame()
{

}

void PauseGame::Init() 
{

    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);

    // Game Title
    m_gameTitle = sf::Text(font, "Pause", 30);
    m_gameTitle->setFont(font);

    
    sf::FloatRect bounds = m_gameTitle->getLocalBounds();

    m_gameTitle->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    ));


    m_gameTitle->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f - 150.f
    ));

    // Play Button
    m_playButton = sf::Text(font, "Resume", 20);
    m_playButton->setFont(font);

    sf::FloatRect play_bounds = m_playButton->getLocalBounds();
    

    m_playButton->setOrigin(sf::Vector2f(
        play_bounds.position.x + play_bounds.size.x / 2.f,
        play_bounds.position.y + play_bounds.size.y / 2.f 
    ));


    m_playButton->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f - 25.f
    ));

    // Exit Button
    m_exitButton = sf::Text(font, "Exit", 20);
    m_exitButton->setFont(font);
    
    sf::FloatRect exit_bounds = m_exitButton->getLocalBounds();

    m_exitButton->setOrigin(sf::Vector2f(
        exit_bounds.position.x + exit_bounds.size.x / 2.f,
        exit_bounds.position.y + exit_bounds.size.y / 2.f
    ));


    m_exitButton->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f + 25.f
    ));
    
}


void PauseGame::ProcessInput() 
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
                break;
            case sf::Keyboard::Key::Escape:
                m_context->m_states->PopCurrent();
                break;
            default:
                break;
           }
        }
        
    }
}


void PauseGame::Update(sf::Time deltaTime) 
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
        // Resume
        m_context->m_states->PopCurrent();

    }

    if (m_isExitButtonPressed)
    {
        //TODO: Make go back to main menu
        m_context->m_states->Add(std::make_unique<MainMenu>(m_context), true);
       //m_context->m_window->close();
    } 
}


void PauseGame::Draw() 
{
    // Set the default view for UI elements
    m_context->m_window->setView(m_context->m_window->getDefaultView());
    
    // Draw a semi-transparent overlay
    //sf::RectangleShape overlay;
    //overlay.setSize(sf::Vector2f(m_context->m_window->getSize()));
    //overlay.setFillColor(sf::Color(0, 0, 0, 128));
    //m_context->m_window->draw(overlay);
    
    m_context->m_window->draw(*m_gameTitle);
    m_context->m_window->draw(*m_playButton);
    m_context->m_window->draw(*m_exitButton);
    m_context->m_window->display();
}
