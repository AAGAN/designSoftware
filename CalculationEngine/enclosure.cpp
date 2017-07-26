#include "enclosure.h"
#include <iostream>

extern bool NFPA2001;

//enclosure::enclosure()
//{
//}

enclosure::~enclosure()
{
}

/**
Constructor for the enclosure class
If the minimum design concentration is not known, concentration should be input as -1 and it will be calculated.
*/
enclosure::enclosure(
	int hazID = 1,
	int ID = 1,
	std::string name = "Enclosure 1",
	double minTemp = 293.15,// * kelvin,
	double maxTemp = 295.15,// * kelvin,
	double maxStrength = 1.0e6,// * pascals,
	double concentration = -1, //-1 if not provided, if this value is provided, it overrides the calculation for minimum design concentration
	double temp = 294.26,// * kelvin, //normal ambient temperature
	std::string VentCalcMethod = "Ansul", //Ansul or FIA
	double Altitude = 0 ,//* meters,
	double alt_cor_factor = 1.0, //altitude correction factor
	bool nozOption = true, // true for "quantity" or false for "fixed"
	double Length = 10 ,//* meters, // just to initialize the dimensions, these dimensions are not of any significant importance
	double Width = 10 ,//* meters,
	double Height = 15 ,//* meters,
	double grossVol = 0 ,//* cubic_meters,
	double RestrictionVol= 0 ,//* cubic_meters,
	int numNozzles = 1,
	double dischargeTime = 60 ,//* seconds,
	std::string fuelClass = "A", //Class ("A" or "B" or "C")
	std::string fuelType = "", //only for Class "B" Type ("commercial grade heptane" or "other class B fuels" or "only manual actuation")
	double cupBurnerValue = 34.2 //For Class "B" Type "only manual actuation" the cup_burber_value should be specified
	
)
:
hazardId(hazID), id(ID),enclosure_name(name),min_temperature(minTemp), max_temperature(maxTemp), normal_temperature(temp),
max_wall_strength(maxStrength), min_design_concentration(concentration), 
vent_calculation_method(VentCalcMethod), altitude(Altitude),nozzle_option(nozOption),
discharge_time(dischargeTime), lengths(Length), width(Width),
height(Height), volume_restrictions(RestrictionVol), fuel_class(fuelClass), fuel_type(fuelType),
cup_burner(cupBurnerValue), nozzle_quantity(numNozzles), altitude_correction_factor(alt_cor_factor)
{
	temperatures = normal_temperature;

	gross_volume = (grossVol != 0) ? grossVol : lengths*width*height;
	
	net_volume = gross_volume + volume_restrictions;

	/*if (concentration == -1)
		min_design_concentration = calculate_design_concentration(fuel_class, fuel_type, cup_burner);
	else 
		min_design_concentration = concentration;*/

	vol_agent_required = get_vol_agent_required();

	get_altitude_correction_factor(Altitude);
}

/**
Calculates the minimum amount of inergen required to achieve the min_design_concentration amount
this function updates the value of the flooding_factor in addition to vol_agent_required
this is the volume of the inergen at 21C and 101300Ps 
*/
double enclosure::get_vol_agent_required()
{
	if (NFPA2001)
	{
		// NFPA2001 page 85 , below Table A.5.5.2(f) IG-541 Total flooding quantity (SI Units)
		double specificVolume = 0.65799 + 0.00239 * (min_temperature - 273.15);
		flooding_factor = log(100.0 / (100.0 - min_design_concentration))*(0.707 / specificVolume);
	}
	else
	{

	}
	vol_agent_required = flooding_factor * net_volume; // this is to update the value of the vol_agent_required of the object
	vol_agent_required *= get_altitude_correction_factor(altitude);
	return vol_agent_required;
}

/**
calculates and returns the concentration for Step 12 and 14 from a temperature and a flooding factor
this is from the equation in NFPA2001 page 85 Table A.5.5.2 (f)
*/
double enclosure::calc_concentration(double t, double floodingfactor)
{
	double dc;
	dc = 100 - 100 * exp((-1.0) * floodingfactor * ( 0.65799 + 0.00239 * (t - 273.15 )) / 0.707);
	return dc;
}

/**
calculated and returns the concentration for max, min and ambient temperature
this is needed in steps 12 and 14 of the system manual
*/
//quantity<dimensionless> enclosure::calc_concentration(std::string condition)
//{
//    quantity<dimensionless> result;
//	if (condition == "max_temperature" ) result = calc_concentration(max_temperature, supplied_flooding_factor);
//	if (condition == "min_temperature") result = calc_concentration(min_temperature, supplied_flooding_factor);
//	if (condition == "ambient_temperature")result = calc_concentration(normal_temperature, supplied_flooding_factor);
//    return result;
//}

/**
calculates the design concentration based on Page 2-5 of the System Manual of inergen 300 bar
The inputs are Class ("A" or "B" or "C") and Type ("commercial grade heptane" or "other class B fuels" or "only manual actuation")
For Class "B" the type should be specified, For Class "B" Type "other class B fuels", cup_burner_value should be specified in %
For Type "only manual actuation" the cup_burber_value should be specified
*/
//quantity<dimensionless> enclosure::calculate_design_concentration(
//	std::string Class, 
//	std::string Type, 
//	quantity<dimensionless> cup_burner_value
//)
//{
//	if (Class == "A")
//	{
//		min_design_concentration = 34.2;
//	}
//	else if (Class == "B" && Type == "commercial grade heptane")
//	{
//		min_design_concentration=40.7;
//	}
//	else if (Class == "B" && Type == "other class B fuels")
//	{
//		min_design_concentration = cup_burner_value*1.008*1.3;
//		(min_design_concentration < 40.7) ? min_design_concentration = 40.7 : 1;
//	}
//	else if (Class == "C")
//	{
//		min_design_concentration = 38.2;
//	}
//	else if (Type == "only manual actuation")
//	{
//		min_design_concentration = cup_burner_value*1.008*1.3;
//		(Class == "A" && min_design_concentration < 34.2) ? min_design_concentration = 34.2 : 1;
//		(Class == "B" && min_design_concentration < 40.7) ? min_design_concentration = 40.7 : 1;
//		(Class == "C" && min_design_concentration < 38.5) ? min_design_concentration = 38.5 : 1;
//	}
//	else
//	{
//		std::cout << "The inputs to the calculate_design_concentration function do not match any of the possibilities!" << std::endl;
//		min_design_concentration = 0.0;
//	}
//
//	std::cout << "Minimum Design concentration is "<< min_design_concentration<<" percent" << std::endl;
//
//	return min_design_concentration;
//}

/**
the altitude correction_factor data is in the "database.db" SQLite database in "altitude" table. 
this data is from page 2-12 of the inergen 300bar system manual
this function calls the interpolate_table function from the SQL_interpolation.cpp to interpolate 
between the values in the "altitude" table.

sqlite> select * from altitude;
altitude_meters  correction_factor
---------------  -----------------
-1000.0          1.13
0.0              1.0
1000.0           0.885
1500.0           0.83
2000.0           0.785
2500.0           0.735
3000.0           0.69
3500.0           0.65
4000.0           0.61
4500.0           0.565
-500.0           1.065
500.0            0.9425
*/
double enclosure::get_altitude_correction_factor(double Altitude)
{
	double cor = 5.48701e-9*std::pow(Altitude, 2.0) - 0.00012*Altitude + 1.00208;
	altitude_correction_factor = cor;
	return altitude_correction_factor;
}
