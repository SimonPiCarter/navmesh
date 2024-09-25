#include "Delaunay.hh"

#include <exception>
#include <iostream>
#include <list>
#include <set>

#include "graph/Graph.hh"

std::shared_ptr<Edge> &get_common_edge(TriangleNode *a, TriangleNode const *b)
{
    for(size_t i = 0 ; i < 3 ; ++ i)
    {
        for(size_t j = 0 ; j < 3 ; ++ j)
        {
            if(a->edges[i] == b->edges[j])
            {
                return a->edges[i];
            }
        }
    }
    static std::shared_ptr<Edge> default_l;
    return default_l;
}

std::list<TriangleNode *>::iterator get_triangle(Point const &new_p, TriangleGraph &graph_p)
{
    for(auto &&it_l = graph_p.nodes.begin() ; it_l != graph_p.nodes.end() ; ++it_l)
    {
        TriangleNode *node_l = *it_l;
        if(is_inside(*node_l, new_p))
        {
            return it_l;
        }
    }
    throw std::logic_error("no triangle found for new point.");
}

void replace_triangle(std::shared_ptr<Edge> &edge_p, TriangleNode const * old_p, TriangleNode * new_p)
{
    if (edge_p->tr1 == old_p)
    {
        edge_p->tr1 = new_p;
    }
    else if(edge_p->tr2 == old_p)
    {
        edge_p->tr2 = new_p;
    }
    else
    {
        throw std::logic_error("could not replace triangle");
    }
}

void set_triangle(std::shared_ptr<Edge> &edge_p, TriangleNode * new_p)
{
    if(edge_p->tr1 == new_p || edge_p->tr2 == new_p)
    {
        return;
    }
    replace_triangle(edge_p, nullptr, new_p);
}

void add_edges_from_list(TriangleNode *tr_p, std::shared_ptr<Edge> &base_p, std::vector<std::shared_ptr<Edge>> const &edges_p)
{
    std::shared_ptr<Edge> &cur_l = base_p;
    std::shared_ptr<Edge> first_l;
    tr_p->edges[0] = cur_l;
    size_t i = 1;
    for(std::shared_ptr<Edge> other_l : edges_p)
    {
        if(first_l.get()
        && (
            ((cur_l->points[0] == other_l->points[0] || cur_l->points[1] == other_l->points[0])
              &&(first_l->points[0] == other_l->points[1] || first_l->points[1] == other_l->points[1]))
            ||((cur_l->points[0] == other_l->points[1] || cur_l->points[1] == other_l->points[1])
              &&(first_l->points[0] == other_l->points[0]|| first_l->points[1] == other_l->points[0]))
        ))
        {
            tr_p->edges[i] = other_l;
            ++i;
        }

        if(!first_l
        && ((cur_l->points[0] == other_l->points[0]
            || cur_l->points[0] == other_l->points[1]
            || cur_l->points[1] == other_l->points[0]
            || cur_l->points[1] == other_l->points[1])))
        {
            first_l = other_l;
            tr_p->edges[i] = first_l;
            ++i;
        }
    }
    if(i != 3)
    {
        throw std::logic_error("Failed to fill edges from list");
    }
}

std::array<TriangleNode *, 3> split_node(Point const &new_p, TriangleNode *node_p)
{
    std::array<TriangleNode *, 3> new_nodes_l;

    std::vector<std::shared_ptr<Edge>> new_edges_l;
    new_edges_l.push_back(nullptr);
    new_edges_l.push_back(nullptr);
    new_edges_l.push_back(nullptr);

    for(size_t i = 0 ; i < 3 ; ++i)
    {
        Point const &point_l = node_p->points[i];
        new_edges_l[i] = std::make_shared<Edge>();
        new_edges_l[i]->points[0] = node_p->points[i];
        new_edges_l[i]->points[1] = new_p;
    }

    size_t idx = 0;
    for(std::shared_ptr<Edge> e : node_p->edges)
    {
        replace_triangle(e, node_p, nullptr);
        TriangleNode * new_node_l = new TriangleNode();
        add_edges_from_list(new_node_l, e, new_edges_l);
        new_node_l->update_points();
        new_nodes_l[idx] = new_node_l;
        ++idx;
    }

    for(TriangleNode * node_l : new_nodes_l)
    {
        // Set all edges to be connected to the new node
        for(std::shared_ptr<Edge> &edge_l : node_l->edges)
        {
            set_triangle(edge_l, node_l);
        }
        for(TriangleNode * other_l : new_nodes_l)
        {
            if(node_l == other_l)
            {
                break;
            }
            std::shared_ptr<Edge> &common_l = get_common_edge(node_l, other_l);
            if(common_l)
            {
                set_triangle(common_l, node_l);
                set_triangle(common_l, other_l);
            }
        }
    }

    for(TriangleNode * node_l : new_nodes_l)
    {
        for(std::shared_ptr<Edge> &edge_l : node_l->edges)
        {
            if(edge_l->tr1 == nullptr && edge_l->tr2 == nullptr)
            {
                set_triangle(edge_l, node_l);
            }
        }
    }


    return new_nodes_l;
}

