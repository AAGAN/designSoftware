#include "pipe.h"


pipe::pipe()
{
}

pipe::~pipe()
{
}

pipe::pipe
(
	int Id, 
	int Type, 
	int Node1id, 
	int Node2id, 
	double Diameter1, 
	double Diameter2,
	int connectionType
)
:id(Id),node1id(Node1id),node2id(Node2id),diameter1(Diameter1),diameter2(Diameter2),connection_type(connectionType)
{
	switch (Type)
	{
	case 0:
		type = "Straight Pipe";
		break;
	case 1:
		type = "Valve";
		break;
	case 2:
		type = "Manifold";
		break;
	default:
		break;
	}
}

/**
calculates the pressure drop starting from p1 (pressure at node1)
*/
int pipe::calculate_pressure_drop()
{
	return 0;
}

/**
populates the pipe data structure "pipeData"
*/
int pipe::addPipeSizeData
(
	int schedule,
	double nominalSize,
	double thickness,
	double internalDiameter,
	double ff,
	double maxP,
	int Type,
	double mass_per_unit_length
)
{
	pData pipeDATA = {};
	pipeDATA.schedule = schedule;
	pipeDATA.nominalSize = nominalSize;
	pipeDATA.thickness = thickness;
	pipeDATA.internalDiameter = internalDiameter;
	pipeDATA.frictionFactor = ff;
	pipeDATA.maximumPressureRating = maxP;
	pipeDATA.type = Type;
	pipeDATA.massPerUnitLength = mass_per_unit_length;
	pipeData.push_back(pipeDATA);
	return 0;
}

int pipe::addValveSizeData
(
	double nominal_size,
	std::string type, 
	double equivalent_length
)
{
	vData valveDATA = {};
	valveDATA.nominalSize = nominal_size;
	valveDATA.type = type;
	valveDATA.equivalentLength = equivalent_length;
	valveData.push_back(valveDATA);
	return 0;
}

/**

*/
double pipe::get_min_mass_flow_rate()
{
	double MIN_MFR_SI;
	double minMassFlowRate;
	double ID;
	ID = internal_diameter * 0.0254; //conversion from meters to inches
	//this equation is derived experimentally and the output is in lb/min
	minMassFlowRate = 0.567*(2.07 - 10.388*ID + 32.82*pow(ID, 2) + 1.696*pow(ID, 3.0));
	minMassFlowRate *= 0.00756; //conversion from lb/min to kg/s
	MIN_MFR_SI = minMassFlowRate;
	return MIN_MFR_SI;
}

double pipe::get_max_mass_flow_rate()
{
	return 10.582 * get_min_mass_flow_rate();
}