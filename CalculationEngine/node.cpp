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
	double x_coord,
	double y_coord,
	double z_coord,
	int enclosureID,
	int connectionType
)
	:id(Id), pipe1id(Pipe1_id), 
	pipe2id(Pipe2_id),pipe3id(Pipe3_id),
	x(x_coord),y(y_coord),z(z_coord),
	connection_type(connectionType),enclosure_id(enclosureID)
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
	pressureDrop1 = 0;// *pascals;
	pressureDrop2 = 0;// *pascals;
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
int node::set_equivalent_length(double internal_diameter, std::string Type)
{
	if(Type == "Elbow")//the node's type should be used instead!
	{
		if (connection_type == 1)
		{
			equivalent_length_1 = 0.5 * 30.0 * internal_diameter;
			equivalent_length_2 = 0;// *meters;
		}
		else if (connection_type == 2)
		{
			equivalent_length_1 = 0.5 * 16.0 * internal_diameter;
			equivalent_length_2 = 0;// *meters;
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
		equivalent_length_1 = 0.0;// *meters;
		equivalent_length_2 = 0.0;// *meters;
	}
	return 0;
}

/**
calculating the mass flow rate from nozzle
*/
double node::calculate_nozzle_mass_flow_rate(double sTime)
{
	double MFR;
	double inergenDensityat68F;
	inergenDensityat68F = 1.416;// *kilogram_per_cubic_meter;
	MFR = inergenDensityat68F * required_gas_quantity / sTime;
	return MFR; //kilogram per second
}

/**
calculates the manifold pressure
*/
double node::calculate_manifold_pressure(double maxMassFlowRate, int numberOfCylinders, double storagePressure,double storageTemperature,double storageDensity, double specificHeatRatio)
{
	//---------------------------------------------------------------------
	//this is some experimental method to find the pressure at the manifold
	double iManPressureSwitch = 805.0;

	int iCylinderBasis = 1;
	double CylFlow = maxMassFlowRate*2.205 / iCylinderBasis; // 1 kg/s = 2.205 lb/s
	double manifoldPressure = -119.47 * pow(CylFlow, 3) + 251.21*pow(CylFlow, 2) - 152.13*CylFlow + 1162.3;
	if (manifoldPressure > 1175)manifoldPressure = 1175;
	while (iCylinderBasis < numberOfCylinders && manifoldPressure < iManPressureSwitch && numberOfCylinders>1)
	{
		iCylinderBasis++;
		CylFlow = maxMassFlowRate*2.205 / iCylinderBasis; // 1 kg/s = 2.205 lb/s
		manifoldPressure = -119.47 * pow(CylFlow, 3) + 251.21*pow(CylFlow, 2) - 152.13*CylFlow + 1162.3;
		if (manifoldPressure > 1175)manifoldPressure = 1175;
	}
	if (manifoldPressure < 550)manifoldPressure = 550;
	//---------------------------------------------------------------------

	static_pressure = manifoldPressure * 6895.0; // 1 psi = 6895 Pa
	density = calculate_density(static_pressure, specificHeatRatio, storagePressure, storageDensity);
	static_temperature = calculate_temperature(static_pressure, storagePressure, storageTemperature, specificHeatRatio);
	return static_pressure;
}

/**
density based on pressure and storage temperature
*/
double node::calculate_density(double staticPressure, double specificHeatRatio, double storagePressure, double storageTemperature, double gasConstant)
{
	
	double staticTemperature = calculate_temperature(staticPressure, storagePressure, storageTemperature, specificHeatRatio);
	density = staticPressure / ( gasConstant * staticTemperature );
	return density;
}

/**
density based on pressure and storage density
*/
double node::calculate_density(double staticPressure, double specificHeatRatio, double storagePressure, double storageDensity)
{
	return density = storageDensity * pow(storagePressure / staticPressure, (1 - 2 * specificHeatRatio) / specificHeatRatio);
}

/**
temperature based on pressure
*/
double node::calculate_temperature(double staticPressure, double storagePressure, double storageTemperature,double specificHeatRatio)
{
	static_temperature = storageTemperature*pow((staticPressure/storagePressure),(specificHeatRatio-1)/specificHeatRatio);
	return static_temperature;
}