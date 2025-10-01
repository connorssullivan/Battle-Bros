#include "Level1.h"

#include <string>
#include <stdlib.h>
#include <time.h>

#include "GameOver.h"
#include "PauseGame.h"



Level1::Level1(std::shared_ptr<Context>& context)
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

Level1::~Level1()
{

}

void Level1::Init() 
{

    // Get textures 
    const sf::Texture& backgroundTex = m_context->m_assets->getTexture(BACKGROUND);
    const sf::Texture& wallTex = m_context->m_assets->getTexture(WALL);
    const sf::Texture& coin1 = m_context->m_assets->getTexture(COIN_1);
    const sf::Texture& coin2 = m_context->m_assets->getTexture(COIN_2);
    const sf::Texture& coin3 = m_context->m_assets->getTexture(COIN_3);
    const sf::Texture& coin4 = m_context->m_assets->getTexture(COIN_4);
    const sf::Texture& coin5 = m_context->m_assets->getTexture(COIN_5);
    const sf::Texture& coin6 = m_context->m_assets->getTexture(COIN_6);
    const sf::Texture& rockTex = m_context->m_assets->getTexture(ROCK);


    // Create walls
    for (int i=0; i < m_walls.size(); i++)
    {
        m_walls[i]  = std::make_unique<sf::Sprite>(wallTex);
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
    for (int i = 0; i < 4; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({200.f + i * brickWidth, 650.f}); 
        m_bricks.push_back(std::move(brick));

        // Make a coin
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
    for (int i = 0; i < 2; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1100.f + i * brickWidth, 380.f});
        m_bricks.push_back(std::move(brick));

        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({1100.f + i * brickWidth, 380.f - 30});
        m_coins.push_back(std::move(coin));
    }

    // === SINGLE BLOCKS (like question blocks) ===
    // Single blocks scattered around
    auto singleBlock1 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock1->setScale({scaleX, scaleY});
    singleBlock1->setPosition({400.f, 450.f});
    m_bricks.push_back(std::move(singleBlock1));
    {
        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({450.f, 350.f - 30});
        m_coins.push_back(std::move(coin));
    }

    auto singleBlock2 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock2->setScale({scaleX, scaleY});
    singleBlock2->setPosition({450.f, 350.f});
    m_bricks.push_back(std::move(singleBlock2));
    {
        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({450.f, 350.f - 30});
        m_coins.push_back(std::move(coin));
    }

    auto singleBlock3 = std::make_unique<sf::Sprite>(brickTex);
    singleBlock3->setScale({scaleX, scaleY});
    singleBlock3->setPosition({750.f, 300.f});
    m_bricks.push_back(std::move(singleBlock3));

    {
        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({750.f, 300.f - 30});
        m_coins.push_back(std::move(coin));
    }

    // === STAIRCASE ===
    // Staircase going up
    // TODO: These stair cases can be usefull, make them their own object
    for (int step = 0; step < 4; ++step) 
    {
        for (int block = 0; block <= step; ++block) 
        {
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

    for (int step = 0; step < 6; ++step) 
    {
        for (int block = 0; block <= step; ++block) 
        {
            auto brick = std::make_unique<sf::Sprite>(brickTex);
            brick->setScale({scaleX, scaleY});
            brick->setPosition({1600.f + step * brickWidth, 500.f - block * brickHeight});
            m_bricks.push_back(std::move(brick));

            // Put coin on top
            if (block == step)
            {
                std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
                coin->GetSprite().setScale({scaleX, scaleY});
                coin->GetSprite().setPosition({1600.f + step * brickWidth, (500.f - block * brickHeight) - 30});

                m_coins.push_back(std::move(coin));

                if (step == 3)
                {
                    auto brick = std::make_unique<sf::Sprite>(brickTex);
                    brick->setScale({scaleX, scaleY});
                    brick->setPosition({1600.f + step * brickWidth, (float)(400.f - block * (brickHeight*1.5))});
                    m_bricks.push_back(std::move(brick));
                    
                    for (int i = 1; i < 5; i++)
                    {
                        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
                        coin->GetSprite().setScale({scaleX, scaleY});
                        coin->GetSprite().setPosition({1600.f + step * brickWidth, (float)((400.f - block * (brickHeight*1.5)) - (30*i))});

                        m_coins.push_back(std::move(coin));
                    }
                }
            }

            

        }

    }

    

    // === MONSTER ===
    const sf::Texture& monsterIdleTex = m_context->m_assets->getTexture(MONSTER_IDLE);
    const sf::Texture& monsterWalkTex = m_context->m_assets->getTexture(MONSTER_WALK);
    const sf::Texture& monsterAttack1Tex = m_context->m_assets->getTexture(MONSTER_ATTACK1);
    const sf::Texture& monsterAttack2Tex = m_context->m_assets->getTexture(MONSTER_ATTACK2);
    const sf::Texture& monsterHurtTex = m_context->m_assets->getTexture(MONSTER_HURT);
    const sf::Texture& monsterDeathTex = m_context->m_assets->getTexture(MONSTER_DEATH);
    
    // Create 5 monsters at different locations
    std::vector<std::pair<float, float>> monsterPositions = {
        {1450.f, 565.f},  
        {350.f, 360.f},   // Moved up (lower Y = higher position)
        
    };
    
    std::vector<std::pair<float, float>> monsterBounds = {
        {1400.f, 1600.f}, 
        {350.f, 400.f},   // Patrol bounds for the ground platform (covers the 4-brick platform)
        
    };
    
    for (int i = 0; i < 2; ++i) 
    {
        auto monster = std::make_unique<Monster>(monsterIdleTex, monsterWalkTex, monsterAttack1Tex, 
                                                monsterAttack2Tex, monsterHurtTex, monsterDeathTex,
                                                monsterBounds[i].first, monsterBounds[i].second);
        
        monster->GetSprite().setPosition(sf::Vector2f(monsterPositions[i].first, monsterPositions[i].second));
        monster->GetSprite().setScale({2.0f, 2.0f});
        monster->SetState(Monster::State::Walk);
        
        m_monsters.push_back(std::move(monster));
    }

    // === HIGH PLATFORMS ===
    // Very high platform for advanced jumping
    for (int i = 0; i < 4; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1500.f + i * brickWidth, 200.f});
        m_bricks.push_back(std::move(brick));

        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({1500.f + i * brickWidth, 200.f - 30});

        m_coins.push_back(std::move(coin));
    }

    // === EXTENDED LEVEL SECTIONS ===
    
    // High platform section
    for (int i = 0; i < 3; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1700.f + i * brickWidth, 150.f});
        m_bricks.push_back(std::move(brick));

        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({1700.f + i * brickWidth, 150.f - 30});
        m_coins.push_back(std::move(coin));
    }

    // Mid-level floating platforms
    for (int i = 0; i < 4; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({1900.f + i * brickWidth, 350.f});
        m_bricks.push_back(std::move(brick));

        if (i == 1 || i == 3) // Coins on alternating platforms
        {
            std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
            coin->GetSprite().setScale({scaleX, scaleY});
            coin->GetSprite().setPosition({1900.f + i * brickWidth, 350.f - 30});
            m_coins.push_back(std::move(coin));
        }
    }

    // Low platform section
    for (int i = 0; i < 5; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({2100.f + i * brickWidth, 500.f});
        m_bricks.push_back(std::move(brick));

        if (i % 2 == 0) // Coins on every other platform
        {
            std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
            coin->GetSprite().setScale({scaleX, scaleY});
            coin->GetSprite().setPosition({2100.f + i * brickWidth, 500.f - 30});
            m_coins.push_back(std::move(coin));
        }
    }

    // Zigzag platforms
    for (int i = 0; i < 6; ++i) 
    {
        float yPos = (i % 2 == 0) ? 350.f : 450.f;
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({2300.f + i * brickWidth, yPos});
        m_bricks.push_back(std::move(brick));

        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({2300.f + i * brickWidth, yPos - 50});
        m_coins.push_back(std::move(coin));
    }

    // Final challenge 
    for (int i = 0; i < 4; ++i) {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({2600.f + i * brickWidth, 250.f});
        m_bricks.push_back(std::move(brick));

        if (i == 0 || i == 3) // Coins on first and last platforms
        {
            std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
            coin->GetSprite().setScale({scaleX, scaleY});
            coin->GetSprite().setPosition({2600.f + i * brickWidth, 250.f - 30});
            m_coins.push_back(std::move(coin));
        }
    }

    // Ground-level platforms near the end
    for (int i = 0; i < 3; ++i) 
    {
        auto brick = std::make_unique<sf::Sprite>(brickTex);
        brick->setScale({scaleX, scaleY});
        brick->setPosition({2800.f + i * brickWidth, 550.f});
        m_bricks.push_back(std::move(brick));

        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({2800.f + i * brickWidth, 550.f - 30});
        m_coins.push_back(std::move(coin));
    }

    // Set up the background
    m_background = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(BACKGROUND));
    
    sf::Vector2u textureSize = m_context->m_assets->getTexture(BACKGROUND).getSize();
    
    // Scale to cover the full level width
    float bgScaleX = static_cast<float>(level_width) / textureSize.x;
    float bgScaleY = static_cast<float>(Config::SCREEN_HEIGHT) / textureSize.y;
    
    float bgScale = std::max(bgScaleX, bgScaleY);
    m_background->setScale({bgScale, bgScale});
    
    // Position the background to cover the full level
    m_background->setPosition({0, 0});
    
    // Create a second background for repeat (If needed)
    m_background2 = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(BACKGROUND));
    m_background2->setScale({bgScale, bgScale});
    m_background2->setPosition({textureSize.x * bgScale, 0}); // Position it right after the first one

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
    const sf::Texture& dudeThrowTex = m_context->m_assets->getTexture(BLUE_DUDE_THROW);
    const sf::Texture& dudeDeathTex = m_context->m_assets->getTexture(BLUE_DUDE_DEATH);
    m_player = std::make_unique<BlueDude>(dudeTex, dudeWalkTex, dudeJumpTex, dudeThrowTex, dudeDeathTex, rockTex, level_width);
    m_player->SetPosition(200, Config::SCREEN_HEIGHT - 32.f);


    // Make Star
    const sf::Texture& starTex = m_context->m_assets->getTexture(STAR);
    m_star = std::make_unique<Star>(starTex);
    m_star->GetSprite().setPosition({2800, 600});


    // Make some coins
    for (int i=0; i < 5; i++)
    {
        std::unique_ptr coin = std::make_unique<Coin>(coin1,coin2,coin3,coin4,coin5,coin6);
        coin->GetSprite().setScale({scaleX, scaleY});
        coin->GetSprite().setPosition({1100.f + i * brickWidth, 380.f - 30});

        m_coins.push_back(std::move(coin));
        
    }

    // Initalize score score and rock
    m_scoreText = std::make_unique<sf::Text>(m_context->m_assets->getFont(MAIN_FONT), "Score: ", 30);
    m_scoreText->setString("Score: " + std::to_string(m_score));
    m_scoreText->setPosition({10.f, 20.f});

    m_rock = std::make_unique<sf::Sprite>(rockTex);
    m_rockAmmo = std::make_unique<sf::Text>(m_context->m_assets->getFont(MAIN_FONT), "1x", 20);
    m_rock->setScale({5.f, 5.f,});
    const float uiPadding = 15.f;
    {
        sf::FloatRect rb = m_rock->getGlobalBounds();
        m_rock->setPosition({static_cast<float>(Config::SCREEN_WIDTH) - rb.size.x - uiPadding, uiPadding});
        m_rockAmmo->setPosition({static_cast<float>(Config::SCREEN_WIDTH) - rb.size.x - uiPadding - 25, 25.f});
    }
    

    // Initialize camera
    m_camera = m_context->m_window->getDefaultView();
    m_cameraTarget = m_player->GetSprite().getPosition();
    
}


