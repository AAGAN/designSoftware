#include "node.h"
#include "boostheader.h"
#include "pipe.h"

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
}

