#include "FileMgr.hpp"
#include <iostream>
#include "GenericHelpers.hpp"

//todo: these can probably also be templated
std::shared_ptr<sf::Texture> FileMgr::GetTxtr(const std::string& filepath)
{
    //check if its already in the map
    auto it = textures.find(filepath);
    if (it != textures.end())
    {
        return it->second;
    }

    //otherwise try to load
    auto txtr = std::make_shared<sf::Texture>();
    if (!txtr->loadFromFile(filepath))
    {
        //if failed to load
        std::cerr<<"Failed to load texture";
        return nullptr;
    }
    //otherwise add to heap
    textures[filepath] = txtr;
    return txtr;
}

std::shared_ptr<sf::Font> FileMgr::GetFont(const std::string& filepath)
{
    //check if its already in the map
    auto it = fonts.find(filepath);
    if (it != fonts.end())
    {
        return it->second;
    }

    //otherwise try to load
    auto font = std::make_shared<sf::Font>();
    if (!font->loadFromFile(filepath))
    {
        //if failed to load
        std::cerr<<"Failed to load texture";
        return nullptr;
    }
    //otherwise add to heap
    fonts[filepath] = font;
    return font;
}

void FileMgr::CleanUp()
{
    // Removes textures that are ONLY held by the manager
    CleanMapOfPtrs(textures);
    CleanMapOfPtrs(fonts);
}