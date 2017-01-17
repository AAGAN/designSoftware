#pragma once
#include <hazard.h>
#include <string>
#include "boostheader.h"
#include <cmath>
#include <vector>

/**
pipe data structure to store the information related to pipes
*/
struct pData
{
	int schedule;
	quantity<length> nominalSize;
	quantity<length> thickness;
	quantity<length> internalDiameter;
	quantity<dimensionless> frictionFactor;
	quantity<pressure> maximumPressureRating;
	//quantity<pressure> minimumPressureRating;//should be removed!
	int type; //Threaded = 1, Welded = 2
	quantity<mass_per_unit_length_unit> massPerUnitLength;
};

/**
valve data structure to store the information related to valves
*/
struct vData
{
	quantity<length> nominalSize;
	std::string type;
	quantity<length> equivalentLength;
};

/*!
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

	int node1;
	int node2;

	

	quantity<pressure> maxPressure;
	quantity<pressure> minPressure;

	quantity<length> pipe_length;
	
	quantity<length> diameter1;
	quantity<length> diameter2;

	quantity<mass_flow_rate_unit> massFlowRate;
	
public:
	pipe();
	~pipe();

	int node1id;
	int node2id;

	int index;

	quantity<pressure> p1;
	quantity<pressure> p2;

	pipe
	(
		int Id,
		int Type,
		int Node1id,
		int Node2id,
		quantity<length> Diameter1,
		quantity<length> Diameter2
	);

	int get_id() { return id; }
	void set_id(int pipe_Id) { id = pipe_Id; }

	quantity<length> get_length() { return pipe_length; }
	void set_pipe_length(quantity<length> lng) { pipe_length = lng; }
	
	void add_node1(int nd1) { node1 = nd1; }
	int get_node1() { return node1; }
	void add_node2(int nd2) { node2 = nd2; }
	int get_node2() { return node2; }

	std::string get_type() { return type; }

	int calculate_pressure_drop();
	void set_p1(quantity<pressure> p) { p1 = p; }
	void set_p2(quantity<pressure> p) { p2 = p; }

	static int addPipeSizeData
	(
		int schedule,
		quantity<length> nominalSize,
		quantity<length> thickness,
		quantity<length> internalDiameter,
		quantity<dimensionless> ff,
		quantity<pressure> maxP,
		int Type,
		quantity<mass_per_unit_length_unit> mass_per_unit_length
	);

	static std::vector<pData> pipeData;

	static int addValveSizeData
	(
		quantity<length> nominal_size,
		std::string type,
		quantity<length> equivalent_length
	);

	static std::vector<vData> valveData;
};