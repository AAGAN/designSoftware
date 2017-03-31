#include "hazard.h"
#include "dll.h"

//defined in dll.cpp
extern bool NFPA2001;
//defined in dll.cpp
extern struct cylinder_data cylinderData[11];
extern struct gas Agent;
//double sTime = 60.0; //just for now before adding a loop to itterate over sTime

hazard::hazard()
{
}

/**
cylinder volume can be 80 Liters (22.3 cubic meters, 788 cu.ft) or 
140 Liters (39.0 cubic meters, 1379 cu.ft)
*/
hazard::hazard(
	int ID,
	std::string Name,
	//std::vector<enclosure>& Enclosures,
	double bottleVol = 22.3,// * cubic_meters,
	double dischargeTime = 60// * seconds
)
	:
	name(Name),
	id(ID),
	containerVolSize(bottleVol),
	//enclosures(Enclosures),
	discharge_time(dischargeTime),
	sTime(dischargeTime),
	numContainers(0),
	minTotalInergenVolReq(0),// * cubic_meters),
	suppliedInergenVol(0),// * cubic_meters),
	numEnclosures(0),
	estimated_system_flow_rate(0)// * cubic_meters)
{
	
}

/**
This function updates the information for the hazard
and all its enclosures. 
*/
int hazard::update_hazard()
{
	numEnclosures = (int)enclosures.size();
	calcMinTotalInergenVol();//Step 7
	calcNumInergenContainers();//Step 8
	calcActualInergenVol();//Step 9
	assign_supplied_inergen_vol();//Step 10
	assign_flooding_factor(); //Step 11
	check_design_concentration(); //Steps 12-13-14
	calc_estimated_system_flow_rate(); //Step 15
	assign_nozzle_quantity(); //Step 16
	assign_gas_flow_rate(); //Step 17
	assign_o2_co2_concentration();//assign the concentration of oxygen and co2 for display on the gui
	return 0;
}

hazard::~hazard()
{
}

/**
Step 7 of the system manual
*/
double hazard::calcMinTotalInergenVol()
{
	minTotalInergenVolReq = 0.0;// *cubic_meters;
	if (enclosures.size() != numEnclosures)
		std::cout << "Error calculating totalInergenReq" << std::endl;
	for (auto& enc : enclosures)
	{
		minTotalInergenVolReq += enc.get_vol_agent_required();
	}
	return minTotalInergenVolReq;
}

void hazard::assign_o2_co2_concentration()
{
	for (auto& enc : enclosures)
	{
		double o2_conc = 0.209*(100 - enc.get_supplied_design_concentration());
		double co2_conc = 0.08 * enc.get_supplied_design_concentration();
		enc.set_o2_concentration(o2_conc);
		enc.set_co2_concentration(co2_conc);
	}
}

void hazard::output_data(std::string filename)
{
	std::ofstream outputfile;
	outputfile.open(filename, std::ios::trunc);
	outputfile << "Enclosures: " << std::endl;
	outputfile << "ID , net volume , est flow rate" << std::endl;
	for (auto& enc : enclosures)
		outputfile << enc.get_id() << "," << enc.get_net_volume() << "," << enc.get_estimated_flow_rate() << std::endl;
	outputfile << "Pipes: " << std::endl;
	outputfile << "ID , type , node1 ID , node2ID , node1 , node2, length, mass flow rate" << std::endl;
	for (auto& pp : pipes)
		outputfile << pp.get_id() << "," << pp.get_type() << "," << pp.node1id << "," << pp.node2id << "," << pp.get_node1_index() << "," << pp.get_node2_index() << "," << pp.get_length() << "," << pp.get_mass_flow_rate() << std::endl;
	outputfile << "Nodes: " << std::endl;
	outputfile << " ID , type ,  x , y , z ,pipe1 ID , pipe2 ID , pipe3 ID , pipe1 , pipe2 , pipe3 , nozzle mass flow rate" << std::endl;
	for (auto& nd : nodes)
		outputfile << nd.get_id() << "," << nd.get_type() << "," << nd.get_x() << "," << nd.get_y() << "," << nd.get_z() << "," << 
		nd.pipe1id << "," << nd.pipe2id << "," << nd.pipe3id << "," << nd.get_pipe1_index() << "," << nd.get_pipe2_index() << "," << nd.get_pipe3_index() << "," << nd.calculate_nozzle_mass_flow_rate(60.0) << std::endl;
	outputfile.close();
}

