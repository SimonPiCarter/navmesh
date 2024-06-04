#include "Node.hh"

std::ostream &operator<<(std::ostream &oss_p, Edge const &edge_p)
{
    return oss_p<<"("<<edge_p.points[0].x<<","<<edge_p.points[0].y<<")"
                <<"-"<<"("<<edge_p.points[1].x<<","<<edge_p.points[1].y<<")";
}

std::ostream &operator<<(std::ostream &oss_p, TriangleNode const &tr_p)
{
    return oss_p<<"("<<*tr_p.edges[0]<<")"
                <<"("<<*tr_p.edges[1]<<")"
                <<"("<<*tr_p.edges[2]<<")";
}

void TriangleNode::update_points()
{
    points[0] = edges[0]->points[0];
    points[1] = edges[0]->points[1];
    if(edges[1]->points[1] != points[0]
    && edges[1]->points[1] != points[1])
    {
        points[2] = edges[1]->points[1];
    }
    else
    {
        points[2] = edges[1]->points[0];
    }
}

TriangleNode * get_other_triangle(std::shared_ptr<Edge> &edge_p, TriangleNode *tr_p)
{
    if(edge_p->tr1 != tr_p)
    {
        return edge_p->tr1;
    }
    return edge_p->tr2;
}

std::vector<TriangleNode *> get_neighbours(TriangleNode *tr_p)
{
	std::vector<TriangleNode *> vect_l;
	for(std::shared_ptr<Edge> &edge_l : tr_p->edges)
	{
		TriangleNode * other_l = get_other_triangle(edge_l, tr_p);
		if(other_l)
		{
			vect_l.push_back(other_l);
		}
	}
	return vect_l;
}

std::ostream & print_edge(std::ostream &oss_p, Edge const &e)
{
    oss_p<<e<<std::endl;
    if(e.tr1)
    {
        oss_p<<"\t"<<*e.tr1<<std::endl;
    }
    else
    {
        oss_p<<"\tnull"<<std::endl;
    }
    if(e.tr2)
    {
        oss_p<<"\t"<<*e.tr2<<std::endl;
    }
    else
    {
        oss_p<<"\tnull"<<std::endl;
    }
    return oss_p;
}