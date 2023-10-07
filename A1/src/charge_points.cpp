#include "charge_points.h"
#include <cmath>

/* Default constructor */
ChargePoint::ChargePoint() : x(0), y(0) {}

/* Parameterized constructor */
ChargePoint::ChargePoint(int x, int y) : x(x), y(y) {}

/* Getter methods */
int ChargePoint::getX() const { return x; }
int ChargePoint::getY() const { return y; }
double ChargePoint::getDist() const { return distance_to_neighbour; }

/* Setter methods */
void ChargePoint::setX(int x) { this->x = x; }
void ChargePoint::setY(int y) { this->y = y; }
void ChargePoint::setDist(double distance_to_neighbour) { this->distance_to_neighbour = distance_to_neighbour; }

/* Member function to compute distance between two points */
double ChargePoint::distance(const ChargePoint& p1, const ChargePoint& p2) {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}
