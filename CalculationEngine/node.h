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
	
	//indeces of the pipes connected to the node
	//these indecies are for the vector of pipes in a hazard
	int pipe1_index;
	int pipe2_index;
	int pipe3_index;

	int enclosure_id;
	
	double x; //!< X coordinate
	double y; //!< Y coordinate
	double z; //!< Z coordinate

	double orifice_diameter; //!< only for nozzles
	double required_gas_quantity; //!< only for nozzles
	double supplied_gas_quantity; //!< only for nozzles

	//double pressureDrop1; //!< pressure drop across the node (if tee, pressure drop across input and first side)
	//double pressureDrop2; //!< pressure drop across the node (if not tee then 0, if tee, pressure drop across input and second side or through)

	double density;
	double static_temperature;
	//double dynamic_pressure;
	double static_pressure;

	double equivalent_length_1; //!<equivalent length in 1st direction
	double equivalent_length_2; //!<equivalent length in 2nd direction (only for tees)

public:
	node();
	~node();

	//ids that are passed by the interface
	int pipe1id;
	int pipe2id;
	int pipe3id;

	//index of the node in the nodes vector of a hazard
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
		int enclosureID,
		int connectionType
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

	void add_pipe1_index(int pp1) { pipe1_index = pp1; }
	int get_pipe1_index() { return pipe1_index; }
	void add_pipe2_index(int pp2) { pipe2_index = pp2; }
	int get_pipe2_index() { return pipe2_index; }
	void add_pipe3_index(int pp3) { pipe3_index = pp3; }
	int get_pipe3_index() { return pipe3_index; }

	void set_type(std::string tp) { tp = type; }
	std::string get_type() { return type; }

	//int update_hydraulics(hazard& Haz);

	int set_equivalent_length(double);

	double calculate_nozzle_mass_flow_rate(double sTime);

	double calculate_manifold_pressure(double, int , double, double, double, double);
	double calculate_density(double, double, double, double, double);
	double calculate_density(double, double, double, double);
	double calculate_temperature(double, double, double, double);

	int get_enclosure_id() { return enclosure_id; }

	void set_required_quantity(double reqQ) { required_gas_quantity = reqQ; }
	void set_supplied_quantity(double supQ) { supplied_gas_quantity = supQ; }

	double get_equivalent_length_1() { return equivalent_length_1; }
	double get_equivalent_length_2() { return equivalent_length_2; }

	double get_static_pressure() { return static_pressure; }
	void set_static_pressure(double staticPressure) { static_pressure = staticPressure; }
	double get_static_temperature() { return static_temperature; }
	void set_static_temperature(double staticTemperature) { static_temperature = staticTemperature; }
	double get_density() { return density; }
	void set_density(double Density) { density = Density; }
};