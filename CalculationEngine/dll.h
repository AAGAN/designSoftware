#pragma once

//////////////////////////////////////////////////////////////////
////// Structures
//////////////////////////////////////////////////////////////////

/**
structure for retreiving a hazard's calculated information
*/
struct HAZARD
{
	int id;
	int number_containers;
	double minimum_total_agent_required; //!< based on the input design concentration
	double supplied_agent_volume; //!< based on the calculated number of cylinders
};


/**
structure for retreiving an enclosure's calculated information
*/
struct ENCLOSURE
{
	int id; 
	int hazard_id;
	double supplied_design_concentration;
	double gross_volume; //!< in cubic meters
	double net_volume; //!< in cubic meters
	double flooding_factor; //!< based on the input data for design concentration
	double supplied_flooding_factor; //!< based on the calculated design concentration
	int minumum_number_nozzles; //!<based on system manual of inergen 300 bar
	double o2_concentration; //!< Oxygen percent after discharge in worst case
	double co2_concentration; //!< Carbon dioxide percent after discharge in worst case
	double minimum_volume_agent_required; //!<minimum amount of agent required to extinguish the fire
	double supplied_gas_quantity; //!< supplied gas quantity from gas quantity calculations
	double supplied_gas_quantity_95;
	double supplied_gas_quantity_60;
	double estimated_gas_flow; //!< estimated flow rate in cubic meters per minutes from Step 15
};

/**
structure for retreiving a pipe's calculated information
*/
struct PIPE
{
	int pipe_id;
	int hazard_id;
	double length;
};

/**
structure for retreiving a node's calculated information
*/
struct NODE
{
	int node_id;
	int hazard_id;
	double orifice_diameter;
};

//////////////////////////////////////////////////////////////////////////////
//// functions for hazard and enclosure manipulations /////////
//////////////////////////////////////////////////////////////////////////////


/**
This function defines a hazard. Each hazard has one or more enclosures.
This function should be called before adding enclosures.
enclosures are associated with each hazard based on the id of the hazard.
This function will replace a previously defined hazard with the same ID.
returns 0 if succeeds
at this point num_cylinderes, standard and agent are not used
*/
extern "C" int __stdcall add_hazard
(
    int ID, //id of the hazard to be added. should be unique.
    double dischargeTime = 60, //in seconds
    double cylinder_volume = 22.3, //in cubic meters
    int num_cylinders = 0, //0 to calculate the number, any other number to define the number of cylinders
    int standard = 0, //NFPA = 0 , ISO = 1 , VdS = 2 , EN = 3 
    int agent = 0 // IG541(Inergen) = 0 , IG-55(N2 & Ar) = 1 , IG-01(Ar) = 2, IG-100(N2) = 3 
);

/**
function to remove a hazard and all its enclosures
returns 0 if succeeds and -1 if didn't succeed and couldn't find the hazard
*/
extern "C" int __stdcall remove_hazard
(
    int ID //!< id of the hazard to be removed
);

/**
function to remove an enclosure from a hazard
returns 0 if succeeds and -1 if couldn't find the enclosure 
*/
extern "C" int __stdcall remove_enclosure
(
    int hazardID, //!< id of the hazard that this enclosure is in
    int ID //!< id of the hazard to be removed
);

/**
This function should be called to add enclosures to a hazard with hazardID
This function will replace a previously defined enclosure with the same ID.
update_hazard_info() should be called to update the hazard. this function
does not update the information of a hazard.
*/
extern "C" int __stdcall add_enclosure
(
    int hazardID, //id of the hazard that this enclosure belongs to
    int ID, //id of the enclosure to be added, should be unique for each hazard
    double min_temperature = 273.0, //in kelvin
    double max_temperature = 330.0, //in kelvin
    double max_wall_strength = 1000.0, //in Pascals
    double minimum_design_concentration = -1, //This should be provided by the interface
    int vent_calculation_method = 0, // 0 for "Ansul", 1 for "FIA"
    double altitude = 0,//in meters
    double altitude_correction_factor = 1.0, //dimensionless, has to be provided by the interface
    double Length = 0,//in meters, either define dimensions or gross volume, if both are defined, dimensions will be ignored
    double Width = 0,//in meters
    double Height = 0,//in meters
    double gross_volume = 0,//in cubic meters - if 0 then gross volume is calculated from the dimensions
    double restriction_volume = 0,// This can be positive or negative, in cubic meters - if 0 then gross volume is the same as net volume
    int nozzleQuantity = 0 //0 to let engine determine the number of nozzles or any other integer to define the number of nozzles
);

/**
this function should be called to calculate the agent quantity of a hazard.
*/
extern "C" int __stdcall update_hazard_info
(
    int hazard_id
);

