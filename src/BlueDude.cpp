#include "BlueDude.h"



BlueDude::BlueDude(const sf::Texture& idleText, const sf::Texture& walkText, const sf::Texture& jumpText, const sf::Texture& throwText, const sf::Texture& deathText, const sf::Texture& rockText, int levelWidth) 
: Character (idleText, walkText, jumpText, throwText, deathText,rockText, levelWidth)
{
    // Set idle frames (4 frames, each 32x32)
    for (int i=0; i < 4; i++)
        m_idleFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));

    // Set Walk frames (6 frames, each 32x32)
    for (int i=0; i < 6; i++)
        m_walkFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));

    // Set Jump frames (8 frames, each 32x32)
    for (int i=0; i < 8; i++)
        m_jumpFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));

    // Throwing frames
    for (int i=0; i < 4; i++)
        m_throwFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));
    

    // Death frames
    for (int i=0; i < 8; i++)
        m_deathFrames.push_back(sf::IntRect({i * 32, 0}, {32, 32}));


    m_sprite.setTextureRect(m_idleFrames[0]);

    // Set the throwing rock 
    m_rock.setTexture(m_rockTex); 
    m_rock.setTextureRect(sf::IntRect({0, 0}, {8, 8}));  // Correct size for 8x8 texture
    
}