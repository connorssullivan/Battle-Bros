#pragma once

#include <SFML/Graphics.hpp>
#include "../core/Config.h"


class Coin 
{
    private:
        sf::Sprite m_sprite;
        std::vector<const sf::Texture*> m_coinFrames; 

        float m_frameTime;
        float m_elapsedTime;
        int m_currentFrame;

    public:
        Coin(const sf::Texture& coin1Tex, const sf::Texture& coin2Tex, const sf::Texture& coin3Tex, const sf::Texture& coin4Tex, const sf::Texture& coin5Tex, const sf::Texture& coin6Tex);
        ~Coin() = default;

        void Update();
        void Draw(sf::RenderWindow& window);

        sf::Sprite& GetSprite();
};