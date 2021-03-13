#include <iostream>
#include <vector>
#include <stdexcept>

bool double_eq(double d1, double d2, double epsilon = 0.005) {
    return std::abs(d1-d2) < epsilon;
}

class Polynomial {
public:
    explicit Polynomial(std::vector<double> coefs, double all_coef = 1) : coefs_(std::move(coefs)) , all_coef_(all_coef) {}

    Polynomial(const Polynomial&) = default;

    ~Polynomial() = default;

    Polynomial& operator=(const Polynomial&) = default;

    double operator[](unsigned i) const {
        if (i<coefs_.size()) {
            return coefs_[i]*all_coef_;
        }
        return 0;
    }

    friend bool operator==(const Polynomial& p1, const Polynomial& p2) {
        for (int i = 0; i < std::max(p1.coefs_.size(), p2.coefs_.size()); ++i) {
            if (!double_eq(p1[i], p2[i])) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const Polynomial& p1, const Polynomial& p2) {
        return !(p1 == p2);
    }

    friend Polynomial operator+(const Polynomial& p1, const Polynomial& p2) {
        std::vector<double> new_coefs;
        for (int i = 0; i < std::max(p1.coefs_.size(), p2.coefs_.size()); ++i) {
            new_coefs.push_back(p1[i] + p2[i]);
        }
        return Polynomial (new_coefs);
    }

    friend Polynomial operator-(const Polynomial& p1, const Polynomial& p2) {
        return p1 + (-p2);
    }

    friend Polynomial operator*(const Polynomial& p, double k) {
        return Polynomial (p.coefs_, p.all_coef_ * k);
    }

    friend Polynomial operator*(double k, const Polynomial& p) {
        return p*k;
    }

    friend Polynomial operator/(const Polynomial& p, double k) {
        if (double_eq(k, 0)) {
            throw std::invalid_argument("division by zero");
        }
        return p * (1/k);
    }

    Polynomial operator-() const {
        return *this * (-1);
    }

    Polynomial& operator+=(const Polynomial& other) {
        for (int i = 0; i < coefs_.size(); ++i) {
            coefs_[i] = (*this)[i] + other[i];
        }
        for (int i = coefs_.size(); i < other.coefs_.size(); ++i) {
            coefs_.push_back(other[i]);
        }
        all_coef_ = 1;
        return *this;
    }

    Polynomial& operator-=(const Polynomial& other) {
        for (int i = 0; i < coefs_.size(); ++i) {
            coefs_[i] = (*this)[i] - other[i];
        }
        for (int i = coefs_.size(); i < other.coefs_.size(); ++i) {
            coefs_.push_back(other[i]);
        }
        all_coef_ = 1;
        return *this;
    }

    Polynomial& operator*=(double k) {
        all_coef_ *= k;
        return *this;
    }

    Polynomial& operator/=(double k) {
        all_coef_ /= k;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polynomial& p) {
        bool first = true;
        int n = p.coefs_.size();
        for (int i = 0; i < n; ++i) {
            if (!double_eq(p[n-1-i], 0)) {
                if (first) {
                    first = false;
                }
                else {
                    os << " + ";
                }

            }
        }
        if (first) {
            os << 0;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream* is, Polynomial& p) {

    }

protected:
    double all_coef_;
    std::vector<double> coefs_;
};

int main() {

    return 0;
}
