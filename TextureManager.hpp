#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

/*
Textures need to be stored outside of components as sprites only store a pointer
to the texture, and components can move around in memory.
*/
class TxtrMgr
{
    TxtrMgr() = delete;
    ~TxtrMgr() = delete;
    private:
        //stored in a map to prevent loading multiples of the same file
        inline static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures; 
    public:
        static std::shared_ptr<sf::Texture> GetTxtr(const std::string& filepath);
        static void ClearTxtrs(); //call this when a deconstructor for a scene is called
};