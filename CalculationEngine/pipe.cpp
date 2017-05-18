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
	double Diameter, //internal diameter
	int connectionType,
	int Schedule
)
:id(Id),node1id(Node1id),node2id(Node2id),internal_diameter(Diameter),connection_type(connectionType),schedule(Schedule)
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

	//pipe size can be fixed by the user, the code to add this capability is not added yet
	//for now all pipes sizes are going to be changed by the calculation engine
	user_defined_size = false;
	total_length = 0.0;
	diameter = 0.0; //nominal size 
	pressure_drop = 0.0;
	temperature_drop = 0.0;
	massFlowRate = -1.0;
}

/**
calculates the total length of the pipe = physical length + eqivalent length of the 
node upstream of the pipe
*/
void pipe::calculate_total_length(double equivLength)
{
	total_length = pipe_length + equivLength;
}

/**
using the fanno line process, this function
calculates the pressure drop starting from p1 (pressure at node1)
this function ignores elevation change (good for ideal gasses)
returns -1 if the length of the pipe is too long (more than Lmax)
*/
int pipe::calculate_pressure_drop
(
	double gamma,
	int method, //1=adiabatic 0=isothermal
	double T1,
	double P1,
	double roughness,
	double gas_constant,
	double V1
)
{
	if (massFlowRate == 0.0)
	{
		pressure_drop = 0.0;
		temperature_drop = 0.0;
		return 0;
	}
	double Mach1 = V1 / sqrt(gamma*gas_constant*T1);
	double density1 = P1 / gas_constant / T1;
	double viscosity = 1.458*pow(10, -6)*pow(T1, 1.5) / (T1 + 110.4);
	double Re = density1*V1*internal_diameter / viscosity;
	double rel_roughness = roughness / internal_diameter;
	double friction_factor = 0.25 * pow(log10(rel_roughness / 3.7 + 5.74 / pow(Re, 0.9)), -2.0);
	double (*function)(double, double, double, double, double, double);
	double (*derivative)(double, double);
	if (method == 0)
	{
		function = isothermal_function;
		derivative = d_dm_isothermal_function;
	}
	else if (method == 1)
	{
		function = adiabatic_function;
		derivative = d_dm_adiabatic_function;
		double Lmax;
		double fanno = (1 - pow(Mach1, 2.0)) / gamma / pow(Mach1, 2.0) + (gamma + 1) / 2.0 / gamma*log(pow(Mach1, 2.0)*(gamma + 1.0) / 2.0 / (1 + (gamma - 1.0) / 2.0*pow(Mach1, 2.0)));
		Lmax = internal_diameter / 4.0 / friction_factor * fanno;
		if (total_length >= Lmax) return -1;
	}
	else
	{
		function = adiabatic_function;
		derivative = d_dm_adiabatic_function;
	}
	double Mach2 = 0.001;
	Newton_raphson(function, derivative, Mach2, 0.000000001, Mach2, 20, Mach1, gamma, friction_factor);
	
	double T2 = T1 * (1 + (gamma - 1) / 2.0*pow(Mach1, 2.0)) / (1 + (gamma - 1) / 2.0*pow(Mach2, 2.0));
	temperature_drop = T1 - T2;
	double P2 = P1 * sqrt((pow(Mach1,2.0)*(1+(gamma-1)/1.0*pow(Mach1,2.0))) / (pow(Mach2, 2.0)*(1 + (gamma - 1) / 1.0*pow(Mach2, 2.0))));
	pressure_drop = P1 - P2;
	return 0;
}

/**
using the NFPA12A or Hessens or GSI or Energy equation method, this function
calculates the pressure drop starting from p1 (pressure at node1) 
this function can handle the elevation head (good for liquified agents)
returns -1 if the length of the pipe is too long (more than Lmax)
*/
int pipe::calculate_pressure_drop
(
	double Z1,
	double Z2,
	double MFR,
	double Density1,
	double P1,
	double T1
)
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
	double mass_per_unit_length,
	double max_flow_rate,
	double min_flow_rate
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
	pipeDATA.maxFlowRate = max_flow_rate;
	pipeDATA.minFlowRate = min_flow_rate;
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

double pipe::isothermal_function(double M1, double M2, double gamma, double f, double D, double L)
{
	return (1 - (pow(M1, 2.0) / pow(M2, 2.0))) / (gamma*pow(M1, 2.0)) - log(pow(M2, 2.0) / pow(M1, 2.0)) - f * L / D;
}

double pipe::d_dm_isothermal_function(double gamma, double M2)
{
	return 2.0 / (gamma*pow(M2, 3.0)) - 2.0 / M2;
}

double pipe::adiabatic_function(double M1, double M2, double gamma, double f, double D, double L)
{
	double A;
	double B;
	double C;

	A = -1.0 / (gamma*pow(M2, 2.0)) - (gamma + 1) / (2.0 * gamma)*log(abs(pow(M2, 2.0) / (1 + (gamma - 1) / 2.0 * pow(M2, 2.0))));
	B = -1.0 / (gamma*pow(M1, 2.0)) - (gamma + 1) / (2.0 * gamma)*log(abs(pow(M1, 2.0) / (1 + (gamma - 1) / 2.0 * pow(M1, 2.0))));
	C = 4.0 / D*f*L;

	return A-B-C;
}

double pipe::d_dm_adiabatic_function(double gamma, double M2)
{
	double A;
	double B;
	double C;

	A = 2.0 / gamma / pow(M2, 3.0);
	B = (2.0 * log((gamma + 1.0) / 2.0 / gamma)*abs(M2)) / (abs((gamma - 1.0) / 2.0)*pow(M2, 2.0) + 1.0);
	C = (2.0 * M2*log((gamma + 1.0) / 2.0 / gamma)*pow(M2, 2.0)*(gamma - 1.0) / 2.0) / pow((gamma - 1.0) / 2.0*pow(M2, 2.0) + 1.0, 2.0);

	return A-B+C;
}

/**
the function f and its derivative with respect to M2 can be changed based on isothermal or adiabatic processes.
f can be isothermal_function or adiabatic function
f_prime can be d_dm_isothermal_function or d_dm_adiabatic_function
returns 0 if root is found
returns 1 if max_num_itteration reached
returns 2 if no root found
*/
int pipe::Newton_raphson(double(*f)(double, double, double, double,double,double), double(*f_prime)(double, double) , double guess, double error, double& result, int max_num_itterations, double M1, double gamma, double friction_factor)
{
	double err;
	int num_itterations = 0;
	do
	{
		double fprime = f_prime(gamma, guess);
		if (fprime == 0.0) return 2;
		double func = f(M1, guess, gamma, friction_factor, internal_diameter, total_length);
		result = guess - func / fprime;
		err = abs(result - guess);
		guess = result;
		num_itterations++;
	} while (err > error && num_itterations < max_num_itterations);
	if (num_itterations == max_num_itterations) return 1;
	return 0;
}
