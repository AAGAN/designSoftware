#include <iostream>
#include <stdlib.h>
#include <vector>
#include "dll.h"
#include "enclosure.h"
#include "hazard.h"
#include "node.h"
#include "pipe.h"
#include <string.h>

bool NFPA2001 = 1;
std::vector<hazard> hazards;
int global_cylinder_quantity;

bool hazard_exists(int);
bool enclosure_exists(int, int);

///these are the static vector of structs used to store the pipe and valve databases
///pipeData is a vector of struct of pData type and is defined as
///part of the pipe class, valveData is a vector of struct of vData type and 
///is defined as part of the pipe class, all pipes will share these databases 
///because these are static members of the pipe class. since these are
///static members of the class, they should be initialized outside the definition
///of the pipe class in the global scope
std::vector<pData> pipe::pipeData;
std::vector<vData> pipe::valveData;
std::vector<drills> node::drill_db;

//structure to define the gas properties
struct gas Agent;

//structure to save the pressure recession test data for cylinders
struct cylinder_data cylinderData[11];

/**
calculate_density is a global function that calculates the density of an inert gas (with specified heat_capacity_ratio)
based on the stagnation density and stagnation pressure at any static pressure. 
*/
double calculate_density
(
	double stagnation_density,
	double stagnation_pressure,
	double static_pressure,
	double heat_capacity_ratio
)
{
	double static_density;
	static_density = 
		stagnation_density 
		* pow(
				(stagnation_pressure / static_pressure), 
				((1 - 2 * heat_capacity_ratio) / heat_capacity_ratio)
			 );
	return static_density;
}

void set_pressure_recession_data(int agent)
{
	switch (agent)
	{
	case 0:
		//agent is inergen (IG-541)
		cylinderData[1].percent_remaining = 10; cylinderData[1].temperature = 102.167; cylinderData[1].pressure = 1059724.1960; cylinderData[1].density = 42.4490;
		cylinderData[2].percent_remaining = 20; cylinderData[2].temperature = 140.333; cylinderData[2].pressure = 2910966.5290; cylinderData[2].density = 85.0580;
		cylinderData[3].percent_remaining = 30; cylinderData[3].temperature = 168.944; cylinderData[3].pressure = 5256562.9600; cylinderData[3].density = 127.507;
		cylinderData[4].percent_remaining = 40; cylinderData[4].temperature = 192.722; cylinderData[4].pressure = 7995160.5570; cylinderData[4].density = 169.956;
		cylinderData[5].percent_remaining = 50; cylinderData[5].temperature = 213.444; cylinderData[5].pressure = 11069532.834; cylinderData[5].density = 212.565;
		cylinderData[6].percent_remaining = 60; cylinderData[6].temperature = 232.000; cylinderData[6].pressure = 14439000.723; cylinderData[6].density = 255.014;
		cylinderData[7].percent_remaining = 70; cylinderData[7].temperature = 249.000; cylinderData[7].pressure = 18077364.147; cylinderData[7].density = 297.463;
		cylinderData[8].percent_remaining = 80; cylinderData[8].temperature = 264.667; cylinderData[8].pressure = 21962559.882; cylinderData[8].density = 340.072;
		cylinderData[9].percent_remaining = 90; cylinderData[9].temperature = 279.333; cylinderData[9].pressure = 26076661.558; cylinderData[9].density = 382.521;
		cylinderData[10].percent_remaining = 100; cylinderData[10].temperature = 293.167; cylinderData[10].pressure = 30405879.663; cylinderData[10].density = 424.97;
		break;
	default:
		break;
	}
}

