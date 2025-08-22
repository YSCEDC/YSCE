#ifndef FSAIRPROPERTY_IS_INCLUDED
#define FSAIRPROPERTY_IS_INCLUDED
/* { */

double FsGetAirDensity(const double &alt);         //Air density in kg/m^3
const double &FsGetSeaLevelAirDensity(void);       //Sea level air density in kg/m^3
double FsGetAirTemperature(const double& alt);     //Air temperature in K
const double& FsGetSeaLevelAirTemperature(void);   //Sea level air temperature in K
double FsGetAirPressure(const double& alt);        //Air pressure in kPa
const double& FsGetSeaLevelAirPressure(void);      //Sea level air pressure in kPa
double FsGetMachOne(const double &alt);            //Speed of sound in m/s
double FsGetGravityMultiplier(const double& alt);  //Scaling of gravity with altitude

/* } */
#endif