void Level1::ProcessInput() 
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

void Level1::Update(sf::Time deltaTime) 
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

        //if (checkGotStar())
        checkGotStar();

        
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
        
        // Update monsters
        for (auto& monster : m_monsters) {
            if (monster) {
                monster->Update(deltaTime.asSeconds());
                CheckMonsterHit();

                // Check if monster attacks player
                if (checkMonsterAttack(monster.get()) && monster->getState() != Monster::State::Death) 
                {
                    // TODO: Monster Attack
                    m_player->kill();
                    if (m_player->getDidDeathEnd())
                    {
                        m_context->m_states->Add(std::make_unique<GameOver>(m_context, m_score), true);
                    }
                }
            }
        }

        // Update UI rock tint 
        if (m_player->getRockState() != Character::RockState::None)
        {
            m_rock->setColor(sf::Color(160, 160, 160, 160));
            m_rockAmmo->setString("0x");
        }
        else
        {
            m_rock->setColor(sf::Color(255, 255, 255, 255));
            m_rockAmmo->setString("1x");
        }
        
        
        // Update camera to follow player
        UpdateCamera();
    }
    else
    {
       
        // Don't call UpdateCamera() when paused
    }
}


void Level1::Draw()
{
    m_context->m_window->clear();
    
    // Set the camera view for world elements
    m_context->m_window->setView(m_camera);
    
    // Draw backgrounds (these move with camera)
    m_context->m_window->draw(*m_background);
    m_context->m_window->draw(*m_background2);
    
    // Draw world elements (these will move with camera)
    m_context->m_window->draw(*m_ground);

    for (auto& wall : m_walls)
    {
        m_context->m_window->draw(*wall);
    }

    // Draw all bricks
    for (auto& brick : m_bricks) {
        m_context->m_window->draw(*brick);
    }

    
    for (auto& coin : m_coins) {
        coin->Draw(*m_context->m_window);
    }
    
    if (m_star)
        m_star->Draw(*m_context->m_window);
    
    // Draw all monsters
    for (auto& monster : m_monsters) {
        if (monster) {
            monster->Draw(*m_context->m_window);
        }
    }
    
    
    m_player->Draw(*m_context->m_window);
    
    // Draw Fixed UI elements 
    m_context->m_window->setView(m_context->m_window->getDefaultView());  // Reset to default view for UI
    m_context->m_window->draw(*m_scoreText);
    m_context->m_window->draw(*m_rock);
    m_context->m_window->draw(*m_rockAmmo);
    
    m_context->m_window->display();
}