void set_agent_properties(int agent)
{
	switch (agent)
	{
	case 0:
		//agent is inergen (IG-541)
		Agent.Cp = 815.408; // J/(kg*K)
		Agent.Cv = 562.906; // J/(kg*K)
		Agent.Gamma = 1.449;
		Agent.MW = 0.034; // kg/mol
		Agent.name = "inergen";
		Agent.R = 252.502; // J / kg*K
		break;
	case 1:
		//agent is N2+Ar (IG-55) PROPERTIES ARE NOT SET YET!
		Agent.Cp = 815.408; // J/(kg*K)
		Agent.Cv = 562.906; // J/(kg*K)
		Agent.Gamma = 1.449;
		Agent.MW = 0.034; // kg/mol
		Agent.name = "IG-55";
		Agent.R = 252.502; // J/kg*K
		break;
	case 2:
		//agent is Ar (IG-01)
		Agent.Cp = 520.0; // J/(kg*K)
		Agent.Cv = 312.0; // J/(kg*K)
		Agent.Gamma = 1.667;
		Agent.MW = 0.039948; // kg/mol
		Agent.name = "IG-01";
		Agent.R = 208.0; // J / kg*K
		break;
	case 3:
		//agent is N2 (IG-100)
		Agent.Cp = 1040.0; // J/(kg*K)
		Agent.Cv = 743.0; // J/(kg*K)
		Agent.Gamma = 1.4;
		Agent.MW = 0.0280134; // kg/mol
		Agent.name = "IG-100";
		Agent.R = 189.0; // J / kg*K
		break;
	default:
		std::cout << "agent should be 0, 1, 2 or 3 for IG-541, IG-55, IG-01 or IG-100" << std::endl;
		break;
	}
}

int __stdcall add_hazard
(
	int ID,
	double dischargeTime,
	double cylinder_volume,
	int num_cylinders,
	int standard,
	int agent
)
{
	set_agent_properties(agent);
	set_pressure_recession_data(agent);
	double discharge_time;
	discharge_time = dischargeTime;
	if (hazard_exists(ID))
	{
		for (auto& haz : hazards)
		{
			if (haz.get_id() == ID)
			{
				double cyl_vol;
				cyl_vol = cylinder_volume;
				haz.set_number_cylinders(num_cylinders);
				haz.set_cylinder_volume(cyl_vol);
				haz.set_discharge_time(discharge_time);
				return 0;
			}
		}
	}
	else
	{
		hazard Haz
		(
			ID,
			std::to_string(ID),
			cylinder_volume,
			discharge_time
		);
		hazards.push_back(Haz);
		return 0;
	}
	return -1;
}

int __stdcall remove_hazard
(
	int ID
)
{
	for (auto haz = hazards.begin(); haz!=hazards.end();++haz)
	{
		if (haz->get_id() == ID)
		{
			hazards.erase(haz);
			return 0;
		}
	}
	return -1;
}

int __stdcall remove_enclosure
(
	int hazardID,
	int ID
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazardID)
		{
			for (auto enc =  haz.enclosures.begin(); enc!=haz.enclosures.end();++enc)
			{
				if (enc->get_id() == ID)
				{
					haz.enclosures.erase(enc);
					return 0;
				}
			}
		}
	}
	return -1;
}

int __stdcall add_enclosure
(
	int hazardID,
	int ID,
	double min_temperature,
	double max_temperature,
	double max_wall_strength,
	double minimum_design_concentration,
	int vent_calculation_method,
	double altitude,
	double altitude_correction_factor,
	double Length,
	double Width,
	double Height,
	double gross_volume,
	double restriction_volume,
	int nozzleQuantity
)
{
	double disTime = 60.0; // *seconds;
	for (auto& Haz : hazards)
	{
		if (Haz.get_id() == hazardID)
		{
			disTime = Haz.get_discharge_time();
			break;
		}
	}
	bool nozzle_option = nozzleQuantity ? false : true;
	std::string vent_method = vent_calculation_method ? "FIA" : "Ansul";
	enclosure enc(
		hazardID,
		ID,
		std::to_string(ID),
		min_temperature,// * kelvins,
		max_temperature, // * kelvins,
		max_wall_strength, // * pascals,
		minimum_design_concentration,
		min_temperature, // * kelvins,
		vent_method,
		altitude, // * meters,
		altitude_correction_factor,
		nozzle_option,
		Length, // * meters,
		Width, // * meters,
		Height, // * meters,
		gross_volume, // * cubic_meters,
		restriction_volume, // * cubic_meters, //!< This can be negative or positive
		nozzleQuantity,
		disTime,
		"A", //fuel class, not used
		"", //fuel type, not used
		34.2 //cup burner value, not used
	);
	//enc.set_altitude_correction_factor(altitude_correction_factor);

	if (enclosure_exists(hazardID, ID))
		remove_enclosure(hazardID, ID);

	for (auto& Haz : hazards)
	{
		if (Haz.get_id() == hazardID)
		{
			Haz.addEnclosure(enc);
			break;
		}
	}
	return 0;
}

