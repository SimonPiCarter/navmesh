#pragma once

#include <set>
#include <vector>

namespace CDT
{

struct Label
{
	std::size_t node = 0;
	double cost = 0;
	double heuristic = 0;
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

template<typename T>
double square_distance(V2d<T> const &a, V2d<T> const &b)
{
	return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}

template<typename T>
inline V2d<T> center(Triangulation<T> const &triangulation, Triangle const &from_p)
{
	V2d<T> sum {0,0};
	for(VertInd const &idx : from_p.vertices)
	{
		sum += triangulation.vertices[idx];
	}
	return {sum.x/3, sum.y/3};
}

template<typename T>
inline double cost(Triangulation<T> const &triangulation, Triangle const &from_p, Triangle const &to_p)
{
	return square_distance(center(triangulation, from_p), center(triangulation, to_p));
}

template<typename T>
inline double heuristic(Triangulation<T> const &triangulation, Triangle const &from_p, Triangle const &to_p)
{
	return cost(triangulation, from_p, to_p);
}

template<typename T>
inline std::vector<std::size_t> shortest_path(Triangulation<T> const &triangulation, std::size_t source, std::size_t target,
    std::vector<bool> const &forbidden_triangles)
{
	// pointer to the labels list
	std::set<Label const *, comparator_ptr<Label> > open_list_l;
	// one label per node at most
	std::vector<Label> labels;

	// init labels
	labels.resize(triangulation.triangles.size(), Label());
	for(std::size_t i = 0 ; i < triangulation.triangles.size() ; ++ i)
	{
		labels[i].node = i;
	}
	// init start
	open_list_l.insert(&labels[source]);
	labels[source].opened = true;
	labels[source].heuristic = heuristic(triangulation, triangulation.triangles[source], triangulation.triangles[target]);

	bool foundPath_l = false;

	while(!open_list_l.empty())
	{
		Label const * cur_l = *open_list_l.begin();
		Triangle const & triangle_l = triangulation.triangles[cur_l->node];
		open_list_l.erase(open_list_l.begin());
		if(cur_l->node == target)
		{
			// found path
			foundPath_l = true;
			break;
		}
		for(TriInd const &n : triangle_l.neighbors)
		{
			if(n == CDT::noNeighbor
			|| forbidden_triangles[n])
			{
				continue;
			}
			double cost_l = cur_l->cost + cost(triangulation, triangle_l, triangulation.triangles[n]);
			double heur_l = cost_l + heuristic(triangulation, triangulation.triangles[n], triangulation.triangles[target]);
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

	std::size_t reached_target = target;

	// else look for closest triangle
	if(!foundPath_l)
	{
		double closest_l = std::numeric_limits<double>::max();
		std::size_t best = target;
		for(Label const &l : labels)
		{
			if(!l.closed) { continue; }
			double h = heuristic(triangulation, triangulation.triangles[l.node], triangulation.triangles[target]);
			if(h < closest_l)
			{
				std::cout<<"found "<<l.node<<" for h "<<h<<std::endl;
				closest_l = h;
				best = l.node;
			}
		}
		reached_target = best;
	}

	// if path
	std::vector<std::size_t> reversed_path_l;
	std::size_t cur_l = reached_target;
	while(cur_l != source)
	{
		reversed_path_l.push_back(cur_l);
		cur_l = labels[cur_l].prev;
	}
	std::vector<std::size_t> path_l;
	path_l.push_back(source);
	for(auto &&rit_l = reversed_path_l.rbegin() ; rit_l != reversed_path_l.rend() ;++rit_l)
	{
		path_l.push_back(*rit_l);
	}
	return path_l;
}

} // namespace CDT


