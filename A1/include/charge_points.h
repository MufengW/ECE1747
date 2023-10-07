#pragma once

class ChargePoint {
private:
    int x, y;
    double distance_to_neighbour;

public:
    ChargePoint();
    ChargePoint(int x, int y);

    int getX() const;
    int getY() const;
    double getDist() const;

    void setX(int x);
    void setY(int y);
    void setDist(double distance_to_neighbour);

    /* Member function to compute distance between two points */
    static double distance(const ChargePoint& p1, const ChargePoint& p2);
};