/**
based on the ids of the pipes and nodes, sets the correct indecies
to pipes and nodes vectors of the hazard
*/
void hazard::update_pipe_network()
{
	for (unsigned int i = 0; i < nodes.size(); i++)
		nodes[i].index = i;

	for (unsigned int i = 0; i < pipes.size(); i++)
		pipes[i].index = i;

	for (auto& nd : nodes)
	{
		if (nd.get_type() != "iFlow Valve")
		{
			for (auto& pp : pipes)
			{
				if (pp.node1id == nd.get_id())
				{
					pp.add_node1_index(nd.index);
				}
				else if (pp.node2id == nd.get_id())
				{
					pp.add_node2_index(nd.index);
				}
			}
		}
	}

	for (auto& pp : pipes)
	{
		for (auto& nd : nodes)
		{
			if (nd.get_type() != "iFlow Valve")
			{
				if (nd.pipe1id == pp.get_id())
				{
					nd.add_pipe1_index(pp.index);
				}
				else if (nd.pipe2id == pp.get_id())
				{
					nd.add_pipe2_index(pp.index);
				}
				else if ((nd.pipe3id) == pp.get_id())
				{
					nd.add_pipe3_index(pp.index);
				}
			}
		}
	}

	set_pipe_length();
	calculate_stime();
	assign_initial_flow_rates();
	assign_pipe_sizes_based_on_max_flow_rate();
	for (auto& pp : pipes)
	{
		assign_total_length(pp.index);
	}
	
	for (auto& nd : nodes)
	{
		if (nd.get_type() == "Manifold Outlet")
		{
			double max_MFR = pipes[nd.get_pipe2_index()].get_mass_flow_rate();
			nd.calculate_manifold_pressure(max_MFR, numContainers, cylinderData[10].pressure, cylinderData[10].temperature, cylinderData[10].density, Agent.Gamma);
			break;
		}
	}
	calculate_pressure_drop();

	output_data("c:\\output.txt");
}

/**
calculates and assigns the length of a pipe based on the coordinates of its 
associated nodes
*/
void hazard::set_pipe_length()
{
	for (auto& pp : pipes)
	{
		double x_diff = nodes[pp.get_node1_index()].get_x() - nodes[pp.get_node2_index()].get_x();
		double y_diff = nodes[pp.get_node1_index()].get_y() - nodes[pp.get_node2_index()].get_y();
		double z_diff = nodes[pp.get_node1_index()].get_z() - nodes[pp.get_node2_index()].get_z();
		double ll = sqrt(pow(x_diff, 2.0) + pow(y_diff, 2.0) + pow(z_diff, 2.0));
		pp.set_pipe_length(ll);
	}
}

int hazard::calcNumInergenContainers()
{
	numContainers = 0;
	numContainers += (int)ceil(minTotalInergenVolReq / containerVolSize);
	return numContainers;
}

/**
this function should be called to add an enclosure
to the hazard. it also updates the hazard information 
based on the added enclosure information.
*/
void hazard::addEnclosure(enclosure enc)
{
	enclosures.push_back(enc);
	//update_hazard(); Ovidiu requested to remove this automatic update and add a separate function to update the hazard.
	return;
}

/**
Step No.9 of the systems manual for inergen 300 bar
*/
double hazard::calcActualInergenVol()
{
	suppliedInergenVol = (double)numContainers * containerVolSize;
	return suppliedInergenVol;
}

/**
Calculates and assigns the actual inergen supplied per enclosure
Step No. 10 of the system manual
*/
void hazard::assign_supplied_inergen_vol()
{	
	double totalInergenVol = 0;// *cubic_meter;
	for (auto& enc : enclosures) totalInergenVol += enc.get_vol_agent_required();
	for (auto& enc : enclosures)
	{
		double volFraction;
		volFraction = enc.get_vol_agent_required()/totalInergenVol;
		enc.set_supplied_vol_agent(volFraction*suppliedInergenVol);
	}
	return;
}

/**
this function calculates the flooding factor based on the number of cylinders
this is the Step No. 11 in the system manual
Note: if all areas use the same "Minimum Design Concentration", the flooding 
factors determined in this step should match for all areas. if this is not the case,
a calculation mistake has been made.
*/
void hazard::assign_flooding_factor()
{
	for (auto& enc : enclosures)
	{
		double suppliedFloodingFactor = 0;
		suppliedFloodingFactor = enc.get_supplied_vol_agent()
			/enc.get_altitude_correction_factor()
			/enc.get_net_volume();
		enc.set_supplied_flooding_factor(suppliedFloodingFactor);
	}
	return;
}

