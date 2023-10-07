#pragma once

class Point {
private:
    double x, y;

public:
    Point();
    Point(double x, double y);

    double getX() const;
    double getY() const;

    void setX(double x);
    void setY(double y);

    // Member function to compute distance between two points
    static double distance(const Point& p1, const Point& p2);
};
