#pragma once

#include <iostream>
#include <map>
#include <string>


#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>

#include "Config.h"

namespace Engine 
{
    class AssetMan
    {
        private:
            std::map<int, std::unique_ptr<sf::Texture>> m_textures;
            std::map<int, std::unique_ptr<sf::Font>> m_fonts;

        public:
            AssetMan();
            ~AssetMan();

            void AddTexture(int id, std::string& filePath, bool wantRepeated = false);
            void AddFont(int id, std::string& filepath);

            const sf::Texture& getTexture(int id) const;
            const sf::Font& getFont(int id) const;

    };
};