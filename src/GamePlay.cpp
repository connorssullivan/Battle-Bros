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
    // These now work because AddTexture takes const std::string&
    std::string grassPath = "assets/textures/grass.png";
    m_context->m_assets->AddTexture(GRASS, grassPath, true);

    std::string foodPath = "assets/textures/food.png";
    m_context->m_assets->AddTexture(FOOD,  foodPath);

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






    // Get the grass and food texture 
    const sf::Texture& grassTex = m_context->m_assets->getTexture(GRASS);
    const sf::Texture& foodTex = m_context->m_assets->getTexture(FOOD);
    const sf::Texture& wallTex = m_context->m_assets->getTexture(WALL);

    for (int i=0; i < m_walls.size(); i++)
    {
        m_walls[i]  = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(WALL));
    }

    // Create the floor
    m_ground = std::make_unique<sf::Sprite>(m_context->m_assets->getTexture(GROUND));
    int level_width { 2000 }; // This is length of gornd
    m_ground->setTextureRect(sf::IntRect({0, 0}, {level_width, Config::WALL_THICKNESS}));
    m_ground->setPosition({0, Config::SCREEN_HEIGHT - Config::WALL_THICKNESS});


    // Emplace the sprite with that texture (required in SFML 3)
    m_grass.emplace(grassTex);
    /*m_grass->setScale({
            16.f / static_cast<float>(grassTex.getSize().x),
            16.f / static_cast<float>(grassTex.getSize().y)
        });*/
    m_food.emplace(foodTex);


    // Apply full viewport rect to the grass sprite
    m_grass->setTextureRect(
        m_context->m_window->getViewport(m_context->m_window->getDefaultView())
    );

    // Create the walls
    m_walls[0]->setTextureRect(sf::IntRect({0, 0}, {Config::SCREEN_WIDTH, Config::WALL_THICKNESS}));
    m_walls[0]->setPosition({0, 0}); // Top wall

    m_walls[1]->setTextureRect(sf::IntRect({0, 0}, {Config::WALL_THICKNESS, Config::SCREEN_HEIGHT}));
    m_walls[1]->setPosition({0, 0}); // Left wall
    
    m_walls[2]->setTextureRect(sf::IntRect({0, 0}, {Config::WALL_THICKNESS, Config::SCREEN_HEIGHT}));
    m_walls[2]->setPosition({static_cast<float>(level_width) - Config::WALL_THICKNESS, 0}); // Right wall

    // Create food
    m_food->setPosition({Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT/2});

    
    // Create BlueDude
    const sf::Texture& dudeTex = m_context->m_assets->getTexture(BLUE_DUDE);
    const sf::Texture& dudeWalkTex = m_context->m_assets->getTexture(BLUE_DUDE_WALK);
    const sf::Texture& dudeJumpTex = m_context->m_assets->getTexture(BLUE_DUDE_JUMP);
    const sf::Texture& rockTex = m_context->m_assets->getTexture(ROCK);
    const sf::Texture& dudeThrowTex = m_context->m_assets->getTexture(BLUE_DUDE_THROW);
    m_player = std::make_unique<BlueDude>(dudeTex, dudeWalkTex, dudeJumpTex, dudeThrowTex, rockTex);
    m_player->SetPosition(200, Config::SCREEN_HEIGHT - 32.f);


    m_scoreText = sf::Text(m_context->m_assets->getFont(MAIN_FONT), "Score: ", 30);
    m_scoreText->setString("Score: " + std::to_string(m_score));

    // Initialize camera
    m_camera = m_context->m_window->getDefaultView();
    m_cameraTarget = m_player->GetSprite().getPosition();
    
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
            std::cout << (m_player->getIsOnGround() ? "On Ground" : "Not On Ground") << "\n";
                if (m_player->getIsOnGround()) {
                    m_player->jump();
                }
                break;
            case sf::Keyboard::Key::Down:
               
                break;
            case sf::Keyboard::Key::Left:
                {
                    // Preserve Y velocity for jumping
                    sf::Vector2f currentVel = m_player->getVelocity();
                    m_player->setVelocity({-m_player->getSpeed(), currentVel.y});
                    m_player->setIsWalking(true);
                    m_player->GetSprite().setScale({-1.f, 1.f});
                }
                break;
            case sf::Keyboard::Key::Right:
                {
                    // Preserve Y velocity for jumping
                    sf::Vector2f currentVel = m_player->getVelocity();
                    m_player->setVelocity({m_player->getSpeed(), currentVel.y});
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
    if (!m_isPaused)
    {
        if(m_player->getWalking())
            m_player->Update(deltaTime.asSeconds(), true); 
        else   
            m_player->Update(deltaTime.asSeconds(), false); 
        
        // Update rock physics with collision detection
        std::vector<sf::Sprite*> platforms = getPlatforms();
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
    m_context->m_window->draw(*m_grass);

    m_context->m_window->draw(*m_ground);

    for (auto& wall : m_walls)
    {
        m_context->m_window->draw(*wall);
    }

    m_context->m_window->draw(*m_food);
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
    
    // Add food as a platform (optional)
    if (m_food.has_value())
    {
        platforms.push_back(&m_food.value());
    }
    
    return platforms;
}
