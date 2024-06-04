#pragma once

#include <iostream>

struct Point
{
    Point() = default;
    Point(long long x_p, long long y_p) : x(x_p), y(y_p) {}
    long long x;
    long long y;
};

long long operator*(Point const &a, Point const &b);
Point operator-(Point const &a, Point const &b);
Point operator+(Point const &a, Point const &b);
Point operator/(Point const &a, long long d);
bool operator==(Point const &a, Point const &b);
bool operator!=(Point const &a, Point const &b);
std::ostream &operator<<(std::ostream& oss_p, Point const &p);

long long square_distance(Point const &a, Point const &b);