void Level1::Pause() 
{
    m_isPaused = true;
}

void Level1::Start()
{
    m_isPaused = false;
}

void Level1::UpdateCamera()
{
    sf::Vector2f playerPos = m_player->GetSprite().getPosition();
    
    m_cameraTarget = playerPos;
    
    // move camera towards target
    sf::Vector2f currentCenter = m_camera.getCenter();
    sf::Vector2f newCenter = currentCenter + (m_cameraTarget - currentCenter) * m_cameraSmoothness;
    
    // Clamp camera to level bounds
    float halfWidth = Config::SCREEN_WIDTH / 2.0f;
    float halfHeight = Config::SCREEN_HEIGHT / 2.0f;
    int level_width = 3000; // Same as defined in Init()
    
    newCenter.x = std::max(halfWidth, std::min(level_width - halfWidth, newCenter.x));
    newCenter.y = std::max(halfHeight, std::min(Config::SCREEN_HEIGHT - halfHeight, newCenter.y));
    
    m_camera.setCenter(newCenter);
    
    // Update background positions for seamless repeating
    if (m_background && m_background2) {
        sf::Vector2f bgOffset = (newCenter - sf::Vector2f(Config::SCREEN_WIDTH / 2.0f, Config::SCREEN_HEIGHT / 2.0f)) * 0.1f;
        sf::Vector2u textureSize = m_context->m_assets->getTexture(BACKGROUND).getSize();
        float bgScaleX = static_cast<float>(3000) / textureSize.x; // level_width value
        float bgScaleY = static_cast<float>(Config::SCREEN_HEIGHT) / textureSize.y;
        float bgScale = std::max(bgScaleX, bgScaleY);
        float bgWidth = textureSize.x * bgScale;
        
        // Calculate positions for seamless repeating
        float bg1X = bgOffset.x;
        float bg2X = bg1X + bgWidth;
        
        // Wrap backgrounds when they go off screen
        if (bg1X + bgWidth < 0) 
        {
            bg1X += bgWidth * 2;
            bg2X += bgWidth * 2;
        } 
        
        else if (bg2X < 0) 
        {
            bg2X += bgWidth * 2;
        }
        
        m_background->setPosition({bg1X, bgOffset.y - 300}); // Move up by 300 pixels
        m_background2->setPosition({bg2X, bgOffset.y - 300}); // Move up by 300 pixels
    }
    
    m_context->m_window->setView(m_camera);
}

