#include "pipe.h"
#include "node.h"

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
	quantity<pressure> minP,
	int Type,
	quantity<si::mass> mass
)
{
	availablePipeSizes++; //this needs to be initialized somewhere
	pipeData[availablePipeSizes].schedule = schedule;
	pipeData[availablePipeSizes].nominalSize = nominalSize;
	pipeData[availablePipeSizes].thickness = thickness;
	pipeData[availablePipeSizes].internalDiameter = internalDiameter;
	pipeData[availablePipeSizes].frictionFactor = ff;
	pipeData[availablePipeSizes].maximumPressureRating = maxP;
	pipeData[availablePipeSizes].minimumPressureRating = minP;
	pipeData[availablePipeSizes].type = Type;
	pipeData[availablePipeSizes].massPerUnitLength = mass;
	return 0;
}

int pipe::addValveSizeData(quantity<length> nominal_size, std::string type, quantity<length> equivalent_length)
{
	availableValveSizes++;
	valveData[availableValveSizes].nominalSize = nominal_size;
	valveData[availableValveSizes].equivalentLength = equivalent_length;
	valveData[availableValveSizes].type = type;
	return 0;
}
