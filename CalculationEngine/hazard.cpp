#include "hazard.h"

extern bool NFPA2001;

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
	quantity<volume> bottleVol = 22.3 * cubic_meters,
	quantity<time> dischargeTime = 60 * seconds
)
	:
	name(Name),
	id(ID),
	containerVolSize(bottleVol),
	//enclosures(Enclosures),
	discharge_time(dischargeTime),
	numContainers(0),
	minTotalInergenVolReq(0 * cubic_meters),
	suppliedInergenVol(0 * cubic_meters),
	numEnclosures(0),
	estimated_system_flow_rate(0 * cubic_meters)
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
quantity<volume> hazard::calcMinTotalInergenVol()
{
	minTotalInergenVolReq = 0 * cubic_meters;
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
		quantity<dimensionless> o2_conc = 0.209*(100 - enc.get_supplied_design_concentration());
		quantity<dimensionless> co2_conc = 0.08 * enc.get_supplied_design_concentration();
		enc.set_o2_concentration(o2_conc);
		enc.set_co2_concentration(co2_conc);
	}
}

void hazard::output_data(std::string filename)
{
	std::ofstream outputfile;
	outputfile.open(filename);
	outputfile << "Enclosures: " << std::endl;
	outputfile << "ID , net volume , est flow rate" << std::endl;
	for (auto& enc : enclosures)
		outputfile << enc.get_id() << "," << enc.get_net_volume() << "," << enc.get_estimated_flow_rate() << std::endl;
	outputfile << "Pipes: " << std::endl;
	outputfile << "ID , type , node1 ID , node2ID , node1 , node2, length" << std::endl;
	for (auto& pp : pipes)
		outputfile << pp.get_id() << "," << pp.get_type() << "," << pp.node1id << "," << pp.node2id << "," << pp.get_node1() << "," << pp.get_node2() << "," << pp.get_length() << std::endl;
	outputfile << "Nodes: " << std::endl;
	outputfile << " ID , type ,  x , y , z ,pipe1 ID , pipe2 ID , pipe3 ID , pipe1 , pipe2 , pipe3 " << std::endl;
	for (auto& nd : nodes)
		outputfile << nd.get_id() << "," << nd.get_type() << "," << nd.get_x() << "," << nd.get_y() << "," << nd.get_z() << "," << 
		nd.pipe1id << "," << nd.pipe2id << "," << nd.pipe3id << "," << nd.get_pipe1() << "," << nd.get_pipe2() << "," << nd.get_pipe3() << std::endl;
	outputfile.close();
}

/**
based on the ids of the pipes and nodes, sets the correct pointers
to pipe and node objects of the hazard

*/
void hazard::update_pipe_network()
{
	for (uint16_t i = 0; i < nodes.size(); i++)
		nodes[i].index = i;

	for (uint16_t i = 0; i < pipes.size(); i++)
		pipes[i].index = i;

	for (auto& nd : nodes)
	{
		if (nd.get_type() != "iFlow Valve")
		{
			for (auto& pp : pipes)
			{
				if (pp.node1id == nd.get_id())
				{
					pp.add_node1(nd.index);
				}
				else if (pp.node2id == nd.get_id())
				{
					pp.add_node2(nd.index);
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
					nd.add_pipe1(pp.index);
				}
				else if (nd.pipe2id == pp.get_id())
				{
					nd.add_pipe2(pp.index);
				}
				else if ((nd.pipe3id) == pp.get_id())
				{
					nd.add_pipe3(pp.index);
				}
			}
		}
	}

	set_pipe_length();

	output_data("C:\\output.txt");
}

void hazard::set_pipe_length()
{
	for (auto& pp : pipes)
	{
		quantity<length> l;
		quantity<length> x_diff = nodes[pp.get_node1()].get_x() - nodes[pp.get_node2()].get_x();
		quantity<length> y_diff = nodes[pp.get_node1()].get_y() - nodes[pp.get_node2()].get_y();;
		quantity<length> z_diff = nodes[pp.get_node1()].get_z() - nodes[pp.get_node2()].get_z();;
		double ll = sqrt(pow(x_diff.value(), 2.0) + pow(y_diff.value(), 2.0) + pow(z_diff.value(), 2.0));
		l = l.from_value(ll);
		pp.set_pipe_length(l);
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
quantity<volume> hazard::calcActualInergenVol()
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
	quantity<volume> totalInergenVol = 0 * cubic_meter;
	for (auto& enc : enclosures) totalInergenVol += enc.get_vol_agent_required();
	for (auto& enc : enclosures)
	{
		quantity<dimensionless> volFraction;
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
		quantity<dimensionless> suppliedFloodingFactor = 0;
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
		quantity<dimensionless> dc;
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
	if (discharge_time == 120 * seconds)
		estimated_system_flow_rate = suppliedInergenVol * 0.95;
	else if (discharge_time == 60 * seconds)
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
		if (discharge_time == 120 * seconds)
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
			nozQ = (int)ceil(ceil(enc.getLength().value() / 9.75)*ceil(enc.getWidth().value() / 9.75));
			enc.set_nozzle_quantity(nozQ);
			if (enc.get_net_volume() > 580.11*cubic_meters)
				nozQ = (int)ceil(enc.get_net_volume() / (580.11*cubic_meters)); //max volume coverage is 95.1sq.m*6.1m = 508.11cu.m
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
