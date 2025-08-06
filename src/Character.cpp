#include "Character.h"
#include <iostream>

Character::Character(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& rockText, int level_width)
: m_frameTime {0.15f}, m_elapsedTime {0.f}, m_currentFrame {0}, m_sprite {idleText}
, m_idleTexture {idleText}, m_walkTexture {walkText}, m_jumpText {jumpText}, m_throwTex {throwText}
, m_isWalking {false}, m_isJumping {false}
, m_velocity{0.f, 0.f}, m_rockVelocity{0.f, 0.f}, m_speed{Config::WALK_SPEED}
, m_gravity {Config::GRAVITY}, m_jumpStrength {Config::JUMP_STRENGTH}, m_isOnGround {true}
,m_rockTex(rockText), m_rock {rockText}, m_rockAmmo {1000}, m_rockState {RockState::None}, m_isThrow {false}, m_isThrowingAnimation {false}, m_throwAnimTime {0.f}, m_throwAnimDuration {0.25f}
, m_levelWidth {level_width}
{
    m_sprite.setOrigin({16.f, 16.f});
    
    // Initialize rock sprite
    m_rock.setOrigin({4.f, 4.f});  // Center the rock (8x8 texture)
    m_rock.setTextureRect(sf::IntRect({0, 0}, {8, 8}));  // Set correct texture rectangle
}



void Character::Update(float dt, bool isWalking)
{
    m_isWalking = isWalking;

    // Apply physics and movement 
    if (!m_isOnGround) {
        // Jumping/falling physics
        m_velocity.y += m_gravity * dt;
        m_sprite.move({m_velocity.x * dt, m_velocity.y * dt});

        // Check landing
        float groundLevel = Config::SCREEN_HEIGHT - 32.f; 
        if (m_sprite.getPosition().y >= groundLevel) {
            m_sprite.setPosition({m_sprite.getPosition().x, groundLevel});
            m_velocity.y = 0.f;
            m_isJumping = false;
            m_isOnGround = true;
        }
    }
    else if (isWalking) {
        // Walking on ground 
        m_sprite.move(m_velocity * dt);
    }
    

    // Handle animation frames
    m_elapsedTime += dt;
    if (m_elapsedTime >= m_frameTime)
    {
        if (m_isThrowingAnimation)
        {
            m_throwAnimTime += dt;
            
            // Animate throw
            if (m_currentFrame < m_throwFrames.size() - 1) 
            {
                m_elapsedTime = 0.f;
                m_currentFrame++;
                m_sprite.setTexture(m_throwTex);
                m_sprite.setTextureRect(m_throwFrames[m_currentFrame]);
            }

            // Stop throw animation after one cycle
            if (m_throwAnimTime >= m_throwAnimDuration)
            {
                m_isThrowingAnimation = false;
                m_currentFrame = 0;
            }
        }
        else
        {
            if (!m_isOnGround) {
                // Jump
                if (m_elapsedTime >= m_frameTime)
                {
                    m_elapsedTime = 0.f;
                    m_currentFrame = (m_currentFrame + 1) % m_jumpFrames.size();
                    m_sprite.setTexture(m_jumpText);
                    m_sprite.setTextureRect(m_jumpFrames[m_currentFrame]);
                }
            }
            else if (isWalking) {
                // Walk
                if (m_elapsedTime >= m_frameTime)
                {
                    m_elapsedTime = 0.f;
                    m_currentFrame = (m_currentFrame + 1) % m_walkFrames.size();
                    m_sprite.setTexture(m_walkTexture);
                    m_sprite.setTextureRect(m_walkFrames[m_currentFrame]);
                }
            }
            else {
                // Idle
                if (m_elapsedTime >= m_frameTime)
                {
                    m_elapsedTime = 0.f;
                    m_currentFrame = (m_currentFrame + 1) % m_idleFrames.size();
                    m_sprite.setTexture(m_idleTexture);
                    m_sprite.setTextureRect(m_idleFrames[m_currentFrame]);
                }
            }
        }
    }
}


void Character::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);

    if (m_rockState != RockState::None)
        window.draw(m_rock);
}

void Character::SetPosition(float x, float y)
{
    m_sprite.setPosition({x, y});

}

sf::Sprite& Character::GetSprite()
{
    return m_sprite;
}

// Actions
void Character::jump() 
{
    // std::cout << "Jump called\n";
    m_isJumping = true;
    m_isOnGround = false;
    m_velocity.y = -m_jumpStrength;  // Negative value to jump UP
}

void Character::throwRock()
{
    if (m_rockAmmo > 0 && m_rockState == RockState::None) 
    {
        m_rockState = RockState::Flying;
        m_rockAmmo--;

        // Trigger throw animation
        m_isThrowingAnimation = true;
        m_throwAnimTime = 0.f;
        m_currentFrame = 0; // Reset throw animation frame

        // Start rock from player position but slightly above to avoid immediate collision
        m_rock.setPosition(m_sprite.getPosition() + sf::Vector2f(0, -20));
        float direction = (m_sprite.getScale().x > 0) ? 1.f : -1.f;
        m_rockVelocity = {direction * Config::ROCK_SPEED, -200.f};
    
    }
}


// Collisions

bool Character::checkRockCollision(const sf::Sprite& otherSprite)
{

    sf::FloatRect rockBounds = m_rock.getGlobalBounds();
    sf::FloatRect otherBounds = otherSprite.getGlobalBounds();    
    
    sf::Vector2f rockPos = m_rock.getPosition();
    sf::Vector2f otherPos = otherSprite.getPosition();
    
    // Check if rock is within the other sprite's bounds
    return (rockPos.x >= otherPos.x - 8 && rockPos.x <= otherPos.x + 32 &&
            rockPos.y >= otherPos.y - 8 && rockPos.y <= otherPos.y + 32);
}

