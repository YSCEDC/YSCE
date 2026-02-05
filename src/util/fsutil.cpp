#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ysclass.h>
#include <ysport.h>
#include <fsdef.h>
#include "fsutil.h"

////////////////////////////////////////////////////////////

// Numerical Differentiation

// Lagrange Polynomials
//  L0=  (x*x-(x1+x2)*x+x1*x2)/((x0-x1)*(x0-x2)
//  L0'= (2*x-(x1+x2))/((x0-x1)*(x0-x2))
//  L0''= 2/((x0-x1)*(x0-x2))

//  L1=  (x*x-(x0+x2)*x+x0*x2)/((x1-x0)*(x1-x2))
//  L1'= (2*x-(x0+x2))/((x1-x0)*(x1-x2))
//  L1''= 2/((x1-x0)*(x1-x2))

//  L2=  (x*x-(x0+x1)*x+x0*x1)/((x2-x0)*(x2-x1))
//  L2'= (2*x-(x0+x1))/((x2-x0)*(x2-x1))
//  L2''= 2/((x2-x0)*(x2-x1));

double FsNumerical1stDifferentiation
    (const double &x0,const double &y0,
     const double &x1,const double &y1,
     const double &x2,const double &y2,
     const double &x)
{
	return y0*(2.0*x-(x1+x2))/((x0-x1)*(x0-x2))
	      +y1*(2.0*x-(x0+x2))/((x1-x0)*(x1-x2))
	      +y2*(2.0*x-(x0+x1))/((x2-x0)*(x2-x1));
}


double FsNumerical2ndDifferentiation
    (const double &x0,const double &y0,
     const double &x1,const double &y1,
     const double &x2,const double &y2,
     const double &/*x*/)
{
	return y0*2.0/((x0-x1)*(x0-x2))
	      +y1*2.0/((x1-x0)*(x1-x2))
	      +y2*2.0/((x2-x0)*(x2-x1));
}

void FsCompressNumberInString(char str[])
{



	YSSIZE_T n=(int)strlen(str);
	auto lastSeparator=n;
	for(auto i=n; i>=0; i--)
	{
		if(str[i]==0 || str[i]==' ' || str[i]=='\t' || str[i]=='\n' ||
		   str[i]==',' || str[i]=='e' || str[i]=='E')
		{
			lastSeparator=i;
		}
		else if(str[i]=='.')
		{
			// Here between i+1 to lastSeparator-1 are below-decimal digits.
			decltype(n) j,firstNonZero,nDel;

			firstNonZero=i;
			for(j=lastSeparator-1; j>i; j--)
			{
				if(str[j]!='0')
				{
					firstNonZero=j;
					break;
				}
			}
			if(firstNonZero==i)
			{
				nDel=lastSeparator-(i+2);  //  0.000 -> 0.0
			}
			else
			{
				nDel=lastSeparator-(firstNonZero+1);
			}

			if(nDel>0)
			{
				for(j=lastSeparator; str[j]!=0; j++)
				{
					str[j-nDel]=str[j];
				}
				str[j-nDel]=0;
			}

			lastSeparator=i;
		}
	}
}


