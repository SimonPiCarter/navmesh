#include "Triangulation.h"

#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

void Triangulation::init(int x, int y)
{
    std::vector<Triangle> triangles_l;

    triangles_l.push_back(Triangle({0,0}, {x,0}, {0,y}));
    triangles_l.push_back(Triangle({x,y}, {x,0}, {0,y}));

	_graph = createTriangleGraphFromTriangleList(triangles_l);
	_path_graph = createFromTriangleGraph(*_graph);
	_size_x = x;
	_size_y = y;

	queue_redraw();
}

void Triangulation::insert_point(int x, int y, int forbidden)
{
	forbidden_points[x][y] = forbidden;
	if(x >= 0 && x <= _size_x && y >= 0 && y <= _size_y && _graph)
	{
    	::insert_point({x, y}, *_graph);
	}

	for(auto &&it_l = _graph->nodes.begin(); _graph->nodes.end() != it_l ;)
	{
		TriangleNode * node_l = *it_l;
		int idx_1 = forbidden_points[node_l->points[0].x][node_l->points[0].y];
		int idx_2 = forbidden_points[node_l->points[1].x][node_l->points[1].y];
		int idx_3 = forbidden_points[node_l->points[2].x][node_l->points[2].y];
		if(idx_1 == idx_2 && idx_2 == idx_3 && idx_1 != 0)
		{
			it_l = remove_triangle(it_l, *_graph);
		}
		else
		{
			it_l++;
		}
	}
	_path_graph = createFromTriangleGraph(*_graph);
	// reset selection
	_selected = -1;
	_path = {};
	queue_redraw();
}

void Triangulation::select(int x, int y)
{
	int old_l = _selected;
	int idx_l = 0;
	// reset selection
	_selected = -1;
	_path = {};
	for(TriangleNode const *tr_l : _graph->nodes)
	{
		if(is_inside(*tr_l, Point {x, y}))
		{
			_selected = idx_l;
			break;
		}
		++idx_l;
	}
	if(old_l>=0 && _selected>=0)
	{
		_path = shortest_path(_path_graph, _path_graph.nodes[old_l], _path_graph.nodes[_selected]);
		UtilityFunctions::print("path");
		for(size_t i : _path)
		{
			UtilityFunctions::print("\t", i);
		}
	}
	queue_redraw();
}

void Triangulation::_physics_process(double delta_p)
{
}

void Triangulation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("init"), &Triangulation::init);
	ClassDB::bind_method(D_METHOD("insert_point", "x", "y"), &Triangulation::insert_point);
	ClassDB::bind_method(D_METHOD("select", "x", "y"), &Triangulation::select);
}

void Triangulation::_draw()
{
	if(_graph)
	{
		int idx_l = 0;
		for(::Node const &node : _path_graph.nodes)
		{
			for(size_t i = 0 ; i < 3 ; ++ i)
			{
				Color color_l = Color(1.,1.,1.,1.);
				double thickness_l = -1.;
				if(_selected == idx_l)
				{
					color_l = Color(0.,1.,0.,1.);
					thickness_l = 2.;
				}
				else if(std::find(_path.begin(), _path.end(), static_cast<size_t>(idx_l)) != _path.end())
				{
					color_l = Color(0.,0.,1.,1.);
					thickness_l = 2.;
				}
				else if(node.forbidden)
				{
					color_l = Color(1.,0.,0.,1.);
					thickness_l = 2.;
				}
				draw_line(
					Vector2(node.triangle->points[i].x, node.triangle->points[i].y),
					Vector2(node.triangle->points[(i+1)%3].x, node.triangle->points[(i+1)%3].y),
					color_l,
					thickness_l
				);
			}
			++idx_l;
		}
	}
}


}