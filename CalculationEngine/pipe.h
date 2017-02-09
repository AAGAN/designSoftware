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
	int type; //Threaded = 1, Welded = 2
	double massPerUnitLength;
	double maxFlowRate;
	double minFlowRate;
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
	int schedule; // 1 = 40 or 2 = 80 for now

	//these are the indecies of the nodes in the nodes vector in a hazard
	int node1_index;
	int node2_index;

	double maxPressure;
	double minPressure;

	double pipe_length;
	
	//diameter at each end of the pipe, these two are always the same!
	double diameter;
	

	double massFlowRate;

	double internal_diameter;

	
	
public:
	pipe();
	~pipe();

	//ids of the nodes passed by the interface
	int node1id;
	int node2id;

	//index of the pipe in the pipes vector of a hazard
	int index;

	double p1; //pressure at the beginneing
	double p2; //pressure at the end

	//if true, the user has defined and fixed the size of the pipe
	bool user_defined_size;

	pipe
	(
		int Id,
		int Type,
		int Node1id,
		int Node2id,
		double Diameter,
		int connectionType,
		int schedule
	);

	int get_id() { return id; }
	void set_id(int pipe_Id) { id = pipe_Id; }

	int get_schedule() { return schedule; }
	void set_schedule(int sch) { schedule = sch; }

	void set_diameter(double diam) { diameter = diam; }
	double get_diameter() { return diameter; }

	double get_length() { return pipe_length; }
	void set_pipe_length(double lng) { pipe_length = lng; }
	
	void add_node1_index(int nd1) { node1_index = nd1; }
	int get_node1_index() { return node1_index; }
	void add_node2_index(int nd2) { node2_index = nd2; }
	int get_node2_index() { return node2_index; }

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
		double mass_per_unit_Length,
		double max_flow_rate,
		double min_flow_rate
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

	void set_mass_flow_rate(double MFR) { massFlowRate = MFR; }
	double get_mass_flow_rate() { return massFlowRate; }
};