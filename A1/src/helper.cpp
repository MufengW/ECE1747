#include "helper.h"
#include <cmath>

/* Default constructor */
Point::Point() : x(0), y(0) {}

/* Parameterized constructor */
Point::Point(double x, double y) : x(x), y(y) {}

/* Getter methods */
double Point::getX() const { return x; }
double Point::getY() const { return y; }

/* Setter methods */
void Point::setX(double x) { this->x = x; }
void Point::setY(double y) { this->y = y; }

/* Member function to compute distance between two points */
double Point::distance(const Point& p1, const Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}
