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

TriangleNode * get_other_triangle(std::shared_ptr<Edge> &edge_p, TriangleNode *tr_p);
std::vector<TriangleNode *> get_neighbours(TriangleNode *tr_p);

std::ostream &operator<<(std::ostream &oss_p, Edge const &edge_p);
std::ostream &operator<<(std::ostream &oss_p, TriangleNode const &tr_p);

std::ostream & print_edge(std::ostream &oss_p, Edge const &e);
