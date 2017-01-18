#include "pipe.h"


pipe::pipe()
{
}

pipe::~pipe()
{
}

pipe::pipe(int Id, int Type, int Node1id, int Node2id, quantity<length> Diameter1, quantity<length> Diameter2)
:id(Id),node1id(Node1id),node2id(Node2id),diameter1(Diameter1),diameter2(Diameter2)
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
	quantity<length> nominalSize,
	quantity<length> thickness,
	quantity<length> internalDiameter,
	quantity<dimensionless> ff,
	quantity<pressure> maxP,
	int Type,
	quantity<mass_per_unit_length_unit> mass_per_unit_length
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
	quantity<length> nominal_size, 
	std::string type, 
	quantity<length> equivalent_length
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
mass_flow_rate pipe::get_min_mass_flow_rate()
{
	mass_flow_rate MIN_MFR_SI;
	double minMassFlowRate;
	double ID;
	ID = internal_diameter.value() * 0.0254; //conversion from meters to inches
	//this equation is derived experimentally and the output is in lb/min
	minMassFlowRate = 0.567*(2.07 - 10.388*ID + 32.82*pow(ID, 2) + 1.696*pow(ID, 3.0));
	minMassFlowRate *= 0.00756; //conversion from lb/min to kg/s
	MIN_MFR_SI.from_value(minMassFlowRate);
	return MIN_MFR_SI;
}

mass_flow_rate pipe::get_max_mass_flow_rate()
{
	return 10.582 * get_min_mass_flow_rate();
}