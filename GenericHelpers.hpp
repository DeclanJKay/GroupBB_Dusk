#pragma once
#include <map>
#include <unordered_map>
#include <vector>

//gets sign of a number
template <typename T> int Sign(T val) 
{
    return (T(0) < val) - (val < T(0));
}

//gets the x element in a provided map
template <typename MapType>
typename MapType::iterator GetIterator(MapType& costMap, int i)
{
    typename MapType::iterator it = costMap.begin();
    std::advance(it, i);
    return it;
}

//mainly used for the stats managers
//swaps an unordered map to a map with the keys and values inverted
template <typename key, typename value>
std::map<value, std::vector<key>> FlipMap(const std::unordered_map<key, value>& map)
{
    std::map<value, std::vector<key>> returnable;
    for (auto pair : map)
    {
        auto it = returnable.find(pair.second);
        if (it == returnable.end())
        {
            returnable.insert({pair.second, {pair.first}});
            continue;
        }
        it->second.push_back(pair.first);
    }
    return returnable;
}