/**
this function returns the calculated properties of the enclosure with 
enclosure_id which is in the hazard with hazard_id. an enclosure struct
with id of -1 will be returned if no enclosure could be find to match 
the inputs
*/
extern "C" bool __stdcall get_enclosure_info
(
    int hazard_id, //!< id of the hazard that contains the enclosure
    int enclosure_id, //!< id of the enclosure in hazard_id
    ENCLOSURE* out
);

/**
This function returns the calculated properties of the hazard 
with id of hazard_id. it will return a HAZARD struct with the 
calculated properties. it will return a HAZARD struct with an id
of -1 if it cannot find the hazard_id in the list of hazards.
*/
extern "C" bool __stdcall get_hazard_info
(
    int hazard_id,
    HAZARD* out
);

////////////////////////////////////////////////////////////////
// functions for piping network manipulations ///
////////////////////////////////////////////////////////////////


/**
returns a pointer to a pipe structure containing pipe data
*/
extern "C" int __stdcall get_pipe_info
(
	int pipe_id,
	int hazard_id,
	PIPE* out
);

/**
returns a pointer to a node structure containing node data 
*/
extern "C" int __stdcall get_node_info
(
	int node_id,
	int hazard_id,
	NODE* out
);

/**
This function adds a node to a hazard
a node can be any of the following types:

0- nozzle (orifice is part of a nozzle)
1- elbow (has an angle)
2- manifold outlet (the first node in the piping system)
3- bullhead tee (side branch is first input for properties)
4- side-through tee (side branch is first, inlet is second, through outlet is third input for properties)
5- coupling 
6- free node
*/
extern "C" int __stdcall add_node
(
	int node_id = 0,
	int hazard_id = 0,
	int type = 0, //0,1,2,3,4,5 or 6
	int pipe1_id = 0,//!< for tees, this is the side branch
	int pipe2_id = 0,//!< for side-through tees this is the inlet
	int pipe3_id = 0,//!< for side-through tees this is the outlet
	double x_coordinate = 0.0, //!< in meters
	double y_coordinate = 0.0, //!< in meters
	double z_coordinate = 0.0, //!< in meters
	double orifice_diameter = 0.0, //!< if the node is a nozzle (type = 0) in meters
	int enclosure_id = 0, //!< if the node is a nozzle the id of the associated enclosure
	double required_gas_quantity = 0.0, //!< if the node is a nozzle, this is the required gas quantity of enclosure divided by number of nozzles in that enclosure
	double supplied_gas_quantity = 0.0 //!< if the node is a nozzle, this is the supplied gas quantity of enclosure divided by number of nozzles in that enclosure
);

/** removes a node from a hazard*/
extern "C" int __stdcall remove_node
(
	int node_id,
	int hazard_id
);

/**
deletes all the piping data from a hazard including pipes and nodes
*/
extern "C" int __stdcall reset_network
(
	int hazard_id = 0
);

/**
This function adds a pipe to a hazard
a pipe can be any of the following types:

0- straight pipe
1- valve (needs pressure coefficient)
2- manifold section
*/
extern "C" int __stdcall add_pipe
(
	int pipe_id = 0,
	int hazard_id = 0,
	int type = 0, //0,1 or 2
	int node1_id = 0,
	int node2_id = 0,
	double diameter1 = 0.0 //!< diameter of the pipe
);

/** removes a pipe from a hazard*/
extern "C" int __stdcall remove_pipe
(
	int pipe_id,
	int hazard_id
);

/**
updates the piping network
should be called after all the nodes and pipes were added
*/
extern "C" int __stdcall update_pipe_network
(
	int hazard_id
);

/**
adds the location of a cylinder head to the hazard
this location is different from the location of the connection
from the cylindet to the manifold
0 - iFlow valve
1 - CV-98 valve
etc.
pressure and size of the cylinders are defined at the hazard level.
it is not possible to mix and match different size cylinders
it is not possible to mix cylinders with different valve types
*/
extern "C" int __stdcall add_cylinder
(
	int cylinder_id = 0,
	int hazard_id = 0,
	int type = 0,
	double x_coordinate = 0.0,
	double y_coordinate = 0.0,
	double z_coordinate = 0.0
);

//////////////////////////////////////////////////////////////////
// functions for populating the piping and material databases ////
//////////////////////////////////////////////////////////////////

/**
populates the pipe size datastructure from the database in StudioCA
this function should be called before adding hazards and pipe topology 
*/
extern "C" int __stdcall add_pipe_size_data
(
	int schedule, // can be 40 or 80
	double nominal_size,
	double thickness,
	double internal_diameter,
	double friction_factor,
	double maximum_pressure_rating,
	double minimum_pressure_rating,
	int type, //threaded = 1, welded = 2
	double mass_per_unit_length
);

/**
populates the valve size and type datastructure 
 type:
	1 = selector valve
this function should be called before adding hazards and pipe topology
*/
extern "C" int __stdcall add_valve_size_data
(
	double nominal_size,
	double equivalent_length,
	int Type
);