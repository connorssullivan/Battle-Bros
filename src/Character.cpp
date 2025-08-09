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
    m_rock.setOrigin({4.f, 4.f});  // Center the rock,  8x8 texture
    m_rock.setTextureRect(sf::IntRect({0, 0}, {8, 8}));  // Set correct texture rec
}



void Character::Update(float dt, bool isWalking, const std::vector<sf::Sprite*>& platforms)
{
    m_isWalking = isWalking;

    sf::Vector2f originalPos = m_sprite.getPosition();
    if (m_isDead)
    {
        //m_deathAnimStarted = true;
    }

    // Apply physics and movement 
    else if (!m_isOnGround) 
    {
        m_velocity.y += m_gravity * dt; // Jumping/falling physics
        
        // Move horizontally first, then check collision
        if (m_velocity.x != 0.f) 
        {
            sf::Vector2f horizontalMovement = {m_velocity.x * dt, 0.f};
            m_sprite.move(horizontalMovement);
            
            // Check horizontal collision 
            if (checkPlayerCollisionDuringMovement(platforms, m_velocity.x > 0 ? 1.f : -1.f)) 
            {
                m_sprite.setPosition({originalPos.x, m_sprite.getPosition().y});
                m_velocity.x = 0.f; // Come to stop
            }
        }
        
        sf::Vector2f originalPosBeforeVertical = m_sprite.getPosition();
        sf::Vector2f verticalMovement = {0.f, m_velocity.y * dt};
        m_sprite.move(verticalMovement);

        // Check vertical collision (ceiling or floor)
        sf::Vector2f landingPosition;
        if (checkVerticalCollision(platforms, landingPosition)) 
        {
            if (m_velocity.y < 0.f) 
            {
                // Hit ceiling 
                m_sprite.setPosition(originalPosBeforeVertical);
                m_velocity.y = 0.f;
            } 
            else 
            {
                // Landing on platform 
                m_sprite.setPosition(landingPosition);
                m_velocity.y = 0.f;
                m_isJumping = false;
                m_isOnGround = true;
            }
        }

        // Check if still on ground after movement 
        if (m_isOnGround && !isOnPlatform(platforms)) 
        {
            // Check landing on ground level 
            float groundLevel = Config::SCREEN_HEIGHT - 32.f; 
            if (m_sprite.getPosition().y >= groundLevel) 
            {
                m_sprite.setPosition({m_sprite.getPosition().x, groundLevel});
                m_velocity.y = 0.f;
                m_isJumping = false;
                m_isOnGround = true;
            } 
            
            else 
            {
                // Not on any platform, start falling
                m_isOnGround = false;
            }
        } 
        
        // Just landed on a platform, make sure were grounded
        else if (!m_isOnGround && isOnPlatform(platforms)) 
        {
            // Just landed on a platform, make sure we're grounded
            m_isOnGround = true;
            m_isJumping = false;
            m_velocity.y = 0.f;
        }
    }

    else if (isWalking) 
    {
        // Walking on ground
        sf::Vector2f movement = m_velocity * dt;
        m_sprite.move(movement);
        
        // Check collision and revert if needed
        float direction = m_velocity.x > 0 ? 1.f : (m_velocity.x < 0 ? -1.f : 0.f);
        if (direction != 0.f && checkPlayerCollisionDuringMovement(platforms, direction)) 
        {
            m_sprite.setPosition(originalPos);
            m_velocity.x = 0.f; // Stop movement
        }
        
        // Check if walked off a platform
        if (m_isOnGround && !isOnPlatform(platforms)) 
        {
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

            // Stop throw animation after first cycle
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
    // Draw the charecter, and the rock

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
    // If the player has the rock, throw it
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


// Collisions///

bool Character::checkRockCollision(const sf::Sprite& otherSprite)
{

    // Get bounds of rock and what ecer it's coliding with
    sf::FloatRect rockBounds = m_rock.getGlobalBounds();
    sf::FloatRect otherBounds = otherSprite.getGlobalBounds();    
    
    sf::Vector2f rockPos = m_rock.getPosition();
    sf::Vector2f otherPos = otherSprite.getPosition();
    
    // Check if rock is within the other sprites bounds
    return (rockPos.x >= otherPos.x - 8 && rockPos.x <= otherPos.x + 32 &&
            rockPos.y >= otherPos.y - 8 && rockPos.y <= otherPos.y + 32);
}

void Character::updateRockPhysics(float dt, const std::vector<sf::Sprite*>& platforms)
{
    if (m_rockState == RockState::Flying)
    {
        // Integrate motion
        m_rockVelocity.y += m_gravity * dt;
        const sf::FloatRect prevRockBounds = m_rock.getGlobalBounds();
        m_rock.move(m_rockVelocity * dt);

        const sf::FloatRect rockBounds = m_rock.getGlobalBounds();

        for (auto* platform : platforms)
        {
            const sf::FloatRect platformBounds = platform->getGlobalBounds();

            if (auto intersection = rockBounds.findIntersection(platformBounds))
            {
                const float platformTop = platformBounds.position.y;
                const float platformBottom = platformBounds.position.y + platformBounds.size.y;
                const float platformLeft = platformBounds.position.x;
                const float platformRight = platformBounds.position.x + platformBounds.size.x;

                const float prevTop = prevRockBounds.position.y;
                const float prevBottom = prevRockBounds.position.y + prevRockBounds.size.y;
                const float prevLeft = prevRockBounds.position.x;
                const float prevRight = prevRockBounds.position.x + prevRockBounds.size.x;

                const float rockHalfW = rockBounds.size.x / 2.0f;
                const float rockHalfH = rockBounds.size.y / 2.0f;

                const bool falling = m_rockVelocity.y >= 0.0f;
                const bool rising = m_rockVelocity.y < 0.0f;

                // Horizontal overlap check
                const bool horizontallyOverlapping =
                    rockBounds.position.x + rockBounds.size.x > platformLeft &&
                    rockBounds.position.x < platformRight;

                // Vertical overlap check
                const bool verticallyOverlapping =
                    rockBounds.position.y + rockBounds.size.y > platformTop &&
                    rockBounds.position.y < platformBottom;

                // Landing on top: coming from above and overlapping horizontally
                if (falling && prevBottom <= platformTop && horizontallyOverlapping)
                {
                    m_rock.setPosition({
                        m_rock.getPosition().x,
                        platformTop - rockHalfH
                    });
                    m_rockVelocity = {0.f, 0.f};
                    m_rockState = RockState::Landed;
                    return;
                }

                // Hitting the bottom (ceiling for the rock): coming from below while rising
                if (rising && prevTop >= platformBottom && horizontallyOverlapping)
                {
                    m_rock.setPosition({
                        m_rock.getPosition().x,
                        platformBottom + rockHalfH
                    });
                    m_rockVelocity.y = std::abs(m_rockVelocity.y) * 0.9f; // bounce down slightly
                    m_rockVelocity.x *= 0.8f;
                    return;
                }

                // Side collisions: coming from left/right while overlapping vertically
                if (verticallyOverlapping)
                {
                    const float centerX = m_rock.getPosition().x;
                    const float platformCenterX = platformBounds.position.x + platformBounds.size.x / 2.0f;
                    if (centerX < platformCenterX)
                    {
                        // Hit left side
                        m_rock.setPosition({ platformLeft - rockHalfW, m_rock.getPosition().y });
                    }

                    else
                    {
                        // Hit right side 
                        m_rock.setPosition({ platformRight + rockHalfW, m_rock.getPosition().y });
                    }

                    // Stop
                    m_rockVelocity = {0.f, 0.f};
                    m_rockState = RockState::Landed;
                    return;
                }
            }
        }

        // Despawn if way off screen
        if (m_rock.getPosition().y > Config::SCREEN_HEIGHT + 50 ||
            m_rock.getPosition().x < -50 || m_rock.getPosition().x > m_levelWidth + 50)
        {
            m_rockState = RockState::None;
            m_rockVelocity = {0.f, 0.f};
        }
    }

    // Landing
    else if (m_rockState == RockState::Landed)
    {
        const auto rockBounds = m_rock.getGlobalBounds();
        bool supported = false;
        const float epsilon = 2.0f; 

        for (auto* platform : platforms)
        {
            const auto platformBounds = platform->getGlobalBounds();
            const float platformTop = platformBounds.position.y;

            // Check if rock is resting on top of any platform 
            const bool horizontallyOverlapping =
                rockBounds.position.x + rockBounds.size.x > platformBounds.position.x &&
                rockBounds.position.x < platformBounds.position.x + platformBounds.size.x;

            const float rockBottom = rockBounds.position.y + rockBounds.size.y;

            if (horizontallyOverlapping && std::abs(rockBottom - platformTop) <= epsilon)
            {
                supported = true;
                break;
            }
        }

        // If not supported, reapply gravity
        if (!supported)
        {
            m_rockState = RockState::Flying;
            m_rockVelocity.y = std::max(m_rockVelocity.y, 50.f);
        }
    }


}

void Character::pickupRock()
{
    // If rock is on the ground and player collides with it, pick it up
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
                // If moving right & player is to the left of platform center block
                if (directionX > 0 && playerCenter.x < platformCenter.x) 
                    return true;
                    
                // If moving left & player is to the right of platform center block
                if (directionX < 0 && playerCenter.x > platformCenter.x) 
                    return true;
            }
        }
    }
    return false;
}

