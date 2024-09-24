#include "Graph.hh"
#include <iostream>
#include <unordered_map>
#include "topology/shapes/Triangle.hh"

std::list<TriangleNode *>::iterator remove_triangle(std::list<TriangleNode *>::iterator it_p, TriangleGraph &graph_p)
{
	TriangleNode * triangle_l = *it_p;
	for(std::shared_ptr<Edge> &edge_l : triangle_l->edges)
	{
		if (edge_l->tr1 == triangle_l)
		{
			edge_l->tr1 = nullptr;
		}
		else if(edge_l->tr2 == triangle_l)
		{
			edge_l->tr2 = nullptr;
		}
	}
	delete triangle_l;
	return graph_p.nodes.erase(it_p);
}

Graph createFromTriangleList(std::vector<Triangle> const &triangles_p)
{
	Graph graph_l;

	for(std::size_t i = 0 ; i < triangles_p.size() ; ++ i)
	{
		graph_l.nodes.push_back(Node());
		// create node
		graph_l.nodes.back().triangle = &triangles_p[i];
		graph_l.nodes.back().id = i;
		for(std::size_t j = 0 ; j < i ; ++ j)
		{
			// connect nodes
			if(are_connected(*graph_l.nodes[i].triangle, *graph_l.nodes[j].triangle))
			{
				graph_l.nodes[i].neighbours.push_back(j);
				graph_l.nodes[j].neighbours.push_back(i);
			}
		}
	}

	return graph_l;
}

void merge_common_edge(TriangleNode *a, TriangleNode *b)
{
	std::size_t idx_a = 0;
	std::size_t idx_b = 0;
	for(std::size_t i = 0 ; i < 3 ; ++ i)
	{
		for(std::size_t j = 0 ; j < 3 ; ++ j)
		{
			if(a->edges[i]->points == b->edges[j]->points)
			{
				idx_a = i;
				idx_b = j;
				break;
			}
		}
	}

	a->edges[idx_a] = b->edges[idx_b];
	a->edges[idx_a]->tr2 = a;
}

TriangleGraph * createTriangleGraphFromTriangleList(std::vector<Triangle> const &triangles_p)
{
	TriangleGraph *graph_l = new TriangleGraph();

	std::vector<TriangleNode *> vecNodes_l;

	for(std::size_t i = 0 ; i < triangles_p.size() ; ++ i)
	{
		TriangleNode *tr_l = new TriangleNode();
		tr_l->points = triangles_p[i].points;
		graph_l->nodes.push_back(tr_l);
		vecNodes_l.push_back(tr_l);
		// create edges
		tr_l->edges[0] = std::make_shared<Edge>();
		tr_l->edges[0]->tr1 = tr_l;
		tr_l->edges[0]->points[0] = triangles_p[i].points[0];
		tr_l->edges[0]->points[1] = triangles_p[i].points[1];
		tr_l->edges[1] = std::make_shared<Edge>();
		tr_l->edges[1]->tr1 = tr_l;
		tr_l->edges[1]->points[0] = triangles_p[i].points[1];
		tr_l->edges[1]->points[1] = triangles_p[i].points[2];
		tr_l->edges[2] = std::make_shared<Edge>();
		tr_l->edges[2]->tr1 = tr_l;
		tr_l->edges[2]->points[0] = triangles_p[i].points[2];
		tr_l->edges[2]->points[1] = triangles_p[i].points[0];
	}

	for(std::size_t i = 0 ; i < triangles_p.size() ; ++ i)
	{
		for(std::size_t j = 0 ; j < i ; ++ j)
		{
			// merge edge
			if(are_connected(*vecNodes_l[i], *vecNodes_l[j]))
			{
				merge_common_edge(vecNodes_l[i], vecNodes_l[j]);
			}
		}
	}

	return graph_l;
}

Graph createFromTriangleGraph(TriangleGraph const &triangleGraph_p)
{
	Graph graph_l;

	std::unordered_map<TriangleNode *, std::size_t> indexes_l;

	auto &&it_l = triangleGraph_p.nodes.begin();
	for(std::size_t i = 0 ; i < triangleGraph_p.nodes.size() ; ++ i, ++it_l)
	{
		graph_l.nodes.push_back(Node());
		// create node
		graph_l.nodes.back().triangle = *it_l;
		graph_l.nodes.back().id = i;
		// save index
		indexes_l[*it_l] = i;
	}

	// connect all
	it_l = triangleGraph_p.nodes.begin();
	for(std::size_t i = 0 ; i < triangleGraph_p.nodes.size() ; ++ i, ++it_l)
	{
		for(TriangleNode * neighbour_l : get_neighbours(*it_l))
		{
			graph_l.nodes[i].neighbours.push_back(indexes_l[neighbour_l]);
		}
	}

	return graph_l;
}

