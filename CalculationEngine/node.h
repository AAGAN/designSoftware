#pragma once
#include <string>
#include "boostheader.h"
#include <vector>

/**
	node class
	a node can be any of the following types:
	
	0- nozzle (orifice is a part of a nozzle)
	1- elbow (has an angle) 
	2- manifold outlet (the first node in the piping system)
	3- bullhead tee (side branch is first input for properties)
	4- side-through tee (side branch is first, inlet is second, through outlet is third input for properties)
	5- coupling (can change the diameter, has pressure drop)
	10 - iFlow valve (on a cylinder)
	11 - CV-98 valve (on a cylinder)
*/
class pipe;
class node
{
protected:
	int id; 
	
	std::string type;
	
	int pipe1;
	int pipe2;
	int pipe3;
	
	quantity<length> x; //!< X coordinate
	quantity<length> y; //!< Y coordinate
	quantity<length> z; //!< Z coordinate

	quantity<length> orifice_diameter; //!< for nozzles

public:
	node();
	~node();

	int pipe1id;
	int pipe2id;
	int pipe3id;

	int index;

	node
	(
		int Id,
		int Type,
		int Pipe1_id,
		int Pipe2_id,
		int Pipe3_id,
		quantity<length> x_coord,
		quantity<length> y_coord,
		quantity<length> z_coord
	);

	int get_id() { return id; }
	void set_id(int node_id) { id = node_id; }

	quantity<length> get_x() { return x; }
	void set_x(quantity<length> x_coordinate) { x = x_coordinate; }

	quantity<length> get_y() { return y; }
	void set_y(quantity<length> y_coordinate) { y = y_coordinate; }

	quantity<length> get_z() { return z; }
	void set_z(quantity<length> z_coordinate) { z = z_coordinate; }

	quantity<length> get_orifice_diameter() { return orifice_diameter; }
	void set_orifice_diameter(quantity<length> diam) { orifice_diameter = diam; }

	void add_pipe1(int pp1) { pipe1 = pp1; }
	int get_pipe1() { return pipe1; }
	void add_pipe2(int pp2) { pipe2 = pp2; }
	int get_pipe2() { return pipe2; }
	void add_pipe3(int pp3) { pipe3 = pp3; }
	int get_pipe3() { return pipe3; }

	std::string get_type() { return type; }
};