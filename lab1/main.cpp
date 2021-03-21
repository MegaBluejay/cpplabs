#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>

bool double_eq(double d1, double d2, double epsilon=0.005) {
    return std::abs(d1 - d2) < epsilon;
}


class Shape {
    virtual double area() const = 0;
};

class Point: public Shape {
public:
    Point(double x, double y) : x_(x), y_(y) {}

    Point(const Point&) = default;

    Point& operator=(const Point&) = default;

    double dist(const Point& other) const {
        return std::sqrt(std::pow(x_-other.x_, 2) + std::pow(y_-other.y_, 2));
    }

    double area() const override {
        return 0;
    }

    double x() const {
        return x_;
    }

    double y() const {
        return y_;
    }

    friend bool operator==(const Point& p1, const Point& p2) {
        return (p1.x() == p2.x()) && (p1.y() == p2.y());
    }
    friend bool operator!=(const Point& p1, const Point& p2) {
        return !(p1 == p2);
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "(" << point.x() << ", " << point.y() << ")";
        return os;
    }

protected:
    double x_;
    double y_;
};

class Line: public Shape {
public:
    explicit Line(std::vector<Point>  points) : points_(std::move(points)) {}

    explicit Line(const std::vector<double>& coords) {
        if (coords.size()%2) {
            throw std::invalid_argument("odd number of coordinates");
        }
        for (int i = 0; i < coords.size() / 2; ++i) {
            points_.push_back(Point(coords[i*2], coords[i*2+1])); // emplace? who?
        }
    }

    double area() const override {
        return 0;
    }

    Line(const Line&) = default;

    Line& operator=(const Line&) = default;

    virtual double length() const {
        double sum = 0;
        for (int i = 0; i < points_.size() - 1; ++i) {
            sum += points_[i].dist(points_[i+1]);
        }
        return sum;
    }

    int n_points() const {
        return points_.size();
    }

    virtual unsigned n_segments() const {
        return points_.size()-1;
    }

    virtual const Point& operator[](int i) const {
        return points_[i];
    }

    virtual std::ostream& print(std::ostream& os) const {
        os << "[";
        bool fst = true;
        for (const Point& p : points_) {
            if (fst) {
                fst = false;
            }
            else {
                os << ", ";
            }
            os << p;
        }
        os << "]";
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const Line& line) {
        line.print(os);
        return os;
    }

protected:
    std::vector<Point> points_;
};

class ClosedLine: public Line {
public:
    using Line::Line;

    double length() const override {
        if (points_.size() > 1) {
            return Line::length() + points_[0].dist(points_[points_.size() - 1]);
        } else {
            return 0;
        }
    }

    unsigned n_segments() const override {
        return points_.size();
    }

    const Point &operator[](int i) const override {
        return points_[i % n_points()];
    }

    virtual std::ostream& print(std::ostream& os) const override {
        os << "Closed(";
        os << *(Line*)this;
    }
};
class Polygon: public Shape {
public:
    explicit Polygon(const std::vector<Point>& points) : Polygon(ClosedLine(points)) {}
    explicit Polygon(ClosedLine  line) : line_(std::move(line)) {
        if (!is_valid_polygon()) {
            throw std::invalid_argument ("not a polygon");
        }
    }
    explicit Polygon(const std::vector<double>& coords) : Polygon(ClosedLine(coords)) {}

    Polygon(const Polygon&) = default;

    Polygon& operator=(const Polygon&) = default;

    int n_points() const {
        return line_.n_points();
    }

    double perimeter() const {
        return line_.length();
    }

    double area() const override {
        double sum = 0;
        for (int i = 0; i < n_points(); ++i) {
            sum += line_[i].x()*line_[i+1].y() - line_[i+1].x()*line_[i].y();
        }
        return std::abs(sum)/2;
    }

protected:
    ClosedLine line_;
    bool is_valid_polygon() const {
        if (n_points() < 3) {
            return false;
        }
        Point old_point = line_[n_points()-2];
        Point new_point = line_[n_points()-1];
        double new_dir = std::atan2(new_point.y()-old_point.y(), new_point.x()-old_point.x());
        double old_dir;
        double angle_sum = 0;
        int orient;
        for (int i = 0; i < n_points(); ++i) {
            old_point = new_point;
            old_dir = new_dir;
            new_point = line_[i];
            new_dir = std::atan2(new_point.y()-old_point.y(), new_point.x()-old_point.x());
            if (old_point == new_point) {
                return false;
            }
            double angle = new_dir - old_dir;
            if (angle <= -M_PI) {
                angle += 2*M_PI;
            }
            else if (angle > M_PI) {
                angle -= 2*M_PI;
            }
            if (i == 0) {
                if (double_eq(angle, 0)) {
                    return false;
                }
                orient = angle > 0 ? 1 : -1;
            }
            else {
                if (orient*angle <= 0) {
                    return false;
                }
            }
            angle_sum += angle;
        }
        return std::abs(std::round(angle_sum / (2*M_PI))) == 1;
    }
};

