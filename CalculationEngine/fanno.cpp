#include "fanno.h"

namespace fanno
{
/**
    calculated the exit mach number based on the fanno line
    returns 0 if a subsonic M2 can be found
    returns 1 if flow reaches Mach 1 before the end of the pipe
*/
int Mach2
(
	double M1,   // inlet mach number
	double & M2 ,// outlet mach number which is going to be calculated (output of the function)
	double L,    // total length of the pipe in meters
	double f,    // friction factor
	double D,    // diameter of the pipe in meters
	double gama, // specific heat ratio
	double guess // guess value for M2; if subsonic, choose 0.000001
)
{
	double fanno1 = (1-std::pow(M1,2.0))/(gama*std::pow(M1,2))+(gama+1)/2.0/gama*std::log((gama+1)*std::pow(M1,2)/(2.0+(gama-1)*std::pow(M1,2)));
	double fanno2 = fanno1 - f*L/D;
	if (fanno2 < 0) 
	{
	    double max_length = fanno1 * D / L;
	    std::cout << std::endl;
	    std::cout << "Mach 1 is reached at length " << max_length << " which is " << L-max_length << " shorter than the pipe length." << std::endl;
	    return 1;
	}
	double error = 0.1;
	while (error > 0.00000001)
	{
	    double numerator = ((1-std::pow(guess,2.0))/(gama*std::pow(guess,2.0)))+(gama+1)/2.0/gama*std::log((gama+1)*std::pow(guess,2.0)/(2+(gama-1)*std::pow(guess,2.0)))-fanno2;
	    double denum = 4.0*(std::pow(guess,2.0)-1)/std::pow(guess,3)/gama/(std::pow(guess,2.0)*gama-std::pow(guess,2.0)+2.0);
	    M2 = guess - numerator / denum;
	    error = std::abs(M2 - guess);
	    guess = M2;
	}
	return 0;
}


/**
    calculates pressure at the end of the pipe
*/
int Pressure2
(
    double P1,   //  pressure at the beginning of the pipe
    double& P2,  //  pressure at the end of the pipe (output of the function)
    double M1,   //  Mach number at the beginning of the pipe
    double M2,   //  Mach number at the end of the pipe
    double gama  //  specific heat ratio
)
{
    double numerator = std::pow(M1,2.0)*(1.0+(gama-1.0)/2.0*std::pow(M1,2.0));
    double denum     = std::pow(M2,2.0)*(1.0+(gama-1.0)/2.0*std::pow(M2,2.0));
    P2 = P1 * std::sqrt(numerator/denum);
    return 0;
}

/**
    calculates temperature drop at the end of the pipe
*/
int temperature2
(
    double  T1,  // temperature at the beginning of the pipe
    double& T2,  // temperature at the end of the pipe (output of the function)
    double gama, // specific heat ratio
    double M1,   // Mach number at the beginning of the pipe
    double M2    // Mach number at the end of the pipe
)
{
    double numerator = 1.0 + (gama-1.0) / 2.0 * std::pow(M1,2.0);
    double denum     = 1.0 + (gama-1.0) / 2.0 * std::pow(M2,2.0);
    T2 = T1 * numerator / denum;
    return 0;
}

/**
    calculates the density of the gas at the end of the pipe
*/
int density2
(
    double rho1, // density at the beginning of the pipe
    double& rho2,// density at the end of the pipe (output of the function)
    double gama, // specific heat ratio
    double M1,   // Mach number at the beginning of the pipe
    double M2    // Mach number at the end of pipe
)
{
    double denum     = 1.0 + (gama-1.0) / 2.0 * std::pow(M1,2.0);
    double numerator = 1.0 + (gama-1.0) / 2.0 * std::pow(M2,2.0);
    rho2 = rho1 * M1 / M2 *std::sqrt(numerator/denum);
    return 0;
}

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
)
{
    int error = Mach2(M1,M2,L,f,D,gama,guess);
    if (error == 1) return 1;
    Pressure2(P1,P2,M1,M2,gama);
    temperature2(T1,T2,gama,M1,M2);
    density2(rho1,rho2,gama,M1,M2);
    return 0;
}
}