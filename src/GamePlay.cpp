#include "GamePlay.h"

#include <string>
#include <stdlib.h>
#include <time.h>

#include "GameOver.h"
#include "PauseGame.h"



GamePlay::GamePlay(std::shared_ptr<Context>& context)
: m_context {context}
, m_elapsedTime {sf::Time::Zero}
, m_score {0}
, m_isPaused {false}
, m_cameraSmoothness {0.1f}  // Adjust this value (0.05f = very smooth, 0.2f = more responsive)
{
    srand(time(nullptr));

    if (!bgSound.openFromFile("assets/audio/bite.ogg")){
        std::cerr << "Failed to load bite \n";
    }else{
        bgSound.setVolume(100);
    }
}

GamePlay::~GamePlay()
{

}

void GamePlay::Init() 
{
    InitTextures();

    // Get textures 
    const sf::Texture& backgroundTex = m_context->m_assets->getTexture(BACKGROUND);
    const sf::Texture& wallTex = m_context->m_assets->getTexture(WALL);
    const sf::Texture& coin1 = m_context->m_assets->getTexture(COIN_1);
    const sf::Texture& coin2 = m_context->m_assets->getTexture(COIN_2);
    const sf::Texture& coin3 = m_context->m_assets->getTexture(COIN_3);
    const sf::Texture& coin4 = m_context->m_assets->getTexture(COIN_4);
    const sf::Texture& coin5 = m_context->m_assets->getTexture(COIN_5);
    const sf::Texture& coin6 = m_context->m_assets->getTexture(COIN_6);

    for (int i=0; i < m_walls.size(); i++)
    {
        m_walls[i]  = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(WALL));
    }

    // Create the floor
    m_ground = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(GROUND));
    int level_width { 3000 }; // Extended for Mario-style level
    m_ground->setTextureRect(sf::IntRect({0, 0}, {level_width, Config::WALL_THICKNESS}));
    m_ground->setPosition({0, Config::SCREEN_HEIGHT - Config::WALL_THICKNESS});

    // set up bricks - Mario-style level design
    const sf::Texture& brickTex = m_context->m_assets->getTexture(BRICK1);

    // Brick dimensions
    float scaleX = 32.f / brickTex.getSize().x;  // Made bigger for more Mario-like feel
    float scaleY = 32.f / brickTex.getSize().y;

    float brickWidth  = brickTex.getSize().x * scaleX;
    float brickHeight = brickTex.getSize().y * scaleY;

    // === GROUND LEVEL PLATFORMS ===
    // Low platform near start
    for (int i = 0; i < 4; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({200.f + i * brickWidth, 650.f}); 
        m_bricks.push_back(std::move(brick));

        if (i == 2)
        {
            std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
            coin->GetSprite().setScale({scaleX, scaleY});
            coin->GetSprite().setPosition({200.f + i * brickWidth, 650.f - 30});

            m_coins.push_back(std::move(coin));
        }
    }

    // Mid-level platform 
    for (int i = 0; i < 5; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({300.f + i * brickWidth, 550.f}); 
        m_bricks.push_back(std::move(brick));
    }

    // === FLOATING PLATFORMS ===
    // High floating platform (original one, moved up)
    for (int i = 0; i < 3; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({600.f + i * brickWidth, 350.f});
        m_bricks.push_back(std::move(brick));
    }

    // Long floating platform
    for (int i = 0; i < 6; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({800.f + i * brickWidth, 450.f});
        m_bricks.push_back(std::move(brick));
    }

    // Small floating platforms
    for (int i = 0; i < 2; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1100.f + i * brickWidth, 380.f});
        m_bricks.push_back(std::move(brick));
    }

    // === SINGLE BLOCKS (like question blocks) ===
    // Single blocks scattered around
    auto singleBlock1 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock1->setScale({scaleX, scaleY});
    singleBlock1->setPosition({400.f, 450.f});
    m_bricks.push_back(std::move(singleBlock1));

    auto singleBlock2 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock2->setScale({scaleX, scaleY});
    singleBlock2->setPosition({450.f, 350.f});
    m_bricks.push_back(std::move(singleBlock2));

    auto singleBlock3 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock3->setScale({scaleX, scaleY});
    singleBlock3->setPosition({750.f, 300.f});
    m_bricks.push_back(std::move(singleBlock3));

    // === STAIRCASE ===
    // Staircase going up
    for (int step = 0; step < 4; ++step) {
        for (int block = 0; block <= step; ++block) {
            auto brick = std::make_unique<sf::Sprite>(brickTex);
            brick->setScale({scaleX, scaleY});
            brick->setPosition({1300.f + step * brickWidth, 650.f - block * brickHeight});
            m_bricks.push_back(std::move(brick));

            // Put coin on top
            if (block == step)
            {
                std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
                coin->GetSprite().setScale({scaleX, scaleY});
                coin->GetSprite().setPosition({1300.f + step * brickWidth, (650.f - block * brickHeight) - 30});

                m_coins.push_back(std::move(coin));
            }
        }
    }

    // === HIGH PLATFORMS ===
    // Very high platform for advanced jumping
    for (int i = 0; i < 4; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1500.f + i * brickWidth, 200.f});
        m_bricks.push_back(std::move(brick));
    }

    // st the background
    m_background= std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(BACKGROUND));


    // Apply full viewport rect to the grass sprite
    m_background->setScale({static_cast<float>(level_width), Config::SCREEN_HEIGHT});

    // Create the walls
    m_walls[0]->setTextureRect(sf::IntRect({0, 0}, {level_width, Config::WALL_THICKNESS}));
    m_walls[0]->setPosition({0, 0}); // Top wall

    m_walls[1]->setTextureRect(sf::IntRect({0, 0}, {Config::WALL_THICKNESS, Config::SCREEN_HEIGHT}));
    m_walls[1]->setPosition({0, 0}); // Left wall
    
    m_walls[2]->setTextureRect(sf::IntRect({0, 0}, {Config::WALL_THICKNESS, Config::SCREEN_HEIGHT}));
    m_walls[2]->setPosition({static_cast<float>(level_width) - Config::WALL_THICKNESS, 0}); // Right wall

    
    // Create BlueDude
    const sf::Texture& dudeTex = m_context->m_assets->getTexture(BLUE_DUDE);
    const sf::Texture& dudeWalkTex = m_context->m_assets->getTexture(BLUE_DUDE_WALK);
    const sf::Texture& dudeJumpTex = m_context->m_assets->getTexture(BLUE_DUDE_JUMP);
    const sf::Texture& rockTex = m_context->m_assets->getTexture(ROCK);
    const sf::Texture& dudeThrowTex = m_context->m_assets->getTexture(BLUE_DUDE_THROW);
    m_player = std::make_unique<BlueDude>(dudeTex, dudeWalkTex, dudeJumpTex, dudeThrowTex, rockTex, level_width);
    m_player->SetPosition(200, Config::SCREEN_HEIGHT - 32.f);


    for (int i=0; i < 5; i++)
    {
        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({1100.f + i * brickWidth, 380.f});

        m_coins.push_back(std::move(coin));
        
    }


    m_scoreText = sf::Text(m_context->m_assets->getFont(MAIN_FONT), "Score: ", 30);
    m_scoreText->setString("Score: " + std::to_string(m_score));

    // Initialize camera
    m_camera = m_context->m_window->getDefaultView();
    m_cameraTarget = m_player->GetSprite().getPosition();
    
}