bool Character::checkPlayerCollisionDuringMovement(const std::vector<sf::Sprite*>& platforms, float directionX)
{
    // Get Player Bounds
    sf::FloatRect playerBounds = m_sprite.getGlobalBounds();
    sf::Vector2f playerCenter{
        playerBounds.position.x + playerBounds.size.x / 2.f,
        playerBounds.position.y + playerBounds.size.y / 2.f
    };
    
    // Check each platform for intersection
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        sf::Vector2f platformCenter {
            platformBounds.position.x + platformBounds.size.x / 2.f,
            platformBounds.position.y + platformBounds.size.y / 2.f
        };
        
        if (auto intersection = playerBounds.findIntersection(platformBounds))
        {
            const auto& rect = *intersection;
            
            // Determine collision type based on intersection shape and movement direction
            if (directionX != 0.f) // Horizontal movement
            {
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
        
        // Determine if there is an intersection between the platform and player
        if (auto intersection = playerBounds.findIntersection(platformBounds))
        {
            const auto& rect = *intersection;
            
            // Check if intersection width is significant enough to be considered a landing
            if (rect.size.x > playerBounds.size.x * 0.3f)
            {
                float playerBottom = playerBounds.position.y + playerBounds.size.y;
                float platformTop = platformBounds.position.y;
                float platformBottom = platformBounds.position.y + platformBounds.size.y;
                
                // Check if player is falling onto the platform
                if (m_velocity.y > 0.f) 
                {
                    
                    if (playerBottom <= platformTop + 10.f)  // Small tolerance for landing detection
                    {
                        landingPosition = {m_sprite.getPosition().x, platformTop - playerBounds.size.y};
                        return true; // Hit flooor
                    }
                }
                // check if hitting ceiling
                else if (m_velocity.y < 0.f) 
                { 
                    float playerTop = playerBounds.position.y;
                    if (playerTop >= platformBottom - 10.f) // Small tolerance
                    { 
                        return true; // Hit ceiling
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
    
    // Check each platform, see if player the player is standing on it
    for (const auto* platform : platforms)
    {
        sf::FloatRect platformBounds = platform->getGlobalBounds();
        float platformTop = platformBounds.position.y;
        
        // Check if player is standing on top of this platform
        if (std::abs(playerBottom - platformTop) <= 5.f) // Tolerance for standing detection
        { 
            float playerLeft = playerBounds.position.x;
            float playerRight = playerBounds.position.x + playerBounds.size.x;
            float platformLeft = platformBounds.position.x;
            float platformRight = platformBounds.position.x + platformBounds.size.x;
            
            // Check horizontal overlap
            float overlapLeft = std::max(playerLeft, platformLeft);
            float overlapRight = std::min(playerRight, platformRight);
            float overlapWidth = overlapRight - overlapLeft;
            
            if (overlapWidth > playerBounds.size.x * 0.3f) 
            {
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




