#include "Character.h"
#include <iostream>
#include <algorithm>

Character::Character(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& deathText, const sf::Texture& rockText, int level_width)
: m_frameTime {0.15f}, m_elapsedTime {0.f}, m_currentFrame {0}, m_sprite {idleText}
, m_idleTexture {idleText}, m_walkTexture {walkText}, m_jumpText {jumpText}, m_throwTex {throwText}, m_deathText {deathText}
, m_isWalking {false}, m_isJumping {false}, m_isDead {false}, m_deathDelayTimer {0.f}, m_deathDelayDuration {0.3}, m_deathEnded {false}
, m_velocity{0.f, 0.f}, m_rockVelocity{0.f, 0.f}, m_speed{Config::WALK_SPEED}
, m_gravity {Config::GRAVITY}, m_jumpStrength {Config::JUMP_STRENGTH}, m_isOnGround {true}
,m_rockTex(rockText), m_rock {rockText}, m_rockAmmo {1}, m_rockState {RockState::None}, m_isThrow {false}, m_isThrowingAnimation {false}, m_throwAnimTime {0.f}, m_throwAnimDuration {0.25f}
, m_levelWidth {level_width}
{
    m_sprite.setOrigin({16.f, 16.f});
    
    // Initialize rock sprite
    m_rock.setOrigin({4.f, 4.f});  // Center the rock (8x8 texture)
    m_rock.setTextureRect(sf::IntRect({0, 0}, {8, 8}));  // Set correct texture rectangle
}



