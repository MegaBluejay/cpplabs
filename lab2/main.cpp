#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <regex>

#include "rational.h"

class Polynomial {
public:
    Polynomial() = default;

    Polynomial (const Polynomial&) = default;

    Polynomial& operator=(const Polynomial&) = default;

    explicit Polynomial(std::map<int, Rational> map) : ks_(std::move(map)) {
        for (auto const& [i,k] : ks_) {
            if (i<0) {
                throw std::invalid_argument("negative power");
            }
        }
    }

    explicit Polynomial(std::vector<Rational> vec) {
        for (int i = 0; i < vec.size(); ++i) {
            if (vec[i] != 0) {
                ks_[i] = vec[i];
            }
        }
    }

    explicit Polynomial(const std::string& s) {
        std::stringstream ss (s);
        ss >> (*this);
        if (ss.fail()) {
            throw std::invalid_argument("couldn't parse polynomial");
        }
    }

    void clean() {
        for (auto it = ks_.cbegin(); it != ks_.cend();) {
            if (it->second == 0) {
                ks_.erase(it++);
            }
            else {
                ++it;
            }
        }
    }

    bool operator==(const Polynomial& other) const {
        for (auto const& [i,k] : ks_) {
            if (k != other[i]) {
                return false;
            }
        }
        for (auto const& [i,k] : other.ks_) {
            if (k != (*this)[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Polynomial& other) const {
        return ks_ != other.ks_;
    }

    Rational& operator[](int i) {
        if (!ks_.count(i)) {
            ks_[i] = 0;
        }
        return ks_[i];
    }

    const Rational& operator[](int i) const {
        static const Rational zero;
        if (!ks_.count(i)) {
            return zero;
        }
        return ks_.at(i);
    }

    Polynomial& operator+=(const Polynomial& other) {
        for (auto const& [i,k] : other.ks_) {
            (*this)[i] += k;
        }
        return *this;
    }

    Polynomial& operator-=(const Polynomial& other) {
        for (auto const& [i,k] : other.ks_) {
            (*this)[i] -= k;
        }
        return *this;
    }

    Polynomial& operator*=(int a) {
        for (auto const& [i,k] : ks_) {
            ks_[i]*=a;
        }
        return *this;
    }

    Polynomial& operator*=(const Polynomial& other) {
        // todo in-place
        std::map<int, Rational> new_ks;
        for (auto const& [i1,k1] : ks_) {
            for (auto const& [i2,k2]: other.ks_) {
                if (!new_ks.count(i1+i2)) {
                    new_ks[i1+i2] = 0;
                }
                new_ks[i1+i2] += k1*k2;
            }
        }
        ks_ = new_ks;
        return *this;
    }

    Polynomial& operator/=(int a) {
        for (auto const& [i,k] : ks_) {
            ks_[i] /= a;
        }
        return *this;
    }

    friend Polynomial operator+(Polynomial l, const Polynomial& r) {
        l += r;
        return l;
    }

    friend Polynomial operator-(Polynomial l, const Polynomial& r) {
        l -= r;
        return l;
    }

    friend Polynomial operator*(Polynomial l, int a) {
        l*=a;
        return l;
    }

    friend Polynomial operator*(Polynomial l, const Polynomial& r) {
        l*=r;
        return l;
    }

    friend Polynomial operator*(int a, Polynomial r) {
        r*=a;
        return r;
    }

    friend Polynomial operator/(Polynomial l, int a) {
        l /= a;
        return l;
    }

    friend Polynomial operator-(const Polynomial& r) {
        return (-1)*r;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polynomial& p) {
        bool fst = true;
        for (auto it = p.ks_.crbegin(); it != p.ks_.crend(); ++it) {
            auto [i,k] = *it;
            if (k != 0) {
                if (k < 0) {
                    if (fst) {
                        os << "-";
                    }
                    else {
                        os << " - ";
                    }
                }
                else if (!fst) {
                    os << " + ";
                }
                if (fst) {
                    fst = false;
                }
                if (k.abs() != 1) {
                    os << k.abs();
                }
                if (i != 0) {
                    os << "x";
                    if (i != 1) {
                        os << "^" << i;
                    }
                }
            }
        }
        if (fst) {
            os << 0;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Polynomial& p) {
        p.ks_ = std::map<int, Rational> ();
        std::string line;
        std::getline(is, line);
        std::regex rgx (R"(\s*(\+|-)\s*)");
        std::vector<std::string> elems;
        std::sregex_token_iterator  iter (line.begin(), line.end(), rgx, {-1,1});
        std::sregex_token_iterator end;
        for (; iter != end; iter++) {
            if ((*iter).length() != 0) {
                elems.push_back(*iter);
            }
        }

        if (elems.empty()) {
            is.setstate(std::ios::failbit);
            return is;
        }
        int sign = 1;
        int start = 0;
        if (elems[0] == "-") {
            sign = -1;
            start = 1;
        }
        else if (elems.size()%2 == 0) {
            is.setstate(std::ios::failbit);
            return is;
        }
        for (int i = start; i < elems.size(); i++) {
            if ((i-start)%2 == 0) {
                std::stringstream ss (elems[i]);
                int j = 0;
                Rational k;
                if (ss.peek() == 'x') {
                    k = 1;
                }
                else {
                    ss >> k;
                }
                if (k < 0) {
                    is.setstate(std::ios::failbit);
                    return is;
                }
                if (!ss.eof() && ss.peek() == 'x') {
                    ss.ignore();
                    j = 1;
                    if (!ss.eof() && ss.peek() == '^') {
                        ss.ignore();
                        ss >> j;
                        if (j < 0) {
                            is.setstate(std::ios::failbit);
                            return is;
                        }
                    }
                }
                if (ss.fail() || !ss.eof()) {
                    is.setstate(std::ios::failbit);
                    return is;
                }
                p[j] += sign*k;
            }
            else if (elems[i] == "-") {
                sign = -1;
            }
            else if (elems[i] == "+") {
                sign = 1;
            }
            else {
                is.setstate(std::ios::failbit);
                return is;
            }
        }

        return is;
    }

protected:
    std::map<int, Rational> ks_;
};

int main() {
    Polynomial p;
    std::stringstream ss ("10/7x^2-3/2x^2");
    ss >> p;
    std::cout << p << std::endl;
}