void GamePlay::InitTextures()
{
    // These now work because AddTexture takes const std::string&
    std::string backgroundPath = "assets/textures/backgrounds/Background_01.png";
    m_context->m_assets->AddTexture(BACKGROUND, backgroundPath, true);


    std::string wallPath = "assets/textures/wall.png";
    m_context->m_assets->AddTexture(WALL,  wallPath, true);

    std::string groundPath = "assets/textures/platforms/ground.png";
    m_context->m_assets->AddTexture(GROUND,  groundPath, true);

    std::string blueDudePath = "assets/textures/charecters/blue_dude/blue_dude_idle.png";
    m_context->m_assets->AddTexture(BLUE_DUDE, blueDudePath);

    std::string blueDudeWalkPath = "assets/textures/charecters/blue_dude/blue_dude_walk.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_WALK, blueDudeWalkPath);

    std::string blueDudeJumpPath = "assets/textures/charecters/blue_dude/blue_dude_jump.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_JUMP, blueDudeJumpPath);

    std::string blueDudeThrowPath = "assets/textures/charecters/blue_dude/blue_dude_throw.png";
    m_context->m_assets->AddTexture(BLUE_DUDE_THROW, blueDudeThrowPath);

    std::string rockPath = "assets/textures/objects/rock.png";
    m_context->m_assets->AddTexture(ROCK, rockPath);

    std::string brick1Path = "assets/textures/platforms/brick1.png";
    m_context->m_assets->AddTexture(BRICK1, brick1Path, true);

    std::string coin1Path = "assets/textures/objects/Coin_01.png";
    m_context->m_assets->AddTexture(COIN_1, coin1Path);

    std::string coin2Path = "assets/textures/objects/Coin_02.png";
    m_context->m_assets->AddTexture(COIN_2, coin2Path);

    std::string coin3Path = "assets/textures/objects/Coin_03.png";
    m_context->m_assets->AddTexture(COIN_3, coin3Path);

    std::string coin4Path = "assets/textures/objects/Coin_04.png";
    m_context->m_assets->AddTexture(COIN_4, coin4Path);

    std::string coin5Path = "assets/textures/objects/Coin_05.png";
    m_context->m_assets->AddTexture(COIN_5, coin5Path);

    std::string coin6Path = "assets/textures/objects/Coin_06.png";
    m_context->m_assets->AddTexture(COIN_6, coin6Path);
}