/**
checks if a hazard with hazard_id exists
*/
bool hazard_exists
(
	int hazard_id
)
{
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
			return true;
	return false;
}

/**
checks if an enclosure with enclosure_id exists in a hazard with hazard_id
*/
bool enclosure_exists
(
	int hazard_id,
	int enclosure_id
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			for (auto& enc : haz.enclosures)
			{
				if (enc.get_id() == enclosure_id)
					return true;
			}
		}
	}
	return false;
}

/**
checks if a node with the id of node_id exists in a hazard with id of hazard_id
*/
bool node_exist
(
	int node_id, 
	int hazard_id
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			for (auto& nd : haz.nodes)
			{
				if (nd.get_id() == node_id)
					return true;
			}
		}
	}
	return false;
}

/**checks if a pipe with the id of pipe_id exists in a hazard with id of hazard_id*/
bool pipe_exist
(
	int pipe_id,
	int hazard_id
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			for (auto& pp : haz.pipes)
			{
				if (pp.get_id() == pipe_id)
					return true;
			}
		}
	}
	return false;
}

int __stdcall update_hazard_info
(
	int hazard_id
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			int error_code = haz.update_hazard();
			return error_code;
		}
	}
	return -1;
}

bool __stdcall get_enclosure_info
(
	int hazard_id,
	int enclosure_id,
    ENCLOSURE* out
)
{
    *out = ENCLOSURE{};
    out->id = -1;
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
			for (auto& enc : haz.enclosures)
				if (enclosure_id == enc.get_id())
				{
					out->hazard_id = hazard_id;
					out->id = enclosure_id;
					
					out->gross_volume = enc.get_gross_volume();// .value();
					out->net_volume = enc.get_net_volume();// .value();
					
					out->flooding_factor = enc.getFloodingFactor();
					out->supplied_flooding_factor = enc.get_supplied_flooding_factor();

					out->supplied_design_concentration = enc.get_supplied_design_concentration();
					
					out->minumum_number_nozzles = enc.get_nozzle_quantity();
					
					out->o2_concentration = enc.get_o2_concentration();
					out->co2_concentration = enc.get_co2_concentration();
					
					out->estimated_gas_flow = enc.get_estimated_flow_rate();// .value();
					
					out->minimum_volume_agent_required = enc.get_vol_agent_required();// .value();
					out->supplied_gas_quantity = enc.get_supplied_vol_agent();// .value();
					out->supplied_gas_quantity_95 = 0.95 * enc.get_supplied_vol_agent();// .value();
					out->supplied_gas_quantity_60 = 0.6 * enc.get_supplied_vol_agent();// .value();
					
					return true;
				}
	return false;
}

bool __stdcall get_hazard_info
(
	int hazard_id,
	HAZARD* out
)
{
    *out = HAZARD{};
    out->id = -1;
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
            out->id = hazard_id;
			out->minimum_total_agent_required = haz.get_min_total_agent_required();// .value();
            out->number_containers = haz.get_number_containers();
			out->supplied_agent_volume = haz.get_supplied_agent_volume();// .value();
			global_cylinder_quantity = haz.get_number_containers();
            return true;
		}
	}
	return false;
}

