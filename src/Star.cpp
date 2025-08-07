#include "Star.h"



Star::Star(const sf::Texture& starTex) : m_sprite {starTex}
{
    m_sprite.setOrigin({16.f, 16.f}); // Center the sprite
    m_sprite.setTextureRect(sf::IntRect({0, 0}, {128, 128})); 
}

void Star::Update()
{
    //TODO
}


void Star::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}