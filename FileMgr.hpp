#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

/*
Textures need to be stored outside of components as sprites only store a pointer
to the texture, and components can move around in memory.
*/
class FileMgr
{
    FileMgr() = delete;
    ~FileMgr() = delete;
    private:
        //stored in a map to prevent loading multiples of the same file
        inline static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures; 
        inline static std::unordered_map<std::string, std::shared_ptr<sf::Font>> fonts; 
    public:
        static std::shared_ptr<sf::Texture> GetTxtr(const std::string& filepath);
        static std::shared_ptr<sf::Font> GetFont(const std::string& filepath);
        static void CleanUp(); //call this when a deconstructor for a scene is called
};