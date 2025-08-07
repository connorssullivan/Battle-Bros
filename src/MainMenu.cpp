#include "MainMenu.h"
#include "GamePlay.h"
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
    std::string bgPath = "assets/textures/backgrounds/Background_01.png";
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

    // Initialize floor
    std::string groundPath = "assets/textures/platforms/ground.png";
    m_context->m_assets->AddTexture(GROUND, groundPath);
    const sf::Texture& groundTex = m_context->m_assets->getTexture(GROUND);
    m_floor.emplace(groundTex);
    
    // Scale and position the floor
    sf::Vector2u groundTexSize = groundTex.getSize();
    m_floor->setScale({
        static_cast<float>(winSize.x) / groundTexSize.x,
        1.0f
    });
    m_floor->setPosition({0.f, static_cast<float>(winSize.y) - groundTexSize.y});

    // Initialize animated background elements
    InitAnimatedBackground();

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
        m_context->m_window->getSize().y / 2.0f - 100.f
    ));

    // Levels Button
    m_levelsButton = sf::Text(font, "Levels", 20);
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
        m_context->m_window->getSize().y / 2.0f + 50.f
    ));
    
}

void MainMenu::InitAnimatedBackground()
{
    // Load textures for animated characters
    std::string blueDudePath = "assets/textures/charecters/blue_dude/blue_dude_idle.png";
    m_context->m_assets->AddTexture(BLUE_DUDE, blueDudePath);
    const sf::Texture& blueDudeTex = m_context->m_assets->getTexture(BLUE_DUDE);
    m_blueDude.emplace(blueDudeTex);

    std::string monsterPath = "assets/textures/charecters/monster/Idle.png";
    m_context->m_assets->AddTexture(MONSTER_IDLE, monsterPath);
    const sf::Texture& monsterTex = m_context->m_assets->getTexture(MONSTER_IDLE);
    m_monster.emplace(monsterTex);

    std::string rockPath = "assets/textures/objects/rock.png";
    m_context->m_assets->AddTexture(ROCK, rockPath);
    const sf::Texture& rockTex = m_context->m_assets->getTexture(ROCK);
    m_rock.emplace(rockTex);

    // Set up BlueDude animation frames (4 frames, each 32x32)
    for (int i = 0; i < 4; i++) {
        m_blueDudeFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));
    }

    // Set up Monster animation frames (4 frames, each 96x96)
    for (int i = 0; i < 4; i++) {
        m_monsterFrames.push_back(sf::IntRect({i * 96, 0}, {96, 96}));
    }

    // Set up Rock animation frames (simple single frame)
    m_rockFrames.push_back(sf::IntRect({0, 0}, {8, 8}));

    // Initialize animation variables
    m_currentBlueDudeFrame = 0;
    m_currentMonsterFrame = 0;
    m_currentRockFrame = 0;
    m_animationTimer = 0.f;
    m_frameTime = 0.2f;
    m_rockTimer = 0.f;
    m_rockFlying = false;

    // Position characters on the floor
    m_blueDude->setPosition({100.f, 500.f});
    m_blueDude->setScale({3.f, 3.f});
    m_blueDude->setTextureRect(m_blueDudeFrames[0]);

    m_monster->setPosition({700.f, 500.f});
    m_monster->setScale({2.f, 2.f});
    m_monster->setTextureRect(m_monsterFrames[0]);

    m_rock->setPosition({-100.f, -100.f}); // Start off-screen
    m_rock->setScale({4.f, 4.f});
    m_rock->setTextureRect(m_rockFrames[0]);

    // Set rock trajectory
    m_rockStartPos = {100.f, 500.f};
    m_rockTargetPos = {700.f, 500.f};
    m_rockVelocity = {(m_rockTargetPos.x - m_rockStartPos.x) / 60.f, -200.f}; // 60 frames to reach target
}

void MainMenu::UpdateAnimation(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();
    m_animationTimer += dt;
    m_rockTimer += dt;

    // Update character animations
    if (m_animationTimer >= m_frameTime)
    {
        m_animationTimer = 0.f;
        
        // Update BlueDude animation
        m_currentBlueDudeFrame = (m_currentBlueDudeFrame + 1) % m_blueDudeFrames.size();
        m_blueDude->setTextureRect(m_blueDudeFrames[m_currentBlueDudeFrame]);
        
        // Update Monster animation
        m_currentMonsterFrame = (m_currentMonsterFrame + 1) % m_monsterFrames.size();
        m_monster->setTextureRect(m_monsterFrames[m_currentMonsterFrame]);
    }

    // Rock throwing animation cycle
    if (m_rockTimer >= 3.0f) // Throw rock every 3 seconds
    {
        m_rockTimer = 0.f;
        m_rockFlying = true;
        m_rock->setPosition(m_rockStartPos);
    }

    // Update rock physics when flying
    if (m_rockFlying)
    {
        sf::Vector2f currentPos = m_rock->getPosition();
        currentPos += m_rockVelocity * dt;
        
        // Add gravity to rock
        m_rockVelocity.y += 800.f * dt; // Gravity
        
        m_rock->setPosition(currentPos);
        
        // Check if rock hit monster or went off screen
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
        m_context->m_states->Add(std::make_unique<LevelSelector>(m_context), true);
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
    
    // Draw floor
    if (m_floor) {
        m_context->m_window->draw(*m_floor);
    }
    
    // Draw animated background characters
    if (m_blueDude) {
        m_context->m_window->draw(*m_blueDude);
    }
    if (m_monster) {
        m_context->m_window->draw(*m_monster);
    }
    if (m_rock && m_rockFlying) {
        m_context->m_window->draw(*m_rock);
    }
    
    m_context->m_window->draw(*m_gameTitle);
    m_context->m_window->draw(*m_levelsButton);
    m_context->m_window->draw(*m_exitButton);
    m_context->m_window->display();
}