Point const & get_other_point(Triangle const *tr_p, Edge const &edge_p)
{
    for(size_t i = 0 ; i < 3 ; ++ i)
    {
        if(tr_p->points[i] != edge_p.points[0]
        && tr_p->points[i] != edge_p.points[1])
        {
            return tr_p->points[i];
        }
    }
    throw std::logic_error("logical error");
}

bool is_valid(Edge const &edge_p)
{
    if(!edge_p.tr1 || !edge_p.tr2)
    {
        return true;
    }
    if(is_inside_circle(*edge_p.tr1, get_other_point(edge_p.tr2, edge_p)))
    {
        return false;
    }
    if(is_inside_circle(*edge_p.tr2, get_other_point(edge_p.tr1, edge_p)))
    {
        return false;
    }
    return true;
}

void remove_edges(std::vector<std::shared_ptr<Edge>> &edges_p, TriangleNode *tr_p)
{
    for(auto &&it_l = edges_p.begin() ; it_l != edges_p.end() ;)
    {
        if(*it_l == tr_p->edges[0]
        || *it_l == tr_p->edges[1]
        || *it_l == tr_p->edges[2])
        {
            it_l = edges_p.erase(it_l);
        }
        else
        {
            ++it_l;
        }
    }
}

void flip_edge(Edge &edge_p, TriangleGraph &graph_p)
{
    // safe guard that should be useless
    if(!edge_p.tr1 || !edge_p.tr2) { return; }

    // comprmised triangles
    std::vector<TriangleNode *> ct = {edge_p.tr1, edge_p.tr2};

    // updated triangles (around the compromised ones)
    std::vector<TriangleNode *> ut;

    // edges of updated triangles (around the compromised ones)
    std::vector<std::shared_ptr<Edge>> ut_edges;

    for(TriangleNode *tr : ct)
    {
        for(std::shared_ptr<Edge> &e : tr->edges)
        {
            TriangleNode * other_triangle = get_other_triangle(e, tr);
            if(other_triangle != ct[0] && other_triangle != ct[1])
            {
                ut_edges.push_back(e);
                // remove old triangles
                replace_triangle(e, tr, nullptr);
                if(other_triangle)
                {
                    ut.push_back(other_triangle);
                }
            }
        }
    }

    // compromised edge (to be flipped)
    std::shared_ptr<Edge> common_l = get_common_edge(ct[0], ct[1]);

    // new edge
    std::shared_ptr<Edge> new_edge_l = std::make_shared<Edge>();
    new_edge_l->points[0] = get_other_point(ct[0], *common_l);
    new_edge_l->points[1] = get_other_point(ct[1], *common_l);

    // new triangles
    TriangleNode * new_triangle1_l = new TriangleNode();
    add_edges_from_list(new_triangle1_l, new_edge_l, ut_edges);
    new_triangle1_l->update_points();

    // remove used edges from the list
    remove_edges(ut_edges, new_triangle1_l);

    TriangleNode * new_triangle2_l = new TriangleNode();
    add_edges_from_list(new_triangle2_l, new_edge_l, ut_edges);
    new_triangle2_l->update_points();

    std::swap(*ct[0], *new_triangle1_l);
    std::swap(*ct[1], *new_triangle2_l);

    set_triangle(new_edge_l, ct[0]);
    set_triangle(new_edge_l, ct[1]);

    for(TriangleNode * tr : ct)
    {
        for(std::shared_ptr<Edge> &e : tr->edges)
        {
            set_triangle(e, tr);
        }
    }
    for(TriangleNode * tr : ut)
    {
        if(get_common_edge(tr, ct[0]))
            set_triangle(get_common_edge(tr, ct[0]), ct[0]);
        if(get_common_edge(tr, ct[1]))
            set_triangle(get_common_edge(tr, ct[1]), ct[1]);
    }

    delete new_triangle1_l;
    delete new_triangle2_l;
}


void insert_point(Point const &new_p, TriangleGraph &graph_p)
{
    auto && it_l = get_triangle(new_p, graph_p);
    TriangleNode * node_l = *it_l;
    if(has_point(new_p, *node_l))
    {
        return;
    }

    // Split triangle in three
    std::array<TriangleNode *, 3> new_triangles_l = split_node(new_p, node_l);

	// Add edges of old triangle to open edges
	std::list<std::shared_ptr<Edge>> open_edges_l {
		node_l->edges[0],
		node_l->edges[1],
		node_l->edges[2]
	};

	while(!open_edges_l.empty())
	{
		std::shared_ptr<Edge> cur_l = open_edges_l.front();
		open_edges_l.pop_front();

		if(!is_valid(*cur_l.get()))
		{
			flip_edge(*cur_l.get(), graph_p);
		}
	}

    graph_p.nodes.erase(it_l);
    graph_p.nodes.insert(graph_p.nodes.end(), new_triangles_l.begin(), new_triangles_l.end());
}