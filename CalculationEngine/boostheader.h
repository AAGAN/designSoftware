#pragma once

#include <boost\units\systems\si\time.hpp>
#include <boost\units\systems\si\area.hpp>
#include <boost\units\systems\si\volume.hpp>
#include <boost\units\systems\si\length.hpp>
#include <boost\units\systems\si\temperature.hpp>
#include <boost\units\systems\si\velocity.hpp>
#include <boost\units\systems\si\dimensionless.hpp>
#include <boost\units\systems\si\dynamic_viscosity.hpp>
#include <boost\units\systems\si\kinematic_viscosity.hpp>
#include <boost\units\systems\si\mass_density.hpp>
#include <boost\units\systems\si\energy.hpp>
#include <boost\units\systems\si\force.hpp>
#include <boost\units\systems\si\io.hpp>
#include <boost\units\systems\si\power.hpp>
#include <boost\units\systems\si\mass.hpp>
#include <boost\units\systems\si\plane_angle.hpp>
#include <boost\units\systems\si\pressure.hpp>
//#include <boost\units\systems\si\specific_volume.hpp>
#include <math.h>

using namespace boost::units;
using namespace boost::units::si;

typedef boost::units::derived_dimension<
	boost::units::mass_base_dimension, 3,
	boost::units::time_base_dimension, -1
>::type mass_flow_rate_dimension;

typedef boost::units::unit<
	mass_flow_rate_dimension,
	boost::units::si::system
> mass_flow_rate_unit;

typedef boost::units::quantity<mass_flow_rate_unit> mass_flow_rate;

BOOST_UNITS_STATIC_CONSTANT(kilogram_per_seconds, mass_flow_rate);

typedef boost::units::derived_dimension<
	boost::units::mass_base_dimension, 1,
	boost::units::length_base_dimension, -1
>::type mass_per_unit_length_dimension;

typedef boost::units::unit<
	mass_per_unit_length_dimension,
	boost::units::si::system
> mass_per_unit_length_unit;

typedef boost::units::quantity<mass_per_unit_length_unit> mass_per_unit_length;

BOOST_UNITS_STATIC_CONSTANT(kilogram_per_meters, mass_per_unit_length);

