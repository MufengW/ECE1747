#include "charge_points.h"
#include <cmath>

/* Default constructor */
ChargePoint::ChargePoint() : x(0), y(0) {}

/* Parameterized constructor */
ChargePoint::ChargePoint(double x, double y) : x(x), y(y) {}

/* Getter methods */
double ChargePoint::getX() const { return x; }
double ChargePoint::getY() const { return y; }
// double ChargePoint::getDist() const { return distance_to_neighbour; }

/* Setter methods */
void ChargePoint::setX(double x) { this->x = x; }
void ChargePoint::setY(double y) { this->y = y; }
// void ChargePoint::setDist(double distance_to_neighbour) { this->distance_to_neighbour = distance_to_neighbour; }

/* Member function to compute distance between two points */
double ChargePoint::distance(const ChargePoint& p1, const ChargePoint& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(std::pow(dx,2) + std::pow(dy,2));
}

/* Member function to compute force between two points */
double ChargePoint::compute_force(double dist) {
    return k * std::pow(q,2) / std::pow(dist,2);
}
