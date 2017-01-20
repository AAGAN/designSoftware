#pragma once
#include <hazard.h>
#include <string>
#include <cmath>
#include <vector>

/**
pipe data structure to store the information related to pipes
*/
struct pData
{
	int schedule;
	double nominalSize;
	double thickness;
	double internalDiameter;
	double frictionFactor;
	double maximumPressureRating;
	//quantity<pressure> minimumPressureRating;//should be removed!
	int type; //Threaded = 1, Welded = 2
	double massPerUnitLength;
};

/**
valve data structure to store the information related to valves
*/
struct vData
{
	double nominalSize;
	std::string type;
	double equivalentLength;
};

/**
	pipe class 
	a pipe can be any of the following types:

	0- straight pipe
	1- valve (needs pressure coefficient)
*/
class node;
class pipe
{
protected:
	int id;

	std::string type;
	int connection_type; // threaded = 1, welded = 2

	int node1;
	int node2;

	

	double maxPressure;
	double minPressure;

	double pipe_length;
	
	double diameter1;
	double diameter2;

	double massFlowRate;

	double internal_diameter;
	
public:
	pipe();
	~pipe();

	int node1id;
	int node2id;

	int index;

	double p1;
	double p2;

	pipe
	(
		int Id,
		int Type,
		int Node1id,
		int Node2id,
		double Diameter1,
		double Diameter2,
		int connectionType
	);

	int get_id() { return id; }
	void set_id(int pipe_Id) { id = pipe_Id; }

	double get_length() { return pipe_length; }
	void set_pipe_length(double lng) { pipe_length = lng; }
	
	void add_node1(int nd1) { node1 = nd1; }
	int get_node1() { return node1; }
	void add_node2(int nd2) { node2 = nd2; }
	int get_node2() { return node2; }

	std::string get_type() { return type; }

	int calculate_pressure_drop();
	void set_p1(double p) { p1 = p; }
	void set_p2(double p) { p2 = p; }

	static int addPipeSizeData
	(
		int schedule,
		double nominalSize,
		double thickness,
		double internalDiameter,
		double ff,
		double maxP,
		int Type,
		double mass_per_unit_Length
	);

	static std::vector<pData> pipeData;

	static int addValveSizeData
	(
		double nominal_size,
		std::string type,
		double equivalent_length
	);

	static std::vector<vData> valveData;

	double get_min_mass_flow_rate();
	double get_max_mass_flow_rate();
};