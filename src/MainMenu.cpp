#include "MainMenu.h"
#include "Level1.h"
#include "LevelSelector.h"

MainMenu::MainMenu(std::shared_ptr<Context>& context)
: m_context {context}
, m_isLevelsButtonSelected {true}, m_isLevelsButtonPressed {false}
, m_isExitButtonSelected {false}, m_isExitButtonPressed {false}

{

}

MainMenu::~MainMenu()
{

}

void MainMenu::Init() 
{
    // Initialize main background using the level background
    const sf::Texture& mainBGTex = m_context->m_assets->getTexture(BACKGROUND);
    m_background = std::make_unique<sf::Sprite>(mainBGTex);

   
    sf::Vector2u texSize = mainBGTex.getSize();
    sf::Vector2u winSize = m_context->m_window->getSize();

    m_background->setScale({
        static_cast<float>(winSize.x) / texSize.x,
        static_cast<float>(winSize.y) / texSize.y
    });

    // Initialize floor
    const sf::Texture& groundTex = m_context->m_assets->getTexture(GROUND);
    m_floor = std::make_unique<sf::Sprite>(groundTex);
    

    sf::Vector2u groundTexSize = groundTex.getSize();
    m_floor->setScale({
        static_cast<float>(winSize.x) / groundTexSize.x,
        1.0f
    });
    m_floor->setPosition({0.f, static_cast<float>(winSize.y) - groundTexSize.y});


    // Create BlueDude
    const sf::Texture& dudeTex = m_context->m_assets->getTexture(BLUE_DUDE);
    const sf::Texture& dudeWalkTex = m_context->m_assets->getTexture(BLUE_DUDE_WALK);
    const sf::Texture& dudeJumpTex = m_context->m_assets->getTexture(BLUE_DUDE_JUMP);
    const sf::Texture& dudeThrowTex = m_context->m_assets->getTexture(BLUE_DUDE_THROW);
    const sf::Texture& dudeDeathTex = m_context->m_assets->getTexture(BLUE_DUDE_DEATH);
    const sf::Texture& rockTex = m_context->m_assets->getTexture(ROCK);
    
    m_blueDude = std::make_unique<BlueDude>(dudeTex, dudeWalkTex, dudeJumpTex, dudeThrowTex, dudeDeathTex, rockTex, 800);
    m_blueDude->SetPosition(100.f, 610.f);
    m_blueDude->GetSprite().scale({2.f, 2.f});

    // Create Monster
    const sf::Texture& monsterIdleTex = m_context->m_assets->getTexture(MONSTER_IDLE);
    const sf::Texture& monsterWalkTex = m_context->m_assets->getTexture(MONSTER_WALK);
    const sf::Texture& monsterAttack1Tex = m_context->m_assets->getTexture(MONSTER_ATTACK1);
    const sf::Texture& monsterAttack2Tex = m_context->m_assets->getTexture(MONSTER_ATTACK2);
    const sf::Texture& monsterHurtTex = m_context->m_assets->getTexture(MONSTER_HURT);
    const sf::Texture& monsterDeathTex = m_context->m_assets->getTexture(MONSTER_DEATH);
    
    m_monster = std::make_unique<Monster>(monsterIdleTex, monsterWalkTex, monsterAttack1Tex, 
                                         monsterAttack2Tex, monsterHurtTex, monsterDeathTex,
                                         700.f, 750.f);

    m_monster->GetSprite().setPosition({700.f, 450.f});
    m_monster->GetSprite().setScale({2.0f, 2.0f});
    m_monster->SetState(Monster::State::Walk);

    // Create rock sprite
    m_rock = std::make_unique<sf::Sprite>(rockTex);
    m_rock->setPosition({-100.f, -100.f}); // Start off-screen
    m_rock->setScale({4.f, 4.f});
    m_rock->setTextureRect(sf::IntRect({0, 0}, {8, 8}));

    // Initialize animation variables
    m_rockTimer = 0.f;
    m_rockFlying = false;

    // Set rock trajectory
    m_rockStartPos = {100.f, 500.f};
    m_rockTargetPos = {700.f, 800.f};
    m_rockVelocity = {(m_rockTargetPos.x - m_rockStartPos.x) / 60.f, -200.f}; // 60 frames to reach target


    const sf::Font& font = m_context->m_assets->getFont(MAIN_FONT);

    // Game Title
    m_gameTitle = std::make_unique<sf::Text>(sf::Text(font, "The Blue Dude", 30));
    m_gameTitle->setFont(font);
    m_gameTitle->setString("The Blue Dude");
    m_gameTitle->setFillColor(sf::Color::Red);
    
    sf::FloatRect bounds = m_gameTitle->getLocalBounds();

    m_gameTitle->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    ));


    m_gameTitle->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f - 300.f
    ));

    // Levels Button
    m_levelsButton = std::make_unique<sf::Text>(sf::Text(font, "Levels", 20));
    m_levelsButton->setFont(font);
    m_levelsButton->setString("Levels");

    sf::FloatRect levels_bounds = m_levelsButton->getLocalBounds();

    m_levelsButton->setOrigin(sf::Vector2f(
        levels_bounds.position.x + levels_bounds.size.x / 2.f,
        levels_bounds.position.y + levels_bounds.size.y / 2.f 
    ));

    m_levelsButton->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f
    ));

    // Exit Button
    m_exitButton = std::make_unique<sf::Text>(sf::Text(font, "Exit", 20));
    m_exitButton->setFont(font);
    m_exitButton->setString("Exit");
    
    sf::FloatRect exit_bounds = m_exitButton->getLocalBounds();

    m_exitButton->setOrigin(sf::Vector2f(
        exit_bounds.position.x + exit_bounds.size.x / 2.f,
        exit_bounds.position.y + exit_bounds.size.y / 2.f
    ));

    m_exitButton->setPosition(sf::Vector2f(
        m_context->m_window->getSize().x / 2.0f,
        m_context->m_window->getSize().y / 2.0f + 50.f
    ));
    
}

