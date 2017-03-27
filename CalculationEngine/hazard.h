#pragma once

#include "enclosure.h"
#include <vector>
#include <iostream>
#include <string>
#include <pipe.h>
#include <node.h>
#include <fstream>

/**
this class defines the hazards which may include many enclosures. 
Each hazard is protected by all the cylinders. 
different hazards can be protected by using selector valves.
*/
class hazard
{
public:
	hazard();
	hazard(int ID, std::string Name, double bottleVol, double dischargeTime);//!<constructing the hazard with the size of the cylinders 
	~hazard();
	void addEnclosure(enclosure enc);
	void info();
	double get_discharge_time() { return discharge_time; }
	int get_id() { return id; }
	std::vector<enclosure> enclosures;
	std::vector<node> nodes;
	std::vector<pipe> pipes;
	double get_min_total_agent_required() { return minTotalInergenVolReq; }
	std::string get_name() { return name; }
	int get_number_containers() { return numContainers; }
	double get_supplied_agent_volume() { return suppliedInergenVol; }
	int update_hazard();
	void set_number_cylinders(int num_cyl) { numContainers = num_cyl; }
	void set_cylinder_volume(double cyl_vol) { containerVolSize = cyl_vol; }
	void set_discharge_time(double dis_time) { discharge_time = dis_time; }
	void update_pipe_network();
	
private:
	std::string name;
	int id;	
	double containerVolSize; //!< common containers used for inergen are either 22.3 or 39.0 cubic meters
	int numContainers;
	double minTotalInergenVolReq;
	double suppliedInergenVol;
	int numEnclosures;
	double estimated_system_flow_rate;
	double discharge_time;
	double sTime;
	
	//-----------------------------------------------------------------------------------
	// functions for the gas quantity calculations
	void assign_supplied_inergen_vol(); //!< Assigns the volume calculated from cylinders
	void assign_flooding_factor();
	void check_design_concentration();
	void calc_estimated_system_flow_rate();
	void assign_gas_flow_rate();
	void assign_nozzle_quantity();
	int calcNumInergenContainers();
	double calcActualInergenVol();
	double calcMinTotalInergenVol();
	void  assign_o2_co2_concentration();
	void output_data(std::string filename);
	void set_pipe_length();
	
	//----------------------------------------------------------------------------------
	// functions for the hydraulic calculations
	void assign_initial_flow_rates();
	void assign_pipe_sizes_based_on_max_flow_rate();
	void assign_total_length(int pipeIndex);
	void calculate_pressure_drop();
	void calculate_stime();
};