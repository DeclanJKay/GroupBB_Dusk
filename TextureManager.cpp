#include "TextureManager.hpp"
#include <iostream>

std::shared_ptr<sf::Texture> TxtrMgr::GetTxtr(const std::string& filepath)
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

void TxtrMgr::ClearTxtrs()
{
    // Removes textures that are ONLY held by the manager
    for (auto it = textures.begin(); it != textures.end(); )
    {
        if (it->second.unique()) 
        {
            it = textures.erase(it);
        }
        else 
        {
            ++it;
        }
    }
}