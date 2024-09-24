#pragma once

#include <iostream>
#include <list>
#include <vector>
#include "Node.hh"

struct Graph
{
    std::vector<Node> nodes;
};

struct TriangleGraph
{
    std::list<TriangleNode *> nodes;
    ~TriangleGraph() { for(auto && node_l : nodes) delete node_l; }
};

// TriangleGraph
std::list<TriangleNode *>::iterator remove_triangle(std::list<TriangleNode *>::iterator it_p, TriangleGraph &graph_p);

std::ostream & print_graph(std::ostream &oss_p, TriangleGraph const &g);

Graph createFromTriangleList(std::vector<Triangle> const &triangles_p);
TriangleGraph * createTriangleGraphFromTriangleList(std::vector<Triangle> const &triangles_p);
Graph createFromTriangleGraph(TriangleGraph const &triangleGraph_p);

std::vector<std::size_t> shortest_path(Graph const &graph_p, Node const &source_p, Node const &target_p);
