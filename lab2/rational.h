#ifndef RATIONAL_H
#define RATIONAL_H

int gcd(int a, int b) {
    while (a != 0) {
        int c = a;
        a = b%a;
        b = c;
    }
    return b;
}

class Rational {
public:
    Rational(int num=0, int denom=1) : num_(num), denom_(denom) {
        if (denom_ == 0) {
            throw std::invalid_argument("division by zero");
        }
        num_ = num_ * denom_ / std::abs(denom_);
        denom_ = std::abs(denom_);
        simplify();
    }

    int num() const {
        return num_;
    }

    int denom() const {
        return denom_;
    }

    explicit operator double() {
        return (double) num_/denom_;
    }

    Rational& operator+=(const Rational& other) {
        num_ = num_*other.denom_ + denom_*other.num_;
        denom_ = denom_*other.denom_;
        simplify();
        return *this;
    }

    Rational& operator*=(const Rational& other) {
        num_ *= other.num_;
        denom_ *= other.denom_;
        simplify();
        return *this;
    }

    Rational& operator*=(int a) {
        num_ *= a;
        simplify();
        return *this;
    }

    Rational& operator/=(int a) {
        denom_ *= a;
        simplify();
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        num_ *= other.denom_;
        denom_ *= other.num_;
        simplify();
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        num_ = num_*other.denom_ - denom_*other.num_;
        denom_ = denom_*other.denom_;
        simplify();
        return *this;
    }

    friend Rational operator+(Rational l, const Rational& r) {
        l+=r;
        return l;
    }

    friend Rational operator*(Rational l, const Rational& r) {
        l*=r;
        return l;
    }

    friend Rational operator*(Rational l, int a) {
        l*=a;
        return l;
    }

    friend Rational operator*(int a, Rational r) {
        r*=a;
        return r;
    }

    friend Rational operator-(Rational l, const Rational& r) {
        l-=r;
        return l;
    }

    Rational operator-() {
        return (*this)*(-1);
    }

    friend Rational operator/(Rational l, int a) {
        l/=a;
        return l;
    }

    friend Rational operator/(Rational l, const Rational& r) {
        l /= r;
        return l;
    }

    bool operator==(const Rational& other) const {
        return num_==other.num_ && denom_==other.denom_;
    }

    bool operator!=(const Rational& other) const {
        return !((*this)==other);
    }

    bool operator<(const Rational& other) const {
        return num_*other.denom_ < denom_*other.num_;
    }

    bool operator>(const Rational &other) const {
        return other < *this;
    }

    bool operator<=(const Rational &other) const {
        return !(other < *this);
    }

    bool operator>=(const Rational &other) const {
        return !(*this < other);
    }


    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        os << r.num_;
        if (r.denom_ != 1) {
            os << '/' << r.denom_;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Rational& r) {
        is >> r.num_;
        if (!is.eof() && is.peek() == '/') {
            is.ignore();
            is >> r.denom_;
            if (r.denom_ < 0) {
                is.setstate(std::ios::failbit);
            }
        }
        else {
            r.denom_ = 1;
        }
        r.simplify();
        return is;
    }

protected:
    void simplify() {
        int k = gcd(std::abs(num_), denom_);
        num_ /= k;
        denom_ /= k;
    }
    int num_;
    int denom_;
};

namespace std {
    Rational abs(const Rational& r) {
        return Rational(std::abs(r.num()), r.denom());
    }
}

#endif
