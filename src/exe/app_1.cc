#include <iostream>

#include "graph/Graph.hh"
#include "topology/shapes/Triangle.hh"

int main()
{
    std::vector<Triangle> triangles_l;

    triangles_l.push_back(Triangle({0,2}, {0,4}, {2,2}));
    triangles_l.push_back(Triangle({2,6}, {0,4}, {2,2})); // 1
    triangles_l.push_back(Triangle({2,2}, {2,6}, {4,2}));
    triangles_l.push_back(Triangle({2,6}, {4,2}, {4,4}));
    triangles_l.push_back(Triangle({2,6}, {4,6}, {4,4})); // 4
    triangles_l.push_back(Triangle({4,4}, {4,6}, {6,6}));
    triangles_l.push_back(Triangle({4,6}, {6,6}, {6,8}));
    triangles_l.push_back(Triangle({4,6}, {6,8}, {4,8}));

    Graph graph_l = createFromTriangleList(triangles_l);

    // for(int i = 0 ; i < graph_l.nodes.size() ; ++ i)
    // {
    //     std::cout<<"node "<<i<<std::endl;
    //     for(int n = 0; n < graph_l.nodes[i].neighbours.size(); ++n)
    //         std::cout<<"\t- "<<graph_l.nodes[i].neighbours[n]<<std::endl;
    // }

    std::vector<std::size_t> path_l = shortest_path(graph_l, graph_l.nodes[3], graph_l.nodes[0]);

    for(int i = 0 ; i < path_l.size() ; ++i)
    {
        std::cout<<"\t"<<path_l[i]<<std::endl;
    }

    std::cout<<generate_svg(triangles_l)<<std::endl;
    return 0;
}
