#include "Game.h"
#include "MainMenu.h"


Game::Game()
: m_context { std::make_shared<Context>() }
{

    m_context->m_window->create(sf::VideoMode({Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT}), "Battle Bros", sf::Style::Default);
    
    InitTextures();

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

void Game::InitTextures()
{
    // Initalize textures
    const std::string backgroundPath = "assets/textures/backgrounds/Background_01.png";
    m_context->m_assets->AddTexture(BACKGROUND, backgroundPath, true);


    const std::string wallPath = "assets/textures/wall.png";
    m_context->m_assets->AddTexture(WALL,  wallPath, true);

    const std::string groundPath = "assets/textures/platforms/ground.png";
    m_context->m_assets->AddTexture(GROUND,  groundPath, true);

    const std::string blueDudePath = "assets/textures/charecters/blue_dude/blue_dude_idle.png";
    m_context->m_assets->AddTexture(BLUE_DUDE, blueDudePath);

    const std::string blueDudeWalkPath = "assets/textures/charecters/blue_dude/blue_dude_walk.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_WALK, blueDudeWalkPath);

    const std::string blueDudeJumpPath = "assets/textures/charecters/blue_dude/blue_dude_jump.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_JUMP, blueDudeJumpPath);

    const std::string blueDudeThrowPath = "assets/textures/charecters/blue_dude/blue_dude_throw.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_THROW, blueDudeThrowPath);

    const std::string blueDudeDeathPath = "assets/textures/charecters/blue_dude/blue_dude_death.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_DEATH, blueDudeDeathPath);

    const std::string rockPath = "assets/textures/objects/rock.png";
    m_context->m_assets->AddTexture(ROCK, rockPath);

    const std::string brick1Path = "assets/textures/platforms/brick1.png";
    m_context->m_assets->AddTexture(BRICK1, brick1Path, true);

    const std::string coin1Path = "assets/textures/objects/Coin_01.png";
    m_context->m_assets->AddTexture(COIN_1, coin1Path);

    const std::string coin2Path = "assets/textures/objects/Coin_02.png";
    m_context->m_assets->AddTexture(COIN_2, coin2Path);

    const std::string coin3Path = "assets/textures/objects/Coin_03.png";
    m_context->m_assets->AddTexture(COIN_3, coin3Path);

    const std::string coin4Path = "assets/textures/objects/Coin_04.png";
    m_context->m_assets->AddTexture(COIN_4, coin4Path);

    const std::string coin5Path = "assets/textures/objects/Coin_05.png";
    m_context->m_assets->AddTexture(COIN_5, coin5Path);

    const std::string coin6Path = "assets/textures/objects/Coin_06.png";
    m_context->m_assets->AddTexture(COIN_6, coin6Path);

    //Monster textures
     std::string monsterIdlePath = "assets/textures/charecters/monster/Idle.png";
    m_context->m_assets->AddTexture(MONSTER_IDLE, monsterIdlePath);

    const std::string monsterWalkPath = "assets/textures/charecters/monster/Walk.png";
    m_context->m_assets->AddTexture(MONSTER_WALK, monsterWalkPath);

    const std::string monsterAttack1pathe = "assets/textures/charecters/monster/Attack1.png";
    m_context->m_assets->AddTexture(MONSTER_ATTACK1, monsterAttack1pathe);

    const std::string monsterAttack2Path = "assets/textures/charecters/monster/Attack2.png";
    m_context->m_assets->AddTexture(MONSTER_ATTACK2, monsterAttack2Path);

    const std::string monsterDeathPath = "assets/textures/charecters/monster/Death.png";
    m_context->m_assets->AddTexture(MONSTER_DEATH, monsterDeathPath);

    const std::string monsterHurtPath = "assets/textures/charecters/monster/Hurt.png";
    m_context->m_assets->AddTexture(MONSTER_HURT, monsterHurtPath);

    const std::string starPath = "assets/textures/objects/Star.png";
    m_context->m_assets->AddTexture(STAR, starPath);

    // Fonts
    const std::string fontPath = "assets/fonts/Bitcount_Grid_Double/BitcountGridDouble.ttf";
    m_context->m_assets->AddFont(MAIN_FONT, fontPath);
}