std::vector<sf::Sprite*> Level1::getPlatforms()
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

bool Level1::checkMonsterAttack(Monster* monster)
{
    if (!monster || monster->getState() == Monster::State::Death || monster->getState() == Monster::State::Attack1 || monster->getState() == Monster::State::Attack2) 
        return false;

    // Get bounds
    sf::FloatRect monsterBounds = monster->GetSprite().getGlobalBounds();
    sf::FloatRect playerBounds = m_player->GetSprite().getGlobalBounds();

    // Use monster's bottom position instead of center for more realistic attack detection
    sf::Vector2f monsterAttackPoint = {monsterBounds.position.x + monsterBounds.size.x / 2.f, 
                                       monsterBounds.position.y + monsterBounds.size.y};

    
    sf::Vector2f playerCenter = playerBounds.position + (playerBounds.size / 2.f);

    // Calculate horizontal and vertical distances
    float dx = monsterAttackPoint.x - playerCenter.x;
    float dy = monsterAttackPoint.y - playerCenter.y;
    
    // Define elliptical attack area (taller than wide)
    float attackRadiusX = 80.f;  // Horizontal radius (narrower)
    float attackRadiusY = 40.f; // Vertical radius (taller)
    
    // Check if player is within elliptical attack area
    float normalizedDistance = (dx * dx) / (attackRadiusX * attackRadiusX) + 
                              (dy * dy) / (attackRadiusY * attackRadiusY);
    
    if (normalizedDistance < 1.0f) 
    {

        // If monster is on right 
        if (dx > 0 && !monster->GetIsFaceingRight())
        {
            monster->Flip();
        }

        // If monster is on left
        if (dx < 0 && monster->GetIsFaceingRight())
        {
            monster->Flip();
        }

        monster->Attack1();
        return true;
    }

    return false;
}

