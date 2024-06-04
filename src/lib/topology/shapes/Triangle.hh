#pragma once

#include "topology/Point.hh"
#include <array>
#include <vector>
#include <string>

struct Triangle
{
    Triangle() = default;
    Triangle(std::array<Point, 3> const &points_p);
    Triangle(Point const &a, Point const &b, Point const &c);
    std::array<Point, 3> points;
    Point center;
};

/// @brief Check if triangles share at least two point (common edge)
/// @param a first triange
/// @param b seconde triangle
/// @return true if the triangles share an edge two points
bool are_connected(Triangle const &a, Triangle const &b);

/// @brief check if the point in inside the circle triangle
bool is_inside_circle(Triangle const &tr, Point const &p);

/// @brief check if the point in inside the triangle
bool is_inside(Triangle const &tr, Point const &p);

std::string generate_svg(std::vector<Triangle> const &triangles_p);
