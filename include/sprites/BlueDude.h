#pragma once

#include "Character.h"
#include "AssetMan.h"

class BlueDude : public Character 
{
    private:

    public:
        BlueDude(const sf::Texture& idleText, const sf::Texture& walkTex, const sf::Texture& jumpText, const sf::Texture& throwText,const sf::Texture& rockText);
};