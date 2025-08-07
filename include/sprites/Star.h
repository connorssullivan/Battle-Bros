#pragma once

#include <SFML/Graphics.hpp>
#include "../core/Config.h"


class Star 
{
    private:
        sf::Sprite m_sprite;

    public:
        Star(const sf::Texture& starTex);


        void Update();
        void Draw(sf::RenderWindow& window);

        sf::Sprite& GetSprite() {return m_sprite;};
};