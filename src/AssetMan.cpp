#include "AssetMan.h"


Engine::AssetMan::AssetMan() 
{


}


Engine::AssetMan::~AssetMan()
{

}

void Engine::AssetMan::AddTexture(const int id, const std::string& filePath, const bool wantRepeated)
{
    // Create a new texture and load it from the file
    
    auto texture = std::make_unique<sf::Texture>();

    if (texture->loadFromFile(filePath))
    {
        texture->setRepeated(wantRepeated);
        m_textures[id] = std::move(texture);
    }
}


void Engine::AssetMan::AddFont(const int id, const std::string& filepath)
{
    // Create a new font and load it from the file
    
    auto font = std::make_unique<sf::Font>();

    if (font->openFromFile(filepath))
    {
        m_fonts[id] = std::move(font);
    }

}

const sf::Texture& Engine::AssetMan::getTexture(const int id) const
{
    return *(m_textures.at(id).get());
}

const sf::Font& Engine::AssetMan::getFont(int id) const
{
    return *(m_fonts.at(id).get());
}