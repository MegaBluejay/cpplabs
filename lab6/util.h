#ifndef UTIL_H
#define UTIL_H

#include <set>
#include <limits>

template<class InputIt, class Comp = std::less<typename InputIt::value_type>>
int inv_count(InputIt it, InputIt end, Comp comp = {}) {
    std::multiset<typename InputIt::value_type, Comp> multiset (comp);
    multiset.insert(*(it++));
    int count = 0;
    for (; it != end; ++it) {
        multiset.insert(*it);
        count += std::distance(multiset.upper_bound(*it), multiset.end());
    }
    return count;
}


#endif
