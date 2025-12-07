#pragma once
#include "Systems.hpp"
#include <map>

class Prefabs
{
    //enemy types per level, sorted and grouped by cost
    const static std::map<int, std::vector<EnemyTypes>> enemiesPerLevel[]; 
    Prefabs() = delete;
    ~Prefabs() = delete;
    public:
};