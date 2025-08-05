#include "Game.h"
#include "MainMenu.h"


Game::Game()
: m_context { std::make_shared<Context>() }
{

    m_context->m_window->create(sf::VideoMode({Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT}), "Pup Chomp", sf::Style::Default);
    
    // Set framerate limit 
    m_context->m_window->setFramerateLimit(60);

    // Load music
    if (!bgMusic.openFromFile("assets/audio/music1.ogg"))
    {
        std::cerr <<"Error loading music\n";
    }else {
        bgMusic.setLooping(true); // loop forever
        bgMusic.setVolume(50); // optional: volume between 0 and 100
        bgMusic.play();
    }

    
    m_context->m_states->Add(std::make_unique<MainMenu>(m_context));

}

Game::~Game()
{

}

void Game::Run()
{
    sf::Clock clock;
    sf::Time timeSinceLastFrame { sf::Time::Zero };

    while (m_context->m_window->isOpen())
    {
        timeSinceLastFrame += clock.restart();

        while (timeSinceLastFrame > TIME_PER_FRAME) 
        {
            timeSinceLastFrame -= TIME_PER_FRAME;

            m_context->m_states->ProcessStateChanged();
            m_context->m_states->GetCurrent()->ProcessInput();
            m_context->m_states->GetCurrent()->Update(TIME_PER_FRAME);
            m_context->m_states->GetCurrent()->Draw();

        }
       
        
        
        
    }
}