void Character::Update(float dt, bool isWalking, const std::vector<sf::Sprite*>& platforms)
{
    m_isWalking = isWalking;

    // Store original position to revert if collision occurs
    sf::Vector2f originalPos = m_sprite.getPosition();
    if (m_isDead)
    {
        //m_deathAnimStarted = true;
    }
    // Apply physics and movement 
    else if (!m_isOnGround) 
    {
        // Jumping/falling physics
        m_velocity.y += m_gravity * dt;
        
        // Move horizontally first, check collision
        if (m_velocity.x != 0.f) {
            sf::Vector2f horizontalMovement = {m_velocity.x * dt, 0.f};
            m_sprite.move(horizontalMovement);
            
            // Check horizontal collision and revert if needed
            if (checkPlayerCollisionDuringMovement(platforms, m_velocity.x > 0 ? 1.f : -1.f)) {
                m_sprite.setPosition({originalPos.x, m_sprite.getPosition().y});
                m_velocity.x = 0.f; // Stop horizontal movement
            }
        }
        
        // Move vertically, check collision
        sf::Vector2f originalPosBeforeVertical = m_sprite.getPosition();
        sf::Vector2f verticalMovement = {0.f, m_velocity.y * dt};
        m_sprite.move(verticalMovement);

        // Check vertical collision (ceiling or floor)
        sf::Vector2f landingPosition;
        if (checkVerticalCollision(platforms, landingPosition)) {
            if (m_velocity.y < 0.f) {
                // Hit ceiling - revert position
                m_sprite.setPosition(originalPosBeforeVertical);
                m_velocity.y = 0.f;
            } else {
                // Landing on platform - position character on top
                m_sprite.setPosition(landingPosition);
                m_velocity.y = 0.f;
                m_isJumping = false;
                m_isOnGround = true;
            }
        }

        // Check if still on ground after movement (only if not already determined to be on ground)
        if (m_isOnGround && !isOnPlatform(platforms)) {
            // Check landing on ground level (fallback)
            float groundLevel = Config::SCREEN_HEIGHT - 32.f; 
            if (m_sprite.getPosition().y >= groundLevel) {
                m_sprite.setPosition({m_sprite.getPosition().x, groundLevel});
                m_velocity.y = 0.f;
                m_isJumping = false;
                m_isOnGround = true;
            } else {
                // Not on any platform, start falling
                m_isOnGround = false;
            }
        } else if (!m_isOnGround && isOnPlatform(platforms)) {
            // Just landed on a platform, make sure we're grounded
            m_isOnGround = true;
            m_isJumping = false;
            m_velocity.y = 0.f;
        }
    }
    else if (isWalking) {
        // Walking on ground - check collision before moving
        sf::Vector2f movement = m_velocity * dt;
        m_sprite.move(movement);
        
        // Check collision and revert if needed
        float direction = m_velocity.x > 0 ? 1.f : (m_velocity.x < 0 ? -1.f : 0.f);
        if (direction != 0.f && checkPlayerCollisionDuringMovement(platforms, direction)) {
            m_sprite.setPosition(originalPos);
            m_velocity.x = 0.f; // Stop movement
        }
        
        // Check if walked off a platform
        if (m_isOnGround && !isOnPlatform(platforms)) {
            float groundLevel = Config::SCREEN_HEIGHT - 32.f; 
            if (m_sprite.getPosition().y < groundLevel - 5.f) { // Some tolerance before falling
                m_isOnGround = false; // Start falling
            }
        }
    }
    

    // Handle animation frames
    m_elapsedTime += dt;
    if (m_elapsedTime >= m_frameTime)
    {
        if (m_isDead)
        {
            // Start delay countdown immediately
            m_deathDelayTimer += dt;
            
            //std::cout << "Death timer: " << m_deathDelayTimer << " / " << m_deathDelayDuration + 1.f << std::endl;

            //Cheack for the  end screen
            if (m_deathDelayTimer > m_deathDelayDuration + 0.1)
            {
                m_deathEnded = true;
            }

            // Wait for delay to finish
            if (m_deathDelayTimer < m_deathDelayDuration)
            {
                return;
            }

            // Now start animation
            m_elapsedTime += dt;
            if (m_elapsedTime >= m_frameTime)
            {
                m_elapsedTime = 0.f;

                if (m_currentFrame < m_deathFrames.size())
                {
                    m_sprite.setTexture(m_deathText);
                    m_sprite.setTextureRect(m_deathFrames[m_currentFrame]);
                    m_currentFrame++;
                }
            }
                
            return; // Don't run any other animations
        }
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

bool Character::checkPlayerCollision(const std::vector<sf::Sprite*>& platforms, float directionX)
{
    sf::FloatRect playerBounds = m_sprite.getGlobalBounds();
    sf::Vector2f playerCenter{
        playerBounds.position.x + playerBounds.size.x / 2.f,
        playerBounds.position.y + playerBounds.size.y / 2.f
    };

    // Gp through all platforms and check for collison
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        sf::Vector2f platformCenter{
            platformBounds.position.x + platformBounds.size.x / 2.f,
            platformBounds.position.y + platformBounds.size.y / 2.f
        };

        //Find intersection
        if (auto intersection = playerBounds.findIntersection(platformBounds))
        {
            const auto& rect = *intersection;

            // Horizontal collision
            if (rect.size.x < rect.size.y)
            {
                // If moving right & player is to the left of platform center → block
                if (directionX > 0 && playerCenter.x < platformCenter.x) 
                    return true;
                // If moving left & player is to the right of platform center → block
                if (directionX < 0 && playerCenter.x > platformCenter.x) 
                    return true;
            }
        }
    }
    return false;
}

bool Character::checkPlayerCollisionDuringMovement(const std::vector<sf::Sprite*>& platforms, float directionX)
{
    sf::FloatRect playerBounds = m_sprite.getGlobalBounds();
    sf::Vector2f playerCenter{
        playerBounds.position.x + playerBounds.size.x / 2.f,
        playerBounds.position.y + playerBounds.size.y / 2.f
    };
    
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        sf::Vector2f platformCenter{
            platformBounds.position.x + platformBounds.size.x / 2.f,
            platformBounds.position.y + platformBounds.size.y / 2.f
        };
        
        if (auto intersection = playerBounds.findIntersection(platformBounds))
        {
            const auto& rect = *intersection;
            
            // Determine collision type based on intersection shape and movement direction
            if (directionX != 0.f) // Horizontal movement
            {
                // Check if this is a side collision (intersection is taller than it is wide)
                // OR if intersection area is significant enough to be considered a collision
                bool isSideCollision = rect.size.y > rect.size.x;
                bool isSignificantCollision = rect.size.y > playerBounds.size.y * 0.3f;
                
                if (isSideCollision || isSignificantCollision)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Character::checkVerticalCollision(const std::vector<sf::Sprite*>& platforms, sf::Vector2f& landingPosition)
{
    sf::FloatRect playerBounds = m_sprite.getGlobalBounds();
    
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        
        if (auto intersection = playerBounds.findIntersection(platformBounds))
        {
            const auto& rect = *intersection;
            
            // Check if intersection width is significant enough to be considered a landing
            if (rect.size.x > playerBounds.size.x * 0.3f)
            {
                float playerBottom = playerBounds.position.y + playerBounds.size.y;
                float platformTop = platformBounds.position.y;
                float platformBottom = platformBounds.position.y + platformBounds.size.y;
                
                if (m_velocity.y > 0.f) {
                    // Player is falling down - check if landing on top of platform
                    if (playerBottom <= platformTop + 10.f) { // Small tolerance for landing detection
                        // Calculate proper landing position (character on top of platform)
                        landingPosition = {m_sprite.getPosition().x, platformTop - playerBounds.size.y};
                        return true;
                    }
                } else if (m_velocity.y < 0.f) {
                    // Player is moving up - check if hitting ceiling
                    float playerTop = playerBounds.position.y;
                    if (playerTop >= platformBottom - 10.f) { // Small tolerance
                        return true; // Hit ceiling, will revert position
                    }
                }
            }
        }
    }
    return false;
}

bool Character::isOnPlatform(const std::vector<sf::Sprite*>& platforms)
{
    sf::FloatRect playerBounds = m_sprite.getGlobalBounds();
    float playerBottom = playerBounds.position.y + playerBounds.size.y;
    
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        float platformTop = platformBounds.position.y;
        
        // Check if player is standing on top of this platform
        if (std::abs(playerBottom - platformTop) <= 5.f) // Tolerance for standing detection
        { 
            // Check horizontal overlap
            float playerLeft = playerBounds.position.x;
            float playerRight = playerBounds.position.x + playerBounds.size.x;
            float platformLeft = platformBounds.position.x;
            float platformRight = platformBounds.position.x + platformBounds.size.x;
            
            // Check if there's sufficient horizontal overlap
            float overlapLeft = std::max(playerLeft, platformLeft);
            float overlapRight = std::min(playerRight, platformRight);
            float overlapWidth = overlapRight - overlapLeft;
            
            if (overlapWidth > playerBounds.size.x * 0.3f) {
                return true; 
            }
        }
    }
    return false;
}

void Character::kill()
{
    m_isDead = true;
}

void Character::resetRock()
{
    m_rockState = RockState::None;
    m_rockVelocity = {0.f, 0.f};
    m_rockAmmo = 1; // Give back the ammo
}