/**
Calculates the desing concentration at the maximum ambient temperature 
using the supplied inergen agent.
Step No. 12 of the System Manual of inergen 300 bar system
*/
void hazard::check_design_concentration(
	//std::string Class, 
	//std::string Type, 
	//quantity<dimensionless> cup_burner_value
)
{
	
	for (auto& enc : enclosures)
	{
		double dc;
		//Steps 12 and 13
		dc = enc.calc_concentration(enc.get_max_temperature(), enc.get_supplied_flooding_factor());
		enc.set_supplied_design_concentration(dc);
		if (dc>52 || dc<34.2)
		{
			std::cout << "System design not acceptable because design concentration at maximum temperature is: " << dc << std::endl;
			std::cout << "It should be between 34.2 and 52 percent" << std::endl;
			if (dc >= 52 || dc < 62)
				std::cout << "This design is acceptable in normally non-occupied areas if evacuation can be acomplished to limit exposure to less than 30 seconds" << std::endl;
		}
		
		//Steps 14 and 13
		dc = enc.calc_concentration(enc.get_normal_ambient_temperature(), enc.get_supplied_flooding_factor());
		if (dc>52 || dc<34.2)
		{
			std::cout << "System design not acceptable because design concentration at normal ambient temperature is: " << dc << std::endl;
			std::cout << "It should be between 34.2 and 52 percent" << std::endl;
			if (dc >= 52 || dc < 62)
				std::cout << "This design is acceptable in normally non-occupied areas if evacuation can be acomplished to limit exposure to less than 30 seconds" << std::endl;
		}
	}

	return;
}

/**
Determines the estimated system flow rate 
Step 15 of the system manual
*/
void hazard::calc_estimated_system_flow_rate()
{
	if (discharge_time == 120)// * seconds)
		estimated_system_flow_rate = suppliedInergenVol * 0.95;
	else if (discharge_time == 60)// * seconds)
		estimated_system_flow_rate = suppliedInergenVol * 0.95 * 2.0;
	else
		std::cout << "System manual only provides a method for estimated flow rate if the discharge time is 60 or 120 seconds but the discharge time is: " << discharge_time << std::endl;
}
/**
Step 17 of the system manual
*/
void hazard::assign_gas_flow_rate()
{
	for (auto& enc : enclosures)
	{
		if (discharge_time == 120)// * seconds)
		{
			enc.set_estimated_flow_rate(enc.get_supplied_vol_agent()*0.95*2.0);
		}
		else
		{
			enc.set_estimated_flow_rate(enc.get_supplied_vol_agent()*0.95);
		}
	}
}

/**
Calculate and assign nozzle quantity per enclosure
Step 16 of the system manual
*/
void hazard::assign_nozzle_quantity()
{
	for (auto& enc : enclosures)
	{
		if (!enc.get_nozzle_option())
		{
			//if it is fixed, the quantity should be provided in the numNozzles and 
			//it will be assigned during the construction of enclosure
		}
		else if (enc.get_nozzle_option())
		{
			int nozQ;
			nozQ = (int)ceil(ceil(enc.getLength() / 9.75)*ceil(enc.getWidth() / 9.75));
			enc.set_nozzle_quantity(nozQ);
			if (enc.get_net_volume() > 580.11)//*cubic_meters)
				nozQ = (int)ceil(enc.get_net_volume() / (580.11));//*cubic_meters)); //max volume coverage is 95.1sq.m*6.1m = 508.11cu.m
			if (nozQ > enc.get_nozzle_quantity())
			{
				enc.set_nozzle_quantity(nozQ);
				std::cout << "Max nozzle height above floor level for a single row of nozzles is 6.1 meters." << std::endl;
			}
		}		
	}
	std::cout << "Please refer to Step 16 of the system manual for Inergen 300 bar system to verify the nozzle placement." << std::endl;
}


