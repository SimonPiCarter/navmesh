#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

#include "graph/Graph.hh"
#include "graph/triangulation/Delaunay.hh"
#include "CDT.h"

namespace godot {


struct FunnelDebug
{
	int old = 0;
	int target = 0;
	int x, y;
	Vector2 orig;
	Vector2 left;
	Vector2 right;
	Vector2 candidate;
	int steps = 0;
};

class Triangulation : public Node2D {
	GDCLASS(Triangulation, Node2D)

public:
	Triangulation() : cdt(CDT::detail::defaults::vertexInsertionOrder, CDT::IntersectingConstraintEdges::TryResolve, 0.001) {}
	~Triangulation() {}
	void init(int x, int y);
	int insert_point(int x, int y, int forbidden);
	void insert_edge(int idx_point_1, int idx_point_2);
	void finalize();
	void debug(int step);

	void select(int x, int y);

	void _physics_process(double delta_p) override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	CDT::Triangulation<double> cdt;
	std::vector<std::pair<CDT::VertInd, CDT::VertInd> > edges;
	std::vector<bool> forbidden_triangles;

	int _size_x = 0;
	int _size_y = 0;
	int _selected = -1;
	std::vector<std::size_t> _path;
	std::vector<Vector2> _funnel;

	FunnelDebug _debug;
};

}
