#include "Character.h"
#include <iostream>

Character::Character(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& rockText)
: m_frameTime {0.15f}, m_elapsedTime {0.f}, m_currentFrame {0}, m_sprite {idleText}
, m_idleTexture {idleText}, m_walkTexture {walkText}, m_jumpText {jumpText}, m_throwTex {throwText}
, m_isWalking {false}, m_isJumping {false}
, m_velocity{0.f, 0.f}, m_rockVelocity{0.f, 0.f}, m_speed{Config::WALK_SPEED}
, m_gravity {Config::GRAVITY}, m_jumpStrength {Config::JUMP_STRENGTH}, m_isOnGround {true}
,m_rockTex(rockText), m_rock {rockText}, m_throwRock {false}, m_rockAmmo {1000}
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
    
    // Handle throw rock - basic physics (will be overridden by updateRockPhysics)
    if (m_throwRock)
    {
        // Apply gravity to rock
        m_rockVelocity.y += m_gravity * dt;
        
        // Move rock with its velocity
        m_rock.move(m_rockVelocity * dt);

        // If rock leaves screen or hits ground, reset throw
        if (m_rock.getPosition().x < -50 || m_rock.getPosition().x > Config::SCREEN_WIDTH + 50 ||
            m_rock.getPosition().y > Config::SCREEN_HEIGHT + 50)
        {
            m_throwRock = false;
            m_rockVelocity = {0.f, 0.f};  // Reset rock velocity
        }
    }

    // Handle animation frames
    m_elapsedTime += dt;
    if (m_elapsedTime >= m_frameTime)
    {
        if (!m_isOnGround) {
            // Handle jump animation
            m_elapsedTime = 0.f;
            m_currentFrame = (m_currentFrame + 1) % m_jumpFrames.size();
            m_sprite.setTexture(m_jumpText);
            m_sprite.setTextureRect(m_jumpFrames[m_currentFrame]);
        }
        else if (isWalking && m_isOnGround)
        {
            m_elapsedTime = 0.f;
            m_currentFrame = (m_currentFrame + 1) % m_walkFrames.size();
            m_sprite.setTexture(m_walkTexture);
            m_sprite.setTextureRect(m_walkFrames[m_currentFrame]);
        }
        else if (isWalking && !m_isOnGround)
        {
            // Walking while in air - show jump animation but don't override physics
            m_elapsedTime = 0.f;
            m_currentFrame = (m_currentFrame + 1) % m_jumpFrames.size();
            m_sprite.setTexture(m_jumpText);
            m_sprite.setTextureRect(m_jumpFrames[m_currentFrame]);
        }
        else if (!isWalking)
        {
            m_elapsedTime = 0.f;
            m_currentFrame = (m_currentFrame + 1) % m_idleFrames.size();
            m_sprite.setTexture(m_idleTexture);
            m_sprite.setTextureRect(m_idleFrames[m_currentFrame]);
        }
    }
}


void Character::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
    
    // Draw rock if it's being thrown
    if (m_throwRock)
    {
        window.draw(m_rock);
    }
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
    if (m_rockAmmo > 0 && !m_throwRock) 
    {
        m_throwRock = true;
        m_rockAmmo--;

        // Position rock at character's position
        m_rock.setPosition(m_sprite.getPosition());
        
        // Set initial rock velocity (horizontal + slight upward)
        float direction = (m_sprite.getScale().x > 0) ? 1.f : -1.f;
        m_rockVelocity = {direction * Config::ROCK_SPEED, -200.f};  // Horizontal speed + upward velocity
        
        std::cout << "Rock thrown! Ammo: " << m_rockAmmo << std::endl;
    }
    else if (m_rockAmmo > 0 && m_throwRock)
    {
        // If rock is already being thrown, reset it to allow new throws
        m_throwRock = false;
        m_rockVelocity = {0.f, 0.f};
        std::cout << "Resetting rock for new throw" << std::endl;
    }
}

bool Character::checkRockCollision(const sf::Sprite& otherSprite)
{
    // Get the bounding boxes
    sf::FloatRect rockBounds = m_rock.getGlobalBounds();
    sf::FloatRect otherBounds = otherSprite.getGlobalBounds();
    
    // Simple collision detection using position and size
    sf::Vector2f rockPos = m_rock.getPosition();
    sf::Vector2f otherPos = otherSprite.getPosition();
    
    // Check if rock is within the other sprite's bounds
    return (rockPos.x >= otherPos.x - 8 && rockPos.x <= otherPos.x + 32 &&
            rockPos.y >= otherPos.y - 8 && rockPos.y <= otherPos.y + 32);
}

void Character::updateRockPhysics(float dt, const std::vector<sf::Sprite*>& platforms)
{
    if (!m_throwRock) return;
    
    // Apply gravity to rock
    m_rockVelocity.y += m_gravity * dt;
    
    // Calculate new position
    sf::Vector2f newPosition = m_rock.getPosition() + m_rockVelocity * dt;
    
    // Check collision with platforms
    bool onPlatform = false;
    for (const auto& platform : platforms)
    {
        sf::Vector2f platformPos = platform->getPosition();
        
        // Use hardcoded values based on wall types
        // Walls are either horizontal (full width, 16 height) or vertical (16 width, full height)
        float platformWidth = 1024.f;  // Default to screen width
        float platformHeight = 16.f;   // Default to wall thickness
        
        // Check if this is a vertical wall (left/right walls)
        if (platformPos.x == 0 || platformPos.x == Config::SCREEN_WIDTH - 16)
        {
            platformWidth = 16.f;
            platformHeight = 768.f;  // Screen height
        }
        
        // Only check collision if rock is moving downward (falling)
        if (m_rockVelocity.y > 0)
        {
            // Check if rock is colliding with platform from above
            if (newPosition.x >= platformPos.x - 4 && newPosition.x <= platformPos.x + platformWidth + 4 &&
                newPosition.y >= platformPos.y - 4 && newPosition.y <= platformPos.y + 4)
            {
                // Rock hit a platform from above
                onPlatform = true;
                
                // Position rock on top of platform
                m_rock.setPosition({newPosition.x, platformPos.y - 4});
                m_rockVelocity = {0.f, 0.f};  // Stop rock movement
                
                std::cout << "Rock landed on platform at (" << platformPos.x << ", " << platformPos.y << ")" << std::endl;
                break;
            }
        }
    }
    
    // If not on platform, continue falling
    if (!onPlatform)
    {
        m_rock.setPosition(newPosition);
        
        // If rock falls off screen, reset
        if (m_rock.getPosition().x < -50 || m_rock.getPosition().x > Config::SCREEN_WIDTH + 50 ||
            m_rock.getPosition().y > Config::SCREEN_HEIGHT + 50)
        {
            m_throwRock = false;
            m_rockVelocity = {0.f, 0.f};
            std::cout << "Rock fell off screen" << std::endl;
        }
    }
}