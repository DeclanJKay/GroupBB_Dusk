#include "EnemyStats.hpp"
#include "Comps.hpp"

const std::map<int, std::vector<EnemyTypes>> EnemyStatsManager::enemiesPerLevel[] = 
{
    //level 1
    {
        {1, {EnemyTypes::Basic}},
        {2, {EnemyTypes::Fast}},
        {3, {EnemyTypes::Tank}}
    },

    //level 2
    {
        {3, {EnemyTypes::shortRanged, EnemyTypes::Exploder}}
    },

    //level 3
    {
        {3, {EnemyTypes::Medium}},
        {4, {EnemyTypes::RangedMelee, EnemyTypes::FastExploder}}
    },

    //level 4
    {
        {4, {EnemyTypes::LongRange}},
        {5, {EnemyTypes::HeavyTank}}
    }
};