int __stdcall get_pipe_info
(
	int pipe_id, 
	int hazard_id, 
	PIPE * out
)
{
	*out = PIPE{};
	out->pipe_id = -1;
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
			for (auto& pp : haz.pipes)
				if (pipe_id == pp.get_id())
				{
					out->length = pp.get_length();// .value();
					out->pipe_id = pp.get_id();
					out->hazard_id = hazard_id;
					out->schedule = pp.get_schedule();
					out->internal_diameter = pp.get_diameter();
					out->mass_flow_rate = pp.get_mass_flow_rate();
					out->pipe_volume = pp.calculate_pipe_volume();
					out->pressure_drop = pp.get_pressure_drop();
					out->temperature_drop = pp.get_temperature_drop();
					out->total_length = pp.get_total_length();
					out->nominal_size = pp.get_nominal_size();
					return 0;
				}
	return -1;
}

int __stdcall get_node_info
(
	int node_id, 
	int hazard_id, 
	NODE * out
)
{
	*out = NODE{};
	out->node_id = -1;
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
			for (auto& nd : haz.nodes)
				if (node_id == nd.get_id())
				{
					out->orifice_diameter = nd.get_orifice_diameter();// .value();
					out->node_id = nd.get_id();
					out->hazard_id = hazard_id;
					out->equivalent_length_1 = nd.get_equivalent_length_1();
					out->equivalent_length_2 = nd.get_equivalent_length_2();
					out->static_pressure = nd.get_static_pressure();
					out->static_temperature = nd.get_static_temperature();
					out->density = nd.get_density();
					return 0;
				}
	return 0;
}

//int __stdcall get_hydraulics_results
//(
//	int hazardID, 
//	results* out
//)
//{
//	*out = results{};
//	for (auto& haz : hazards)
//	{
//		if (haz.get_id() == hazardID)
//		{
//			out->hazard_id = hazardID;
//			out->maximum_pressure = haz.get_maximum_pressure();
//			out->pipe_volume_to_cylinder_volume = haz.calculate_total_pipe_volume() / haz.get_supplied_agent_volume();
//			out->time_to_95_percent_discharge = haz.calculate_95percent_discharge_time();
//			return 0;
//		}
//	}
//	return -1; //if hazard couldn't be found
//}

int __stdcall add_node(
	int node_id, 
	int hazard_id, 
	int type, 
	int pipe1_id, 
	int pipe2_id, 
	int pipe3_id, 
	double x_coordinate, 
	double y_coordinate, 
	double z_coordinate, 
	double orifice_diameter,
	int enclosure_id,
	int connection_type,
	double agent_volume
)
{
	node nd
	(
		node_id,
		type,
		pipe1_id,
		pipe2_id,
		pipe3_id,
		x_coordinate,
		y_coordinate,
		z_coordinate,
		enclosure_id,
		connection_type
	);
	if (type == 0)
	{
		nd.set_orifice_diameter(orifice_diameter);
		nd.set_supplied_quantity(agent_volume);
	}
	if (node_exist(node_id, hazard_id))
		remove_node(node_id, hazard_id);
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
		{
			haz.nodes.push_back(nd);
			return 0;
		}
	return -1; //if we get here, there is something wrong!
}

/**
Cylinder is a type of node.
*/
int __stdcall add_cylinder
(
	int cylinder_id, 
	int hazard_id, 
	int type, 
	double x_coordinate, 
	double y_coordinate, 
	double z_coordinate
)
{
	node nd
	(
		cylinder_id, 
		10 + type,
		0,
		0,
		0,
		x_coordinate, // * meters,
		y_coordinate, // * meters,
		z_coordinate, // * meters,
		0, //only for nozzles
		1
	);
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
		{
			haz.nodes.push_back(nd);
			return 0;
		}
	return -1; //hazard_id did not exist!
}