void Character::updateRockPhysics(float dt, const std::vector<sf::Sprite*>& platforms)
{
    if (m_rockState == RockState::Flying)
    {
        m_rockVelocity.y += m_gravity * dt;
        m_rock.move(m_rockVelocity * dt);

        auto rockBounds = m_rock.getGlobalBounds();

        for (auto* platform : platforms)
        {
            auto platformBounds = platform->getGlobalBounds();

            if (auto intersection = rockBounds.findIntersection(platformBounds))
            {

                // TODO: find better way to do this
                bool isHorizontalWall = platformBounds.size.y < platformBounds.size.x;
                
                if (isHorizontalWall)
                {
                    // For horizontal walls, only the bottom wall should support the rock
                    // Top wall should make the rock fall down
                    if (platformBounds.position.y > Config::SCREEN_HEIGHT / 2)
                    {
                        // This is the bottom wall: rock can land on it
                        float platformTop = platformBounds.position.y;

                        float rock_height {4.f};
                        m_rock.setPosition({
                            m_rock.getPosition().x,
                            platformTop - (rock_height/2)   
                        });
                        m_rockVelocity = {0.f, 0.f};
                        m_rockState = RockState::Landed;
                    }

                    else
                    {
                        m_rockVelocity.y = std::abs(m_rockVelocity.y) * 0.9f; // Bounce with reduced velocity
                        m_rockVelocity.x *= 0.8f; 
                    }
                }
                else
                {
                    // Vertical wall (left/right) - position rock on the side
                    float rockCenterX = m_rock.getPosition().x;
                    float wallLeft = platformBounds.position.x;
                    float wallRight = platformBounds.position.x + platformBounds.size.x;
                    

                    
                    // Determine which side of the wall the rock hit
                    if (rockCenterX < (wallLeft + wallRight) / 2)
                    {
                        // Rock hit left side of wall - position it right at the left edge
                        float newX = wallLeft; 

                        // Make sure it doesn't go outside the world bounds
                        newX = std::max(newX, 0.f); 
                        m_rock.setPosition({
                            newX,
                            m_rock.getPosition().y
                        });
                    }
                    else
                    {
                        // Rock hit right side of wall - position it right at the right edge
                        float newX = wallRight;  

                        // Make sure it doesn't go outside the world bounds
                        newX = std::min(newX, static_cast<float>(Config::SCREEN_WIDTH));  
                        m_rock.setPosition({
                            newX,
                            m_rock.getPosition().y
                        });
                    }
                    
                    m_rockVelocity = {0.f, 0.f};
                    m_rockState = RockState::Landed;
                }
                return;
            }
        }

        if (m_rock.getPosition().y > Config::SCREEN_HEIGHT + 50 ||
            m_rock.getPosition().x < -50 || m_rock.getPosition().x > m_levelWidth + 50)
        {
            m_rockState = RockState::None;
            m_rockVelocity = {0.f, 0.f};
        }
    }
    else if (m_rockState == RockState::Landed)
    {
        auto rockBounds = m_rock.getGlobalBounds();
        bool supported = false;

        for (auto* platform : platforms)
        {
            auto platformBounds = platform->getGlobalBounds();
            const float epsilon = 2.0f; // Allow tiny float error

            // Determine if this is a horizontal or vertical wall
            // TODO: Find better way to do this
            bool isHorizontalWall = platformBounds.size.y < platformBounds.size.x;
            
            if (isHorizontalWall)
            {
                // Only the bottom wall should support the rock
                if (platformBounds.position.y > Config::SCREEN_HEIGHT / 2)
                {
                    // Check if rock is sitting on top of bottom platform
                    float rockBottom = rockBounds.position.y + rockBounds.size.y;
                    float platformTop = platformBounds.position.y;

                    bool horizontallyOverlapping =
                        rockBounds.position.x + rockBounds.size.x > platformBounds.position.x &&
                        rockBounds.position.x < platformBounds.position.x + platformBounds.size.x;

                    if (horizontallyOverlapping &&
                        std::abs(rockBottom - platformTop) <= epsilon)
                    {
                        supported = true;
                        break;
                    }
                }
            }
            else
            {
                // Check if rock is stuck to the side of vertical wall
                float rockLeft = rockBounds.position.x;
                float rockRight = rockBounds.position.x + rockBounds.size.x;
                float wallLeft = platformBounds.position.x;
                float wallRight = platformBounds.position.x + platformBounds.size.x;

                bool verticallyOverlapping =
                    rockBounds.position.y + rockBounds.size.y > platformBounds.position.y &&
                    rockBounds.position.y < platformBounds.position.y + platformBounds.size.y;

                // Check if rock is touching left or right side of wall
                if (verticallyOverlapping &&
                    (std::abs(rockRight - wallLeft) <= epsilon || std::abs(rockLeft - wallRight) <= epsilon))
                {
                    supported = true;
                    break;
                }
            }
        }

        // If not supported, drop it again
        if (!supported)
        {
            m_rockState = RockState::Flying;
            m_rockVelocity.y = 50.f; // Small downward velocity so it continues falling
        }
    }


}

void Character::pickupRock()
{
    if (m_rockState == RockState::Landed)
    {
        auto rockBounds = m_rock.getGlobalBounds();
        auto playerBounds = m_sprite.getGlobalBounds();

        if (rockBounds.findIntersection(playerBounds))
        {
            m_rockAmmo++;
            m_rockState = RockState::None; // Remove rock from world
            std::cout << "Picked up rock! Ammo: " << m_rockAmmo << std::endl;
        }
    }
}


