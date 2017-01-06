#include "node.h"

node::node()
{
}

node::~node()
{
}

node::node(int Id, int Type, int Pipe1_id, int Pipe2_id, int Pipe3_id, quantity<length> x_coord, quantity<length> y_coord, quantity<length> z_coord)
	:id(Id), pipe1id(Pipe1_id), pipe2id(Pipe2_id),pipe3id(Pipe3_id),x(x_coord),y(y_coord),z(z_coord)
{
	switch (Type)
	{
	case 0:
		type = "Nozzle";
		break;
	case 1:
		type = "Elbow";
		break;
	case 2:
		type = "Manifold Outlet";
		break;
	case 3:
		type = "Bull Tee";
		break;
	case 4:
		type = "Side Tee";
		break;
	case 5:
		type = "Coupling";
		break;
	case 6:
		type = "Free Node";
		break;
	case 10:
		type = "iFlow Valve";
		break;
	case 11:
		type = "CV98 Valve";
		break;
	default:
		break;
	}
	pressureDrop1 = 0 * pascals;
	pressureDrop2 = 0 * pascals;
}

/**
calculates the pressure across the node, uses the pressure 
from the end of the input pipe and assigns the 
calculated pressure(s) to the beginning of the pipe(s) connected
to the node
*/
int node::update_hydraulics(hazard& Haz)
{
	//calculate pressureDrop1
	//calculate PressureDrop2

	Haz.pipes[pipe2id].set_p1(Haz.pipes[pipe1id].p2 - pressureDrop1);
	if (type == "Bull Tee" || type == "Side Tee")
		Haz.pipes[pipe3id].set_p1(Haz.pipes[pipe1id].p2 - pressureDrop2);
	return 0;
}

