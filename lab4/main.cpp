#include <iostream>
#include <vector>

template <class InputIt, class Pred>
bool all_of(InputIt it, InputIt end, Pred pred) {
    for (; it != end; ++it) {
        if (!pred(*it)) {
            return false;
        }
    }
    return true;
}

template <class InputIt, class Pred>
bool any_of(InputIt it, InputIt end, Pred pred) {
    for (; it != end; ++it) {
        if (pred(*it)) {
            return true;
        }
    }
    return false;
}

template <class InputIt, class Pred>
bool none_of(InputIt it, InputIt end, Pred pred) {
    return !any_of(it, end, pred);
}

template <class InputIt, class Pred>
bool one_of(InputIt it, InputIt end, Pred pred) {
    bool one = false;
    for (; it != end; ++it) {
        if (pred(*it)) {
            if (one) {
                return false;
            } else {
                one = true;
            }
        }
    }
    return one;
}

template <class InputIt, class Comp>
bool is_sorted(InputIt it, InputIt end, Comp comp) {
    using T = typename InputIt::value_type;
    if (it == end) {
        return true;
    }
    T f = *(it++);
    for (; it != end; ++it) {
        if (!comp(f, *it)) {
            return false;
        }
        f = *it;
    }
    return true;
}

template <class InputIt, class Pred>
bool is_partitioned(InputIt it, InputIt end, Pred pred) {
    bool snd = false;
    for (; it!=end; ++it) {
        bool r = pred(*it);
        if (r && snd) {
            return false;
        }
        if (!r && !snd) {
            snd = true;
        }
    }
    return true;
}

template <class InputIt, class T>
InputIt find_not(InputIt it, InputIt end, T t) {
    for (; it!=end && (*it) == t; ++it) {}
    return it;
}

template <class InputIt, class T>
InputIt find_backward(InputIt it, InputIt end, T t) {
    InputIt ans = end;
    for (; it != end; ++it) {
        if ((*it) == t) {
            ans = it;
        }
    }
    return ans;
}

template <class BiDirIt, class Conv>
bool is_palindrome(BiDirIt sit, BiDirIt eit, Conv conv) {
    if (sit == eit) {
        return true;
    }
    eit--;
    while (sit != eit) {
        if (conv(*sit) != conv(*eit)) {
            return false;
        }
        if ((sit++) == (--eit)) {
            return true;
        }
    }
    return true;
}

int main() {
    std::vector<int> a ({2,2,4,6,8,10});

    std::cout << all_of(a.cbegin(), a.cend(), [](int x) {
        return 1-(x%2);
    }) << std::endl;

    std::cout << any_of(a.begin(), a.end(), [](int x) {
        return x > 7;
    }) << std::endl;

    std::cout << none_of(a.crbegin(), a.crend(), [](int x) {
        return x>10;
    }) << std::endl;

    std::cout << one_of(a.cbegin(), a.cend(), [](int x) {
        return x > 9;
    }) << std::endl;

    std::cout << is_sorted(a.cbegin(), a.cend(), std::less_equal<int>{}) << std::endl;

    std::cout << is_partitioned(a.cbegin(), a.cend(), [](int x) {
        return x < 3;
    }) << std::endl;

    std::cout << *find_not(a.cbegin(), a.cend(), 2) << std::endl;

    std::cout << *(--find_backward(a.cbegin(), a.cend(), 2)) << std::endl;

    std::cout << is_palindrome(a.cbegin(), a.cend(), [](int x) {return x;}) << std::endl;
    std::vector<float> pali1 ({1,2,3,2,1});
    std::vector<int> pali2 ({1,2,2,1});
    std::cout << is_palindrome(pali1.cbegin(), pali1.cend(), [](float x) {return x;}) << std::endl;
    std::cout << is_palindrome(pali2.cbegin(), pali2.cend(), [](int x) {return x;}) << std::endl;
}
