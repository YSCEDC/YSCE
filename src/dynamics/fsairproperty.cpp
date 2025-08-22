#include "fsairproperty.h"
#include <cmath>


double FsGetAirDensity(const double &alt)
{
	int a, index;
	static double rhoTab[]=
	{
		1.478080,   /*	-2000m	0*/
		1.224991,   /*	0m		1*/
		1.006490,   /*	2000m	2*/
		0.819129,   /*	4000m	3*/
		0.659697,   /*	6000m	4*/
		0.525168,   /*	8000m	5*/
		0.412707,   /*	10000m	6*/
		0.310828,   /*	12000m	7*/
		0.226753,   /*	14000m	8*/
		0.165420,   /*	16000m	9*/
		0.120676,   /*	18000m	10*/
		0.088034,   /*	20000m	11*/
		0.063727,   /*	22000m	12*/
		0.046267,   /*	24000m	13*/
		0.033688,   /*	26000m	14*/
		0.024598,   /*	28000m	15*/
		0.018011,   /*	30000m	16*/
		0.013225,   /*	32000m	17*/
		0.009608,   /*	34000m	18*/
		0.007034,   /*	36000m	19*/
		0.000000,   /*	44000m	20*/
	};

	a = (int)floor(alt / 2000.0);
	if (21 < a)
	{
		return rhoTab[20];
	}
	else if (a > 17)
	{
		double sampleAlt = alt - 36000.0;
		double diff = rhoTab[19] - rhoTab[20];
		double localResult = rhoTab[19] - (diff / 8000.0 * sampleAlt);
		return localResult;
	}
	else if (a < -1)
	{
		return rhoTab[0];
	}
	else
	{
		double base, diff, t;
		base = rhoTab[a + 1];
		diff = rhoTab[a + 2] - rhoTab[a + 1];
		t = (alt - 2000.0F * a) / 2000.0F;
		return base + diff * t;
	}
}

const double &FsGetSeaLevelAirDensity(void)
{
	static const double rhoZero=1.224991;
	return rhoZero;
}

double FsGetAirTemperature(const double& alt)
{
	int a;
	static double tempTab[] =
	{
		301.15,   /*	-2000m	*/
		288.15,   /*	0m		*/
		275.15,   /*	2000m	*/
		262.15,   /*	4000m	*/
		249.15,   /*	6000m	*/
		236.15,   /*	8000m	*/
		223.15,   /*	10000m	*/
		216.65,   /*	12000m	*/
		216.65,   /*	14000m	*/
		216.65,   /*	16000m	*/
		216.65,   /*	18000m	*/
		216.65,   /*	20000m	*/
		218.65,   /*	22000m	*/
		220.65,   /*	24000m	*/
		222.65,   /*	26000m	*/
		224.65,   /*	28000m	*/
		226.65,   /*	30000m	*/
		228.65,   /*	32000m	*/
		234.25,   /*	34000m	*/
		239.85,   /*	36000m	*/
		265.00,   /*	44000m	*/
	};

	a = (int)floor(alt / 2000.0);
	if (21 < a)
	{
		return tempTab[20];
	}
	else if (a > 17)
	{
		double sampleAlt = alt - 36000.0;
		double diff = tempTab[19] - tempTab[20];
		double localResult = tempTab[19] - (diff / 8000.0 * sampleAlt);
		return localResult;
	}
	else if (a < -1)
	{
		return tempTab[0];
	}
	else
	{
		double base, diff, t;
		base = tempTab[a + 1];
		diff = tempTab[a + 2] - tempTab[a + 1];
		t = (alt - 2000.0F * a) / 2000.0F;
		return base + diff * t;
	}
}

const double& FsGetSeaLevelAirTemperature(void)
{
	static const double kelvin = 288.15;
	return kelvin;
}

double FsGetAirPressure(const double& alt)
{
	int a;
	static double pressTab[] =
	{
		127.774,	/*	-2000m	*/
		101.325,	/*	0m		*/
		79.495,		/*	2000m	*/
		61.640,		/*	4000m	*/
		47.181,		/*	6000m	*/
		35.599,		/*	8000m	*/
		26.436,		/*	10000m	*/
		19.330,		/*	12000m	*/
		14.101,		/*	14000m	*/
		10.287,		/*	16000m	*/
		7.504,		/*	18000m	*/
		5.474,		/*	20000m	*/
		3.999,		/*	22000m	*/
		2.930,		/*	24000m	*/
		2.153,		/*	26000m	*/
		1.586,		/*	28000m	*/
		1.171,		/*	30000m	*/
		0.868,		/*	32000m	*/
		0.646,		/*	34000m	*/
		0.484,		/*	36000m	*/
		0.000,		/*	44000m	*/
	};

	a = (int)floor(alt / 2000.0);
	if (21 < a)
	{
		return pressTab[20];
	}
	else if (a > 17)
	{
		double sampleAlt = alt - 36000.0;
		double diff = pressTab[19] - pressTab[20];
		double localResult = pressTab[19] - (diff / 8000.0 * sampleAlt);
		return localResult;
	}
	else if (a < -1)
	{
		return pressTab[0];
	}
	else
	{
		double base, diff, t;
		base = pressTab[a + 1];
		diff = pressTab[a + 2] - pressTab[a + 1];
		t = (alt - 2000.0F * a) / 2000.0F;
		return base + diff * t;
	}
}

const double& FsGetSeaLevelAirPressure(void)
{
	static const double kilopascals = 288.15;
	return kilopascals;
}

////////////////////////////////////////////////////////////

double FsGetMachOne(const double &alt)
{
	int a;
	static double machTab[] =
	{
		347.886,   /*	-2000m	*/
		340.294,   /*	0m		*/
		332.529,   /*	2000m	*/
		324.579,   /*	4000m	*/
		316.428,   /*	6000m	*/
		308.063,   /*	8000m	*/
		299.463,   /*	10000m	*/
		295.070,   /*	12000m	*/
		295.070,   /*	14000m	*/
		295.070,   /*	16000m	*/
		295.070,   /*	18000m	*/
		295.070,   /*	20000m	*/
		296.428,   /*	22000m	*/
		297.781,   /*	24000m	*/
		299.128,   /*	26000m	*/
		300.468,   /*	28000m	*/
		301.803,   /*	30000m	*/
		303.131,   /*	32000m	*/
		306.821,   /*	34000m	*/
		310.467,   /*	36000m	*/
		330.000,   /*	44000m	*/
	};

	a = (int)floor(alt / 2000.0);
	if (21 < a)
	{
		return machTab[20];
	}
	else if (a > 17)
	{
		double sampleAlt = alt - 36000.0;
		double diff = machTab[19] - machTab[20];
		double localResult = machTab[19] - (diff / 8000.0 * sampleAlt);
		return localResult;
	}
	else if (a < -1)
	{
		return machTab[0];
	}
	else
	{
		double base, diff, t;
		base = machTab[a + 1];
		diff = machTab[a + 2] - machTab[a + 1];
		t = (alt - 2000.0F * a) / 2000.0F;
		return base + diff * t;
	}
}


double FsGetGravityMultiplier(const double& alt)
{
	double fullGravAlt, noGravAlt, gravMult;
	fullGravAlt = 50000.0;
	noGravAlt = 150000.0;

	if (alt < fullGravAlt)
	{
		gravMult = 1.0;
	}
	else if (alt > noGravAlt)
	{
		gravMult = 0;
	}
	else
	{
		gravMult = 1 - (alt - fullGravAlt) / (noGravAlt - fullGravAlt);
	}
	return gravMult;
}