void MainMenu::UpdateAnimation(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();
    m_rockTimer += dt;

    // Update character animations using their built-in Update methods
    std::vector<sf::Sprite*> emptyPlatforms; // No platforms for menu animation
    m_blueDude->Update(dt, false, emptyPlatforms);
    m_monster->Update(dt);

    // Rock throwing animation cycle
    if (m_rockTimer >= 3.0f) 
    {
        if (m_monster->getState() == Monster::State::Walk)
            m_monster->Attack1();
        else
            m_monster->SetState(Monster::State::Walk);
        
        m_rockTimer = 0.f;
        m_rockFlying = true;
        m_rock->setPosition(m_rockStartPos);
    }

    if (m_rockFlying)
    {
        sf::Vector2f currentPos = m_rock->getPosition();
        currentPos += m_rockVelocity * dt;
        
        m_rockVelocity.y += 800.f * dt; // Gravity
        
        m_rock->setPosition(currentPos);
        
        // Check if rock is gone
        if (currentPos.x > m_rockTargetPos.x + 50.f || currentPos.y > 600.f)
        {
            m_rockFlying = false;
            m_rock->setPosition({-100.f, -100.f}); // Move off screen
            m_rockVelocity.y = -200.f; // Reset velocity
        }
    }
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
                if (m_isExitButtonSelected)
                {
                    m_isLevelsButtonSelected = true;
                    m_isExitButtonSelected = false;
                }
                break;
            case sf::Keyboard::Key::Down:
                if (m_isLevelsButtonSelected)
                {
                    m_isExitButtonSelected = true;
                    m_isLevelsButtonSelected = false;
                }
                break;
            case sf::Keyboard::Key::Enter:
                m_isLevelsButtonPressed = false;
                m_isExitButtonPressed = false;

                if (m_isLevelsButtonSelected)
                {
                    m_isLevelsButtonPressed = true;
                }
                if (m_isExitButtonSelected)
                {
                    m_isExitButtonPressed = true;
                    //m_context->m_window->close();
                }
            default:
                break;
           }
        }
        
    }
}


void MainMenu::Update(sf::Time deltaTime) 
{
    if (m_isLevelsButtonSelected)
    {
        m_levelsButton->setFillColor(sf::Color::Green);
        m_exitButton->setFillColor(sf::Color::Black);
    }
    else 
    {
        m_levelsButton->setFillColor(sf::Color::Black);
        m_exitButton->setFillColor(sf::Color::Green);
    }

    if (m_isLevelsButtonPressed)
    {
        m_isLevelsButtonPressed = false;
        m_context->m_states->Add(std::make_unique<LeaderBoard>(m_context), true);
    }

    if (m_isExitButtonPressed)
    {
        m_context->m_window->close();
    } 

    // Update background animation
    UpdateAnimation(deltaTime);
}


void MainMenu::Draw() 
{
    m_context->m_window->clear(sf::Color::Green);

    m_context->m_window->draw(*m_background);
    
    
    m_context->m_window->draw(*m_floor);
    
    //animated background characters
    m_blueDude->Draw(*m_context->m_window);
    m_monster->Draw(*m_context->m_window);
    
    if (m_rockFlying) {
        m_context->m_window->draw(*m_rock);
    }
    
    m_context->m_window->draw(*m_gameTitle);
    m_context->m_window->draw(*m_levelsButton);
    m_context->m_window->draw(*m_exitButton);
    m_context->m_window->display();
}
