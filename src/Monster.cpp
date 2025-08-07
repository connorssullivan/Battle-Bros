#include "Monster.h"

Monster::Monster(const sf::Texture& idleTex,
                 const sf::Texture& walkTex,
                 const sf::Texture& attack1Tex,
                 const sf::Texture& attack2Tex,
                 const sf::Texture& hurtTex,
                 const sf::Texture& deathTex,
                 float leftBound, float rightBound)
    : m_idleTexture(idleTex), m_walkTexture(walkTex), m_attack1Texture(attack1Tex),
      m_attack2Texture(attack2Tex), m_hurtTexture(hurtTex),
      m_deathTexture(deathTex),
      m_leftBound(leftBound), m_rightBound(rightBound),
      m_speed(50.f), m_direction(1.f),
      m_frameTime(0.15f), m_elapsedTime(0.f), m_currentFrame(0),
      m_state(State::Idle), m_facingRight(true),
      m_sprite {idleTex}
{
    m_sprite.setTexture(m_idleTexture);
    const int frameWidth = 96;
    const int frameHeight = 96;

    // Set up all the animations (64x96 frames)
    for (int i = 0; i < 4; i++)  
        m_idleFrames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));

    for (int i = 0; i < 6; i++)
        m_walkFrames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));

    for (int i = 0; i < 6; i++)
        m_attack1Frames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));

    for (int i = 0; i < 6; i++)
        m_attack2Frames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));

    for (int i = 0; i < 6; i++)
        m_deathFrames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));

    for (int i = 0; i < 4; i++)  
        m_hurtFrames.push_back(sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight}));
    
    if (!m_idleFrames.empty()) {
        m_sprite.setTextureRect(m_idleFrames[0]);
    }

    m_sprite.setOrigin({frameWidth / 2.f, 0.f}); // Center the origin horizontally

}

void Monster::Update(float dt)
{
    m_elapsedTime += dt;

    // Patrol movement
    if (m_state == State::Walk)
    {
        sf::Vector2f pos = m_sprite.getPosition();
        pos.x += m_speed * m_direction * dt;
        m_sprite.setPosition(pos);

        if (pos.x < m_leftBound) 
        {
            m_direction = 1.f;
            m_facingRight = false;
        }

        if (pos.x > m_rightBound) 
        {
            m_direction = -1.f;
            m_facingRight = true;
        }
    }

    if (m_direction > 0)
        m_sprite.setScale({-2.f, 2.f});
    else
        m_sprite.setScale({2.f, 2.f});

    // Animation update
    if (m_elapsedTime >= m_frameTime)
    {
        m_elapsedTime = 0.f;
        m_currentFrame++;

        const std::vector<sf::IntRect>* currentFrames = nullptr;
        const sf::Texture* currentTex = nullptr;


        switch (m_state)
        {
            case State::Idle: 
                // Maintain direction and scale even when idle
                if (m_direction > 0) {
                    m_sprite.setScale({-2.f, 2.f}); 
                } 
                else {
                    m_sprite.setScale({2.f, 2.f}); 
                }
                currentFrames = &m_idleFrames; 
                currentTex = &m_idleTexture; 
                break;
            case State::Walk: 
                if (m_direction > 0) {
                    m_sprite.setScale({-2.f, 2.f}); // Maintain the 2x scale from GamePlay
                } 
                else {
                    m_sprite.setScale({2.f, 2.f}); // Maintain the 2x scale from GamePlay
                }
                currentFrames = &m_walkFrames; 
                currentTex = &m_walkTexture; 
                break;
            case State::Attack1: 
                currentFrames = &m_attack1Frames; 
                currentTex = &m_attack1Texture; 
                break;
            case State::Attack2: 
                currentFrames = &m_attack2Frames; 
                currentTex = &m_attack2Texture; 
                break;
            case State::Hurt: 
                currentFrames = &m_hurtFrames; 
                currentTex = &m_hurtTexture; 
                break;
            case State::Death: 
                currentFrames = &m_deathFrames; 
                currentTex = &m_deathTexture; 
                break;
        }

        if (currentFrames && !currentFrames->empty())
        {
            if (m_currentFrame >= (int)currentFrames->size())
                m_currentFrame = 0;
            m_sprite.setTexture(*currentTex);
            m_sprite.setTextureRect((*currentFrames)[m_currentFrame]);
        }
    }
}

void Monster::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}

void Monster::SetState(State newState)
{
    if (m_state != newState)
    {
        m_state = newState;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }
}

void Monster::Attack1() { SetState(State::Attack1); }
void Monster::Attack2() { SetState(State::Attack2); }
void Monster::Hurt()    { SetState(State::Hurt); }
void Monster::Die()     { SetState(State::Death); }

sf::Sprite& Monster::GetSprite()
{
    return m_sprite;
}


void Monster::Flip()
{
    if (m_state == State::Walk)
        m_direction *= -1.f; // Reverse direction
}