/**
This function 
*/
void hazard::info()
{
	std::cout << "=========================================================" << std::endl; 
	std::cout << "Number of containers needed: " << numContainers << std::endl;
	std::cout << "Size of the containers: " << containerVolSize << std::endl;
	std::cout << "Supplied volume of Inergen: " << suppliedInergenVol << std::endl;
	std::cout << "Estimated flow rate of Inergen is: " << estimated_system_flow_rate << std::endl;
	std::cout << "Discharge time is: " << discharge_time << std::endl;
	std::cout << "=========================================================" << std::endl;

	for (size_t i = 0; i < enclosures.size(); i++)
	{
		std::cout << "Supplied design concentration of " << enclosures[i].get_name() << " is: " << enclosures[i].get_supplied_design_concentration() << std::endl;
		std::cout << "Supplied volume of agent for " << enclosures[i].get_name() << " is: " << enclosures[i].get_supplied_vol_agent() << std::endl;
		std::cout << "Net volume of " << enclosures[i].get_name() << " is: " << enclosures[i].get_net_volume() << std::endl;
		std::cout << "Number of nozzles needed in " << enclosures[i].get_name() << " is: " << enclosures[i].get_nozzle_quantity() << std::endl;
		std::cout << "The altitude correction factor for " << enclosures[i].get_name() << " is: " << enclosures[i].get_altitude_correction_factor() << std::endl << std::endl;
		std::cout << "----------------------------------------------------" << std::endl;
	}
	
}

/**
assigns mass flow rates to all pipes and nodes based on the required gas quantity from nozzles
*/
void hazard::assign_initial_flow_rates()
{
	//set the mass flow rate for all the pipes to zero
	for (unsigned int i = 0; i < pipes.size(); i++)
	{
		pipes[i].set_mass_flow_rate(0.0);
	}

	// find all the nozzles in the piping structure of this hazard
	std::vector<int> nozzle_indecies;
	for (unsigned int i = 0 ; i < nodes.size() ; i++)
	{
		if (nodes[i].get_type() == "Nozzle")
		{
			nozzle_indecies.push_back(i);
		}
	}

	//assign the required and supplied quantity from enclosure to the nozzles
	for (unsigned int i = 0; i < enclosures.size(); i++)
	{
		for (unsigned int j = 0; j < nozzle_indecies.size(); j++)
		{
			if (nodes[nozzle_indecies[j]].get_enclosure_id() == enclosures[i].get_id())
			{
				enclosures[i].add_nozzle_index(nozzle_indecies[j]);
			}
		}
	}
	for (unsigned int i = 0; i < enclosures.size(); i++)
	{
		for (unsigned int j = 0; j < nozzle_indecies.size(); j++)
		{
			if (nodes[nozzle_indecies[j]].get_enclosure_id() == enclosures[i].get_id())
			{
				double required_quantity = enclosures[i].get_vol_agent_required() / enclosures[i].number_of_nozzles();
				double supplied_quantity = enclosures[i].get_supplied_vol_agent() / enclosures[i].number_of_nozzles();
				nodes[nozzle_indecies[j]].set_required_quantity(required_quantity);
				nodes[nozzle_indecies[j]].set_supplied_quantity(supplied_quantity);
			}
		}
	}

	// find all the tees in the piping structure of this hazard
	std::vector<int> tee_indecies;
	for (unsigned int i = 0; i < nodes.size() ; i++)
	{
		if (nodes[i].get_type() == "Bull Tee" || nodes[i].get_type() == "Side Tee")
		{
			tee_indecies.push_back(i);
		}
	}

	// find the manifold outlet in the piping structure of this hazard
	int manifold_outlet_index;
	for (unsigned int i = 0; i < nodes.size() ; i++)
	{
		if (nodes[i].get_type() == "Manifold Outlet")
		{
			manifold_outlet_index = i;
			break;
		}
	}

	int current_node_index;
	int upstream_pipe_index;
	int downstream_pipe_index;

	//this part assigns the mass flow rates from each nozzle to the closest tee
	for (unsigned int i = 0; i < nozzle_indecies.size(); i++)
	{
		current_node_index = nozzle_indecies[i]; //i-th nozzle's index
		
		//assign the upstream pipe index of the nozzle and calculate and assign the mass flow rate from the nozzle to the pipe connecting to it
		//because nozzles don't have downstream pipes, this calculation should be done for nozzles once and then move to the upstream nodes and pipes
		upstream_pipe_index = nodes[current_node_index].get_pipe1_index();
		double i_th_nozzle_mass_flow_rate = nodes[current_node_index].calculate_nozzle_mass_flow_rate(sTime);
		pipes[upstream_pipe_index].set_mass_flow_rate(i_th_nozzle_mass_flow_rate);
		
		//assign the indecies for current node (node upstream of the nozzle) and its upstream and downstream pipes
		current_node_index = pipes[upstream_pipe_index].get_node1_index();
		upstream_pipe_index = nodes[current_node_index].get_pipe1_index();
		downstream_pipe_index = nodes[current_node_index].get_pipe2_index();

		//only elbows and couplings can be in a "pipe section" 
		while (nodes[current_node_index].get_type() == "Elbow" || nodes[current_node_index].get_type() == "Coupling")
		{
			//assign the flow rate of downstream pipe to upstream pipe
			pipes[upstream_pipe_index].set_mass_flow_rate(pipes[downstream_pipe_index].get_mass_flow_rate());

			//use the inlet node of the upstream pipe as the current node and use its upstream and downstream pipes for next loop
			current_node_index = pipes[upstream_pipe_index].get_node1_index();
			upstream_pipe_index = nodes[current_node_index].get_pipe1_index();
			downstream_pipe_index = nodes[current_node_index].get_pipe2_index();
		}
	}

	//this part assigns initial mass flow rates for the rest of the piping system until the manifold outlet
	bool pipes_with_no_flow_rate = true;

	while (pipes_with_no_flow_rate)
	{
		pipes_with_no_flow_rate = false;
		for (unsigned int i = 0; i < tee_indecies.size(); i++)
		{
			int current_tee = tee_indecies[i];
			double mass_flow_rate_inlet = pipes[nodes[current_tee].get_pipe1_index()].get_mass_flow_rate();
			double mass_flow_rate_outlet_1 = pipes[nodes[current_tee].get_pipe2_index()].get_mass_flow_rate();
			double mass_flow_rate_outlet_2 = pipes[nodes[current_tee].get_pipe3_index()].get_mass_flow_rate();
			//if both of the outlets have values and the inlet doesn't have a value then
			if (mass_flow_rate_outlet_1 != 0.0 && mass_flow_rate_outlet_2 != 0.0 && mass_flow_rate_inlet == 0.0)
			{
				pipes_with_no_flow_rate = true;

				upstream_pipe_index = nodes[current_tee].get_pipe1_index();
				pipes[upstream_pipe_index].set_mass_flow_rate(mass_flow_rate_outlet_1 + mass_flow_rate_outlet_2);

				//assign the same mass flow rate to the pipes until the next tee or maniforl outlet
				current_node_index = pipes[upstream_pipe_index].get_node1_index();
				upstream_pipe_index = nodes[current_node_index].get_pipe1_index();
				downstream_pipe_index = nodes[current_node_index].get_pipe2_index();
				while (nodes[current_node_index].get_type() == "Elbow" || nodes[current_node_index].get_type() == "Coupling")
				{
					//assign the flow rate of downstream pipe to upstream pipe
					pipes[upstream_pipe_index].set_mass_flow_rate(pipes[downstream_pipe_index].get_mass_flow_rate());

					//use the inlet node of the upstream pipe as the current node and use its upstream and downstream pipes for next loop
					current_node_index = pipes[upstream_pipe_index].get_node1_index();
					upstream_pipe_index = nodes[current_node_index].get_pipe1_index();
					downstream_pipe_index = nodes[current_node_index].get_pipe2_index();
				}
			}
		}
	}
}