class Triangle : public Polygon {
public:
    explicit Triangle(const ClosedLine& line) : Polygon(line) {
        if (!is_valid_triangle()) {
            throw std::invalid_argument("not a triangle");
        }
    }

    explicit Triangle(const std::vector<Point>& points) : Triangle(ClosedLine(points)) {}

    explicit Triangle(const std::vector<double>& coords) : Triangle(ClosedLine(coords)) {}

protected:
    bool is_valid_triangle() {
        return n_points() == 3;
    }
};

class Trapezoid : public Polygon {
public:
    explicit Trapezoid(const ClosedLine& line) : Polygon(line) {
        if (!is_valid_trapezoid()) {
            throw std::invalid_argument("not a trapezoid");
        }
    }

    explicit Trapezoid(const std::vector<Point>& points) : Trapezoid(ClosedLine(points)) {}

    explicit Trapezoid(const std::vector<double>& coords) : Trapezoid(ClosedLine(coords)) {}

protected:
    bool is_valid_trapezoid() {
        if (n_points() != 4) {
            return false;
        }
        int n_parallel = 0;
        for (int i = 0; i < 2; ++i) {
            Point v1s = line_[i];
            Point v1e = line_[i+1];
            Point v2s = line_[i+2];
            Point v2e = line_[i+3];
            double v1x = v1e.x()-v1s.x();
            double v1y = v1e.y()-v1s.y();
            double v2x = v2e.x()-v2s.x();
            double v2y = v2e.y()-v2s.y();
            if (double_eq(v1x*v2y, v1y*v2x)) {
                n_parallel++;
            }
        }
        return n_parallel == 1;
    }
};

class RegularPolygon : public Polygon {
public:
    explicit RegularPolygon(const ClosedLine& line) : Polygon(line) {
        if (!is_valid_regular_polygon()) {
            throw std::invalid_argument("not a regular polygon");
        }
    }

    explicit RegularPolygon(const std::vector<Point>& points) : RegularPolygon(ClosedLine(points)) {}

    explicit RegularPolygon(const std::vector<double>& coords) : RegularPolygon(ClosedLine(coords)) {}

protected:
    bool is_valid_regular_polygon() {
        double good_length = perimeter() / n_points();
        double good_angle = ((n_points() - 2) * M_PI) / n_points();
        Point old_point = line_[n_points()-2];
        Point new_point = line_[n_points()-1];
        double new_dir = std::atan2(new_point.y()-old_point.y(), new_point.x()-old_point.x());
        double old_dir;
        for (int i = 0; i < n_points(); ++i) {
            old_point = new_point;
            old_dir = new_dir;
            new_point = line_[i];
            new_dir = std::atan2(new_point.y()-old_point.y(), new_point.x()-old_point.x());
            if (!double_eq(old_point.dist(new_point), good_length)) {
                return false;
            }
            double angle = new_dir-old_dir;
            if (angle <= -M_PI) {
                angle += 2*M_PI;
            }
            else if (angle > M_PI) {
                angle -= 2 * M_PI;
            }
            if (!double_eq(M_PI-std::abs(angle), good_angle)) {
                return false;
            }
        }
        return true;
    }
};

double radians(double degrees) {
    return (degrees*M_PI) / 180;
}

int main() {
//    std::vector<double> coords;
//    std::vector<int> inds {0,2,4,1,3};
//    std::vector<int> inds {0,1,2,3,4};
//    for (int i: inds) {
//        coords.push_back(5*std::cos(radians(90+72*i)));
//        coords.push_back(5*std::sin(radians(90+72*i)));
//    }
//    RegularPolygon star (coords);
//    std::cout << star.perimeter() <<std::endl;

    std::vector<Line*> lines;
    std::vector<double> coords {0,0,0,1,1,1};
    lines.push_back(new Line (coords));
    lines.push_back(new ClosedLine (coords));
    std::cout << lines[0]->n_segments() << std::endl;
    std::cout << lines[1]->n_segments() << std::endl;
}