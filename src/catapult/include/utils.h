#ifndef __UTILS_H__
#define __UTILS_H__

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace catapult
{

//! @brief returns a vector of keys when given an unordered_map
template<class Key, class Value>
std::vector<Key> getKeys(std::unordered_map<Key, Value> map) {
    std::vector<Key> keys;
    std::transform(map.begin(), map.end(), std::back_inserter(keys), [](auto pair){return pair.first;});
    return keys;
}

} // namespace catapult


#endif // __UTILS_H__