/**
assigns the diameter of the pipe based on the mass flow rate calculated from the agent quantity requirement 
and the maximum flow rate rating of a pipe
*/
void hazard::assign_pipe_sizes_based_on_max_flow_rate()
{
	for (auto& pp : pipes)
	{
		if (!pp.user_defined_size && pp.get_type() != "Manifold")
		{
			for (auto& pip : pipe::pipeData)
			{
				if (pip.schedule == pp.get_schedule() &&  pip.maxFlowRate > pp.get_mass_flow_rate())
				{
					pp.set_diameter(pip.internalDiameter);
					break;
				}
			}
		}
	}
}

/**
assigns the total length of the pipe with pipe index of pipeIndex
*/
void hazard::assign_total_length(int pipeIndex)
{
	double equivalent_length = 0.0;
	int upstream_node_index = pipes[pipeIndex].get_node1_index();
	//set the equivalent length of the node based on the internal diameter 
	//of the pipe downstream of the node
	nodes[upstream_node_index].set_equivalent_length(pipes[pipeIndex].get_diameter());
	//when node is a tee, there are two equivalent lengths, when elbow or coupling, only the first
	//equivalent length is used and the next one is ignored.
	//depending on which side of the tee the pipe is connected to, calculate the equivalent length
	if (nodes[upstream_node_index].get_pipe2_index() == pipes[pipeIndex].index)
	{
		pipes[pipeIndex].calculate_total_length(nodes[upstream_node_index].get_equivalent_length_1());
	}
	else if (nodes[upstream_node_index].get_pipe3_index() == pipes[pipeIndex].index)
	{
		pipes[pipeIndex].calculate_total_length(nodes[upstream_node_index].get_equivalent_length_2());
	}
}

