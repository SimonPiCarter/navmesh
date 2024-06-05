#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

#include "graph/Graph.hh"
#include "graph/triangulation/Delaunay.hh"

namespace godot {

class Triangulation : public Node2D {
	GDCLASS(Triangulation, Node2D)

public:
	~Triangulation() { delete _graph; }
	void init(int x, int y);
	void insert_point(int x, int y);
	void select(int x, int y);

	void _physics_process(double delta_p) override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	TriangleGraph *_graph = nullptr;
	Graph _path_graph;
	int _size_x = 0;
	int _size_y = 0;
	int _selected = 0;
	std::vector<std::size_t> _path;
};

}
