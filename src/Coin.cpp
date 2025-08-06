#include "Coin.h"

Coin::Coin(const sf::Texture& coin1Tex, const sf::Texture& coin2Tex, const sf::Texture& coin3Tex, const sf::Texture& coin4Tex, const sf::Texture& coin5Tex, const sf::Texture& coin6Tex)
: m_sprite {coin1Tex}, m_currentFrame {0}, m_frameTime {0.f}, m_elapsedTime {0}
{
    // Set Coin Frames
    m_coinFrames.push_back(&coin1Tex);
    m_coinFrames.push_back(&coin2Tex);
    m_coinFrames.push_back(&coin3Tex);
    m_coinFrames.push_back(&coin4Tex);
    m_coinFrames.push_back(&coin5Tex);
    m_coinFrames.push_back(&coin6Tex);
    
}


void Coin::Update()
{
    m_elapsedTime += 0.016f; 
    if (m_elapsedTime >= m_frameTime)
    {
        m_elapsedTime = 0.f;
        m_currentFrame = (m_currentFrame + 1) % m_coinFrames.size();
        m_sprite.setTexture(*m_coinFrames[m_currentFrame]);
    }
}

void Coin::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}

sf::Sprite& Coin::GetSprite()
{
    return m_sprite;
}