//	Fonction cheminPlusCourt(g:Graphe, objectif:Nœud, depart:Nœud)
//		closedList = File()
//		openList = FilePrioritaire(comparateur = compareParHeuristique)
//		openList.ajouter(depart)
//		tant que openList n'est pas vide
//			u = openList.defiler()
//			si u.x == objectif.x et u.y == objectif.y
//				reconstituerChemin(u)
//				terminer le programme
//			pour chaque voisin v de u dans g
//				si non(   v existe dans closedList
//							 ou v existe dans openList avec un coût inférieur)
//					 v.cout = u.cout +1
//					 v.heuristique = v.cout + distance([v.x, v.y], [objectif.x, objectif.y])
//					 openList.ajouter(v)
//			closedList.ajouter(u)
//		terminer le programme (avec erreur)

#include <set>
#include <vector>

struct Label
{
	std::size_t node = 0;
	long long cost = 0;
	long long heuristic = 0;
	bool opened = false;
	bool closed = false;
	std::size_t prev = 0;

	bool operator<(Label const &other_p) const
	{
		if(heuristic == other_p.heuristic)
		{
			return node < other_p.node;
		}
		return heuristic < other_p.heuristic;
	}
};

template<typename T>
struct comparator_ptr
{
	bool operator()(T const *a, T const *b) const { return *a < *b; }
};

long long cost(Node const &from_p, Node const &to_p)
{
	return square_distance(from_p.triangle->center, to_p.triangle->center);
}

long long heuristic(Node const &from_p, Node const &to_p)
{
	return cost(from_p, to_p);
}

std::vector<std::size_t> shortest_path(Graph const &graph_p, Node const &source_p, Node const &target_p)
{
	// pointer to the labels list
	std::set<Label const *, comparator_ptr<Label> > open_list_l;
	// one label per node at most
	std::vector<Label> labels;

	// init labels
	labels.resize(graph_p.nodes.size(), Label());
	for(std::size_t i = 0 ; i < graph_p.nodes.size() ; ++ i)
	{
		labels[i].node = i;
	}
	// init start
	open_list_l.insert(&labels[source_p.id]);
	labels[source_p.id].opened = true;

	bool foundPath_l = false;

	while(!open_list_l.empty())
	{
		Label const * cur_l = *open_list_l.begin();
		Node const & node_l = graph_p.nodes[cur_l->node];
		open_list_l.erase(open_list_l.begin());
		if(cur_l->node == target_p.id)
		{
			// found path
			foundPath_l = true;
			break;
		}
		for(std::size_t n : node_l.neighbours)
		{
			if(graph_p.nodes[n].forbidden)
			{
				continue;
			}
			long long cost_l = cur_l->cost + cost(graph_p.nodes[cur_l->node], graph_p.nodes[n]);
			long long heur_l = cost_l + heuristic(graph_p.nodes[n], target_p);
			if(!labels[n].closed && (!labels[n].opened || labels[n].heuristic > heur_l))
			{
				// remove
				if(labels[n].opened)
					open_list_l.erase(open_list_l.find(&labels[n]));
				// update
				labels[n].cost = cost_l;
				labels[n].heuristic = heur_l;
				labels[n].opened = true;
				labels[n].prev = cur_l->node;
				// reinsert
				open_list_l.insert(&labels[n]);
			}
		}
		labels[cur_l->node].closed = true;
		labels[cur_l->node].opened = false;
	}

	// if path
	if(foundPath_l)
	{
		std::vector<std::size_t> reversed_path_l;
		std::size_t cur_l = target_p.id;
		while(cur_l != source_p.id)
		{
			reversed_path_l.push_back(cur_l);
			cur_l = labels[cur_l].prev;
		}
		std::vector<std::size_t> path_l;
		path_l.push_back(source_p.id);
		for(auto &&rit_l = reversed_path_l.rbegin() ; rit_l != reversed_path_l.rend() ;++rit_l)
		{
			path_l.push_back(*rit_l);
		}
		return path_l;
	}
	return {};
}

std::ostream & print_graph(std::ostream &oss_p, TriangleGraph const &g)
{
	oss_p<<"graph["<<std::endl;
	std::set<std::shared_ptr<Edge>> edges_l;
	for(auto && node_l : g.nodes)
	{
		edges_l.insert(node_l->edges.begin(), node_l->edges.end());
	}
	for(auto && e : edges_l)
	{
		print_edge(oss_p, *e.get())<<std::endl;
	}
	oss_p<<"]"<<std::endl;
	return oss_p;
}
