#include "node.h"

node::node()
{
}

node::~node()
{
}

node::node
(
	int Id, 
	int Type, 
	int Pipe1_id, 
	int Pipe2_id, 
	int Pipe3_id, 
	quantity<length> x_coord, 
	quantity<length> y_coord, 
	quantity<length> z_coord,
	quantity<volume> suppliedGasQuantity,
	quantity<volume> requiredGasQuantity
)
	:id(Id), pipe1id(Pipe1_id), 
	pipe2id(Pipe2_id),pipe3id(Pipe3_id),
	x(x_coord),y(y_coord),z(z_coord),
	required_gas_quantity(requiredGasQuantity),
	supplied_gas_quantity(suppliedGasQuantity)
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

	//calculate and assign the flow (mass flow rate) at the beginning of the 
	//next pipes based on the mass flow rate at the end of the previous
	//pipe

	return 0;
}

/**
----------------------------------------
Equivalent length for threaded fittings:
Elbow: 0.5 * 30 * ID
Through Tee: 20 * ID
Side Tee Branch: 60 * ID
Coupling: 5 * ID
----------------------------------------
Equivalent length for welded fittings:
Elbow: 0.5 * 16 * ID
Through Tee: 13 * ID
Side Tee branch: 40 * ID
Coupling: 5 * ID
----------------------------------------
Standard eq. length of 90s is halved per request of customer 21/12/2009
*/
int node::set_equivalent_length(quantity<length> internal_diameter, std::string Type)
{
	if(Type == "Elbow")
	{
		if (connection_type == 1)
		{
			equivalent_length_1 = 0.5 * 30.0 * internal_diameter;
			equivalent_length_2 = 0 * meters;
		}
		else if (connection_type == 2)
		{
			equivalent_length_1 = 0.5 * 16.0 * internal_diameter;
			equivalent_length_2 = 0 * meters;
		}
	}
	else if (Type == "Bull Tee")
	{
		if (connection_type == 1)
		{
			equivalent_length_1 = 0.5 * 30.0 * internal_diameter;
			equivalent_length_2 = 0.5 * 30.0 * internal_diameter;
		}
		else if (connection_type == 2)
		{
			equivalent_length_1 = 0.5 * 16.0 * internal_diameter;
			equivalent_length_2 = 0.5 * 16.0 * internal_diameter;
		}
	}
	else if (Type == "Side Tee")
	{
		if (connection_type == 1)
		{
			equivalent_length_1 = 20.0 * internal_diameter;
			equivalent_length_2 = 60.0 * internal_diameter;
		}
		else if (connection_type == 2)
		{
			equivalent_length_1 = 13.0 * internal_diameter;
			equivalent_length_2 = 40.0 * internal_diameter;
		}
	}
	else if (Type == "Coupling")
	{
		equivalent_length_1 = 5.0 * internal_diameter;
		equivalent_length_2 = 0.0 * internal_diameter;
	}
	else
	{
		equivalent_length_1 = 0.0 * meters;
		equivalent_length_2 = 0.0 * meters;
	}
	
}