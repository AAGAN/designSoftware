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
	//pressureDrop1 = 0;// *pascals;
	//pressureDrop2 = 0;// *pascals;
	density = 0.0;
	static_pressure = 0.0;
	static_temperature = 0.0;
	equivalent_length_1 = 0.0;
	equivalent_length_2 = 0.0;
	pipe1_index = 0;
	pipe2_index = 0;
	pipe3_index = 0;
}

/**
calculates the pressure across the node, uses the pressure 
from the end of the input pipe and assigns the 
calculated pressure(s) to the beginning of the pipe(s) connected
to the node
*/
//int node::update_hydraulics(hazard& Haz)
//{
	//calculate pressureDrop1
	//calculate PressureDrop2

	/*Haz.pipes[pipe2id].set_p1(Haz.pipes[pipe1id].p2 - pressureDrop1);
	if (type == "Bull Tee" || type == "Side Tee")
		Haz.pipes[pipe3id].set_p1(Haz.pipes[pipe1id].p2 - pressureDrop2);*/

	//calculate and assign the flow (mass flow rate) at the beginning of the 
	//next pipes based on the mass flow rate at the end of the previous
	//pipe

//	return 0;
//}

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
int node::set_equivalent_length(double internal_diameter)
{
	if(type == "Elbow")//the node's type should be used instead!
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
	else if (type == "Bull Tee")
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
	else if (type == "Side Tee")
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
	else if (type == "Coupling")
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
calculates manifold's pressure
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
	return density = storageDensity * pow(staticPressure / storagePressure, 1 / specificHeatRatio);
}

/**
temperature based on pressure
*/
double node::calculate_temperature(double staticPressure, double storagePressure, double storageTemperature,double specificHeatRatio)
{
	static_temperature = storageTemperature*pow((staticPressure/storagePressure),(specificHeatRatio-1)/specificHeatRatio);
	return static_temperature;
}

/**
this function uses the iterative scheme of Annex A Page 19 of ISO 5167-1:2003(E)
and uses the previously calculated pressure drop and mass flow rate and pipe diameter to 
calculate the orifice diameter
there are some limitations for this method mentioned in the ISO 5167 standard
  P2/P1 >= 0.75 for the expansibility factor to be accurate
  d >= 12.5 mm
  50 mm <= D <= 1000 mm
  0.1 <= beta <= 0.75
  ReD >= 5000 for 0.1 <= beta <= 0.56
  ReD >= 16000*beta^2 for beta > 0.56
*/
void node::calculate_orifice_diameter(double D1, double qm, double gas_constant, double specific_heat_ratio)
{
	double C = 0.606; // first guess
	double E = 0.97; //first guess
	double L1 = 0; //for corner tapping -- L1 = 1 for D tapping -- L1 = l1 / D
	double L2 = 0;  // for corner tapping -- L2 = 0.47 for D/2 tapping -- L2 = l2 / D
	double p2 = 100000; //1 bar atmospheric pressure = 100000 pascals
	double viscosity = 1.458*pow(10, -6)*pow(static_temperature, 1.5) / (static_temperature + 110.4);
	double ReD = 4 * qm / 3.1415927 / viscosity / D1;
	double deltaP = static_pressure - p2; //1 bar atmospheric pressure = 100000 pascals
	double A2 = viscosity * ReD / D1 / sqrt(2.0 * deltaP * density);
	double X2 = A2 / C / E; //X_n-2
	double delta2 = abs(A2 - X2*C*E); //d_n-2
	double precision_criterion = abs((A2 - X2 * C * E) / A2);
	double d = D1 * pow(pow(X2, 2.0) / (1 + pow(X2, 2.0)), 0.25);
	double beta = d / D1;
	double A = pow(19000 * beta / ReD, 0.8);
	double M2 = 2 * L2 / (1 - beta);
	C = 0.5961 + 0.0261 * pow(beta, 2.0) - 0.216 * pow(beta, 8.0) + 0.000521*pow(pow(10, 6.0)*beta / ReD, 0.7) +
		(0.0188 + 0.0063*A)*pow(beta, 3.5)*pow(pow(10, 6.0) / ReD, 0.3) + (0.043 + 0.080*pow(2.718282, -10.0*L1) -
			0.123*pow(2.718282, -7.0*L1)*(1 - 0.11*A)*pow(beta, 4.0) / (1 - pow(beta, 4.0)) -
			0.031*(M2 - 0.8*pow(M2, 1.1))*pow(beta, 1.3));
	if (D1 > 0.07112) C = C + 0.011*(0.75 - beta)*(2.8 - D1 / 25.4);
	E = 1 - (0.351 + 0.256*pow(beta, 4.0) + 0.93*pow(beta, 8.0))*(1 - pow(p2 / static_pressure, 1 / specific_heat_ratio));

	int num_iterations = 0; //preventing endless loop
	while (precision_criterion > 0.000001 && num_iterations < 100) //n = 6
	{
		num_iterations++;
		double X3 = A2 / C / E; // X_n-1
		double delta3 = abs(A2 - X3*C*E); // d_n-1

		double d = D1 * pow(pow(X3, 2.0) / (1 + pow(X3, 2.0)), 0.25);
		double beta = d / D1;
		double A = pow(19000 * beta / ReD, 0.8);
		double M2 = 2 * L2 / (1 - beta);
		C = 0.5961 + 0.0261 * pow(beta, 2.0) - 0.216 * pow(beta, 8.0) + 0.000521*pow(pow(10, 6.0)*beta / ReD, 0.7) +
			(0.0188 + 0.0063*A)*pow(beta, 3.5)*pow(pow(10, 6.0) / ReD, 0.3) + (0.043 + 0.080*pow(2.718282, -10.0*L1) -
				0.123*pow(2.718282, -7.0*L1)*(1 - 0.11*A)*pow(beta, 4.0) / (1 - pow(beta, 4.0)) -
				0.031*(M2 - 0.8*pow(M2, 1.1))*pow(beta, 1.3));
		if (D1 > 0.07112) C = C + 0.011*(0.75 - beta)*(2.8 - D1 / 25.4);
		E = 1 - (0.351 + 0.256*pow(beta, 4.0) + 0.93*pow(beta, 8.0))*(1 - pow(p2 / static_pressure, 1 / specific_heat_ratio));
		double temp = X3;
		X3 = X3 - delta3*((X3 - X2) / (delta3 - delta2)); //calculating X_n
		X2 = temp;
		precision_criterion = abs((A2 - X2 * C * E) / A2);
	}
	if (num_iterations < 100 && d > 0.0 && d < D1) //just checking!
	{
		orifice_diameter = d;
	}
	else
	{
		orifice_diameter = -1;
	}
}