int __stdcall add_drill_size_data
(
	wchar_t drill_size[8],
	double drill_diameter
)
{
	std::wstring a(drill_size);
	node::add_drill_size_data
	(
		a,
		drill_diameter
	);
	return 0;
}

int __stdcall add_pipe_size_data
(
	int schedule, 
	double nominal_size, 
	double thickness, 
	double internal_diameter, 
	double friction_factor,
	double maximum_pressure_rating,
	int type,
	double mass_per_unit_Length
)
{
	double maximum_flow_rate;
	double minimum_flow_rate;
	minimum_flow_rate = 0.567 * (2.07 - 10.388 * (internal_diameter*39.37) + 32.82 * pow((internal_diameter*39.37), 2.0) + 1.696*pow((internal_diameter*39.37), 3.0)); // experimental equation, 1 m = 39.37 in
	maximum_flow_rate = 10.582 * minimum_flow_rate;
	maximum_flow_rate *= 0.00756; //1 lb/min = 0.00756 kg/s
	minimum_flow_rate *= 0.00756;
	pipe::addPipeSizeData
	(
		schedule, 
		nominal_size, // * meters, 
		thickness, // * meters, 
		internal_diameter, // * meters, 
		friction_factor,
		maximum_pressure_rating, // * pascals,
		type,
		mass_per_unit_Length, // * kilogram_per_meters
		maximum_flow_rate, //kg/s
		minimum_flow_rate //kg/s
	);
	return 0;
}

int __stdcall add_valve_size_data
(
	double nominal_size, 
	double equivalent_length, 
	int Type
)
{
	std::string valve_type;
	if (Type == 1) valve_type = "selector";
	pipe::addValveSizeData
	(
		nominal_size, // * meters, 
		valve_type, 
		equivalent_length //*meters
	);
	return 0;
}

int __stdcall remove_node
(
	int node_id, 
	int hazard_id
)
{
	for (auto& Haz : hazards)
		if (Haz.get_id() == hazard_id)
			for (auto nd = Haz.nodes.begin(); nd!=Haz.nodes.end();nd++)
				if (nd->get_id() == node_id)
				{
					Haz.nodes.erase(nd);
					return 0;
				}
	return -1;
}

int __stdcall reset_network
(
	int hazard_id
)
{
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
		{
			haz.nodes.clear();
			haz.pipes.clear();
			for (auto& enc : haz.enclosures)
				enc.reset_nozzle_indecies();
			haz.reset_hydraulic_calculations();
			break;
		}
	return 0;
}

int __stdcall add_pipe
(
	int pipe_id, 
	int hazard_id, 
	int type, 
	int node1_id, 
	int node2_id, 
	double diameter,
	int connection_type,
	int schedule_id
)
{
	pipe pp(pipe_id, type, node1_id, node2_id, diameter, connection_type,schedule_id);
	if (pipe_exist(pipe_id, hazard_id))
		remove_pipe(pipe_id, hazard_id);
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
		{
			haz.pipes.push_back(pp);
			return 0;
		}
	return -1; //if we get here, there is something wrong!
}

int __stdcall remove_pipe
(
	int pipe_id, 
	int hazard_id
)
{
	for (auto& Haz : hazards)
		if (Haz.get_id() == hazard_id)
			for (auto pp = Haz.nodes.begin(); pp != Haz.nodes.end(); pp++)
				if (pp->get_id() == pipe_id)
				{
					Haz.nodes.erase(pp);
					return 0;
				}
	return -1;
}

int __stdcall update_pipe_network
(
	int hazard_id,
	results* out
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			*out = results{};
			haz.update_pipe_network();
			out->hazard_id = hazard_id;
			out->maximum_pressure = haz.get_maximum_pressure();
			out->pipe_volume_to_cylinder_volume = haz.calculate_total_pipe_volume() / haz.get_supplied_agent_volume();
			out->time_to_95_percent_discharge = haz.calculate_95percent_discharge_time();
			return 0;
		}
	}
	return -1;
}