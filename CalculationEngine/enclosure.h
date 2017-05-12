#pragma once
#include <string>
#include <vector>
#include <math.h>


/**
	Class to define an enclosure object
*/
class enclosure
{
protected:
	int id; //!< id of the enclosure
	int hazardId; //!< id of the hazard that enclosure is a part of
	std::string enclosure_name; 

	double min_temperature; //!< room minimum temperature
	double max_temperature; //!< room maximum temperature
	double temperatures; //!< instantanuous temperature of the hazard area (used s at the end to remove the confusion from boost units)
	double normal_temperature; //!< normal ambient temperature of the enclosure

	double max_wall_strength; //!< Maximum allowable wall strength

	double min_design_concentration; //!< percent minimum design concentration
	double supplied_design_concentration;//!< concentration that is the result of discharging actual number of cylinders in the hazard at maximum temperature 
	
	std::string fuel_class; 
	std::string fuel_type;
	double cup_burner;

	std::string vent_calculation_method; //!<  this is the vent calculatoin method which can be "Ansul" or "FIA"
	bool num_cylinder_locked; //!<  true if number of cylinders is locked, false if software should change the number of cylinders

	double altitude; //!< altitude in meters
	double altitude_correction_factor;
	
	bool nozzle_option; //!< true for "quantity" or false for "fixed"
	
	double discharge_time = 60.0; // * seconds; //!< discharge time in seconds

	double lengths; //!< length of the hazard area
	double width; //!< width of the hazard area
	double height; //!< height of the hazard area
	double gross_volume; //!<  gross volume of the hazard area, if it is zero in the constructor, then it is calculated from dimensions
	double volume_restrictions; //!< Structural volume restrictions
	double net_volume; //!< net volume of the hazard area
	
	double flooding_factor; //!< cubic meter of inergen per cubic meter of the room volume
	double supplied_flooding_factor;
	
	double vol_agent_required; //!< minimum required amount of inergen in cubic meters
	double supplied_vol_agent; //!< supplied amount of inergen which is more thatn minimum required amount because of round up of number of cylinders

	double o2_concentration;
	double co2_concentration;
	
	int nozzle_quantity;

	//std::vector<nozzle> nozzle;//!< nozzles in the hazard area

	double estimated_flow_rate;

	std::vector<int> nozzle_indecies;

public:
	//enclosure();
	~enclosure();

	
	enclosure(
		int hazID,
		int ID,
		std::string name, 
		double minTemp,
		double maxTemp,
		double maxStrength,
		double concentration,
		double temp,
		std::string VentCalcMethod,
		double Altitude,
		double alt_cor_factor,
		bool nozOption,
		double Length,
		double Width,
		double Height,
		double grossVol,
		double RestrictionVol,
		int numNozzles,
		double dischargeTime,
		std::string fuelClass,
		std::string fuelType,
		double cupBurnerValue
		
	);//!< Concstructor

	int get_id() { return id; }

	std::string get_name() { return enclosure_name; }

	//quantity<mass> get_required_agent_mass();//!<Calculates the mass of agent required
	
	void setLength(double Lengths) { lengths = Lengths; }
	double getLength() { return lengths; }
	void setWidth(double Width) { width = Width; }
	double getWidth() { return width; }
	void setHeight(double Height) { height = Height; }
	double getHeight() { return height; }

	double get_net_volume() { return net_volume; }
	double get_gross_volume() { return gross_volume; }

	double get_max_temperature() { return max_temperature; }
	double get_normal_ambient_temperature() { return normal_temperature; }
	
	double calc_concentration(double t, double floodingfactor);
	//quantity<dimensionless> calc_concentration(std::string condition);
	//!< sets the defined design concentration of the agent in the hazard area
	//quantity<dimensionless> getConcentration() { return min_design_concentration; }//!< returns the defined design concentration of the agent in the hazard area
	//quantity<dimensionless> calculate_design_concentration(std::string Class, std::string Type, quantity<dimensionless> cup_burner_value);
	//setting the supplied design concentration at maximum temperature 
	void set_supplied_design_concentration(double ds) { supplied_design_concentration = ds; }
	double get_supplied_design_concentration() { return supplied_design_concentration; }

	double getFloodingFactor() { return flooding_factor; };//!< Calculated and returns the flooding factor
	void set_supplied_flooding_factor(double sff) { supplied_flooding_factor = sff; }
	double get_supplied_flooding_factor() { return supplied_flooding_factor; }

	void set_vol_agent_supplied(double Volume) { supplied_vol_agent = Volume; }//<! this is function is required to set the volume after calculating the number of cylinders, we need to assign the correct amount of agent to each hazard
	void set_supplied_vol_agent(double actualAgentVol) { supplied_vol_agent = actualAgentVol; }
	double get_supplied_vol_agent() { return supplied_vol_agent; }
	double get_vol_agent_required();//!< Calculates the volume of the required agent based on the min_temperature, design_concentration and CoolProp properties and updates vol_agent_required

	void set_estimated_flow_rate(double rate) { estimated_flow_rate = rate; }
	double get_estimated_flow_rate() { return estimated_flow_rate; }

	int get_nozzle_quantity() { return nozzle_quantity; }
	void set_nozzle_quantity(int nozQuantity) { nozzle_quantity = nozQuantity; }
	bool get_nozzle_option() { return nozzle_option; }

	int get_hazard_id() { return hazardId; }

	double get_o2_concentration() { return o2_concentration; }
	double get_co2_concentration() { return co2_concentration; }
	void set_o2_concentration(double o2_conc) { o2_concentration = o2_conc; }
	void set_co2_concentration(double co2_conc) { co2_concentration = co2_conc; }

	double get_altitude_correction_factor(double Altitude);//!< calculates and return the altitude_correction_factor based on the "altitude" table in "database.db"
	void set_altitude_correction_factor(double cor_factor) { altitude_correction_factor = cor_factor; }
	double get_altitude_correction_factor() { return altitude_correction_factor; }

	void add_nozzle_index(int ind) { nozzle_indecies.push_back(ind); }
	int number_of_nozzles() { return nozzle_indecies.size(); }

	void reset_nozzle_indecies() { nozzle_indecies.clear(); }

};