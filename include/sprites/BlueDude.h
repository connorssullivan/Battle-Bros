#pragma once

#include "Character.h"
#include "AssetMan.h"

class BlueDude : public Character // All members have same guard as Character. Public stays public 
{
    private:

    public:
        BlueDude(const sf::Texture& idleText, const sf::Texture& walkTex, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& deathText, const sf::Texture& rockText, int levelWidth);
};