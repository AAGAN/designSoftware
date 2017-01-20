#include "dll.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "enclosure.h"
#include "hazard.h"
#include "node.h"
#include "pipe.h"

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
			haz.update_hazard();
			return 0;
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
					return 0;
				}
	return 0;
}

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
	double required_gas_quantity,
	double supplied_gas_quantity
)
{
	double x_coord, y_coord, z_coord, orifice_diam;
	double requiredGasQuantity, suppliedGasQuantity;
	node nd
	(
		node_id,
		type,
		pipe1_id,
		pipe2_id,
		pipe3_id,
		x_coord = x_coordinate,
		y_coord = y_coordinate,
		z_coord = z_coordinate,
		requiredGasQuantity = required_gas_quantity,
		suppliedGasQuantity = supplied_gas_quantity
	);
	orifice_diam = orifice_diameter;
	if (type == 0) nd.set_orifice_diameter(orifice_diam);
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
		0.0, // * cubic_meter,
		0.0 // * cubic_meters
	);
	for (auto& haz : hazards)
		if (haz.get_id() == hazard_id)
		{
			haz.nodes.push_back(nd);
			return 0;
		}
	return -1; //hazard_id did not exist!
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
	pipe::addPipeSizeData
	(
		schedule, 
		nominal_size, // * meters, 
		thickness, // * meters, 
		internal_diameter, // * meters, 
		friction_factor,
		maximum_pressure_rating, // * pascals,
		type,
		mass_per_unit_Length // * kilogram_per_meters
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
	double diameter
)
{
	pipe pp(pipe_id, type, node1_id, node2_id, diameter, diameter);
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
	int hazard_id
)
{
	for (auto& haz : hazards)
	{
		if (haz.get_id() == hazard_id)
		{
			haz.update_pipe_network();
			return 0;
		}
	}
	return -1;
}