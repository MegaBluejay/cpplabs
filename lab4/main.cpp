#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <regex>

#include "rational.h"
#include "point.h"

namespace lab {

    template<class InputIt, class Pred>
    bool all_of(InputIt it, InputIt end, Pred pred) {
        for (; it != end; ++it) {
            if (!pred(*it)) {
                return false;
            }
        }
        return true;
    }

    template<class InputIt, class Pred>
    bool any_of(InputIt it, InputIt end, Pred pred) {
        for (; it != end; ++it) {
            if (pred(*it)) {
                return true;
            }
        }
        return false;
    }

    template<class InputIt, class Pred>
    bool none_of(InputIt it, InputIt end, Pred pred) {
        return !lab::any_of(it, end, pred);
    }

    template<class InputIt, class Pred>
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

    template<class InputIt, class Comp = std::less<typename InputIt::value_type>>
    bool is_sorted(InputIt it, InputIt end, Comp comp = {}) {
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

    template<class InputIt, class Pred>
    bool is_partitioned(InputIt it, InputIt end, Pred pred) {
        bool snd = false;
        for (; it != end; ++it) {
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

    template<class InputIt, class T>
    InputIt find_not(InputIt it, InputIt end, T t) {
        for (; it != end && (*it) == t; ++it) {}
        return it;
    }

    template<class InputIt, class T>
    InputIt find_backward(InputIt it, InputIt end, T t) {
        InputIt ans = end;
        for (; it != end; ++it) {
            if ((*it) == t) {
                ans = it;
            }
        }
        return ans;
    }

    namespace internal {

        template<class BiDirIt, class Conv>
        bool is_palindrome(BiDirIt sit, BiDirIt eit, Conv conv, std::bidirectional_iterator_tag) {
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

        template<class InputIt, class Conv>
        bool is_palindrome(InputIt sit, InputIt eit, Conv conv, std::input_iterator_tag) {
            using T = typename InputIt::value_type;
            std::vector<T> save;
            std::copy(sit, eit, std::back_inserter(save));
            return is_palindrome(save.cbegin(), save.cend(), conv, std::bidirectional_iterator_tag());
        }

        struct identity {
            template<class T>
            constexpr T&& operator()(T&& t) const {
                return std::forward<T>(t);
            }
        };
    }

    template<class Iter, class Conv = internal::identity>
    bool is_palindrome(Iter sit, Iter eit, Conv conv = {}) {
        return internal::is_palindrome(sit, eit, conv, typename std::iterator_traits<Iter>::iterator_category());
    }
}

#define defop const struct
#define oper(ReturnType, OperatorName, FirstOperandType, SecondOperandType) OperatorName ## _ {} OperatorName; template <typename T> struct OperatorName ## Proxy{public:OperatorName ## Proxy(const T& t) : t_(t){}const T& t_;static ReturnType _ ## OperatorName ## _(const FirstOperandType a, const SecondOperandType b);};template <typename T> OperatorName ## Proxy<T> operator<(const T& lhs, const OperatorName ## _& rhs){return OperatorName ## Proxy<T>(lhs);}ReturnType operator>(const OperatorName ## Proxy<FirstOperandType>& lhs, const SecondOperandType& rhs){return OperatorName ## Proxy<FirstOperandType>::_ ## OperatorName ## _(lhs.t_, rhs);}template <typename T> inline ReturnType OperatorName ## Proxy<T>::_ ## OperatorName ## _(const FirstOperandType a, const SecondOperandType b)

defop oper(Rational, over, int, int) {
    return Rational(a, b);
}
#define over <over>

std::string get_func(const std::string& expr) {
    static const std::regex rgx (R"(lab::(?:internal::)?(\w+)\((\w+)\.)");
    std::smatch match;
    std::regex_search(expr, match, rgx);
    return match[1].str() + "(" + match[2].str() + ")";
}

#define prnt(thing) std::cout << get_func(#thing) << ": " << (thing) << std::endl

int main() {
    std::vector<int> a ({2,2,4,6,8,10,10});
    std::set<Rational> b ({0, 1 over 10, 1 over 10, 1 over 3, 1 over 2, 2 over 3, 9 over 10, 9 over 10, 1});
    std::unordered_set<Point, point_hash> c ({{0,0}, {1, 0}, {1,1}, {0, 1}});

    prnt(lab::all_of(a.cbegin(), a.cend(), [](int x) {
        return 1 - (x % 2);
    }));
    prnt(lab::all_of(b.cbegin(), b.cend(), [](const Rational& r) {
        return r <= 1;
    }));
    prnt(lab::all_of(c.cbegin(), c.cend(), [](const Point& p) {
        return p.x() >= 0 && p.y() >= 0;
    }));

    prnt(lab::any_of(a.begin(), a.end(), [](int x) {
        return x > 7;
    }));
    prnt(lab::any_of(b.begin(), b.end(), [](const Rational& r) {
        return r > (1 over 4);
    }));
    prnt(lab::any_of(c.cbegin(), c.cend(), [](const Point& p) {
        return p.x() == p.y();
    }));

    prnt(lab::none_of(a.crbegin(), a.crend(), [](int x) {
        return x>10;
    }));
    prnt(lab::none_of(b.begin(), b.end(), [](const Rational& r) {
        return r < 0;
    }));
    prnt(lab::none_of(c.cbegin(), c.cend(), [](const Point& p) {
        return p.dist({10, 10}) > 5;
    }));

    prnt(lab::one_of(a.cbegin(), a.cend(), [](int x) {
        return x > 9;
    }));
    prnt(lab::one_of(b.begin(), b.end(), [](const Rational& r) {
        return r.denom() == 1;
    }));

    prnt(lab::is_sorted(a.cbegin(), a.cend(), std::less_equal<int>{}));
    prnt(lab::is_sorted(b.begin(), b.end(), std::less<Rational>{}));

    prnt(lab::is_partitioned(a.cbegin(), a.cend(), [](int x) {
        return x < 3;
    }));
    prnt(lab::is_partitioned(b.begin(), b.end(), [](Rational r) {
        return r < (2 over 3);
    }));

    prnt(*lab::find_not(a.cbegin(), a.cend(), 2));
    prnt(*lab::find_not(b.cbegin(), b.cend(), 0));
    // not applicable to unordered_set

    prnt(*(--lab::find_backward(a.cbegin(), a.cend(), 2)));
    prnt(*(--lab::find_backward(b.cbegin(), b.cend(), 1 over 10)));
    // not applicable to unordered_set

    std::vector<float> pali1 ({1,2,3,2,1});
    std::vector<int> pali2 ({1,2,2,1});
    prnt(lab::is_palindrome(pali1.cbegin(), pali1.cend()));
    prnt(lab::is_palindrome(pali2.cbegin(), pali2.cend()));
    prnt(lab::is_palindrome(b.cbegin(), b.cend(), [](const Rational& r) {
        return std::abs((1 over 2) - r);
    }));
    prnt(lab::internal::is_palindrome(a.cbegin(), a.cend(), [](int x) {
        return std::abs(6 - x);
    }, std::input_iterator_tag()));
    // not applicable to unordered_set
}
