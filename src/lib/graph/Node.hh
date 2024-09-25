#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#include "topology/Point.hh"
#include "topology/shapes/Triangle.hh"

struct Triangle;

struct Node
{
    std::size_t id = 0;
    Triangle const * triangle = nullptr;
    std::vector<std::size_t> neighbours;
	bool forbidden = false;
};

struct TriangleNode;

struct Edge
{
    std::array<Point, 2> points;
    TriangleNode * tr1 = nullptr;
    TriangleNode * tr2 = nullptr;
};

struct TriangleNode : public Triangle
{
    std::array<std::shared_ptr<Edge>, 3> edges;

    void update_points();
};

bool has_point(Point const &p, TriangleNode const &tr_p);
/// @brief get the other triangle connected to the edge
TriangleNode * get_other_triangle(std::shared_ptr<Edge> const &edge_p, TriangleNode *tr_p);
std::vector<TriangleNode *> get_neighbours(TriangleNode *tr_p);
/// @brief get the triangle opposite of the edge not containing the given point
TriangleNode * get_opposite_triangle(Point const &pt_p, TriangleNode *tr_p);
/// @brief get the vertex of the triangle that is not in the given edge
Point const & get_opposite_vertex(std::shared_ptr<Edge> const &edge_p, TriangleNode *tr_p);
/// @brief check if the point is on the right of the edge (point order is important here)
bool is_on_the_right(Point const &pt_p, Edge const &edge_p);
bool is_on_the_edge(Point const &pt_p, Edge const &edge_p);

std::ostream &operator<<(std::ostream &oss_p, Edge const &edge_p);
std::ostream &operator<<(std::ostream &oss_p, TriangleNode const &tr_p);

std::ostream & print_edge(std::ostream &oss_p, Edge const &e);
