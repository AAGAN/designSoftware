#pragma once
#include <string>
#include <vector>
#include "boostheader.h"
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

	quantity<temperature> min_temperature; //!< room minimum temperature
	quantity<temperature> max_temperature; //!< room maximum temperature
	quantity<temperature> temperatures; //!< instantanuous temperature of the hazard area (used s at the end to remove the confusion from boost units)
	quantity<temperature> normal_temperature; //!< normal ambient temperature of the enclosure

	quantity<pressure> max_wall_strength; //!< Maximum allowable wall strength

	quantity<dimensionless> min_design_concentration; //!< percent minimum design concentration
	quantity<dimensionless> supplied_design_concentration;//!< concentration that is the result of discharging actual number of cylinders in the hazard at maximum temperature 
	
	std::string fuel_class; 
	std::string fuel_type;
	quantity<dimensionless> cup_burner;

	std::string vent_calculation_method; //!<  this is the vent calculatoin method which can be "Ansul" or "FIA"
	bool num_cylinder_locked; //!<  true if number of cylinders is locked, false if software should change the number of cylinders

	quantity<length> altitude; //!< altitude in meters
	quantity<dimensionless> altitude_correction_factor;
	
	bool nozzle_option; //!< true for "quantity" or false for "fixed"
	
	quantity<time> discharge_time = 60.0 * seconds; //!< discharge time in seconds

	quantity<length> lengths; //!< length of the hazard area
	quantity<length> width; //!< width of the hazard area
	quantity<length> height; //!< height of the hazard area
	quantity<volume> gross_volume; //!<  gross volume of the hazard area, if it is zero in the constructor, then it is calculated from dimensions
	quantity<volume> volume_restrictions; //!< Structural volume restrictions
	quantity<volume> net_volume; //!< net volume of the hazard area
	
	quantity<dimensionless> flooding_factor; //!< cubic meter of inergen per cubic meter of the room volume
	quantity<dimensionless> supplied_flooding_factor;
	
	quantity<volume> vol_agent_required; //!< minimum required amount of inergen in cubic meters
	quantity<volume> supplied_vol_agent; //!< supplied amount of inergen which is more thatn minimum required amount because of round up of number of cylinders

	quantity<dimensionless> o2_concentration;
	quantity<dimensionless> co2_concentration;
	
	int nozzle_quantity;

	//std::vector<nozzle> nozzle;//!< nozzles in the hazard area

	quantity<volume> estimated_flow_rate;

public:
	//enclosure();
	~enclosure();

	
	enclosure(
		int hazID,
		int ID,
		std::string name, 
		quantity<temperature> minTemp, 
		quantity<temperature> maxTemp, 
		quantity<pressure> maxStrength, 
		quantity<dimensionless> concentration, 
		quantity<temperature> temp,
		std::string VentCalcMethod,
		quantity<length> Altitude,
		quantity<dimensionless> alt_cor_factor,
		bool nozOption,
		quantity<length> Length,
		quantity<length> Width,
		quantity<length> Height,
		quantity<volume> grossVol, 
		quantity<volume> RestrictionVol,
		int numNozzles,
		quantity<time> dischargeTime,
		std::string fuelClass,
		std::string fuelType,
		quantity<dimensionless> cupBurnerValue
		
	);//!< Concstructor

	std::string get_name() { return enclosure_name; }

	//quantity<mass> get_required_agent_mass();//!<Calculates the mass of agent required
	
	void setLength(quantity<length> Lengths) { lengths = Lengths; }
	quantity<length> getLength() { return lengths; }
	void setWidth(quantity<length> Width) { width = Width; }
	quantity<length> getWidth() { return width; }
	void setHeight(quantity<length> Height) { height = Height; }
	quantity<length> getHeight() { return height; }

	quantity<volume> get_net_volume() { return net_volume; }
	quantity<volume> get_gross_volume() { return gross_volume; }

	quantity<temperature> get_max_temperature() { return max_temperature; }
	quantity<temperature> get_normal_ambient_temperature() { return normal_temperature; }
	
	quantity<dimensionless> calc_concentration(quantity<temperature> t, quantity<dimensionless> floodingfactor);
	//quantity<dimensionless> calc_concentration(std::string condition);
	//!< sets the defined design concentration of the agent in the hazard area
	//quantity<dimensionless> getConcentration() { return min_design_concentration; }//!< returns the defined design concentration of the agent in the hazard area
	//quantity<dimensionless> calculate_design_concentration(std::string Class, std::string Type, quantity<dimensionless> cup_burner_value);
	//setting the supplied design concentration at maximum temperature 
	void set_supplied_design_concentration(quantity<dimensionless> ds) { supplied_design_concentration = ds; }
	quantity<dimensionless> get_supplied_design_concentration() { return supplied_design_concentration; }

	quantity<dimensionless> getFloodingFactor() { return flooding_factor; };//!< Calculated and returns the flooding factor
	void set_supplied_flooding_factor(quantity<dimensionless> sff) { supplied_flooding_factor = sff; }
	quantity<dimensionless> get_supplied_flooding_factor() { return supplied_flooding_factor; }

	void set_vol_agent_supplied(quantity<volume> Volume) { supplied_vol_agent = Volume; }//<! this is function is required to set the volume after calculating the number of cylinders, we need to assign the correct amount of agent to each hazard
	void set_supplied_vol_agent(quantity<volume> actualAgentVol) { supplied_vol_agent = actualAgentVol; }
	quantity<volume> get_supplied_vol_agent() { return supplied_vol_agent; }
	quantity<volume> get_vol_agent_required();//!< Calculates the volume of the required agent based on the min_temperature, design_concentration and CoolProp properties and updates vol_agent_required

	void set_estimated_flow_rate(quantity<volume> rate) { estimated_flow_rate = rate; }
	quantity<volume> get_estimated_flow_rate() { return estimated_flow_rate; }

	int get_nozzle_quantity() { return nozzle_quantity; }
	void set_nozzle_quantity(int nozQuantity) { nozzle_quantity = nozQuantity; }
	bool get_nozzle_option() { return nozzle_option; }

	int get_hazard_id() { return hazardId; }
	int get_id() { return id; }

	quantity<dimensionless> get_o2_concentration() { return o2_concentration; }
	quantity<dimensionless> get_co2_concentration() { return co2_concentration; }
	void set_o2_concentration(quantity<dimensionless> o2_conc) { o2_concentration = o2_conc; }
	void set_co2_concentration(quantity<dimensionless> co2_conc) { co2_concentration = co2_conc; }

	quantity<dimensionless> get_altitude_correction_factor(quantity<length> Altitude);//!< calculates and return the altitude_correction_factor based on the "altitude" table in "database.db"
	void set_altitude_correction_factor(quantity<dimensionless> cor_factor) { altitude_correction_factor = cor_factor; }
	quantity<dimensionless> get_altitude_correction_factor() { return altitude_correction_factor; }
};