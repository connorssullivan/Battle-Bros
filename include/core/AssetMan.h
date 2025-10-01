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

            void AddTexture(const int id, const std::string& filePath, const bool wantRepeated = false);
            void AddFont(const int id, const std::string& filepath);

            const sf::Texture& getTexture(const int id) const;
            const sf::Font& getFont(const int id) const;

    };
};