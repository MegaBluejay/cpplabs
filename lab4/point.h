#ifndef POINT_H
#define POINT_H

#include <cmath>

class Point {
public:

    Point(double x, double y) : x_(x), y_(y) {}

    Point() : Point(0, 0) {}

    double x() const {
        return x_;
    }

    double y() const {
        return y_;
    }

    double norm() const {
        return std::sqrt(std::pow(x_, 2) + std::pow(y_, 2));
    }

    Point& operator+=(const Point& other) {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }

    Point& operator-=(const Point& other) {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }

    Point& operator*=(double scale) {
        x_ *= scale;
        y_ *= scale;
        return *this;
    }

    Point& operator/=(double scale) {
        x_ /= scale;
        y_ /= scale;
        return *this;
    }

    friend Point operator+(Point p1, const Point& p2) {
        p1 += p2;
        return p1;
    }

    friend Point operator-(Point p1, const Point& p2) {
        p1 -= p2;
        return p1;
    }

    friend Point operator*(Point p, double scale) {
        p *= scale;
        return p;
    }

    friend Point operator/(Point p, double scale) {
        p /= scale;
        return p;
    }

    constexpr bool operator==(const Point& other) const {
        return x_ == other.x_ && y_ == other.y_;
    }

    constexpr bool operator!=(const Point& other) const {
        return !((*this) == other);
    }

    double dist(const Point& other) const {
        return ((*this) - other).norm();
    }

protected:
    double x_;
    double y_;
};

struct point_hash {
public:
    std::size_t operator()(const Point& p) const {
        double x = p.x();
        double y = p.y();
        long xi = * (long *) &x;
        long yi = * (long *) &y;
        return (xi<<32)^yi;
    }
};

#endif