void GamePlay::ProcessInput() 
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
                if (m_player->getIsOnGround()) {
                    m_player->jump();
                }
                break;
            case sf::Keyboard::Key::Down:
               
                break;
            case sf::Keyboard::Key::Left:
                {
                    // Preserve Y velocity for jumping

                    float dirX = -1.f;
                    if (!m_player->checkPlayerCollision(getPlatforms(),dirX))
                    {
                         m_player->setVelocity({dirX * m_player->getSpeed(), m_player->getVelocity().y});
                    }
                    m_player->setIsWalking(true);
                    m_player->GetSprite().setScale({-1.f, 1.f});
                }
                break;
            case sf::Keyboard::Key::Right:
                {
                    // Preserve Y velocity for jumping
                    float dirX = 1.f;
                    if (!m_player->checkPlayerCollision(getPlatforms(),dirX))
                    {
                         m_player->setVelocity({dirX * m_player->getSpeed(), m_player->getVelocity().y});
                    }
                    m_player->setIsWalking(true);
                    m_player->GetSprite().setScale({1.f, 1.f});
                }
                break;
            case sf::Keyboard::Key::Space:
                m_player->throwRock();
                break;
            case sf::Keyboard::Key::Escape:
                Pause();
                m_context->m_states->Add(std::make_unique<PauseGame>(m_context));
                break;
            default:
                break;
           }
           
        }
        else if (event->is<sf::Event::KeyReleased>())
        {
            const auto& keyEvent = event->getIf<sf::Event::KeyReleased>();
            switch (keyEvent->code)
            {
                case sf::Keyboard::Key::Up:
                case sf::Keyboard::Key::Down:
                    // Don't reset velocity for Up/Down keys to allow jump physics
                    break;
                case sf::Keyboard::Key::Left:
                case sf::Keyboard::Key::Right:
                    {
                        // Only reset horizontal velocity, preserve vertical velocity for jumping
                        sf::Vector2f currentVel = m_player->getVelocity();
                        m_player->setVelocity({0.f, currentVel.y});
                        m_player->setIsWalking(false);
                    }
                    break;
                default:
                    break;
            }
        }
        
    }
}

void GamePlay::Update(sf::Time deltaTime) 
{
    for (auto& coin : m_coins)
    {
        coin->Update();
        // Check if player collects the coin
        if (m_player->GetSprite().getGlobalBounds().findIntersection(coin->GetSprite().getGlobalBounds()))
        {
            m_score += 10; // Increment score by 10 for each coin collected
            m_scoreText->setString("Score: " + std::to_string(m_score));
            coin->GetSprite().setPosition({-100.f, -100.f}); // Move coin off-screen after collection
        }
    }
    if (!m_isPaused)
    {
        // Get platforms once for all collision checks
        std::vector<sf::Sprite*> platforms = getPlatforms();
        
        if(m_player->getWalking())
            m_player->Update(deltaTime.asSeconds(), true, platforms); 
        else   
            m_player->Update(deltaTime.asSeconds(), false, platforms); 
        
        // Update rock physics with collision detection
        if (m_player->getRockState() == Character::RockState::Flying)
            m_player->updateRockPhysics(deltaTime.asSeconds(), platforms);
        else if (m_player->getRockState() == Character::RockState::Landed)
        {
            m_player->updateRockPhysics(deltaTime.asSeconds(), platforms);
            m_player->pickupRock();
        }
        
        // Update camera to follow player
        UpdateCamera();
    }
    else
    {
        // When paused, don't update anything but keep the camera stable
        // Don't call UpdateCamera() when paused
    }
}


void GamePlay::Draw()
{
    m_context->m_window->clear();
    
    // Draw world elements (these will move with camera)
    m_context->m_window->draw(*m_background);

    m_context->m_window->draw(*m_ground);

    for (auto& wall : m_walls)
    {
        m_context->m_window->draw(*wall);
    }

    // Draw all bricks
    for (auto& brick : m_bricks) {
        m_context->m_window->draw(*brick);
    }

    // Draw all coins
    for (auto& coin : m_coins) {
        coin->Draw(*m_context->m_window);
    }
    
    m_player->Draw(*m_context->m_window);
    
    // Draw UI elements (these stay fixed on screen)
    m_context->m_window->setView(m_context->m_window->getDefaultView());  // Reset to default view for UI
    m_context->m_window->draw(*m_scoreText);
    
    m_context->m_window->display();
}

void GamePlay::Pause() 
{
    m_isPaused = true;
}

void GamePlay::Start()
{
    m_isPaused = false;
}

void GamePlay::UpdateCamera()
{
    sf::Vector2f playerPos = m_player->GetSprite().getPosition();
    
    m_cameraTarget = playerPos;
    
    // move camera towards target
    sf::Vector2f currentCenter = m_camera.getCenter();
    sf::Vector2f newCenter = currentCenter + (m_cameraTarget - currentCenter) * m_cameraSmoothness;
    
    m_camera.setCenter(newCenter);
    
    m_context->m_window->setView(m_camera);
}

std::vector<sf::Sprite*> GamePlay::getPlatforms()
{
    std::vector<sf::Sprite*> platforms;
    
    // Add walls as platforms
    for (auto& wall : m_walls)
    {    
        if (wall)
            platforms.push_back(wall.get());
    }

    if (m_ground)
        platforms.push_back(m_ground.get());
    
    // Add all bricks as platforms
    for (auto& brick : m_bricks) {
        if (brick) platforms.push_back(brick.get());
    }
    
    return platforms;
}
