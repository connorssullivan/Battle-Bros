#include "MainMenu.h"
#include "GamePlay.h"

MainMenu::MainMenu(std::shared_ptr<Context>& context)
: m_context {context}
, m_isPlayButtonSelected {true}, m_isPlayButtonPressed {false}
, m_isExitButtonSelected {false}, m_isExitButtonPressed {false}

{

}

MainMenu::~MainMenu()
{

}

void MainMenu::Init() 
{
    //Initalize mane backgroung 
    std::string bgPath = "assets/textures/gunnie.png";
    m_context->m_assets->AddTexture(MAIN_BACKGROUND, bgPath);
    const sf::Texture& mainBGTex = m_context->m_assets->getTexture(MAIN_BACKGROUND);
    m_background.emplace(mainBGTex);

    // Scale the background to fit the window size
    sf::Vector2u texSize = mainBGTex.getSize();
    sf::Vector2u winSize = m_context->m_window->getSize();

    m_background->setScale({
        static_cast<float>(winSize.x) / texSize.x,
        static_cast<float>(winSize.y) / texSize.y
    });

    std::string fontPath = "assets/fonts/Bitcount_Grid_Double/BitcountGridDouble.ttf";
    m_context->m_assets->AddFont(MAIN_FONT, fontPath);

    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);

    // Game Title
    m_gameTitle = sf::Text(font, "Main Menu", 30);
    m_gameTitle->setFont(font);
    m_gameTitle->setString("Pup Chomp");
    m_gameTitle->setFillColor(sf::Color::Red);
    
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
    m_playButton = sf::Text(font, "Play", 20);
    m_playButton->setFont(font);
    m_playButton->setString("Play");

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
    m_exitButton->setString("Exit");
    
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


void MainMenu::ProcessInput() 
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


void MainMenu::Update(sf::Time deltaTime) 
{
    if (m_isPlayButtonSelected)
    {
        m_playButton->setFillColor(sf::Color::Green);
        m_exitButton->setFillColor(sf::Color::Black);
    }
    else 
    {
        m_exitButton->setFillColor(sf::Color::Green);
        m_playButton->setFillColor(sf::Color::Black);
    }

    if (m_isPlayButtonPressed)
    {
        m_isPlayButtonPressed = false; 
        m_context->m_states->Add(std::make_unique<GamePlay>(m_context), true);

    }

    if (m_isExitButtonPressed)
    {
        m_context->m_window->close();
    } 
}


void MainMenu::Draw() 
{
    m_context->m_window->clear(sf::Color::Green);

    m_context->m_window->draw(*m_background);
    m_context->m_window->draw(*m_gameTitle);
    m_context->m_window->draw(*m_playButton);
    m_context->m_window->draw(*m_exitButton);
    m_context->m_window->display();
}