void Level1::CheckMonsterHit()
{
    // Only check for collision when rock is flying
    if (m_player->getRockState() != Character::RockState::Flying) 
    {
        return;
    }
    
    sf::FloatRect rockBounds = m_player->getRock().getGlobalBounds();
    
    // Check collision with all monsters
    for (auto& monster : m_monsters)
    {
        if (monster && monster->getState() != Monster::State::Death) 
        {
            sf::FloatRect monsterBounds = monster->GetSprite().getGlobalBounds();
            
            sf::Vector2f monsterCenter = monsterBounds.position + (monsterBounds.size / 2.f);
            sf::Vector2f rockCenter = rockBounds.position + (rockBounds.size / 2.f);

            float dx = monsterCenter.x - rockCenter.x;
            float dy = monsterCenter.y - rockCenter.y;
            
            // Define elliptical attack area (taller than wide)
            float attackRadiusX = 60.f;  
            float attackRadiusY = 60.f;

            float normalizedDistance = (dx * dx) / (attackRadiusX * attackRadiusX) + 
                                      (dy * dy) / (attackRadiusY * attackRadiusY);

            // Exit after hitting one monster
            if (normalizedDistance < 1.0f) {
                //std::cout << "Monster hit by rock!" << std::endl;
                monster->SetState(Monster::State::Death);
                m_player->resetRock();
                return; 
            }
        }
    }
}

bool Level1::checkGotStar()
{
    sf::FloatRect playerBounds = m_player->GetSprite().getGlobalBounds();
    sf::FloatRect starBounds = m_star->GetSprite().getGlobalBounds();

    sf::Vector2f playerCenter = playerBounds.position + (playerBounds.size / 2.f);
    sf::Vector2f starCenter = starBounds.position + (starBounds.size / 2.f);

    float dx = playerCenter.x - starCenter.x;
    float dy = playerCenter.y - starCenter.y;

    float hitRadiusX = 60.f;  
    float hitRadiusY = 60.f;

    float normalizedDistance = (dx * dx) / (hitRadiusX * hitRadiusX) + 
                              (dy * dy) / (hitRadiusY * hitRadiusY);

    if (normalizedDistance < 1.0f) {
        std::cout << "Player got the star!" << std::endl;
        // Kill all monsters when player gets the star
        for (auto& monster : m_monsters) {
            if (monster) {
                monster->SetState(Monster::State::Death);
            }
        }
        m_score += 100;
        int level {1};
        std::cout << "Getting Game Records\n";
        std::vector<Records::GameRecord> records = Records::getTop5Records(level);
        std::cout << records.size();
        if (records.size() < 10 || records[9].m_score < m_score)
            m_context->m_states->Add(std::make_unique<HighScoreForm>(m_context, m_score, level), true);
        else 
            m_context->m_states->Add(std::make_unique<GameOver>(m_context, m_score), true);
        
        return true;
    }

    return false;
}