YSBOOL FsTestKeyWordMatch(const YsString &str,YSSIZE_T ac,const YsString av[])
{
	if(ac==0)
	{
		return YSTRUE;
	}

	for(YSSIZE_T i=0; i<ac; i++)
	{
		YSBOOL match;
		match=YSFALSE;
		for(YSSIZE_T j=0; j<=str.Strlen()-av[i].Strlen(); j++)
		{
			if(strncmp(str+j,av[i],av[i].Strlen())==0)
			{
				match=YSTRUE;
				break;
			}
		}
		if(match!=YSTRUE)
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}


const char *FsTrueFalseString(YSBOOL tf)
{
	switch(tf)
	{
	case YSTRUE:
		return "TRUE";
	default:
		return "FALSE";
	}
}


void FsGetTodayFileString(YsString &str)
{
	struct tm *tmStr;
	time_t tm;
	char buf[256];

	tm=time(NULL);
	tmStr=localtime(&tm);
	sprintf(buf,"%04d%02d%02d",tmStr->tm_year+1900,tmStr->tm_mon+1,tmStr->tm_mday);

	str.Set(buf);
}

void FsGetTodayTimeString(YsString &str)
{
	time_t tm;
	tm=time(NULL);
	str.Set(ctime(&tm));
}



FsAutoCloseFile::FsAutoCloseFile()
{
	filePtr=NULL;
}

FsAutoCloseFile::~FsAutoCloseFile()
{
	if(filePtr!=NULL)
	{
		fclose(filePtr);
	}
}


FILE *FsAutoCloseFile::Fp()
{
	return filePtr;
}

FILE *FsAutoCloseFile::Fopen(const wchar_t fn[],const char mode[])
{
	if(filePtr==NULL)
	{
		filePtr=YsFileIO::Fopen(fn,mode);
		return filePtr;
	}
	else
	{
		return NULL;
	}
}

void FsAutoCloseFile::Fclose(void)
{
	if(filePtr!=NULL)
	{
		fclose(filePtr);
		filePtr=NULL;
	}
}
////////////////////////////////////////////////////////////

void FsInternationalText::CleanUp(void)
{
	this->languageCode="";
	this->msg=L"";
}
void FsInternationalText::SetText(const char lang[],const wchar_t msg[])
{
	this->languageCode=lang;
	this->msg=msg;
}
YSBOOL FsInternationalText::MatchLanguage(const char lang[]) const
{
	if(0==strcmp(lang,languageCode))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
const char *FsInternationalText::GetLanguageCode(void) const
{
	return languageCode;
}
const wchar_t *FsInternationalText::GetText(void) const
{
	return msg;
}

////////////////////////////////////////////////////////////

unsigned int FsGetDate(void)
{
	time_t t0;
	t0=time(NULL);

	struct tm *t;
	t=localtime(&t0);

	unsigned int d=(t->tm_year+1900)*10000 +(t->tm_mon+1)*100+ t->tm_mday;
	printf("Date %d\n",d);

	return d;
}

////////////////////////////////////////////////////////////

const char *FsRegionIdToString(int id)
{
	switch(id)
	{
	default:
		return "Unknown Region Type";
	case FS_RGNID_RUNWAY:
		return "FS_RGNID_RUNWAY";
	case FS_RGNID_TAXIWAY:
		return "FS_RGNID_TAXIWAY";
	case FS_RGNID_AIRPORT_AREA:
		return "FS_RGNID_AIRPORT_AREA";
	case FS_RGNID_ENEMY_TANK_GENERATOR:
		return "FS_RGNID_ENEMY_TANK_GENERATOR";
	case FS_RGNID_FRIENDLY_TANK_GENERATOR:
		return "FS_RGNID_FRIENDLY_TANK_GENERATOR";
	case FS_RGNID_VIEWPOINT:
		return "FS_RGNID_VIEWPOINT";
	}
}

////////////////////////////////////////////////////////////

double FsGetLevelTurnRadius(const double v,const double bank)
{
	return (v*v)/(FsGravityConst*tan(bank));
}

////////////////////////////////////////////////////////////

static const char *const FsLengthUnit[]=
{
	"IN","FT","KM","CM","M","SM","NM",NULL
};

static const char *const FsAreaUnit[]=
{
	"IN^2","M^2",NULL
};

static const char *const FsWeightUnit[]=
{
	"KG","LB","T","N",NULL
};

static const char *const FsSpeedUnit[]=
{
	"MACH","KM/H","M/S","KT",NULL
};

static const char *const FsAngleUnit[]=
{
	"RAD","DEG",NULL
};
static const char *const FsPowerUnit[]=
{
	"HP","J/S",NULL
};

static int FsGetUnit(const char in[],const char *const unitLst[])
{
	int i;
	char str[256];
	strncpy(str,in,255);
	str[255]=0;

	YsCapitalize(str);
	for(i=0; unitLst[i]!=NULL; i++)
	{

		int ls=(int)strlen(str);
		int lu=(int)strlen(unitLst[i]);
		if(lu<ls && strcmp(str+ls-lu,unitLst[i])==0 &&
		  '0'<=str[ls-lu-1] && str[ls-lu-1]<='9')
		{
			return i;
		}
	}

	static YSBOOL first=YSTRUE;
	if(YSTRUE==first)
	{
		printf("Unkown or Inproper Unit %s\n",in);
		first=YSFALSE;
	}
	return -1;
}

YSRESULT FsGetJoulePerSecond(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsPowerUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"HP"
			dat=atof(in)*740.0;
			break;
		case 1://"J/S"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetLength(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsLengthUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"IN"
			dat=atof(in)*0.0254;
			break;
		case 1://"FT"
			dat=atof(in)*0.3048;
			break;
		case 2://"KM"
			dat=atof(in)*1000.0;
			break;
		case 3://"CM"
			dat=atof(in)/100.0;
			break;
		case 4://"M"
			dat=atof(in);
			break;
		case 5: // "SM"
			dat=atof(in)*1609.0;
			break;
		case 6: // "NM"
			dat=atof(in)*1852.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetArea(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsAreaUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"IN^2"
			dat=atof(in)*0.000645;
			break;
		case 1://"M^2"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetWeight(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsWeightUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"KG"
			dat=atof(in);
			break;
		case 1://"LB"
			dat=atof(in)*0.453597;
			break;
		case 2://"T"
			dat=atof(in)*1000.0;
			break;
		case 3://"N"
			dat=atof(in)/FsGravityConst;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetForce(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsWeightUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"KG"
			dat=atof(in)*FsGravityConst;
			break;
		case 1://"LB"
			dat=atof(in)*0.453597*FsGravityConst;
			break;
		case 2://"T"
			dat=atof(in)*1000.0*FsGravityConst;
			break;
		case 3://"N"
			dat=atof(in);
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetSpeed(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsSpeedUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0://"MACH"
			dat=atof(in)*340.0;
			break;
		case 1://"KM/H"
			dat=atof(in)*1000.0/3600.0;
			break;
		case 2://"M/S"
			dat=atof(in);
			break;
		case 3://"KT"
			dat=atof(in)*1852.0/3600.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetAngle(double &dat,const char in[])
{
	int uni;
	uni=FsGetUnit(in,FsAngleUnit);
	if(uni>=0)
	{
		switch(uni)
		{
		case 0: //"RAD"
			dat=atof(in);
			break;
		case 1: //"DEG"
			dat=atof(in)*YsPi/180.0;
			break;
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetBool(YSBOOL &dat,const char in[])
{
	char str[256];
	strcpy(str,in);
	YsCapitalize(str);
	if(strncmp(str,"TRUE",4)==0)
	{
		dat=YSTRUE;
		return YSOK;
	}
	else if(strncmp(str,"FALSE",5)==0)
	{
		dat=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGetNonDimensional(double &dat,const char in[])
{
	dat=atof(in);
	return YSOK;
}

YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const char * const av[])
{
	if(ac>=3)
	{
		double x,y,z;
		if(FsGetLength(x,av[0])==YSOK &&
		   FsGetLength(y,av[1])==YSOK &&
		   FsGetLength(z,av[2])==YSOK)
		{
			vec.Set(x,y,z);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetVec3(YsVec3 &vec,YSSIZE_T ac,const YsString av[])
{
	if(ac>=3)
	{
		double x,y,z;
		if(FsGetLength(x,av[0])==YSOK &&
		   FsGetLength(y,av[1])==YSOK &&
		   FsGetLength(z,av[2])==YSOK)
		{
			vec.Set(x,y,z);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const char * const av[])
{
	if(ac>=3)
	{
		double h,p,b;
		if(FsGetAngle(h,av[0])==YSOK &&
		   FsGetAngle(p,av[1])==YSOK &&
		   FsGetAngle(b,av[2])==YSOK)
		{
			att.Set(h,p,b);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGetAtt3(YsAtt3 &att,YSSIZE_T ac,const YsString av[])
{
	if(ac>=3)
	{
		double h,p,b;
		if(FsGetAngle(h,av[0])==YSOK &&
		   FsGetAngle(p,av[1])==YSOK &&
		   FsGetAngle(b,av[2])==YSOK)
		{
			att.Set(h,p,b);
			return YSOK;
		}
	}
	return YSERR;
}

int FsGetRandomBetween(int min, int max) 
{
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

double FsGetRandomBetween(double min, double max)
{
	double random = ((double)rand()) / (double)RAND_MAX;
	double range = max - min;
	return (random * range) + min;
}

////////////////////////////////////////////////////////////

OldSojiStockCheck::OldSojiStockCheck()
{
	//keywordList.Initialize();
}

const char *const OldSojiStockCheck::sojiAirList[] =
{
	"A-4_SKYHAWK",
	"A-6_INTRUDER",
	"A6M5_ZERO",
	"A-10A_THUNDERBOLT2",
	"AIRBUS300",
	"AIRBUS320",
	"AH-1_COBRA",
	"AH-64_APACHE",
	"ALPHAJET",
	"AMX",
	"F-18C_BLUEANGELS",
	"PIPER_ARCHER(WASHIN-AIR_MARKING)",
	"AV-8B_HARRIER",
	"B-2_SPIRIT",
	"B-29_SUPERFORTRESS",
	"B-52_STRATOFORTRESS",
	"B737",
	"B747",
	"B767",
	"B777",
	"C-130_HERCULES",
	"CESSNA_172R",
	"CONCORDE",
	"DRAKEN",
	"E-2C_HAWKEYE",
	"EA-6B_PROWLER",
	"DIAMOND_ECLIPSE",
	"EUROFIGHTER_TYPHOON",
	"F-1",
	"F-2",
	"F-4E_PHANTOM",
	"F-4EJ_PHANTOM",
	"F-5_TIGER2",
	"F-14_TOMCAT",
	"F-15C_EAGLE",
	"F-15D_EAGLE",
	"F-15DJ_EAGLE",
	"F-15DJ_EAGLE_CHAIRO",
	"F-15DJ_EAGLE_KURO",
	"F-15DJ_EAGLE_MADARA",
	"F-15DJ_EAGLE_MIDORI",
	"F-15DJ_EAGLE_NAKAAO",
	"F-15DJ_EAGLE_SOTOAO",
	"F-15DJ_EAGLE_SOTOMIDORI",
	"F-15J_EAGLE",
	"F-16C_FIGHTINGFALCON",
	"F-18C_HORNET",
	"F-18E_SUPERHORNET",
	"F-22_RAPTOR",
	"F-86_BLUEIMPULSE",
	"F-100D_SUPERSABRE",
	"F-104_STARFIGHTER",
	"F-117A_NIGHTHAWK",
	"FOCKE_WULF_FW190",
	"G4M_TYPE1BOMBER",
	"GRIPEN",
	"HAWK",
	"HURRICANE",
	"KFIR",
	"LANCASTER",
	"MIG-15_FAGOT",
	"MIG-21_FISHBED",
	"MIG-23_FLOGGER",
	"MIG-29_FULCRUM",
	"MIRAGE2000",
	"MU-2S",
	"P-51_MUSTANG",
	"RAFALE",
	"HAWK_REDARROWS",
	"S-3_VIKING",
	"SPITFIRE",
	"SU-22_FITTER",
	"SU-24_FENCER",
	"SU-25_FROGFOOT",
	"SU-27_FLANKER",
	"T-2_BLUEIMPULSE",
	"T-4_BLUEIMPULSE",
	"T-400",
	"F-16C_THUNDERBIRDS",
	"TORNADO",
	"TU-160_BLACKJACK",
	"U-125",
	"U-125A",
	"UH-60_BLACKHAWK",
	"UH-60J_BLACKHAWK",
	"V-107",
	"VIGGEN",
	"YS-11"
};

const char *const OldSojiStockCheck::sojiGndList[] =
{
	"AAA",
	"ALERT",
	"ASPAM",
	"ATSUGI_TOWER",
	"BIGBEN",
	"BOX1000X1000X1000",
	"BRIDGE1",
	"BRIDGE2",
	"BRIDGE3",
	"BUILD01",
	"BUILD10W",
	"BUILD11",
	"BUILD15",
	"BUILD30",
	"BUILD60",
	"BUILD90",
	"BURKE_DESTROYER",
	"BUS_1",
	"AIRCRAFTCARRIER",
	"CASTLE",
	"CAVE_BASE",
	"CAVE_BASE_COLL",
	"CIELING1",
	"CONTAINERSHIP_L",
	"ELEPHANT",
	"ELEVATED_RUNWAY_1000X60",
	"FIXED_CHECKPOINT_500FT",
	"FLOATINGBASE1",
	"FLOATINGBASE2",
	"FUEL_TRUCK",
	"FUEL_TRUCK(W)",
	"GATEGUARD_CONCORDE",
	"GEPARD",
	"GROUNDTARGET1",
	"GROUNDTARGET2",
	"HANGAR1",
	"HANGAR2",
	"HANGAR3",
	"HANGER",
	"HANGER1",
	"HANGER4",
	"HG01",
	"HG02",
	"HG03",
	"HG04",
	"HG05",
	"HG06",
	"HG07",
	"ILS",
	"HMS_INVINCIBLE",
	"ISLAND_BASE",
	"JEEP",
	"TOWER_OF_LONDON",
	"M1A1ABRAMS",
	"M60PATTON",
	"NDB",
	"PALACE",
	"PAPI_LEFT",
	"PAPI_RIGHT",
	"PIRATE",
	"PLANT",
	"POWER_PLANT",
	"RADAR",
	"RADARBASE",
	"NUCLEAR_REACTOR",
	"SAM",
	"SFC",
	"SFC_ALPHA",
	"SFC_CLUB",
	"SFC_GAMMA",
	"SFC_LAMBDA",
	"SFC_MEDIA",
	"SFC_O1",
	"SFC_O2",
	"SFC_OMEGA",
	"SFC_SIGMA",
	"SFC_TAU",
	"SHOP",
	"SHOPPING1",
	"SHOPPING2",
	"STATION",
	"T-64",
	"T-80",
	"TERMINAL",
	"TERMINAL_SHORT",
	"TEST",
	"TOWER",
	"TOWER1",
	"TOWER2",
	"TOWER3",
	"TRACKER",
	"TREE1",
	"TREE2",
	"TREES_200M_10M",
	"TREES_400M_20M",
	"TRUCK",
	"TUNNEL1",
	"TUNNEL2",
	"TUNNEL3",
	"TUNNEL4",
	"TUNNEL5",
	"UDALOY_DESTROYER",
	"VASI",
	"VOR",
	"VORDME",
	"WALL1",
	"WINDMILLBASE",
	"WINDMILLBLADE"
};

const char* const OldSojiStockCheck::sojiSceList[] =
{
	"RACING_VALLEY",
	"RACING_DESERT",
	"ATSUGI_AIRBASE",
	"CRESCENT_ISLAND",
	"HAWAII",
	"ISLAND_GOURD",
	"MATSUSHIMA_AIRBASE",
	"NAHA_AIRPORT",
	"NEWTA_AIRBASE",
	"PACIFIC_ATOLL",
	"SMALL_MAP",
	"TOHOKU",
	"AOMORI",
	"NORTH_KYUSYU",
	"AIRSTRIKE_CHALLENGE",
	"HEATHROW",
	"SLAPSTICK",
	"OCEAN"
};

YSBOOL OldSojiStockCheck::FsCheckIfOldStock(const char in[])
{
	if (FsCheckIfOldAir(in) == YSTRUE)
	{
		return YSTRUE;
	}
	else if (FsCheckIfOldGnd(in) == YSTRUE)
	{
		return YSTRUE;
	}
	else if (FsCheckIfOldSce(in) == YSTRUE)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCheckIfOldAir(const char in[])
{
	for (int i = 0; sojiAirList[i] != NULL; i++)
	{
		if (strcmp(sojiAirList[i], in) == 0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCheckIfOldGnd(const char in[])
{
	for (int i = 0; sojiAirList[i] != NULL; i++)
	{
		if (strcmp(sojiAirList[i], in) == 0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCheckIfOldSce(const char in[])
{
	for (int i = 0; sojiAirList[i] != NULL; i++)
	{
		if (strcmp(sojiAirList[i], in) == 0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCorrectIfOldAir(const char in[],YsString &out)
{
	if (FsCheckIfOldAir(in) == YSTRUE)
	{
		int i;
		char prefix[] = {'[','S','O','J','I',']'};
		for (i=0;i<6;i++)
		{
			out.Append(prefix[i]);
		}
		for (i=0; in[i] != NULL; i++)
		{
			out.Append(in[i]);
		}
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCorrectIfOldGnd(const char in[], YsString &out)
{
	if (FsCheckIfOldGnd(in) == YSTRUE)
	{
		int i;
		char prefix[] = { '[','S','O','J','I',']' };
		for (i = 0; i < 6; i++)
		{
			out.Append(prefix[i]);
		}
		for (i = 0; in[i] != NULL; i++)
		{
			out.Append(in[i]);
		}
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL OldSojiStockCheck::FsCorrectIfOldSce(const char in[], YsString &out)
{
	if (FsCheckIfOldSce(in) == YSTRUE)
	{
		int i;
		char prefix[] = { '[','S','O','J','I',']' };
		for (i = 0; i < 6; i++)
		{
			out.Append(prefix[i]);
		}
		for (i = 0; in[i] != NULL; i++)
		{
			out.Append(in[i]);
		}
		return YSTRUE;
	}
	return YSFALSE;
}