void hazard::calculate_pressure_drop()
{
	// find the manifold outlet among the nodes of the piping structure of this hazard
	int manifold_outlet_index;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].get_type() == "Manifold Outlet")
		{
			manifold_outlet_index = i;
			break;
		}
	}
	double maximum_MFR = pipes[nodes[manifold_outlet_index].get_pipe2_index()].get_mass_flow_rate();
	double storage_pressure = cylinderData[10].pressure; // the pressure recession data is in the dll.h and dll.cpp files
	double storage_temperature = cylinderData[10].temperature; //10 indicates the full cylinder condition (100% full)
	double storage_density = cylinderData[10].density;
	double specific_heat_ratio = Agent.Gamma;
	double gas_constant = Agent.R;
	nodes[manifold_outlet_index].calculate_manifold_pressure(maximum_MFR, numContainers, storage_pressure, storage_temperature, storage_density, specific_heat_ratio);

	std::vector<int> tees_remaining;
	int current_node_index = manifold_outlet_index;
	int current_pipe_index = nodes[current_node_index].get_pipe2_index();
	double T_1 = nodes[current_node_index].get_static_temperature();
	double P_1 = nodes[current_node_index].get_static_pressure();
	double V_1 = maximum_MFR / nodes[current_node_index].get_density() / pipes[current_pipe_index].get_diameter();
	int method = 1; //0 for isothermal and 1 for adiabatic
	double roughness = 0.00005;

	do
	{
		pipes[current_pipe_index].calculate_pressure_drop(specific_heat_ratio, method, T_1, P_1, roughness, gas_constant, V_1);
		double pressure_drop = pipes[current_pipe_index].get_pressure_drop();
		double current_node_pressure = nodes[current_node_index].get_static_pressure() - pressure_drop;
		nodes[pipes[current_pipe_index].get_node2_index()].set_static_pressure(current_node_pressure);
		double temperature_drop = pipes[current_pipe_index].get_temperature_drop();//storage_temperature*pow(current_node_pressure / storage_pressure, (specific_heat_ratio - 1.0) / specific_heat_ratio);
		double current_node_temperature = nodes[current_node_index].get_static_temperature() - temperature_drop;
		nodes[pipes[current_pipe_index].get_node2_index()].set_static_temperature(current_node_temperature);
		double current_node_density = current_node_pressure / current_node_temperature / gas_constant;
		nodes[pipes[current_pipe_index].get_node2_index()].set_density(current_node_density);
		
		current_node_index = pipes[current_pipe_index].get_node2_index();
		std::string current_node_type = nodes[current_node_index].get_type();

		if (current_node_type == "Nozzle")
		{
			if (tees_remaining.size() == 0)
			{
				break;	
			}
			else
			{
				current_node_index = tees_remaining.back();
				current_pipe_index = nodes[current_node_index].get_pipe3_index();
				//current_node_index = pipes[current_node_index].get_node2_index();
				tees_remaining.pop_back();
			}
		}
		else if (current_node_type == "Bull Tee" || current_node_type == "Side Tee")
		{
			tees_remaining.push_back(current_node_index);
			current_pipe_index = nodes[current_node_index].get_pipe1_index();
			//current_node_index = pipes[current_pipe_index].get_node2_index();
		}
		else
		{
			current_pipe_index = nodes[current_node_index].get_pipe2_index();
			//current_node_index = pipes[current_pipe_index].get_node2_index();
		}
	} while (true);
}

void hazard::calculate_stime()
{
	double stempSingle = 95.0 * minTotalInergenVolReq / numContainers / containerVolSize;
	stempSingle = 0.00074956*pow(stempSingle, 2.0) - 0.18112756*stempSingle + 11.56569296;
	sTime = 34 * stempSingle*discharge_time / 60.0;
	if (sTime > discharge_time)
	{
		sTime = discharge_time * 34 / 60;
	}
}
