#pragma once
#include <limits>
class ChargePoint {
private:
    double x, y;
    // double distance_to_neighbour;

public:
    ChargePoint();
    ChargePoint(double x, double y);

    double getX() const;
    double getY() const;
    // double getDist() const;

    void setX(double x);
    void setY(double y);
    // void setDist(double distance_to_neighbour);

    /* Member function to compute the distance between two points */
    static double distance(const ChargePoint& p1, const ChargePoint& p2);
    double compute_force(double dist);
    const double q = 1.6e-19;
    const double k = 8.99e9;
};
