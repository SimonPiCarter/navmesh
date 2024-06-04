#include "Triangulation.h"

namespace godot {

void Triangulation::init(int x, int y)
{
    std::vector<Triangle> triangles_l;

    triangles_l.push_back(Triangle({0,0}, {x,0}, {0,y}));
    triangles_l.push_back(Triangle({x,y}, {x,0}, {0,y}));

	_graph = createTriangleGraphFromTriangleList(triangles_l);
	_size_x = x;
	_size_y = y;

	queue_redraw();
}

void Triangulation::insert_point(int x, int y)
{
	if(x >= 0 && x <= _size_x && y >= 0 && y <= _size_y && _graph)
	{
    	::insert_point({x, y}, *_graph);
	}
	// reset selection
	_selected = -1;
	queue_redraw();
}

void Triangulation::select(int x, int y)
{
	int idx_l = 0;
	for(TriangleNode const *tr_l : _graph->nodes)
	{
		if(is_inside(*tr_l, Point {x, y}))
		{
			_selected = idx_l;
			break;
		}
		++idx_l;
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
		for(TriangleNode const *tr : _graph->nodes)
		{
			for(size_t i = 0 ; i < 3 ; ++ i)
			{
				draw_line(
					Vector2(tr->edges[i]->points[0].x, tr->edges[i]->points[0].y),
					Vector2(tr->edges[i]->points[1].x, tr->edges[i]->points[1].y),
					_selected == idx_l?Color(0.,1.,0.,1.):Color(1.,1.,1.,1.),
					_selected == idx_l?2.:-1.
				);
			}
			++idx_l;
		}
	}
}


}