#include <iostream>

#include <exception>
#include <list>
#include <set>
#include <memory>
#include <sstream>
#include "topology/shapes/Triangle.hh"
#include "graph/Node.hh"
#include "graph/Graph.hh"
#include "graph/triangulation/Delaunay.hh"

std::string _draw_line(Point const &a, Point const &b)
{
    std::stringstream ss_l;
    ss_l<<"<line x1=\""<<a.x<<"\" x2=\""<<b.x<<"\" y1=\""<<a.y<<"\" y2=\""<<b.y<<"\" stroke=\"white\" stroke-width=\"0.01\"/>"<<std::endl;
    return ss_l.str();
}

std::string _generate_svg(TriangleGraph const &graph_p)
{
    std::stringstream ss_l;
    ss_l<<"<svg width=\"10\" height=\"10\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"<<std::endl;
    for(TriangleNode const *tr : graph_p.nodes)
    {
        ss_l<<_draw_line(tr->edges[0]->points[0], tr->edges[0]->points[1]);
        ss_l<<_draw_line(tr->edges[1]->points[0], tr->edges[1]->points[1]);
        ss_l<<_draw_line(tr->edges[2]->points[0], tr->edges[2]->points[1]);
    }
    ss_l<<"</svg>"<<std::endl;
    return ss_l.str();
}

int main(int argc, char *argv[])
{
    std::vector<Triangle> triangles_l;

    triangles_l.push_back(Triangle({0,0}, {10,0}, {0,10}));
    triangles_l.push_back(Triangle({10,10}, {10,0}, {0,10}));

	TriangleGraph &graph_l = *createTriangleGraphFromTriangleList(triangles_l);


    print_graph(std::cout, graph_l);
    // std::cout<<_generate_svg(graph_l)<<std::endl;

    insert_point({2, 2}, graph_l);
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    print_graph(std::cout, graph_l);
    insert_point({4 ,3}, graph_l);
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    print_graph(std::cout, graph_l);
    insert_point({4 ,2}, graph_l);
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;
    insert_point({5 ,3}, graph_l);

    std::cout<<_generate_svg(graph_l)<<std::endl;

	delete &graph_l;

    return 0;
}
