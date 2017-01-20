#pragma once
#include <hazard.h>
#include <string>
#include <vector>

/**
	node class
	a node can be any of the following types:
	
	0- nozzle (orifice is a part of a nozzle), has required quantity associated with it
	1- elbow (has an angle) 
	2- manifold outlet (the first node in the piping system)
	3- bullhead tee (side branch is first input for properties, pipe 2 is second and pipe 3 is third)
	4- side-through tee (side branch is first, inlet is second, through outlet is third input for properties)
	5- coupling (can change the diameter, has pressure drop)
	10 - iFlow valve (on a cylinder)
	11 - CV-98 valve (on a cylinder)
*/
class hazard;
class pipe;
class node
{
protected:
	int id; 
	
	std::string type;
	int connection_type; //!< threaded = 1 , welded = 2
	
	int pipe1;
	int pipe2;
	int pipe3;
	
	double x; //!< X coordinate
	double y; //!< Y coordinate
	double z; //!< Z coordinate

	double orifice_diameter; //!< only for nozzles
	double required_gas_quantity; //!< only for nozzles
	double supplied_gas_quantity; //!< only for nozzles

	double pressureDrop1; //!< pressure drop across the node (if tee, pressure drop across input and first side)
	double pressureDrop2; //!< pressure drop across the node (if not tee then 0, if tee, pressure drop across input and second side or through)

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
		double x_coord,
		double y_coord,
		double z_coord,
		double suppliedGasQuantity,
		double requiredGasQuantity
	);

	int get_id() { return id; }
	void set_id(int node_id) { id = node_id; }

	double get_x() { return x; }
	void set_x(double x_coordinate) { x = x_coordinate; }

	double get_y() { return y; }
	void set_y(double y_coordinate) { y = y_coordinate; }

	double get_z() { return z; }
	void set_z(double z_coordinate) { z = z_coordinate; }

	double get_orifice_diameter() { return orifice_diameter; }
	void set_orifice_diameter(double diam) { orifice_diameter = diam; }

	void add_pipe1(int pp1) { pipe1 = pp1; }
	int get_pipe1() { return pipe1; }
	void add_pipe2(int pp2) { pipe2 = pp2; }
	int get_pipe2() { return pipe2; }
	void add_pipe3(int pp3) { pipe3 = pp3; }
	int get_pipe3() { return pipe3; }

	std::string get_type() { return type; }

	int update_hydraulics(hazard& Haz);

	double equivalent_length_1; //!<equivalent length in 1st direction
	double equivalent_length_2; //!<equivalent length in 2nd direction (only for tees)

	int set_equivalent_length(double, std::string);

	double calculate_mass_flow_rate(double sTime);
};