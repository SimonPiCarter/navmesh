#include "Triangulation.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <algorithm>
#include <chrono>

#include "ShortestPath.h"
#include "FunnelAlgorithm.h"

namespace godot {

void Triangulation::init(int x, int y)
{
	_size_x = x;
	_size_y = y;

	cdt.insertVertices({{0,0},{x,0},{x,y},{0,y},
		{x/2-50,y/2-50},
		{x/2-50,y/2+50},
		{x/2+50,y/2+50},
		{x/2+50,y/2-50},
		{100+x/2+50,y/2+50},
		{100+x/2+50,y/2-50}
	});
	insert_edge(3,0);
	insert_edge(2,3);
	insert_edge(1,2);
	insert_edge(0,1);
	insert_edge(4,5);
	insert_edge(5,6);
	insert_edge(6,7);
	insert_edge(7,4);
	insert_edge(4,5);
	insert_edge(5,8);
	insert_edge(8,9);
	insert_edge(9,4);

	queue_redraw();
}

int Triangulation::insert_point(int x, int y, int forbidden)
{
	int i = 0;
	for(auto &&v : cdt.vertices)
	{
		if(v.x ==x && v.y ==y)
		{
			return i-3;
		}
		++i;
	}
	cdt.insertVertices({{x,y}});
	int idx = cdt.vertices.size()-4;

	// reset selection
	_selected = -1;
	_path = {};

	forbidden_triangles.resize(cdt.triangles.size(), false);
	queue_redraw();

	return idx;
}

void Triangulation::insert_edge(int idx_point_1, int idx_point_2)
{
	cdt.insertEdges({CDT::Edge(idx_point_1,idx_point_2)});
	edges.push_back(std::make_pair(idx_point_1+3, idx_point_2+3));
	forbidden_triangles.resize(cdt.triangles.size(), false);
}

bool containsPoint(CDT::V2d<double> lineP, CDT::V2d<double> lineQ, CDT::V2d<double> point)
{
    bool xBetween = ((point.x - lineP.x) * (point.x - lineQ.x) <= 0.f);
    bool yBetween = ((point.y - lineP.y) * (point.y - lineQ.y) <= 0.f);
    if (!xBetween || !yBetween) { // early return or can be moved to the end
        return false;
    }
    float dxPQ = lineQ.x - lineP.x;
    float dyPQ = lineQ.y - lineP.y;
    float lineLengthSquared = dxPQ*dxPQ + dyPQ*dyPQ;
    float crossproduct = (point.y - lineP.y) * dxPQ - (point.x - lineP.x) * dyPQ;
    float tolerance = std::numeric_limits<float>::epsilon();
    return crossproduct * crossproduct <= lineLengthSquared * tolerance * tolerance;
}

void Triangulation::finalize()
{
	forbidden_triangles.clear();
	forbidden_triangles.resize(cdt.triangles.size(), false);
	size_t i = 0;
	for(CDT::Triangle const &tr : cdt.triangles)
	{
		for(auto const &edge_l : edges)
		{
			bool p0_on_edge_l = containsPoint(cdt.vertices[edge_l.first], cdt.vertices[edge_l.second], cdt.vertices[tr.vertices[0]]);
			bool p1_on_edge_l = containsPoint(cdt.vertices[edge_l.first], cdt.vertices[edge_l.second], cdt.vertices[tr.vertices[1]]);
			bool p2_on_edge_l = containsPoint(cdt.vertices[edge_l.first], cdt.vertices[edge_l.second], cdt.vertices[tr.vertices[2]]);
			if((p0_on_edge_l && p1_on_edge_l)
			|| (p0_on_edge_l && p2_on_edge_l)
			|| (p1_on_edge_l && p2_on_edge_l))
			{
				CDT::VertInd v = tr.vertices[0];
				if(p0_on_edge_l && p1_on_edge_l)
				{
					v = tr.vertices[2];
				}
				if(p0_on_edge_l && p2_on_edge_l)
				{
					v = tr.vertices[1];
				}
				CDT::PtLineLocation::Enum pos = CDT::locatePointLine(cdt.vertices[v], cdt.vertices[edge_l.first], cdt.vertices[edge_l.second]);
				if(pos == CDT::PtLineLocation::Right)
				{
					forbidden_triangles[i] = true;
				}
			}
		}
		++i;
	}
	queue_redraw();
	// cdt.eraseOuterTrianglesAndHoles();
	// queue_redraw();
}

void Triangulation::select(int x, int y)
{
	int old_l = _selected;
	int idx_l = 0;
	// reset selection
	_selected = -1;
	_path = {};
	for(CDT::Triangle const &tr : cdt.triangles)
	{
		if(CDT::locatePointTriangle({x,y},
			cdt.vertices[tr.vertices[0]],
			cdt.vertices[tr.vertices[1]],
			cdt.vertices[tr.vertices[2]]) != CDT::PtTriLocation::Outside )
		{
			_selected = idx_l;
			break;
		}
		++idx_l;
	}
	if(old_l>=0 && _selected>=0)
	{
		auto start{std::chrono::high_resolution_clock::now()};

		_path = CDT::shortest_path(cdt, old_l, _selected, forbidden_triangles);

		auto end{std::chrono::high_resolution_clock::now()};
		std::chrono::duration<double> elapsed_seconds{end - start};
		UtilityFunctions::print("path - time elapsed : ", elapsed_seconds.count() * 1000., "ms");

		_debug.steps = 10000;
		_debug.old = old_l;
		_debug.target = _selected;
		_debug.x = x;
		_debug.y = y;

		start = std::chrono::high_resolution_clock::now();

		std::vector<CDT::V2d<double>> funnel = CDT::funnel_algorithm(
			cdt,
			_path,
			center(cdt, cdt.triangles[old_l]), CDT::V2d<double>{x,y},
			_debug);
		_debug.steps = 0;
		_funnel.clear();
		for(auto v2 : funnel)
		{
			_funnel.push_back(Vector2(v2.x, v2.y));
		}
		_selected = -1;
		end = std::chrono::high_resolution_clock::now();
		elapsed_seconds = end - start;
		UtilityFunctions::print("funel - time elapsed : ", elapsed_seconds.count() * 1000., "ms");
	}
	queue_redraw();
}

void Triangulation::debug(int step)
{

	if(_debug.old >= 0 && _debug.target >= 0)
	{
		_path = CDT::shortest_path(cdt, _debug.old, _debug.target, forbidden_triangles);

		_debug.steps = step;

		std::vector<CDT::V2d<double>> funnel = CDT::funnel_algorithm(
			cdt,
			_path,
			center(cdt, cdt.triangles[_debug.old]), CDT::V2d<double>{_debug.x,_debug.y},
			_debug);
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
	ClassDB::bind_method(D_METHOD("insert_edge", "x", "y"), &Triangulation::insert_edge);
	ClassDB::bind_method(D_METHOD("finalize"), &Triangulation::finalize);
	ClassDB::bind_method(D_METHOD("select", "x", "y"), &Triangulation::select);
	ClassDB::bind_method(D_METHOD("debug", "step"), &Triangulation::debug);
}

void Triangulation::_draw()
{
	size_t i = 0;
	for(auto && tr : cdt.triangles)
	{
		if(forbidden_triangles[i])
		{
			PackedVector2Array arr;
			arr.append(Vector2(cdt.vertices[tr.vertices[0]].x, cdt.vertices[tr.vertices[0]].y));
			arr.append(Vector2(cdt.vertices[tr.vertices[1]].x, cdt.vertices[tr.vertices[1]].y));
			arr.append(Vector2(cdt.vertices[tr.vertices[2]].x, cdt.vertices[tr.vertices[2]].y));
			draw_colored_polygon(arr, Color(1.,0,0,0.5));
			++i;
			continue;
		}
		Color color_l = Color(1.,1.,1.,1.);
		double thickness_l = i == _selected ? 5. : 1.;
		for(size_t j : _path)
		{
			if(j==i) {
				color_l = Color(0.,0.,1.,0.5);
				thickness_l = 3.;
			}
		}
		if(i == _selected ) {
			color_l = Color(0.,1.,0.,0.5);
		}

		draw_line(
			Vector2(cdt.vertices[tr.vertices[0]].x, cdt.vertices[tr.vertices[0]].y),
			Vector2(cdt.vertices[tr.vertices[1]].x, cdt.vertices[tr.vertices[1]].y),
			color_l,
			thickness_l
		);
		draw_line(
			Vector2(cdt.vertices[tr.vertices[1]].x, cdt.vertices[tr.vertices[1]].y),
			Vector2(cdt.vertices[tr.vertices[2]].x, cdt.vertices[tr.vertices[2]].y),
			color_l,
			thickness_l
		);
		draw_line(
			Vector2(cdt.vertices[tr.vertices[2]].x, cdt.vertices[tr.vertices[2]].y),
			Vector2(cdt.vertices[tr.vertices[0]].x, cdt.vertices[tr.vertices[0]].y),
			color_l,
			thickness_l
		);
		++i;
	}

	for(size_t i = 0 ; i+1 < _funnel.size() ; ++i)
	{
		draw_line(
			_funnel[i],
			_funnel[i+1],
			Color(0.5,1.,0.,0.5),
			3
		);
	}

	if(_debug.steps > 0)
	{
		draw_line(
			_debug.orig,
			_debug.left,
			Color(1.,1,0,0.5),
			5
		);
		draw_line(
			_debug.orig,
			_debug.right,
			Color(0.,1,1,0.5),
			5
		);
		draw_line(
			_debug.orig,
			_debug.candidate,
			Color(1.,0,1,0.5),
			5
		);
	}
}


}