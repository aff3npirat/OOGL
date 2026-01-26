#pragma once

#include <algorithm>
#include <vector>


template<typename T, typename Pred>
typename std::vector<T>::iterator sortedInsert(std::vector<T>& vec, const T item, Pred pred) {
    return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
}