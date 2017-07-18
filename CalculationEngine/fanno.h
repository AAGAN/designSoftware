#pragma once
#include <iostream>
#include <cmath>

namespace fanno
{
/**
    calculated the exit mach number based on the fanno line
*/
int Mach2
(
	double , // inlet mach number
	double &,// outlet mach number which is going to be calculated (output of the function)
	double , // total length of the pipe in meters
	double , // friction factor
	double , // diameter of the pipe in meters
	double , // specific heat ratio
	double   // guess value for M2; if subsonic, choose 0.000001
);

/**
    calculates pressure at the end of the pipe
*/
int Pressure2
(
    double ,  //  pressure at the beginning of the pipe
    double&,  //  pressure at the end of the pipe (output of the function)
    double ,  //  Mach number at the beginning of the pipe
    double ,  //  Mach number at the end of the pipe
    double    // specific heat ratio
);

/**
    calculates temperature drop at the end of the pipe
*/
int temperature2
(
    double,  // temperature at the beginning of the pipe
    double&, // temperature at the end of the pipe (output of the function)
    double,  // specific heat ratio
    double,  // Mach number at the beginning of the pipe
    double   // Mach number at the end of the pipe
);

/**
    calculates the density of the gas at the end of the pipe
*/
int density2
(
    double ,   // density at the beginning of the pipe
    double&,   // density at the end of the pipe (output of the function)
    double ,   // specific heat ratio
    double ,   // Mach number at the beginning of the pipe
    double     // Mach number at the end of pipe
);

/**
    calculates all the properties at the end of a pipe based on fanno line
*/
int fanno_flow
(
    double M1,   // inlet mach number
	double & M2 ,// outlet mach number which is going to be calculated (output)
	double P1,   // pressure at the beginning of the pipe
	double&P2,   // pressure at the end of the pipe (output)
	double rho1, // density at the beginiing of the pipe
	double&rho2, // density at the end of the pipe (output)
	double T1,   // temperature at the beginning of the pipe
	double&T2,   // temperature at the end of the pipe (output)
	double L,    // total length of the pipe in meters
	double f,    // friction factor
	double D,    // diameter of the pipe in meters
	double gama, // specific heat ratio
	double guess // guess value for M2; if subsonic, choose 0.000001
);
}